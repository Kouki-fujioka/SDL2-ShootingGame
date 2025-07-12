/*****************************************************************
Name : server_net.c
Function : ネットワーク処理 (サーバ)
*****************************************************************/

#include "server_common.h"
#include "server_func.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

typedef struct
{
    int fd;                   // ファイルディスクリプタ (通信用ソケット)
    char name[MAX_NAME_SIZE]; // ユーザ名
} CLIENT;

static CLIENT gClients[MAX_CLIENTS]; // クライアントデータ
static int gClientNum;               // クライアント数
static fd_set gMask;                 // 監視対象 FD 集合 (全クライアント通信用ソケット)
static int gWidth;                   // 監視対象 FD 最大値 + 1
static int MultiAccept(int request_soc, int num);
static void Enter(int pos, int fd);
static void SetMask(int maxfd);
static void SendAllName(void);
static int RecvData(int pos, void *data, int dataSize);

/*****************************************************************
Name : SetUpServer
Function : コネクション (クライアントサーバ間) 設立 → ユーザ名受信
Parameter : int num : クライアント数
Return : コネクション設立失敗時 : -1, コネクション設立成功時 : 0
*****************************************************************/
int SetUpServer(int num)
{
    struct sockaddr_in server; // ソケットアドレス構造体 (IPv4 用)
    int request_soc;           // ファイルディスクリプタ (待ち受け用ソケット)
    int maxfd;                 // ファイルディスクリプタ (通信用ソケット) 最大値
    int val = 1;               // SO_REUSEADDR 有効化オプション

    assert(0 < num && num <= MAX_CLIENTS); // 引数チェック

    gClientNum = num;
    bzero((char *)&server, sizeof(server)); // server 構造体初期化
    server.sin_family = AF_INET;            // アドレスファミリ (ソケット通信タイプ) → IPv4

    // エンディアン変換必要 (u_short 型 → 2 byte, uint32_t 型 → 4 byte)
    server.sin_port = htons(PORT);              // エンディアン変換 (host to network long : little endian → big endian)
    server.sin_addr.s_addr = htonl(INADDR_ANY); // エンディアン変換 (host to network long : little endian → big endian)

    // 待ち受け用ソケット作成
    if ((request_soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) // 待ち受け用ソケット作成失敗時
    {
        fprintf(stderr, "Socket allocation failed\n");
        return -1;
    }

    setsockopt(request_soc, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)); // 待ち受け用ソケット詳細設定

    // 待ち受け用ソケット割当 (IP アドレス, ポート番号)
    if (bind(request_soc, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        fprintf(stderr, "Cannot bind\n");
        close(request_soc);
        return -1;
    }

    fprintf(stderr, "Successfully bind!\n");

    // 待ち受け用ソケット接続待ち状態 (同時接続数 == gClientNum)
    if (listen(request_soc, gClientNum) == -1) // 待ち受け用ソケット接続待ち状態失敗時
    {
        fprintf(stderr, "Cannot listen\n");
        close(request_soc);
        return -1;
    }

    fprintf(stderr, "Listen OK\n");

    // 通信用ソケット作成
    maxfd = MultiAccept(request_soc, gClientNum);
    close(request_soc);

    if (maxfd == -1) // 通信用ソケット作成失敗時
        return -1;

    SendAllName();  // 全ユーザ名送信 (to 全クライアント)
    SetMask(maxfd); // 監視対象 (全クライアント通信用ソケット) 設定

    return 0;
}

/*****************************************************************
Name : MultiAccept
Function : 複数コネクション (クライアントサーバ間) 設立
Parameters : int request_soc : ファイルディスクリプタ (待ち受け用ソケット)
             int num : クライアント数
Return : fd : 最大ファイルディスクリプタ (通信用ソケット)
*****************************************************************/
static int MultiAccept(int request_soc, int num)
{
    int fd; // ファイルディスクリプタ (通信用ソケット)

    for (int i = 0; i < num; i++)
    {
        // 通信用ソケット作成
        /*
                    +---------------+
          Client 1--|               |
          Client 2--|  request_soc  | ← listen中
          Client 3--|               |
                    +---------------+
                           ↓ accept()
                    +------------------+
                    | 通信用 socket #1 | ← fd = 4
                    +------------------+
                    | 通信用 socket #2 | ← fd = 5
                    +------------------+
                    | 通信用 socket #3 | ← fd = 6
                    +------------------+
         */
        if ((fd = accept(request_soc, NULL, NULL)) == -1) // 通信用ソケット作成失敗時
        {
            fprintf(stderr, "Accept error\n");

            for (int j = i - 1; j >= 0; j--)
                close(gClients[j].fd);

            return -1;
        }

        Enter(i, fd); // ユーザ名受信待機
    }

    return fd; // 最大ファイルディスクリプタ (通信用ソケット)
}

/*****************************************************************
Name : Enter
Function : ユーザ名受信
Parameters : int pos : クライアント番号
             int fd : ファイルディスクリプタ (通信用ソケット)
*****************************************************************/
static void Enter(int pos, int fd)
{
    read(fd, gClients[pos].name, MAX_NAME_SIZE); // 受信データ格納 (gClients[pos].name = ユーザ名)
    gClients[pos].fd = fd;
}

/*****************************************************************
Name : SendAllName
Function : 全ユーザ名送信 (to 全クライアント)
*****************************************************************/
static void SendAllName(void)
{
    int tmp1;
    int tmp2;

    // エンディアン変換必要 (int 型 → 4 byte)
    tmp2 = htonl(gClientNum); // エンディアン変換 (host to network long : little endian → big endian)

    for (int i = 0; i < gClientNum; i++)
    {
        // エンディアン変換必要 (int 型 → 4 byte)
        tmp1 = htonl(i);                 // エンディアン変換 (host to network long : little endian → big endian)
        SendData(i, &tmp1, sizeof(int)); // クライアント番号データ送信
        SendData(i, &tmp2, sizeof(int)); // クライアント数データ送信

        for (int j = 0; j < gClientNum; j++)
        {
            SendData(i, gClients[j].name, MAX_NAME_SIZE); // ユーザ名データ送信
        }
    }
}

/*****************************************************************
Name : SetMask
Function : 監視対象 (全クライアント通信用ソケット) 設定
Parameter : int maxfd : 最大ファイルディスクリプタ (通信用ソケット)
*****************************************************************/
static void SetMask(int maxfd)
{
    gWidth = maxfd + 1; // 監視対象 FD 最大値 + 1
    FD_ZERO(&gMask);    // fd_set 構造体初期化

    for (int i = 0; i < gClientNum; i++)
        FD_SET(gClients[i].fd, &gMask); // 各クライアント通信用ソケット → gMask
}

/*****************************************************************
Name : SendRecvManager
Function : 受信データ処理
Return : endFlag : 終了フラグ
*****************************************************************/
int SendRecvManager(void)
{
    fd_set readOK = gMask; // 監視対象 FD 集合 (初期化 : 全クライアント通信用ソケット)
    char command;          // コマンド格納用
    int endFlag = 1;       // 終了フラグ

    // FD (全クライアント) 監視
    // gWidth : 監視対象 FD 最大値 + 1
    // readOK : 監視対象 FD 集合 (全クライアント)
    // 読み込み可能データ受信 → readOK 更新 (受信 FD 以外削除)
    if (select(gWidth, &readOK, NULL, NULL, NULL) < 0) // エラー発生時
    {
        return endFlag;
    }

    // 読み込み可能データ受信後
    for (int i = 0; i < gClientNum; i++)
    {
        if (FD_ISSET(gClients[i].fd, &readOK)) // 送信元クライアント判別
        {
            RecvData(i, &command, sizeof(char));  // コマンド読み込み (送信元クライアント)
            endFlag = ExecuteCommand(command, i); // コマンド送信 (送信元クライアント)

            if (endFlag == 0)
                break;
        }
    }

    return endFlag;
}

/*****************************************************************
Name : RecvIntData
Function : int 型データ受信 (from クライアント)
Parameters : int pos : 送信元クライアント番号
             int *intData : int 型データ格納用
Return : n : 受信データバイト数
*****************************************************************/
int RecvIntData(int pos, int *intData)
{
    int n;   // 受信データバイト数
    int tmp; // 受信データ格納用

    assert(0 <= pos && pos < gClientNum); // 引数チェック
    assert(intData != NULL);              // 引数チェック

    n = RecvData(pos, &tmp, sizeof(int)); // 受信データ格納
    (*intData) = ntohl(tmp);              // エンディアン変換 (network to host long : big endian → little endian)

    return n;
}

/*****************************************************************
Name : RecvData
Function : データ受信 (from クライアント)
Parameters : int pos : 送信元クライアント番号
             void *data : 受信データ格納用
             void data : 受信データ格納用アドレス (&data)
             int dataSize : 受信データサイズ
Return : n : 受信データバイト数
*****************************************************************/
static int RecvData(int pos, void *data, int dataSize)
{
    int n; // 受信データバイト数

    assert(0 <= pos && pos < gClientNum); // 引数チェック
    assert(data != NULL);                 // 引数チェック
    assert(0 < dataSize);                 // 引数チェック

    n = read(gClients[pos].fd, data, dataSize); // 受信データ格納

    return n;
}

/*****************************************************************
Name : SendData
Function : データ送信 (to クライアント)
Parameters : int pos : 送信先クライアント番号
             void *data : 送信データ
             void data : 送信データアドレス (&data)
             int dataSize : 送信データサイズ
*****************************************************************/
void SendData(int pos, void *data, int dataSize)
{
    assert(0 <= pos && pos < gClientNum || pos == ALL_CLIENTS); // 引数チェック
    assert(data != NULL);                                       // 引数チェック
    assert(0 < dataSize);                                       // 引数チェック

    if (pos == ALL_CLIENTS)
    {
        for (int i = 0; i < gClientNum; i++)
        {
            write(gClients[i].fd, data, dataSize); // FD データ書き込み (送信)
        }
    }
    else
    {
        write(gClients[pos].fd, data, dataSize); // FD データ書き込み (送信)
    }
}

/*****************************************************************
Name : Ending
Function : 全コネクション (クライアントサーバ間) 切断
*****************************************************************/
void Ending(void)
{
    printf("... Connection closed\n");

    for (int i = 0; i < gClientNum; i++)
        close(gClients[i].fd);
}

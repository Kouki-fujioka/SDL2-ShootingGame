/*****************************************************************
Name : client_net.c
Function : ネットワーク処理 (クライアント)
*****************************************************************/

#include "common.h"
#include "client_func.h"
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 100

static int gSocket;  // ファイルディスクリプタ (サーバ通信用ソケット)
static fd_set gMask; // 監視対象 FD 集合 (サーバ通信用ソケット)
static int gWidth;   // 監視対象 FD 最大値 + 1

static void GetAllName(int *clientID, int *num, char clientNames[][MAX_NAME_SIZE]);
static void SetMask(void);
static int RecvData(void *data, int dataSize);

/*****************************************************************
Name : SetUpClient
Function : コネクション (クライアントサーバ間) 接続 → ユーザ名送信
Parameters : char *hostName : サーバホスト名
             int *clientID : クライアント番号格納用
             int	*num : クライアント数格納用
             char clientNames[][MAX_NAME_SIZE] : 全ユーザ名格納用
Return : コネクション接続失敗時 : -1, コネクション接続成功時 : 0
*****************************************************************/
int SetUpClient(char *hostName, int *clientID, int *num, char clientNames[][MAX_NAME_SIZE])
{
  struct hostent *servHost;  // ホスト情報構造体 (IPv4 用)
  struct sockaddr_in server; // ソケットアドレス構造体 (IPv4 用)
  int len;                   // ユーザ名字数
  char str[BUF_SIZE];        // ユーザ名格納用

  // サーバホスト名 → IP アドレス取得
  if ((servHost = gethostbyname(hostName)) == NULL) // IP アドレス取得失敗時
  {
    fprintf(stderr, "Unknown host\n");
    return -1;
  }

  bzero((char *)&server, sizeof(server)); // server 構造体初期化
  server.sin_family = AF_INET;            // アドレスファミリ (ソケット通信タイプ) → IPv4

  // エンディアン変換必要 (u_short 型 → 2 byte, uint32_t 型 → 4 byte)
  server.sin_port = htons(PORT);                                         // エンディアン変換 (host to network long : little endian → big endian)
  bcopy(servHost->h_addr, (char *)&server.sin_addr, servHost->h_length); // 接続先サーバ IP アドレスコピー → server.sin_addr

  // サーバ通信用ソケット作成
  if ((gSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) // サーバ通信用ソケット作成失敗時
  {
    fprintf(stderr, "socket allocation failed\n");
    return -1;
  }

  // サーバ接続
  if (connect(gSocket, (struct sockaddr *)&server, sizeof(server)) == -1) // サーバ接続失敗時
  {
    fprintf(stderr, "cannot connect\n");
    close(gSocket);
    return -1;
  }

  fprintf(stderr, "connected\n");

  // ユーザ名入力
  do
  {
    printf("Enter Your Name\n");
    fgets(str, BUF_SIZE, stdin);
    len = strlen(str) - 1;
    str[len] = '\0';
  } while (len > MAX_NAME_SIZE - 1 || len == 0);

  SendData(str, MAX_NAME_SIZE); // ユーザ名送信 (to サーバ)
  printf("Please Wait\n");

  GetAllName(clientID, num, clientNames); // 全ユーザ名受信 (from サーバ)
  SetMask();                              // 監視対象 (サーバ通信用ソケット) 設定

  return 0;
}

/*****************************************************************
Name : SendData
Function : データ送信 (to サーバ)
Parameters : void *data : 送信データ
             void data : 送信データアドレス (&data)
             int dataSize : 送信データサイズ
*****************************************************************/
void SendData(void *data, int dataSize)
{
  assert(data != NULL); // 引数チェック
  assert(0 < dataSize); // 引数チェック

  write(gSocket, data, dataSize); // FD データ書き込み (送信)
}

/*****************************************************************
Name : GetAllName
Function : 全ユーザ名受信 (from サーバ)
Parameters : int *clientID : クライアント番号格納用
             int *num : クライアント数格納用
             char clientNames[][] : 全ユーザ名格納用
*****************************************************************/
static void GetAllName(int *clientID, int *num, char clientNames[][MAX_NAME_SIZE])
{
  RecvIntData(clientID); // 受信データ格納
  RecvIntData(num);      // 受信データ格納

  for (int i = 0; i < (*num); i++)
  {
    RecvData(clientNames[i], MAX_NAME_SIZE); // ユーザ名読み込み (送信元クライアント)
  }
}

/*****************************************************************
Name : SetMask
Function : 監視対象 (サーバ通信用ソケット) 設定
*****************************************************************/
static void SetMask(void)
{
  FD_ZERO(&gMask);         // fd_set 構造体初期化
  FD_SET(gSocket, &gMask); // サーバ通信用ソケット → gMask
  gWidth = gSocket + 1;    // 監視対象 FD 最大値 + 1
}

/*****************************************************************
Name : SendRecvManager
Function : 受信データ処理
Return : endFlag : 終了フラグ
*****************************************************************/
int SendRecvManager(void)
{
  fd_set readOK = gMask;  // 監視対象 FD 集合 (初期化 : サーバ通信用ソケット)
  char command;           // コマンド格納用
  int endFlag = 1;        // 終了フラグ
  struct timeval timeout; // 時間格納用
  timeout.tv_sec = 0;     // 秒
  timeout.tv_usec = 20;   // マイクロ秒

  // FD (サーバ) 監視
  // gWidth : 監視対象 FD 最大値 + 1
  // readOK : 監視対象 FD 集合 (サーバ)
  // 20 マイクロ秒間監視
  select(gWidth, &readOK, NULL, NULL, &timeout);

  // 読み込み可能データ受信後
  if (FD_ISSET(gSocket, &readOK))
  {
    RecvData(&command, sizeof(char));  // コマンド読み込み (サーバ)
    endFlag = ExecuteCommand(command); // コマンド送信 (サーバ)
  }

  return endFlag;
}

/*****************************************************************
Name : RecvIntData
Function : int 型データ受信 (from サーバ)
Parameters : int *intData : int 型データ格納用
Return : n : 受信データバイト数
*****************************************************************/
int RecvIntData(int *intData)
{
  int n;   // 受信データバイト数
  int tmp; // 受信データ格納用

  assert(intData != NULL); // 引数チェック

  n = RecvData(&tmp, sizeof(int)); // 受信データ格納
  (*intData) = ntohl(tmp);         // エンディアン変換 (network to host long : big endian → little endian)

  return n;
}

/*****************************************************************
Name : RecvData
Function : データ受信 (from サーバ)
Parameters : void *data : 受信データ格納用
             void data : 受信データ格納用アドレス (&data)
             int dataSize : 受信データサイズ
Return : n : 受信データバイト数
*****************************************************************/
int RecvData(void *data, int dataSize)
{
  int n; // 受信データバイト数

  assert(data != NULL); // 引数チェック
  assert(0 < dataSize); // 引数チェック

  n = read(gSocket, data, dataSize); // 受信データ格納

  return n;
}

/*****************************************************************
Name : CloseSoc
Function : コネクション (クライアントサーバ間) 切断
*****************************************************************/
void CloseSoc(void)
{
  printf("...Connection closed\n");
  close(gSocket);
}

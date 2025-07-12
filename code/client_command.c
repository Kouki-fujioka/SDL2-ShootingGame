/*****************************************************************
Name : client_command.c
Function : コマンド処理 (クライアント)
*****************************************************************/

#include "common.h"
#include "client_func.h"

static void SetIntData2DataBlock(void *data, int intData, int *dataSize);
static void SetCharData2DataBlock(void *data, char charData, int *dataSize);
static void RecvUpData(void);
static void RecvDownData(void);
static void RecvLeftData(void);
static void RecvRightData(void);
static void RecvItemData(void);
static void RecvMeteoData(void);
static void RecvBeamData(void);
static void RecvLoadData(void);

/*****************************************************************
Name : ExecuteCommand
Function : コマンド送信 (to サーバ)
Parameter : char command : 送信コマンド
Return : endflag : 終了フラグ
*****************************************************************/
int ExecuteCommand(char command)
{
    int endFlag = 1;

    switch (command)
    {
    case END_COMMAND:
        endFlag = 0;
        break;

    case UP_COMMAND:
        RecvUpData();
        break;

    case DOWN_COMMAND:
        RecvDownData();
        break;

    case LEFT_COMMAND:
        RecvLeftData();
        break;

    case RIGHT_COMMAND:
        RecvRightData();
        break;

    case ITEM_COMMAND:
        RecvItemData();
        break;

    case METEO_COMMAND:
        RecvMeteoData();
        break;

    case SPACE_COMMAND:
        RecvBeamData();
        break;

    case LOAD_COMMAND:
        RecvLoadData();
        break;
    }

    return endFlag;
}

/*****************************************************************
Name : SendUpCommand
Function : 機体上移動コマンド送信 (to サーバ)
*****************************************************************/
void SendUpCommand(void)
{
    unsigned char data[MAX_DATA]; // 送信用データ
    int dataSize = 0;             // 送信用データインデックス

    SetCharData2DataBlock(data, UP_COMMAND, &dataSize); // 送信コマンドセット
    SendData(data, dataSize);                           // データ送信 (to サーバ)
}

/*****************************************************************
Name : SendDownCommand
Function : 機体下移動コマンド送信 (to サーバ)
*****************************************************************/
void SendDownCommand(void)
{
    unsigned char data[MAX_DATA]; // 送信用データ
    int dataSize = 0;             // 送信用データインデックス

    SetCharData2DataBlock(data, DOWN_COMMAND, &dataSize); // 送信コマンドセット
    SendData(data, dataSize);                             // データ送信 (to サーバ)
}

/*****************************************************************
Name : SendLeftCommand
Function : 機体左移動コマンド送信 (to サーバ)
*****************************************************************/
void SendLeftCommand(void)
{
    unsigned char data[MAX_DATA]; // 送信用データ
    int dataSize = 0;             // 送信用データインデックス

    SetCharData2DataBlock(data, LEFT_COMMAND, &dataSize); // 送信コマンドセット
    SendData(data, dataSize);                             // データ送信 (to サーバ)
}

/*****************************************************************
Name : SendRightCommand
Function : 機体右移動コマンド送信 (to サーバ)
*****************************************************************/
void SendRightCommand(void)
{
    unsigned char data[MAX_DATA]; // 送信用データ
    int dataSize = 0;             // 送信用データインデックス

    SetCharData2DataBlock(data, RIGHT_COMMAND, &dataSize); // 送信コマンドセット
    SendData(data, dataSize);                              // データ送信 (to サーバ)
}

/*****************************************************************
Name : SendBeamCommand
Function : ビーム発射コマンド送信 (to サーバ)
*****************************************************************/
void SendBeamCommand(void)
{
    unsigned char data[MAX_DATA]; // 送信用データ
    int dataSize = 0;             // 送信用データインデックス

    SetCharData2DataBlock(data, SPACE_COMMAND, &dataSize); // 送信コマンドセット
    SendData(data, dataSize);                              // データ送信 (to サーバ)
}

/*****************************************************************
Name : SendLoadCommand
Function : ビーム装填コマンド送信 (to サーバ)
*****************************************************************/
void SendLoadCommand(void)
{
    unsigned char data[MAX_DATA]; // 送信用データ
    int dataSize = 0;             // 送信用データインデックス

    SetCharData2DataBlock(data, LOAD_COMMAND, &dataSize); // 送信コマンドセット
    SendData(data, dataSize);                             // データ送信 (to サーバ)
}

/*****************************************************************
Name : SendEndCommand
Function : プログラム終了コマンド送信 (to サーバ)
*****************************************************************/
void SendEndCommand(void)
{
    unsigned char data[MAX_DATA]; // 送信用データ
    int dataSize = 0;             // 送信用データインデックス

    SetCharData2DataBlock(data, END_COMMAND, &dataSize); // 送信コマンドセット
    SendData(data, dataSize);                            // データ送信 (to サーバ)
}

/*****************************************************************
Name : SetIntData2DataBlock
Function : int 型データ → 送信用データ (最後尾) コピー
Parameters : void *data : 送信用データ (data[0] ~ data[MAX_DATA])
             void data : 送信用データアドレス (&data[0] ~ &data[MAX_DATA])
             int intData : int 型データ
             int *dataSize : 送信用データインデックス
*****************************************************************/
static void SetIntData2DataBlock(void *data, int intData, int *dataSize)
{
    int tmp;

    assert(data != NULL);     // 引数チェック
    assert(0 <= (*dataSize)); // 引数チェック

    // エンディアン変換必要 (int 型 → 4 byte)
    tmp = htonl(intData);                          // エンディアン変換 (host to network long : little endian → big endian)
    memcpy(data + (*dataSize), &tmp, sizeof(int)); // int 型データ → 送信用データ (最後尾) コピー
    (*dataSize) += sizeof(int);                    // 送信用データインデックス更新
}

/*****************************************************************
Name : SetCharData2DataBlock
Function : char 型データ → 送信用データ (最後尾) コピー
Parameters : void *data : 送信用データ (data[0] ~ data[MAX_DATA])
             void data : 送信用データアドレス (&data[0] ~ &data[MAX_DATA])
             char charData : char 型データ
             int *dataSize : 送信用データインデックス
*****************************************************************/
static void SetCharData2DataBlock(unsigned char *data, char charData, int *dataSize)
{
    assert(data != NULL);     // 引数チェック
    assert(0 <= (*dataSize)); // 引数チェック

    // エンディアン変換不要 (char 型 → 1 byte)
    *(data + (*dataSize)) = charData; // char 型データ → 送信用データ (最後尾) コピー
    (*dataSize) += sizeof(char);      // 送信用データインデックス更新
}

/*****************************************************************
Name : RecvUpData
Function : 機体上移動データ受信 (from サーバ)
*****************************************************************/
static void RecvUpData(void)
{
    int y;                 // 移動量 (y 座標)
    int charaID;           // クライアント番号
    RecvIntData(&y);       // 受信データ格納
    RecvIntData(&charaID); // 受信データ格納
    MoveUp(y, charaID);    // 対象機体上移動
}

/*****************************************************************
Name : RecvDownData
Function : 機体下移動データ受信 (from サーバ)
*****************************************************************/
static void RecvDownData(void)
{
    int y;                 // 移動量 (y 座標)
    int charaID;           // クライアント番号
    RecvIntData(&y);       // 受信データ格納
    RecvIntData(&charaID); // 受信データ格納
    MoveDown(y, charaID);  // 対象機体下移動
}

/*****************************************************************
Name : RecvLeftData
Function : 機体左移動データ受信 (from サーバ)
*****************************************************************/
static void RecvLeftData(void)
{
    int x;                 // 移動量 (x 座標)
    int charaID;           // クライアント番号
    RecvIntData(&x);       // 受信データ格納
    RecvIntData(&charaID); // 受信データ格納
    MoveLeft(x, charaID);  // 対象機体左移動
}

/*****************************************************************
Name : RecvRightData
Function : 機体右移動データ受信 (from サーバ)
*****************************************************************/
static void RecvRightData(void)
{
    int x;                 // 移動量 (x 座標)
    int charaID;           // クライアント番号
    RecvIntData(&x);       // 受信データ格納
    RecvIntData(&charaID); // 受信データ格納
    MoveRight(x, charaID); // 対象機体右移動
}

/*****************************************************************
Name : RecvItemData
Function : アイテム出現データ受信 (from サーバ)
*****************************************************************/
static void RecvItemData(void)
{
    int y; // 出現座標 (y 座標)

    RecvIntData(&y); // 受信データ格納
    MoveItem(y);     // アイテム出現
}

/*****************************************************************
Name : RecvMeteoData
Function : 隕石出現データ受信 (from サーバ)
*****************************************************************/
static void RecvMeteoData(void)
{
    int y; // 出現座標 (y 座標)

    RecvIntData(&y); // 受信データ格納
    MoveMeteo(y);    // 隕石出現
}

/*****************************************************************
Name : RecvLoadData
Function : ビーム装填データ受信 (from サーバ)
*****************************************************************/
static void RecvLoadData(void)
{
    int charaID; // クライアント番号

    RecvIntData(&charaID); // 受信データ格納
    MoveLoad(charaID);     // 対象機体ビーム装填
}

/*****************************************************************
Name : RecvBeamData
Function : ビーム発射データ受信 (from サーバ)
*****************************************************************/
static void RecvBeamData(void)
{
    int charaID; // クライアント番号

    RecvIntData(&charaID); // 受信データ格納
    MoveBeam(charaID);     // 対象機体ビーム発射
}

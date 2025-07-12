/*****************************************************************
Name : server_command.c
Function : コマンド処理 (サーバ)
*****************************************************************/

#include "server_common.h"
#include "server_func.h"

static void SetIntData2DataBlock(void *data, int intData, int *dataSize);
static void SetCharData2DataBlock(void *data, char charData, int *dataSize);
static int GetRandomInt(int n);
static int RecvDataInfo = 0;

/*****************************************************************
Name : ExecuteCommand
Function : コマンド送信 (to クライアント)
Parameters : char command : 送信コマンド
			 int pos : クライアント番号
Return : endflag : 終了フラグ
*****************************************************************/
int ExecuteCommand(char command, int pos)
{
	unsigned char data[MAX_DATA]; // 送信用データ
	int dataSize = 0;			  // 送信用データインデックス
	int endflag = 1;			  // 終了フラグ

	assert(0 <= pos && pos < MAX_CLIENTS); // 引数チェック

	switch (command)
	{
	case END_COMMAND:
		endflag = 0;
		SetCharData2DataBlock(data, command, &dataSize); // 送信コマンドセット
		SendData(ALL_CLIENTS, data, dataSize);			 // データ送信 (全クライアント)
		break;

	case UP_COMMAND:

	case DOWN_COMMAND:

	case LEFT_COMMAND:

	case RIGHT_COMMAND:
		SetCharData2DataBlock(data, command, &dataSize); // 送信コマンドセット
		SetIntData2DataBlock(data, 20, &dataSize);		 // 移動量セット
		SetIntData2DataBlock(data, pos, &dataSize);		 // クライアント番号セット
		SendData(ALL_CLIENTS, data, dataSize);			 // データ送信 (全クライアント)
		break;

	case SPACE_COMMAND:

	case LOAD_COMMAND:
		SetCharData2DataBlock(data, command, &dataSize); // 送信コマンドセット
		SetIntData2DataBlock(data, pos, &dataSize);		 // クライアント番号セット
		SendData(ALL_CLIENTS, data, dataSize);			 // データ送信 (全クライアント)
		break;

	default:
		fprintf(stderr, "0x%02x is absence!\n", command);
	}

	return endflag;
}

/*****************************************************************
Name : SendItemCommand
Function : アイテム出現コマンド送信 (to クライアント)
*****************************************************************/
void SendItemCommand(void)
{
	unsigned char data[MAX_DATA];							  // 送信用データ
	int dataSize = 0;										  // 送信用データインデックス
	SetCharData2DataBlock(data, ITEM_COMMAND, &dataSize);	  // 送信コマンドセット
	SetIntData2DataBlock(data, GetRandomInt(960), &dataSize); // アイテム出現位置 (y 座標) セット
	SendData(ALL_CLIENTS, data, dataSize);					  // データ送信 (全クライアント)
}

/*****************************************************************
Name : SendMeteoCommand
Function : 隕石出現コマンド送信 (to クライアント)
*****************************************************************/
void SendMeteoCommand(void)
{
	unsigned char data[MAX_DATA];							  // 送信用データ
	int dataSize = 0;										  // 送信用データインデックス
	SetCharData2DataBlock(data, METEO_COMMAND, &dataSize);	  // 送信コマンドセット
	SetIntData2DataBlock(data, GetRandomInt(960), &dataSize); // 隕石出現位置 (y 座標) セット
	SendData(ALL_CLIENTS, data, dataSize);					  // データ送信 (全クライアント)
}

/*****************************************************************
Name : SetIntData2DataBlock
Function : int 型データ → 送信用データ (最後尾) コピー
Parameters : void *data : 送信用データ (data[0] ~ data[MAX_DATA])
			 void data : 送信用データアドレス (&data[0] ~ &data[MAX_DATA])
			 int intData : int 型データ
			 int *dataSize : 送信用データインデックス
*****************************************************************/
static void SetIntData2DataBlock(unsigned char *data, int intData, int *dataSize)
{
	int tmp;

	assert(data != NULL);	  // 引数チェック
	assert(0 <= (*dataSize)); // 引数チェック

	// エンディアン変換必要 (int 型 → 4 byte)
	tmp = htonl(intData);						   // エンディアン変換 (host to network long : little endian → big endian)
	memcpy(data + (*dataSize), &tmp, sizeof(int)); // int 型データ → 送信用データ (最後尾) コピー
	(*dataSize) += sizeof(int);					   // 送信用データインデックス更新
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
	assert(data != NULL);	  // 引数チェック
	assert(0 <= (*dataSize)); // 引数チェック

	// エンディアン変換不要 (char 型 → 1 byte)
	*(data + (*dataSize)) = charData; // char 型データ → 送信用データ (最後尾) コピー
	(*dataSize) += sizeof(char);	  // 送信用データインデックス更新
}

/*****************************************************************
Name : GetRandomInt
Function : 乱数値返却
Parameter : int n : 乱数最大値
Return : 乱数値
*****************************************************************/
static int GetRandomInt(int n)
{
	return rand() % n;
}

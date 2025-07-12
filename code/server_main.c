/*****************************************************************
Name : server_main.c
Function : メインルーチン (サーバ)
*****************************************************************/

#include <SDL2/SDL.h>
#include "server_func.h"

static Uint32 SignalHandler1(Uint32 interval, void *param);
static Uint32 SignalHandler2(Uint32 interval, void *param);

int main(int argc, char *argv[])
{
	int num;		 // クライアント数
	int endFlag = 1; // 終了フラグ

	// argc == 2 (argv[0] == プログラム名, argv[1] == クライアント数)
	if (argc != 2)
	{
		fprintf(stderr, "Usage: number of clients\n");
		exit(-1);
	}

	// クライアント数格納
	if ((num = atoi(argv[1])) < 0 || num > MAX_CLIENTS)
	{
		fprintf(stderr, "clients limit = %d \n", MAX_CLIENTS);
		exit(-1);
	}

	// SDL 初期化
	if (SDL_Init(SDL_INIT_TIMER) < 0) // SDL 初期化失敗時
	{
		printf("failed to initialize SDL.\n");
		exit(-1);
	}

	// コネクション (クライアントサーバ間) 設立
	if (SetUpServer(num) == -1) // コネクション (クライアントサーバ間) 設立失敗時
	{
		fprintf(stderr, "Cannot setup server\n");
		exit(-1);
	}

	// 割り込み処理セット
	SDL_AddTimer(15000, SignalHandler1, NULL);
	SDL_AddTimer(5000, SignalHandler2, NULL);

	// メインイベントループ
	while (endFlag)
	{
		endFlag = SendRecvManager(); // 受信データ処理
		SDL_Delay(10);
	};

	Ending(); // 全コネクション (クライアントサーバ間) 切断

	return 0;
}

/*****************************************************************
Name : SignalHandler1
Function : 割り込み関数 (アイテム出現コマンド送信)
Parameters : Uint32 interval : 割り込み間隔
			 void *param : 割り込み関数用引数
Return : interval : 割り込み間隔
*****************************************************************/
static Uint32 SignalHandler1(Uint32 interval, void *param)
{
	SendItemCommand(); // アイテム出現コマンド送信

	return interval;
}

/*****************************************************************
Name : SignalHandler2
Function : 割り込み関数 (隕石出現コマンド送信)
Parameters : Uint32 interval : 割り込み間隔
			 void *param : 割り込み関数用引数
Return : interval : 割り込み間隔
*****************************************************************/
static Uint32 SignalHandler2(Uint32 interval, void *param)
{
	SendMeteoCommand(); // 隕石出現コマンド送信

	return interval;
}

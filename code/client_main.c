/*****************************************************************
Name : client_main.c
Function : メインルーチン (クライアント)
*****************************************************************/

#include "common.h"
#include "client_func.h"

int main(int argc, char *argv[])
{
  int clientID;                          // クライアント番号格納用
  int num;                               // クライアント数格納用
  char name[MAX_CLIENTS][MAX_NAME_SIZE]; // 全ユーザ名格納用
  int endFlag = 1;                       // 終了フラグ
  char localHostName[] = "localhost";    // ローカルホスト名
  char *serverName;                      // サーバホスト名

  // argc == 2 (argv[0] == プログラム名, argv[1] == サーバホスト名)
  if (argc == 1)
  {
    serverName = localHostName;
  }
  else if (argc == 2)
  {
    serverName = argv[1];
  }
  else
  {
    fprintf(stderr, "Usage: %s, Cannot find a Server Name.\n", argv[0]);
    return -1;
  }

  // コネクション (クライアントサーバ間) 接続
  if (SetUpClient(serverName, &clientID, &num, name) == -1) // コネクション (クライアントサーバ間) 接続失敗時
  {
    fprintf(stderr, "setup failed : SetUpClient\n");
    return -1;
  }

  // ウィンドウ初期化
  if (InitWindows(clientID, num, name) == -1) // ウィンドウ初期化失敗時
  {
    fprintf(stderr, "setup failed : InitWindows\n");
    return -1;
  }

  // メインイベントループ
  while (endFlag)
  {
    WindowEvent(num, clientID);  // ウィンドウイベント処理
    endFlag = SendRecvManager(); // 受信データ処理
    SDL_Delay(10);
  };

  DestroyWindow(); // SDL 終了
  CloseSoc();      // コネクション (クライアントサーバ間) 切断

  return 0;
}

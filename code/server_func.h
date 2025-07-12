/*****************************************************************
Name : server_func.h
Function : 外部関数定義 (サーバ用)
*****************************************************************/

#ifndef _SERVER_FUNC_H_ // _SERVER_FUNC_H_ 未定義時
#define _SERVER_FUNC_H_ // _SERVER_FUNC_H_ 定義 (値未設定)
#include "server_common.h"
// server_net.c
extern int SetUpServer(int num);
extern void Ending(void);
extern int RecvIntData(int pos, int *intData);
extern void SendData(int pos, void *data, int dataSize);
extern int SendRecvManager(void);
// server_command.c
extern int ExecuteCommand(char command, int pos);
extern void SendItemCommand(void);
extern void SendMeteoCommand(void);
#endif

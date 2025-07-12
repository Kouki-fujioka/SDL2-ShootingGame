/*****************************************************************
Name : client_func.h
Function : 外部関数定義 (クライアント用)
*****************************************************************/

#ifndef _CLIENT_FUNC_H_ // _CLIENT_FUNC_H_ 未定義時
#define _CLIENT_FUNC_H_ // _CLIENT_FUNC_H_ 定義 (値未設定)
#include "common.h"
/* client_net.c */
extern int SetUpClient(char *hostName, int *clientID, int *num, char clientName[][MAX_NAME_SIZE]);
extern void CloseSoc(void);
extern int RecvIntData(int *intData);
extern void SendData(void *data, int dataSize);
extern int SendRecvManager(void);
/* client_win.c */
extern int InitWindows(int clientID, int num, char name[][MAX_NAME_SIZE]);
extern void DestroyWindow(void);
extern void WindowEvent(int num, int clientID);
extern void MoveUp(int y, int clientID);
extern void MoveDown(int y, int clientID);
extern void MoveLeft(int x, int clientID);
extern void MoveRight(int x, int clientID);
extern void MoveItem(int y);
extern void MoveMeteo(int y);
extern void MoveBeam(int clientID);
extern void MoveLoad(int clientID);
/* client_command.c */
extern int ExecuteCommand(char command);
extern void SendUpCommand(void);
extern void SendDownCommand(void);
extern void SendLeftCommand(void);
extern void SendRightCommand(void);
extern void SendBeamCommand(void);
extern void SendLoadCommand(void);
extern void SendEndCommand(void);
#endif

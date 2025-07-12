/*****************************************************************
Name : common.h
Function : 定数宣言 (クライアント, サーバ用)
*****************************************************************/

#ifndef _COMMON_H_ // _COMMON_H_ 未定義時
#define _COMMON_H_ // _COMMON_H_ 定義 (値未設定)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include <math.h>
#define PORT (u_short)8888 // ポート番号
#define MAX_CLIENTS 4      // 最大クライアント数
#define MAX_NAME_SIZE 10   // ユーザ名最大長
#define MAX_DATA 300       // 送受信データ最大値
#define WINDOW_WIDTH 640   // ウィンドウ横幅
#define WINDOW_HEIGHT 960  // ウィンドウ縦幅
#define ITEMSIZE 30        // アイテムサイズ
#define METEOSIZE 45       // 隕石サイズ
#define BEAMSIZE 40        // ビームサイズ
#define BEAMSPEED 3        // ビーム速度
#define BEAMNUM 1000       // 最大ビーム数
#define END_COMMAND 'E'    // プログラム終了コマンド
#define UP_COMMAND 'U'     // 機体上移動コマンド
#define DOWN_COMMAND 'D'   // 機体下移動コマンド
#define LEFT_COMMAND 'L'   // 機体左移動コマンド
#define RIGHT_COMMAND 'R'  // 機体右移動コマンド
#define ITEM_COMMAND 'I'   // アイテム出現コマンド
#define METEO_COMMAND 'M'  // 隕石出現コマンド
#define SPACE_COMMAND 'S'  // ビーム発射コマンド
#define LOAD_COMMAND 'X'   // ビーム装填コマンド
#endif

/*****************************************************************
Name : client_win.c
Function : ユーザインタフェース処理 (クライアント)
*****************************************************************/

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "common.h"
#include "client_func.h"

static SDL_Window *gMainWindow;
static SDL_Renderer *gMainRenderer;
static SDL_Rect gCharaBackRect;
static SDL_Rect gCharaEarthARect;
static SDL_Rect gCharaEarthERect;
static SDL_Rect gCharaLoadRect;
static SDL_Rect gCharaAllyRect[MAX_CLIENTS];
static SDL_Rect gCharaEnemyRect[MAX_CLIENTS];
static SDL_Rect gCharaBeam0Rect[BEAMNUM];
static SDL_Rect gCharaBeam1Rect[BEAMNUM];
static SDL_Rect gCharaBeam2Rect[BEAMNUM];
static SDL_Rect gCharaBeam3Rect[BEAMNUM];
static SDL_Texture *textureBack;
static SDL_Texture *textureEarthA;
static SDL_Texture *textureEarthE;
static SDL_Texture *textureItem;
static SDL_Texture *textureLoad0;
static SDL_Texture *textureLoad1;
static SDL_Texture *textureLoad2;
static SDL_Texture *textureLoad3;
static SDL_Texture *textureMeteo;
static SDL_Texture *textureAlly;
static SDL_Texture *textureEnemy;
static SDL_Texture *textureBeam0;
static SDL_Texture *textureBeam1;
static SDL_Texture *textureBeam2;
static SDL_Texture *textureBeam3;
static SDL_Surface *imageBack;
static SDL_Surface *imageEarthA;
static SDL_Surface *imageEarthE;
static SDL_Surface *imageItem;
static SDL_Surface *imageLoad0;
static SDL_Surface *imageLoad1;
static SDL_Surface *imageLoad2;
static SDL_Surface *imageLoad3;
static SDL_Surface *imageMeteo;
static SDL_Surface *imageAlly;
static SDL_Surface *imageEnemy;
static SDL_Surface *imageBeam0;
static SDL_Surface *imageBeam1;
static SDL_Surface *imageBeam2;
static SDL_Surface *imageBeam3;
static SDL_Rect srcBack_rect;
static SDL_Rect srcEarthA_rect;
static SDL_Rect srcEarthE_rect;
static SDL_Rect srcItem_rect;
static SDL_Rect srcLoad0_rect;
static SDL_Rect srcLoad1_rect;
static SDL_Rect srcLoad2_rect;
static SDL_Rect srcLoad3_rect;
static SDL_Rect srcMeteo_rect;
static SDL_Rect srcAlly_rect;
static SDL_Rect srcEnemy_rect;
static SDL_Rect srcBeam0_rect;
static SDL_Rect srcBeam1_rect;
static SDL_Rect srcBeam2_rect;
static SDL_Rect srcBeam3_rect;
static SDL_Rect dstBack_rect;
static SDL_Rect dstEarthA_rect;
static SDL_Rect dstEarthE_rect;
static SDL_Rect dstItem_rect;
static SDL_Rect dstLoad0_rect;
static SDL_Rect dstLoad1_rect;
static SDL_Rect dstLoad2_rect;
static SDL_Rect dstLoad3_rect;
static SDL_Rect dstMeteo_rect;
static SDL_Rect dstHp_rect;
static SDL_Rect dstAlly_rect;
static SDL_Rect dstEnemy_rect;
static SDL_Rect dstBeam0_rect;
static SDL_Rect dstBeam1_rect;
static SDL_Rect dstBeam2_rect;
static SDL_Rect dstBeam3_rect;
static SDL_Surface *strings;
static SDL_Texture *texture;
static TTF_Font *font;
static SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};
static SDL_Rect src_rect;
static SDL_Rect dst_rect;
static Mix_Chunk *chunk1;
static Mix_Chunk *chunk2;
static Mix_Chunk *chunk3;
static Mix_Chunk *chunk4;
static Mix_Chunk *chunk5;
static Mix_Music *music;
static char charaAlly[] = "../data/chara_ally.png";
static char charaEnemy[] = "../data/chara_enemy.png";
static char charaBeam0[] = "../data/sozai.png";
static char charaBeam1[] = "../data/sozai.png";
static char charaBeam2[] = "../data/sozai.png";
static char charaBeam3[] = "../data/sozai.png";
static int timer0cnt;
static int timer1cnt;
static int timer2cnt;
static int timer3cnt;
static int timerIcnt;
static int timerMcnt;
static int beam0cnt;
static int beam1cnt;
static int beam2cnt;
static int beam3cnt;
static int cnt;
static int HP[4] = {3, 3, 3, 3};
static int load[4] = {0, 0, 0, 0};
static void Collision(int num);
static Uint32 BeamAnimation0(Uint32 interval, void *param);
static Uint32 BeamAnimation1(Uint32 interval, void *param);
static Uint32 BeamAnimation2(Uint32 interval, void *param);
static Uint32 BeamAnimation3(Uint32 interval, void *param);
static Uint32 ItemAnimation(Uint32 interval, void *param);
static Uint32 MeteoAnimation(Uint32 interval, void *param);

/*****************************************************************
Name : InitWindows
Function : メインウィンドウ表示, 設定
Parameters : int clientID : クライアント番号
			 int num : クライアント数
			 char name[][MAX_NAME_SIZE] : 全ユーザ名
Return : 失敗時 : -1
*****************************************************************/
int InitWindows(int clientID, int num, char name[][MAX_NAME_SIZE])
{
	char *s;		// 文字列格納用
	char title[10]; // ウィンドウタイトル格納用

	assert(0 < num && num <= MAX_CLIENTS); // 引数チェック

	// SDL 初期化
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // SDL 初期化失敗時
	{
		printf("failed to initialize SDL.\n");
		return -1;
	}

	// SDL_mixer 初期化 (MP3 ファイル使用)
	Mix_Init(MIX_INIT_MP3);

	// オーディオデバイス初期化
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) < 0) // オーディオデバイス初期化失敗時
	{
		printf("failed to initialize SDL_mixer.\n");
		return -1;
	}

	// サウンドファイル (BGM, 効果音) 読み込み
	if ((music = Mix_LoadMUS("../data/boss.mp3")) == NULL || (chunk1 = Mix_LoadWAV("../data/tap.mp3")) == NULL || (chunk2 = Mix_LoadWAV("../data/damage.mp3")) == NULL || (chunk3 = Mix_LoadWAV("../data/heal.mp3")) == NULL || (chunk4 = Mix_LoadWAV("../data/death.mp3")) == NULL || (chunk5 = Mix_LoadWAV("../data/gameset.mp3")) == NULL) // サウンドファイル (BGM, 効果音) 読み込み失敗時
	{
		printf("failed to load music and chunk.\n");
		Mix_CloseAudio();
		return -1;
	}

	// メインウィンドウ作成
	if ((gMainWindow = SDL_CreateWindow("My Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0)) == NULL) // メインウィンドウ作成失敗時
	{
		printf("failed to initialize videomode.\n");
		return -1;
	}

	// レンダラー作成
	gMainRenderer = SDL_CreateRenderer(gMainWindow, -1, SDL_RENDERER_SOFTWARE);

	// ウィンドウタイトルセット
	sprintf(title, "%dP", clientID + 1);
	SDL_SetWindowTitle(gMainWindow, title);

	// 背景設定 (白色)
	SDL_SetRenderDrawColor(gMainRenderer, 255, 255, 255, 255);
	SDL_RenderClear(gMainRenderer);

	// 背景作成
	imageBack = IMG_Load("../data/back.png");
	textureBack = SDL_CreateTextureFromSurface(gMainRenderer, imageBack);
	srcBack_rect = (SDL_Rect){0, 0, imageBack->w, imageBack->h};
	SDL_RenderCopy(gMainRenderer, textureBack, NULL, NULL);
	SDL_FreeSurface(imageBack);

	imageEarthA = IMG_Load("../data/earthA.png");
	textureEarthA = SDL_CreateTextureFromSurface(gMainRenderer, imageEarthA);
	srcEarthA_rect = (SDL_Rect){0, 0, imageEarthA->w, imageEarthA->h};
	SDL_RenderCopy(gMainRenderer, textureEarthA, NULL, NULL);
	SDL_FreeSurface(imageEarthA);

	imageEarthE = IMG_Load("../data/earthE.png");
	textureEarthE = SDL_CreateTextureFromSurface(gMainRenderer, imageEarthE);
	srcEarthE_rect = (SDL_Rect){0, 0, imageEarthE->w, imageEarthE->h};
	SDL_RenderCopy(gMainRenderer, textureEarthE, NULL, NULL);
	SDL_FreeSurface(imageEarthE);

	// HP 作成
	imageItem = IMG_Load("../data/item.png");
	textureItem = SDL_CreateTextureFromSurface(gMainRenderer, imageItem);
	srcItem_rect = (SDL_Rect){0, 0, imageItem->w, imageItem->h};
	dstItem_rect = (SDL_Rect){0, 0, 0, 0};
	SDL_RenderCopy(gMainRenderer, textureItem, &srcItem_rect, &dstItem_rect);
	SDL_FreeSurface(imageItem);

	// 弾数作成
	imageLoad0 = IMG_Load("../data/sozai.png");
	imageLoad1 = IMG_Load("../data/sozai.png");
	imageLoad2 = IMG_Load("../data/sozai.png");
	imageLoad3 = IMG_Load("../data/sozai.png");
	textureLoad0 = SDL_CreateTextureFromSurface(gMainRenderer, imageLoad0);
	textureLoad1 = SDL_CreateTextureFromSurface(gMainRenderer, imageLoad1);
	textureLoad2 = SDL_CreateTextureFromSurface(gMainRenderer, imageLoad2);
	textureLoad3 = SDL_CreateTextureFromSurface(gMainRenderer, imageLoad3);
	srcLoad0_rect = (SDL_Rect){8, 88, 12, 12};
	srcLoad1_rect = (SDL_Rect){8, 88, 12, 12};
	srcLoad2_rect = (SDL_Rect){8, 108, 12, 12};
	srcLoad3_rect = (SDL_Rect){8, 108, 12, 12};
	dstLoad0_rect = (SDL_Rect){0, 0, 0, 0};
	dstLoad1_rect = (SDL_Rect){0, 0, 0, 0};
	dstLoad2_rect = (SDL_Rect){0, 0, 0, 0};
	dstLoad3_rect = (SDL_Rect){0, 0, 0, 0};
	SDL_RenderCopy(gMainRenderer, textureLoad0, &srcLoad0_rect, &dstLoad0_rect);
	SDL_RenderCopy(gMainRenderer, textureLoad1, &srcLoad1_rect, &dstLoad1_rect);
	SDL_RenderCopy(gMainRenderer, textureLoad2, &srcLoad2_rect, &dstLoad2_rect);
	SDL_RenderCopy(gMainRenderer, textureLoad3, &srcLoad3_rect, &dstLoad3_rect);
	SDL_FreeSurface(imageLoad0);
	SDL_FreeSurface(imageLoad1);
	SDL_FreeSurface(imageLoad2);
	SDL_FreeSurface(imageLoad3);

	// 隕石作成
	imageMeteo = IMG_Load("../data/meteo.png");
	textureMeteo = SDL_CreateTextureFromSurface(gMainRenderer, imageMeteo);
	srcMeteo_rect = (SDL_Rect){0, 0, imageMeteo->w, imageMeteo->h};
	dstMeteo_rect = (SDL_Rect){0, 0, 0, 0};
	SDL_RenderCopy(gMainRenderer, textureMeteo, &srcMeteo_rect, &dstMeteo_rect);
	SDL_FreeSurface(imageMeteo);

	// テキスト画像作成
	TTF_Init();
	font = TTF_OpenFont("../data/FreeMono.ttf", 480);
	strings = TTF_RenderUTF8_Blended(font, "GAME SET", white);
	texture = SDL_CreateTextureFromSurface(gMainRenderer, strings);
	src_rect = (SDL_Rect){0, 0, strings->w, strings->h};
	dst_rect = (SDL_Rect){0, 0, 0, 0};
	SDL_RenderCopy(gMainRenderer, texture, &src_rect, &dst_rect);
	SDL_FreeSurface(strings);

	// 機体作成
	for (int i = 0; i < num; i++)
	{
		if (i < num / 2)
		{
			gCharaAllyRect[i].x = 160 + 240 * i;
			gCharaAllyRect[i].y = 80;
			gCharaAllyRect[i].w = 80;
			gCharaAllyRect[i].h = 80;

			s = charaAlly;
			imageAlly = IMG_Load(s);
			textureAlly = SDL_CreateTextureFromSurface(gMainRenderer, imageAlly);
			srcAlly_rect = (SDL_Rect){0, 0, imageAlly->w, imageAlly->h};
			SDL_RenderCopy(gMainRenderer, textureAlly, &srcAlly_rect, (&gCharaAllyRect[i]));
			SDL_FreeSurface(imageAlly);
		}
		else if (num == 4)
		{
			gCharaEnemyRect[i].x = 160 + 240 * (i - 2);
			gCharaEnemyRect[i].y = 800;
			gCharaEnemyRect[i].w = 80;
			gCharaEnemyRect[i].h = 80;

			s = charaEnemy;
			imageEnemy = IMG_Load(s);
			textureEnemy = SDL_CreateTextureFromSurface(gMainRenderer, imageEnemy);
			srcEnemy_rect = (SDL_Rect){0, 0, imageEnemy->w, imageEnemy->h};
			SDL_RenderCopy(gMainRenderer, textureEnemy, &srcEnemy_rect, (&gCharaEnemyRect[i]));
			SDL_FreeSurface(imageEnemy);
		}
		else
		{
			gCharaEnemyRect[i].x = 160 + 240 * i;
			gCharaEnemyRect[i].y = 800;
			gCharaEnemyRect[i].w = 80;
			gCharaEnemyRect[i].h = 80;

			s = charaEnemy;
			imageEnemy = IMG_Load(s);
			textureEnemy = SDL_CreateTextureFromSurface(gMainRenderer, imageEnemy);
			srcEnemy_rect = (SDL_Rect){0, 0, imageEnemy->w, imageEnemy->h};
			SDL_RenderCopy(gMainRenderer, textureEnemy, &srcEnemy_rect, (&gCharaEnemyRect[i]));
			SDL_FreeSurface(imageEnemy);
		}
	}

	// ビーム作成
	for (int i = 0; i < BEAMNUM; i++)
	{
		gCharaBeam0Rect[i].x = 1000;
		gCharaBeam0Rect[i].y = 1000;
		gCharaBeam0Rect[i].w = 0;
		gCharaBeam0Rect[i].h = 0;
		s = charaBeam0;
		imageBeam0 = IMG_Load(s);
		textureBeam0 = SDL_CreateTextureFromSurface(gMainRenderer, imageBeam0);
		srcBeam0_rect = (SDL_Rect){8, 88, 12, 12};
		SDL_RenderCopy(gMainRenderer, textureBeam0, &srcBeam0_rect, (&gCharaBeam0Rect[i]));
		SDL_FreeSurface(imageBeam0);

		gCharaBeam1Rect[i].x = 1000;
		gCharaBeam1Rect[i].y = 1000;
		gCharaBeam1Rect[i].w = 0;
		gCharaBeam1Rect[i].h = 0;
		s = charaBeam1;
		imageBeam1 = IMG_Load(s);
		textureBeam1 = SDL_CreateTextureFromSurface(gMainRenderer, imageBeam1);
		srcBeam1_rect = (SDL_Rect){8, 88, 12, 12};
		SDL_RenderCopy(gMainRenderer, textureBeam1, &srcBeam1_rect, (&gCharaBeam1Rect[i]));
		SDL_FreeSurface(imageBeam1);

		gCharaBeam2Rect[i].x = 1000;
		gCharaBeam2Rect[i].y = 1000;
		gCharaBeam2Rect[i].w = 0;
		gCharaBeam2Rect[i].h = 0;
		s = charaBeam2;
		imageBeam2 = IMG_Load(s);
		textureBeam2 = SDL_CreateTextureFromSurface(gMainRenderer, imageBeam2);
		srcBeam2_rect = (SDL_Rect){8, 108, 12, 12};
		SDL_RenderCopy(gMainRenderer, textureBeam2, &srcBeam2_rect, (&gCharaBeam2Rect[i]));
		SDL_FreeSurface(imageBeam2);

		gCharaBeam3Rect[i].x = 1000;
		gCharaBeam3Rect[i].y = 1000;
		gCharaBeam3Rect[i].w = 0;
		gCharaBeam3Rect[i].h = 0;
		s = charaBeam3;
		imageBeam3 = IMG_Load(s);
		textureBeam3 = SDL_CreateTextureFromSurface(gMainRenderer, imageBeam3);
		srcBeam3_rect = (SDL_Rect){8, 108, 12, 12};
		SDL_RenderCopy(gMainRenderer, textureBeam3, &srcBeam3_rect, (&gCharaBeam3Rect[i]));
		SDL_FreeSurface(imageBeam3);
	}

	Mix_PlayMusic(music, -1);
}

/*****************************************************************
Name : Update
Function : 描画更新
Parameters : int num : クライアント数
			 int clientID : クライアント番号
*****************************************************************/
void Update(int num, int clientID)
{
	SDL_RenderClear(gMainRenderer);
	SDL_RenderCopy(gMainRenderer, textureBack, NULL, NULL);
	SDL_RenderCopy(gMainRenderer, textureEarthA, NULL, NULL);
	SDL_RenderCopy(gMainRenderer, textureEarthE, NULL, NULL);
	SDL_RenderCopy(gMainRenderer, textureItem, &srcItem_rect, &dstItem_rect);
	SDL_RenderCopy(gMainRenderer, textureMeteo, &srcMeteo_rect, &dstMeteo_rect);

	for (int i = 0; i < num; i++)
	{
		if (i < num / 2)
			SDL_RenderCopy(gMainRenderer, textureAlly, &srcAlly_rect, (&gCharaAllyRect[i]));
		else
			SDL_RenderCopy(gMainRenderer, textureEnemy, &srcEnemy_rect, (&gCharaEnemyRect[i]));
	}

	for (int i = 0; i < BEAMNUM; i++)
	{
		SDL_RenderCopy(gMainRenderer, textureBeam0, &srcBeam0_rect, (&gCharaBeam0Rect[i]));
		SDL_RenderCopy(gMainRenderer, textureBeam1, &srcBeam1_rect, (&gCharaBeam1Rect[i]));
		SDL_RenderCopy(gMainRenderer, textureBeam2, &srcBeam2_rect, (&gCharaBeam2Rect[i]));
		SDL_RenderCopy(gMainRenderer, textureBeam3, &srcBeam3_rect, (&gCharaBeam3Rect[i]));
	}

	boxColor(gMainRenderer, 0, 475, 640, 485, 0xff00ff00);

	if (clientID == 0)
	{
		for (int i = 0; i < HP[clientID]; i++)
		{
			dstHp_rect = (SDL_Rect){30 * i, 440, 30, 30};
			SDL_RenderCopy(gMainRenderer, textureItem, &srcItem_rect, &dstHp_rect);
		}
	}

	if (clientID == 1)
	{
		for (int i = 0; i < HP[clientID]; i++)
		{
			dstHp_rect = (SDL_Rect){30 * i, 440, 30, 30};
			SDL_RenderCopy(gMainRenderer, textureItem, &srcItem_rect, &dstHp_rect);
		}
	}

	if (clientID == 2)
	{
		for (int i = 0; i < HP[clientID]; i++)
		{
			dstHp_rect = (SDL_Rect){30 * i, 490, 30, 30};
			SDL_RenderCopy(gMainRenderer, textureItem, &srcItem_rect, &dstHp_rect);
		}
	}

	if (clientID == 3)
	{
		for (int i = 0; i < HP[clientID]; i++)
		{
			dstHp_rect = (SDL_Rect){30 * i, 490, 30, 30};
			SDL_RenderCopy(gMainRenderer, textureItem, &srcItem_rect, &dstHp_rect);
		}
	}

	if (clientID == 0)
	{
		for (int i = 0; i < load[0] - 1; i++)
		{
			dstLoad0_rect = (SDL_Rect){30 * (i / 2), 410, 30, 30};
			SDL_RenderCopy(gMainRenderer, textureLoad0, &srcLoad0_rect, &dstLoad0_rect);
		}
	}

	if (clientID == 1)
	{
		for (int i = 0; i < load[1] - 1; i++)
		{
			if (load[1])
			{
				dstLoad1_rect = (SDL_Rect){30 * (i / 2), 410, 30, 30};
				SDL_RenderCopy(gMainRenderer, textureLoad1, &srcLoad1_rect, &dstLoad1_rect);
			}
		}
	}

	if (clientID == 2)
	{
		for (int i = 0; i < load[2] - 1; i++)
		{
			if (load[2])
			{
				dstLoad2_rect = (SDL_Rect){30 * (i / 2), 520, 30, 30};
				SDL_RenderCopy(gMainRenderer, textureLoad2, &srcLoad2_rect, &dstLoad2_rect);
			}
		}
	}

	if (clientID == 3)
	{
		for (int i = 0; i < load[3] - 1; i++)
		{
			if (load[3])
			{
				dstLoad3_rect = (SDL_Rect){30 * (i / 2), 520, 30, 30};
				SDL_RenderCopy(gMainRenderer, textureLoad3, &srcLoad3_rect, &dstLoad3_rect);
			}
		}
	}

	SDL_RenderCopy(gMainRenderer, texture, &src_rect, &dst_rect);
	SDL_RenderPresent(gMainRenderer);
}

/*****************************************************************
Name : DestroyWindow
Parameter : SDL 終了
*****************************************************************/
void DestroyWindow(void)
{
	SDL_Quit();
}

/*****************************************************************
Name : WindowEvent
Function : メインウィンドウイベント処理
Parameters : int num : クライアント数
			 int clientID : クライアント番号
*****************************************************************/
void WindowEvent(int num, int clientID)
{
	SDL_Event event;

	assert(0 < num && num <= MAX_CLIENTS); // 引数チェック

	Collision(num);		   // 当たり判定
	Update(num, clientID); // 描画更新

	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			SendEndCommand();
			break;

		case SDL_KEYDOWN:
			if (event.key.repeat)
				break;

			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				SendEndCommand();
				break;

			case SDLK_UP:
				SendUpCommand();
				break;

			case SDLK_DOWN:
				SendDownCommand();
				break;

			case SDLK_LEFT:
				SendLeftCommand();
				break;

			case SDLK_RIGHT:
				SendRightCommand();
				break;

			case SDLK_SPACE:
				SendBeamCommand();

				if (load[clientID] > 0)
					Mix_PlayChannel(1, chunk1, 0);
				break;

			case SDLK_TAB:
				SendLoadCommand();
				break;
			}
			break;
		}
	}
}

/*****************************************************************
Name : MoveUp
Function : 対象機体上移動
Parameters : int y : 移動量 (y 座標)
			 int clientID : クライアント番号
*****************************************************************/
void MoveUp(int y, int clientID)
{
	if (clientID < 2)
	{
		if (gCharaAllyRect[clientID].y > 0)
		{
			gCharaAllyRect[clientID].y -= y;
		}
	}
	else
	{
		if (gCharaEnemyRect[clientID].y > 485)
		{
			gCharaEnemyRect[clientID].y -= y;
		}
	}
}

/*****************************************************************
Name : MoveDown
Function : 対象機体下移動
Parameters : int y : 移動量 (y 座標)
			 int clientID : クライアント番号
*****************************************************************/
void MoveDown(int y, int clientID)
{
	if (clientID < 2)
	{
		if (gCharaAllyRect[clientID].y + 80 < WINDOW_HEIGHT / 2)
		{
			gCharaAllyRect[clientID].y += y;
		}
	}
	else
	{
		if (gCharaEnemyRect[clientID].y + 80 < WINDOW_HEIGHT)
		{
			gCharaEnemyRect[clientID].y += y;
		}
	}
}

/*****************************************************************
Name : MoveLeft
Function : 対象機体左移動
Parameters : int x : 移動量 (x 座標)
			 int clientID : クライアント番号
*****************************************************************/
void MoveLeft(int x, int clientID)
{
	if (clientID < 2)
	{
		if (gCharaAllyRect[clientID].x > 0)
		{
			gCharaAllyRect[clientID].x -= x;
		}
	}
	else
	{
		if (gCharaEnemyRect[clientID].x > 0)
		{
			gCharaEnemyRect[clientID].x -= x;
		}
	}
}

/*****************************************************************
Name : MoveRight
Function : 対象機体右移動
Parameters : int x : 移動量 (x 座標)
			 int clientID : クライアント番号
*****************************************************************/
void MoveRight(int x, int clientID)
{
	if (clientID < 2)
	{
		if (gCharaAllyRect[clientID].x + 80 < WINDOW_WIDTH)
		{
			gCharaAllyRect[clientID].x += x;
		}
	}
	else
	{
		if (gCharaEnemyRect[clientID].x + 80 < WINDOW_WIDTH)
		{
			gCharaEnemyRect[clientID].x += x;
		}
	}
}

/*****************************************************************
Name : MoveItem
Function : アイテム出現
Parameter : int y : 出現座標 (y 座標)
*****************************************************************/
void MoveItem(int y)
{
	timerIcnt++;

	if (timerIcnt == 1)
		SDL_AddTimer(20, ItemAnimation, NULL); // アイテム移動

	if (gCharaBeam0Rect[0].h == 0)
	{
		dstItem_rect.x = 700;
		dstItem_rect.y = y;
		dstItem_rect.w = ITEMSIZE;
		dstItem_rect.h = ITEMSIZE;
	}
}

/*****************************************************************
Name : MoveMeteo
Function : 隕石出現
Parameter : int y : 出現座標 (y 座標)
*****************************************************************/
void MoveMeteo(int y)
{
	timerMcnt++;

	if (timerMcnt == 1)
		SDL_AddTimer(5, MeteoAnimation, NULL); // 隕石移動

	dstMeteo_rect.x = -50;
	dstMeteo_rect.y = y;
	dstMeteo_rect.w = METEOSIZE;
	dstMeteo_rect.h = METEOSIZE;
}

/*****************************************************************
Name : MoveBeam
Function : ビーム発射
Parameter : int clientID : クライアント番号
*****************************************************************/
void MoveBeam(int clientID)
{
	if (beam0cnt == BEAMNUM)
		beam0cnt = 0;

	if (clientID == 0 && load[0] > 1)
	{
		timer0cnt++;

		if (timer0cnt == 1)
			SDL_AddTimer(BEAMSPEED, BeamAnimation0, NULL); // ビーム移動

		gCharaBeam0Rect[beam0cnt].x = gCharaAllyRect[0].x + 20;
		gCharaBeam0Rect[beam0cnt].y = gCharaAllyRect[0].y + 80;
		gCharaBeam0Rect[beam0cnt].w = BEAMSIZE;
		gCharaBeam0Rect[beam0cnt].h = BEAMSIZE;
		beam0cnt++;

		if (load[0] % 2 == 1 && load[0] != 1)
			load[0] -= 3;
		else if (load[0] == 1)
			load[0] -= 1;
		else
			load[0] -= 2;
	}
	else if (clientID == 1 && load[1] > 1)
	{
		timer1cnt++;

		if (timer1cnt == 1)
			SDL_AddTimer(BEAMSPEED, BeamAnimation1, NULL); // ビーム移動

		gCharaBeam1Rect[beam1cnt].x = gCharaAllyRect[1].x + 20;
		gCharaBeam1Rect[beam1cnt].y = gCharaAllyRect[1].y + 80;
		gCharaBeam1Rect[beam1cnt].w = BEAMSIZE;
		gCharaBeam1Rect[beam1cnt].h = BEAMSIZE;
		beam1cnt++;

		if (load[1] % 2 == 1 && load[1] != 1)
			load[1] -= 3;
		else if (load[1] == 1)
			load[1] -= 1;
		else
			load[1] -= 2;
	}
	else if (clientID == 2 && load[2] > 1)
	{
		timer2cnt++;

		if (timer2cnt == 1)
			SDL_AddTimer(BEAMSPEED, BeamAnimation2, NULL); // ビーム移動

		gCharaBeam2Rect[beam2cnt].x = gCharaEnemyRect[2].x + 20;
		gCharaBeam2Rect[beam2cnt].y = gCharaEnemyRect[2].y - 40;
		gCharaBeam2Rect[beam2cnt].w = BEAMSIZE;
		gCharaBeam2Rect[beam2cnt].h = BEAMSIZE;
		beam2cnt++;

		if (load[2] % 2 == 1 && load[2] != 1)
			load[2] -= 3;
		else if (load[2] == 1)
			load[2] -= 1;
		else
			load[2] -= 2;
	}
	else if (clientID == 3 && load[3] > 1)
	{
		timer3cnt++;

		if (timer3cnt == 1)
			SDL_AddTimer(BEAMSPEED, BeamAnimation3, NULL); // ビーム移動

		gCharaBeam3Rect[beam3cnt].x = gCharaEnemyRect[3].x + 20;
		gCharaBeam3Rect[beam3cnt].y = gCharaEnemyRect[3].y - 40;
		gCharaBeam3Rect[beam3cnt].w = BEAMSIZE;
		gCharaBeam3Rect[beam3cnt].h = BEAMSIZE;
		beam3cnt++;

		if (load[3] % 2 == 1 && load[3] != 1)
			load[3] -= 3;
		else if (load[3] == 1)
			load[3] -= 1;
		else
			load[3] -= 2;
	}
}

/*****************************************************************
Name : MoveLoad
Function : ビーム装填
Parameter : int clientID : クライアント番号
*****************************************************************/
void MoveLoad(int clientID)
{
	if (clientID == 0 && load[0] < 10)
		load[0]++;
	if (clientID == 1 && load[1] < 10)
		load[1]++;
	if (clientID == 2 && load[2] < 10)
		load[2]++;
	if (clientID == 3 && load[3] < 10)
		load[3]++;
}

/*****************************************************************
Name : Collision
Function : 当たり判定
Parameter : int num : クライアント数
*****************************************************************/
void Collision(int num)
{
	for (int i = 0; i < BEAMNUM; i++)
	{
		for (int j = 2; j < num; j++)
		{
			if (gCharaBeam0Rect[i].x + BEAMSIZE - 10 > gCharaEnemyRect[j].x && gCharaBeam0Rect[i].x < gCharaEnemyRect[j].x + 65 && gCharaBeam0Rect[i].y + BEAMSIZE - 37 > gCharaEnemyRect[j].y && gCharaBeam0Rect[i].y < gCharaEnemyRect[j].y + 65)
			{
				gCharaBeam0Rect[i].x = 1000;
				gCharaBeam0Rect[i].y = 1000;
				gCharaBeam0Rect[i].w = 0;
				gCharaBeam0Rect[i].h = 0;

				if (HP[j] == 1)
				{
					HP[j]--;
					Mix_PlayChannel(4, chunk4, 0);
				}
				else if (HP[j] > 0 || HP[j] != 1)
				{
					HP[j]--;
					Mix_PlayChannel(2, chunk2, 0);
				}
			}
			if (gCharaBeam1Rect[i].x + BEAMSIZE - 10 > gCharaEnemyRect[j].x && gCharaBeam1Rect[i].x < gCharaEnemyRect[j].x + 65 && gCharaBeam1Rect[i].y + BEAMSIZE - 37 > gCharaEnemyRect[j].y && gCharaBeam1Rect[i].y < gCharaEnemyRect[j].y + 65)
			{
				gCharaBeam1Rect[i].x = 1000;
				gCharaBeam1Rect[i].y = 1000;
				gCharaBeam1Rect[i].w = 0;
				gCharaBeam1Rect[i].h = 0;

				if (HP[j] == 1)
				{
					HP[j]--;
					Mix_PlayChannel(4, chunk4, 0);
				}
				else if (HP[j] > 0 || HP[j] != 1)
				{
					HP[j]--;
					Mix_PlayChannel(2, chunk2, 0);
				}
			}
			if (gCharaBeam0Rect[i].x + BEAMSIZE - 30 > gCharaEnemyRect[j].x && gCharaBeam0Rect[i].x < gCharaEnemyRect[j].x + 45 && gCharaBeam0Rect[i].y + BEAMSIZE - 10 > gCharaEnemyRect[j].y && gCharaBeam0Rect[i].y < gCharaEnemyRect[j].y + 65)
			{
				gCharaBeam0Rect[i].x = 1000;
				gCharaBeam0Rect[i].y = 1000;
				gCharaBeam0Rect[i].w = 0;
				gCharaBeam0Rect[i].h = 0;

				if (HP[j] == 1)
				{
					HP[j]--;
					Mix_PlayChannel(4, chunk4, 0);
				}
				else if (HP[j] > 0 || HP[j] != 1)
				{
					HP[j]--;
					Mix_PlayChannel(2, chunk2, 0);
				}
			}
			if (gCharaBeam1Rect[1].x + BEAMSIZE - 30 > gCharaEnemyRect[j].x && gCharaBeam1Rect[i].x < gCharaEnemyRect[j].x + 45 && gCharaBeam1Rect[i].y + BEAMSIZE - 10 > gCharaEnemyRect[j].y && gCharaBeam1Rect[i].y < gCharaEnemyRect[j].y + 65)
			{
				gCharaBeam1Rect[i].x = 1000;
				gCharaBeam1Rect[i].y = 1000;
				gCharaBeam1Rect[i].w = 0;
				gCharaBeam1Rect[i].h = 0;

				if (HP[j] == 1)
				{
					HP[j]--;
					Mix_PlayChannel(4, chunk4, 0);
				}
				else if (HP[j] > 0 || HP[j] != 1)
				{
					HP[j]--;
					Mix_PlayChannel(2, chunk2, 0);
				}
			}
			if (HP[j] == 0)
			{
				gCharaEnemyRect[j].x = -1000;
				gCharaEnemyRect[j].y = -1000;
				gCharaEnemyRect[j].w = 0;
				gCharaEnemyRect[j].h = 0;
			}
		}
	}

	for (int i = 0; i < BEAMNUM; i++)
	{
		for (int j = 0; j < num / 2; j++)
		{
			if (gCharaBeam2Rect[i].x + BEAMSIZE - 10 > gCharaAllyRect[j].x && gCharaBeam2Rect[i].x < gCharaAllyRect[j].x + 65 && gCharaBeam2Rect[i].y + BEAMSIZE - 10 > gCharaAllyRect[j].y && gCharaBeam2Rect[i].y < gCharaAllyRect[j].y + 46)
			{
				gCharaBeam2Rect[i].x = 1000;
				gCharaBeam2Rect[i].y = 1000;
				gCharaBeam2Rect[i].w = 0;
				gCharaBeam2Rect[i].h = 0;

				if (HP[j] == 1)
				{
					HP[j]--;
					Mix_PlayChannel(4, chunk4, 0);
				}
				else if (HP[j] > 0 || HP[j] != 1)
				{
					HP[j]--;
					Mix_PlayChannel(2, chunk2, 0);
				}
			}
			if (gCharaBeam3Rect[i].x + BEAMSIZE - 10 > gCharaAllyRect[j].x && gCharaBeam3Rect[i].x < gCharaAllyRect[j].x + 65 && gCharaBeam3Rect[i].y + BEAMSIZE - 10 > gCharaAllyRect[j].y && gCharaBeam3Rect[i].y < gCharaAllyRect[j].y + 46)
			{
				gCharaBeam3Rect[i].x = 1000;
				gCharaBeam3Rect[i].y = 1000;
				gCharaBeam3Rect[i].w = 0;
				gCharaBeam3Rect[i].h = 0;

				if (HP[j] == 1)
				{
					HP[j]--;
					Mix_PlayChannel(4, chunk4, 0);
				}
				else if (HP[j] > 0 || HP[j] != 1)
				{
					HP[j]--;
					Mix_PlayChannel(2, chunk2, 0);
				}
			}
			if (gCharaBeam2Rect[i].x + BEAMSIZE - 30 > gCharaAllyRect[j].x && gCharaBeam2Rect[i].x < gCharaAllyRect[j].x + 45 && gCharaBeam2Rect[i].y + BEAMSIZE - 10 > gCharaAllyRect[j].y && gCharaBeam2Rect[i].y < gCharaAllyRect[j].y + 65)
			{
				gCharaBeam2Rect[i].x = 1000;
				gCharaBeam2Rect[i].y = 1000;
				gCharaBeam2Rect[i].w = 0;
				gCharaBeam2Rect[i].h = 0;

				if (HP[j] == 1)
				{
					HP[j]--;
					Mix_PlayChannel(4, chunk4, 0);
				}
				else if (HP[j] > 0 || HP[j] != 1)
				{
					HP[j]--;
					Mix_PlayChannel(2, chunk2, 0);
				}
			}
			if (gCharaBeam3Rect[i].x + BEAMSIZE - 30 > gCharaAllyRect[j].x && gCharaBeam3Rect[i].x < gCharaAllyRect[j].x + 45 && gCharaBeam3Rect[i].y + BEAMSIZE - 10 > gCharaAllyRect[j].y && gCharaBeam3Rect[i].y < gCharaAllyRect[j].y + 65)
			{
				gCharaBeam3Rect[i].x = 1000;
				gCharaBeam3Rect[i].y = 1000;
				gCharaBeam3Rect[i].w = 0;
				gCharaBeam3Rect[i].h = 0;

				if (HP[j] == 1)
				{
					HP[j]--;
					Mix_PlayChannel(4, chunk4, 0);
				}
				else if (HP[j] > 0 || HP[j] != 1)
				{
					HP[j]--;
					Mix_PlayChannel(2, chunk2, 0);
				}
			}
			if (HP[j] == 0)
			{
				gCharaAllyRect[j].x = -1000;
				gCharaAllyRect[j].y = -1000;
				gCharaAllyRect[j].w = 0;
				gCharaAllyRect[j].h = 0;
			}
		}
	}

	for (int i = 0; i < num / 2; i++)
	{
		if (dstItem_rect.x + ITEMSIZE - 10 > gCharaAllyRect[i].x && dstItem_rect.x < gCharaAllyRect[i].x + 65 && dstItem_rect.y + ITEMSIZE - 10 > gCharaAllyRect[i].y && dstItem_rect.y < gCharaAllyRect[i].y + 46)
		{
			dstItem_rect.x = 1000;
			dstItem_rect.y = -1000;
			dstItem_rect.w = 0;
			dstItem_rect.h = 0;

			if (HP[i] < 6)
			{
				HP[i]++;
				Mix_PlayChannel(3, chunk3, 0);
			}
		}
		if (dstItem_rect.x + ITEMSIZE - 30 > gCharaAllyRect[i].x && dstItem_rect.x < gCharaAllyRect[i].x + 45 && dstItem_rect.y + ITEMSIZE - 10 > gCharaAllyRect[i].y && dstItem_rect.y < gCharaAllyRect[i].y + 65)
		{
			dstItem_rect.x = 1000;
			dstItem_rect.y = -1000;
			dstItem_rect.w = 0;
			dstItem_rect.h = 0;

			if (HP[i] < 6)
			{
				HP[i]++;
				Mix_PlayChannel(3, chunk3, 0);
			}
		}
	}
	for (int i = 2; i < num; i++)
	{
		if (dstItem_rect.x + ITEMSIZE - 10 > gCharaEnemyRect[i].x && dstItem_rect.x < gCharaEnemyRect[i].x + 65 && dstItem_rect.y + ITEMSIZE - 37 > gCharaEnemyRect[i].y && dstItem_rect.y < gCharaEnemyRect[i].y + 65)
		{
			dstItem_rect.x = 1000;
			dstItem_rect.y = -1000;
			dstItem_rect.w = 0;
			dstItem_rect.h = 0;

			if (HP[i] < 6)
			{
				HP[i]++;
				Mix_PlayChannel(3, chunk3, 0);
			}
		}
		if (dstItem_rect.x + ITEMSIZE - 30 > gCharaEnemyRect[i].x && dstItem_rect.x < gCharaEnemyRect[i].x + 45 && dstItem_rect.y + ITEMSIZE - 10 > gCharaEnemyRect[i].y && dstItem_rect.y < gCharaEnemyRect[i].y + 65)
		{
			dstItem_rect.x = 1000;
			dstItem_rect.y = -1000;
			dstItem_rect.w = 0;
			dstItem_rect.h = 0;

			if (HP[i] < 6)
			{
				HP[i]++;
				Mix_PlayChannel(3, chunk3, 0);
			}
		}
	}

	for (int i = 0; i < num / 2; i++)
	{
		if (dstMeteo_rect.x + ITEMSIZE - 10 > gCharaAllyRect[i].x && dstMeteo_rect.x < gCharaAllyRect[i].x + 65 && dstMeteo_rect.y + ITEMSIZE - 10 > gCharaAllyRect[i].y && dstMeteo_rect.y < gCharaAllyRect[i].y + 46)
		{
			dstMeteo_rect.x = -1000;
			dstMeteo_rect.y = 1000;
			dstMeteo_rect.w = 0;
			dstMeteo_rect.h = 0;

			if (HP[i] == 1)
			{
				HP[i]--;
				Mix_PlayChannel(4, chunk4, 0);
			}
			else if (HP[i] > 0 || HP[i] != 1)
			{
				HP[i]--;
				Mix_PlayChannel(2, chunk2, 0);
			}
		}
		if (dstMeteo_rect.x + ITEMSIZE - 30 > gCharaAllyRect[i].x && dstMeteo_rect.x < gCharaAllyRect[i].x + 45 && dstMeteo_rect.y + ITEMSIZE - 10 > gCharaAllyRect[i].y && dstMeteo_rect.y < gCharaAllyRect[i].y + 65)
		{
			dstMeteo_rect.x = -1000;
			dstMeteo_rect.y = 1000;
			dstMeteo_rect.w = 0;
			dstMeteo_rect.h = 0;

			if (HP[i] == 1)
			{
				HP[i]--;
				Mix_PlayChannel(4, chunk4, 0);
			}
			else if (HP[i] > 0 || HP[i] != 1)
			{
				HP[i]--;
				Mix_PlayChannel(2, chunk2, 0);
			}
		}
	}
	for (int i = 2; i < num; i++)
	{
		if (dstMeteo_rect.x + ITEMSIZE - 10 > gCharaEnemyRect[i].x && dstMeteo_rect.x < gCharaEnemyRect[i].x + 65 && dstMeteo_rect.y + ITEMSIZE - 37 > gCharaEnemyRect[i].y && dstMeteo_rect.y < gCharaEnemyRect[i].y + 65)
		{
			dstMeteo_rect.x = -1000;
			dstMeteo_rect.y = 1000;
			dstMeteo_rect.w = 0;
			dstMeteo_rect.h = 0;

			if (HP[i] == 1)
			{
				HP[i]--;
				Mix_PlayChannel(4, chunk4, 0);
			}
			else if (HP[i] > 0 || HP[i] != 1)
			{
				HP[i]--;
				Mix_PlayChannel(2, chunk2, 0);
			}
		}
		if (dstMeteo_rect.x + ITEMSIZE - 30 > gCharaEnemyRect[i].x && dstMeteo_rect.x < gCharaEnemyRect[i].x + 45 && dstMeteo_rect.y + ITEMSIZE - 10 > gCharaEnemyRect[i].y && dstMeteo_rect.y < gCharaEnemyRect[i].y + 65)
		{
			dstMeteo_rect.x = -1000;
			dstMeteo_rect.y = 1000;
			dstMeteo_rect.w = 0;
			dstMeteo_rect.h = 0;

			if (HP[i] == 1)
			{
				HP[i]--;
				Mix_PlayChannel(4, chunk4, 0);
			}
			else if (HP[i] > 0 || HP[i] != 1)
			{
				HP[i]--;
				Mix_PlayChannel(2, chunk2, 0);
			}
		}
	}

	if (((gCharaAllyRect[0].h == 0 && gCharaAllyRect[1].h == 0) || (gCharaEnemyRect[2].h == 0 && gCharaEnemyRect[3].h == 0)) && cnt == 0)
	{
		dst_rect.x = 120;
		dst_rect.y = 355;
		dst_rect.w = 400;
		dst_rect.h = 250;
		Mix_HaltChannel(1);
		Mix_HaltChannel(2);
		Mix_HaltChannel(3);
		Mix_HaltChannel(4);
		Mix_HaltMusic();
		Mix_PlayChannel(5, chunk5, 0);
		cnt++;
	}

	SDL_RenderPresent(gMainRenderer);
}

/*****************************************************************
Name : ItemAnimation
Function : 割り込み関数 (アイテム移動)
Parameters : Uint32 interval : 割り込み間隔
			 void *param : 割り込み関数用引数
Return : interval : 割り込み間隔
*****************************************************************/
static Uint32 ItemAnimation(Uint32 interval, void *param)
{
	dstItem_rect.x -= 1;

	if (dstItem_rect.x + ITEMSIZE < 0)
	{
		dstItem_rect.x = 1000;
		dstItem_rect.y = -1000;
		dstItem_rect.w = 0;
		dstItem_rect.h = 0;
	}

	return interval;
}

/*****************************************************************
Name : MeteoAnimation
Function : 割り込み関数 (隕石移動)
Parameters : Uint32 interval : 割り込み間隔
			 void *param : 割り込み関数用引数
Return : interval : 割り込み間隔
*****************************************************************/
static Uint32 MeteoAnimation(Uint32 interval, void *param)
{
	dstMeteo_rect.x += 1;

	if (dstMeteo_rect.x > 650)
	{
		dstMeteo_rect.x = -1000;
		dstMeteo_rect.y = 1000;
		dstMeteo_rect.w = 0;
		dstMeteo_rect.h = 0;
	}

	return interval;
}

/*****************************************************************
Name : BeamAnimation0
Function : 割り込み関数 (ビーム移動)
Parameters : Uint32 interval : 割り込み間隔
			 void *param : 割り込み関数用引数
Return : interval : 割り込み間隔
*****************************************************************/
static Uint32 BeamAnimation0(Uint32 interval, void *param)
{
	for (int i = 0; i < beam0cnt + 1; i++)
	{
		gCharaBeam0Rect[i].y += 1;

		if (gCharaBeam0Rect[i].y > WINDOW_HEIGHT)
		{
			gCharaBeam0Rect[i].x = 1000;
			gCharaBeam0Rect[i].y = 1000;
			gCharaBeam0Rect[i].w = 0;
			gCharaBeam0Rect[i].h = 0;
		}
	}

	return interval;
}

/*****************************************************************
Name : BeamAnimation1
Function : 割り込み関数 (ビーム移動)
Parameters : Uint32 interval : 割り込み間隔
			 void *param : 割り込み関数用引数
Return : interval : 割り込み間隔
*****************************************************************/
static Uint32 BeamAnimation1(Uint32 interval, void *param)
{
	for (int i = 0; i < beam1cnt + 1; i++)
	{
		gCharaBeam1Rect[i].y += 1;

		if (gCharaBeam1Rect[i].y > WINDOW_HEIGHT)
		{
			gCharaBeam1Rect[i].x = 1000;
			gCharaBeam1Rect[i].y = 1000;
			gCharaBeam1Rect[i].w = 0;
			gCharaBeam1Rect[i].h = 0;
		}
	}

	return interval;
}

/*****************************************************************
Name : BeamAnimation2
Function : 割り込み関数 (ビーム移動)
Parameters : Uint32 interval : 割り込み間隔
			 void *param : 割り込み関数用引数
Return : interval : 割り込み間隔
*****************************************************************/
static Uint32 BeamAnimation2(Uint32 interval, void *param)
{
	for (int i = 0; i < beam2cnt + 1; i++)
	{
		gCharaBeam2Rect[i].y -= 1;

		if (gCharaBeam2Rect[i].y + 40 < 0)
		{
			gCharaBeam2Rect[i].x = 1000;
			gCharaBeam2Rect[i].y = 1000;
			gCharaBeam2Rect[i].w = 0;
			gCharaBeam2Rect[i].h = 0;
		}
	}

	return interval;
}

/*****************************************************************
Name : BeamAnimation3
Function : 割り込み関数 (ビーム移動)
Parameters : Uint32 interval : 割り込み間隔
			 void *param : 割り込み関数用引数
Return : interval : 割り込み間隔
*****************************************************************/
static Uint32 BeamAnimation3(Uint32 interval, void *param)
{
	for (int i = 0; i < beam3cnt + 1; i++)
	{
		gCharaBeam3Rect[i].y -= 1;

		if (gCharaBeam3Rect[i].y + 40 < 0)
		{
			gCharaBeam3Rect[i].x = 1000;
			gCharaBeam3Rect[i].y = 1000;
			gCharaBeam3Rect[i].w = 0;
			gCharaBeam3Rect[i].h = 0;
		}
	}

	return interval;
}

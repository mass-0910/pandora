#include <DxLib.h>
#include "define.h"
#include "note.h"
#include "event.h"
#include "struct.h"
#include "menu.h"
#include "input.h"
#include "battleScene.h"
#include <stdio.h>

int font_mes;
bool initflag = true;

exception initialize();

void walkMainInit(char *mapname, int& mapmaxX, int& mapmaxY, MAP map[MAPMAX + 2][MAPMAX + 2], EventManager& em, int *music_handle, int *chipdata, int& encountflag, int& encountwalknum){

    char tmpstr[64];
    char bgmname[64];
    char line[1024];

    //マップ読み込み
    Note mapbuf;
    sprintf(tmpstr, "data/%s.map", mapname);
    if(mapbuf.noteload(tmpstr) == -1){
        printf("%s not found.\n", tmpstr);
    }
    //mapbuf.print_note();
    getstr(tmpstr, mapbuf.noteget(0), 0);
    mapmaxX = atoi(tmpstr);
    getstr(tmpstr, mapbuf.noteget(0), 1);
    mapmaxY = atoi(tmpstr);
    strncpy(bgmname, mapbuf.noteget(1), 64);
    for(int i = 0; i < MAPMAX; i++){
        strncpy(line, mapbuf.noteget(i + 2), 1024);
        for(int j = 0; j < MAPMAX; j++){
            getstr(tmpstr, line, j);
            map[j][i].chipID = atoi(tmpstr);
        }
    }
    puts("\tマップ読み込み完了");

    //マップのBGMの読み込み
    sprintf(tmpstr, "data/%s", bgmname);
    if(music_handle[1] != -1)DeleteSoundMem(music_handle[1]);
    music_handle[1] = LoadSoundMem(tmpstr);
    PlaySoundMem(music_handle[1], DX_PLAYTYPE_LOOP);
    puts("\tマップのBGMの読み込み完了");

    //マップの歩行可能データの読み込み
    for(int i = 0; i < MAPMAX; i++){
        for(int j = 0; j < MAPMAX; j++){
            map[j][i].mapdata = chipdata[map[j][i].chipID];
        }
    }
    puts("\tマップの歩行データの読み込み完了");

    //マップのエンカウント情報の読み込み
    int encountpro;
    Note mapencountbuf;
    sprintf(tmpstr, "data/%s.encountdata", mapname);
    if(mapencountbuf.noteload(tmpstr) == -1){
        printf("%s not found.\n", tmpstr);
    }else{
        strncpy(line, mapencountbuf.noteget(0), 512);
        getstr(tmpstr, line, 0);
        encountflag = atoi(tmpstr);
        if(encountflag){
            getstr(tmpstr, line, 1);
            encountpro = atoi(tmpstr);
        }
    }
    puts("\tマップのエンカウント情報の読み込み完了");

    //エンカウント歩数の決定
    if(encountflag){
        encountwalknum = encountpro + GetRand((int)((float)encountpro / 5.0) - 1);
    }
    puts("\tエンカウント歩数の決定完了");

    //イベント情報の読み込み
    em.eventLoad(mapname);
    puts("\tイベント情報の読み込み完了");

    //イベントのページナンバーのチェック
    em.flagCheck();
    puts("\tイベントのページナンバーのチェック完了");

}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{

    MAP map[MAPMAX + 2][MAPMAX + 2];
    ENEMY enemy[EMAX];
    PLAYER player;
    BATTLEDATA bdata;
    int wincenterX, wincenterY;
    int chipdata[CHIPMAXNUM];
    int mapmaxX, mapmaxY;
    int encountflag, encountwalknum;
    int walknum;
    int pvector = DOWNNUM;
    int runeventnum;
    int eventattacked = 0;
    float cameraX, cameraY;
    float shadowrate;
    char mapname[128];
    char tmpstr[64];
    bool walkable;
    bool walkablecopy;
    bool oktrigger = false;
    int buffer_handle[200] = {-1};
    int music_handle[200] = {-1};

    if(initialize() == failure){
        fprintf(stderr, "Some error has occurred in DxLib_Init().\n");
		return -1;
    }

    player.x = 21;
    player.y = 20;
    player.realX = float(player.x * MAPCHIP + MAPCHIP/2);
    player.realY = float(player.y * MAPCHIP + MAPCHIP/2);
    player.pagewait = 0;
    player.pagenum = 0;
    player.page = 0;
    cameraX = player.realX;
    cameraY = player.realY;
    player.realXV = 0.0;
    player.realYV = 0.0;
    player.money = 1000;
    sprintf(mapname, "testmap");
    shadowrate = 0.0;

    SetTransColor(0, 0, 0);

    //フェードアウト用の画像読み込み
    buffer_handle[101] = LoadGraph("data/shadow.png");

    //メニューバーの読み込み
    buffer_handle[4] = LoadGraph("data/menubar.png");

    //メッセージウィンドウ画像の読み込み
    buffer_handle[5] = LoadGraph("data/msgwindow.png");

    //真っ白画像の読み込み
	buffer_handle[52] = LoadGraph("data/white.png");

    //メニュー始まりの効果音の読み込み
    music_handle[50] = LoadSoundMem("data/menustart.wav");

    //決定効果音の読み込み
    music_handle[51] = LoadSoundMem("data/okSE.wav");

    //キャンセル効果音の読み込み
    music_handle[52] = LoadSoundMem("data/cancelSE.wav");

    //エンカウント時の効果音読み込み
    music_handle[53] = LoadSoundMem("data/encount.wav");

    //カーソル音の読み込み
    music_handle[54] = LoadSoundMem("data/cursor.wav");

    //レジ音の読み込み
    music_handle[55] = LoadSoundMem("data/buySE.wav");

    //はい/いいえ表示音の読み込み
    music_handle[56] = LoadSoundMem("data/cursor7.mp3");

    //セリフ送り音の読み込み
    music_handle[57] = LoadSoundMem("data/button57.mp3");

    //数字フォント画像の読み込み
    int numberGraphic[10];
    LoadDivGraph("data/number.png", 15, 5, 3, 32, 32, numberGraphic);

    //現在ウィンドウ中心点の初期化
    wincenterX = WinX / (2 * MAPCHIP) + 1;
    wincenterY = WinY / (2 * MAPCHIP) + 1;

    //マップ画像読み込み
    int mapchipGraphic[104];
    if(LoadDivGraph("data/map.bmp", 104, 8, 13, MAPCHIP, MAPCHIP, mapchipGraphic) == -1){
        puts("data/map.bmp loading failed");
    }

    //プレイヤー画像読み込み
    int playerGraphic[24];
    if(LoadDivGraph("data/player_0306.png", 24, 6, 4, 32, 48, playerGraphic) == -1){
        puts("data/cc8_reimu.png loading failed");
    }

    //イベント用スプライトの読み込み
    int spriteGraphic[384];
    LoadDivGraph("data/spritepic.bmp", 384, 6, 64, 32, 48, spriteGraphic);

    //ウィンドウチップ画像の読み込み
    int windowGraphic[9];
    if(LoadDivGraph("data/windowcells.png", 9, 9, 1, 64, 64, windowGraphic) == -1){
        puts("data/windowcells.png loading failed");
    }

    //アイテムアイコン画像の読み込み
    int itemGraphic[280];
    LoadDivGraph("data/itemicon.bmp", 280, 20, 14, 64, 64, itemGraphic);

    //矢印画像の読み込み
    int arrowGraphic[4];
    LoadDivGraph("data/arrow.png", 4, 4, 1, 64, 64, arrowGraphic);

    //マップチップが通過可能であるかどうかの情報を読み込む
    Note chipdatabuf;
    chipdatabuf.noteload("data/chipdata.txt");
    for(int i = 0; i < CHIPMAXNUM; i++){
        getstr(tmpstr, chipdatabuf.noteget(0), i);
        chipdata[i] = atoi(tmpstr);
    }
    chipdatabuf.delete_note();

    //アイテムデータの読み込み
    Note itemdatabuf;
    itemdatabuf.noteload("data/itemdata.txt");

    //敵データの読み込み
    Note enemydatabuf;
    enemydatabuf.noteload("data/monsterdata.txt");

    gamemode gamescene = mapwalk;

    puts("素材ファイルのロード完了");

    EventManager eventmanager(&cameraX, &cameraY, &player, &shadowrate, music_handle, windowGraphic, spriteGraphic, &itemdatabuf, itemGraphic, arrowGraphic, mapname);
    Menu menu(&player, &itemdatabuf, &shadowrate, windowGraphic, buffer_handle, music_handle);
    BattleScene battleScene(buffer_handle, music_handle, map, windowGraphic);

    puts("イベントのロード完了");

    while(ProcessMessage() == 0){
        ClearDrawScreen();
		SetDrawScreen(DX_SCREEN_BACK);

        //マップ描画
        if(!initflag){
            int camposX, camposY;
            camposX = (int)(cameraX / MAPCHIP);
            camposY = (int)(cameraY / MAPCHIP);
            for(int y = 0; y < WinY / MAPCHIP + 2; y++){
                for(int x = 0; x < WinX / MAPCHIP + 2; x++){
                    int posX, posY;
                    int mapi, mapj;
                    posX = x * MAPCHIP - (int)cameraX % MAPCHIP - MAPCHIP / 2;
                    posY = y * MAPCHIP - (int)cameraY % MAPCHIP - MAPCHIP / 2;
                    mapi = camposX - ((WinX/MAPCHIP) / 2) + x;
                    mapj = camposY - ((WinY/MAPCHIP) / 2) + y;
                    if(mapi - 1 >= 0 && mapj - 1 >= 0 && mapi - 1 <= MAPMAX && mapj - 1 <= MAPMAX){
                        if(mapi - 1 >= 0 && mapi - 1 < MAPMAX + 2)DrawGraph(posX, posY, mapchipGraphic[map[mapi - 1][mapj - 1].chipID], TRUE);
                    }else{
                        if(mapi - 1 < 0 || mapj - 1 < 0 ){
                            if(mapi >= 0 && mapi < MAPMAX + 2)DrawGraph(posX, posY, mapchipGraphic[map[mapi][mapj].chipID], TRUE);
                        }
                        if(mapi - 1 >= MAPMAX || mapj - 1 >= MAPMAX){
                            if(mapi - 2 >= 0 && mapi - 2 < MAPMAX + 2)DrawGraph(posX, posY, mapchipGraphic[map[mapi - 2][mapj - 2].chipID], TRUE);
                        }
                    }
                }
            }
        }

        if(gamescene == mapwalk){

            if(initflag){
                puts("移動フェイズの初期化開始");
                walkMainInit(mapname, mapmaxX, mapmaxY, map, eventmanager, music_handle, chipdata, encountflag, encountwalknum);
                walkable = true;
                walkablecopy = true;
                walknum = 0;
                initflag = false;
                puts("移動フェイズの初期化完了");
            }

            //イベント画像
            eventmanager.draw();

            //主人公画像
            player.pagewait++;
            if(player.pagewait == 10){
                switch(player.pagenum){
                    case 0: player.pagenum = 1; break;
                    case 1: player.pagenum = 2; break;
                    case 2: player.pagenum = 3; break;
                    case 3: player.pagenum = 0; break;
                }
                if(player.pagenum != 3){
                    player.page = player.pagenum;
                }else{
                    player.page = 1;
                }
                player.pagewait = 0;
            }
            if(eventmanager.isEventRunning() || menu.isInMenu() == true){
                DrawGraph(player.realX - (cameraX - WinX/2) - 16, player.realY - (cameraY - WinY/2) - 24, playerGraphic[1 + pvector * 3], TRUE);
            }else{
                if(walkable == false){
                    DrawGraph(player.realX - (cameraX - WinX/2) - 16, player.realY - (cameraY - WinY/2) - 24, playerGraphic[player.page + pvector * 3], TRUE);
                }else{
                    if(!checkInput(KEY_INPUT_UP) && !checkInput(KEY_INPUT_DOWN) && !checkInput(KEY_INPUT_LEFT) && !checkInput(KEY_INPUT_RIGHT)){
                        DrawGraph(player.realX - (cameraX - WinX/2) - 16, player.realY - (cameraY - WinY/2) - 24, playerGraphic[1 + pvector * 3], TRUE);
                    }else{
                        DrawGraph(player.realX - (cameraX - WinX/2) - 16, player.realY - (cameraY - WinY/2) - 24, playerGraphic[player.page + pvector * 3], TRUE);
                    }
                }
            }

            //フェード効果
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)shadowrate);
            DrawGraph(0, 0, buffer_handle[101], FALSE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
            if(eventmanager.isEventRunning() == false && shadowrate > 0.0){
                shadowrate -= 4.0;
                if(shadowrate < 0.0) shadowrate = 0.0;
            }

            if(menu.isInMenu() == false){
                //主人公移動
                int left = 0, up = 0, down = 0, right = 0;
                if(walkable == true && eventmanager.isEventRunning() == 0){
                    if(checkInput(KEY_INPUT_LEFT))left = 1;
                    if(checkInput(KEY_INPUT_UP))up = 1;
                    if(checkInput(KEY_INPUT_RIGHT))right = 1;
                    if(checkInput(KEY_INPUT_DOWN))down = 1;
                }
                if(left){
                    walkable = false;
                    player.realXV = -PLAYERMAPV;
                    player.XV = -1;
                    left = 0;
                }
                if(up){
                    walkable = false;
                    player.realYV = -PLAYERMAPV;
                    player.YV = -1;
                    up = 0;
                }
                if(right){
                    walkable = false;
                    player.realXV = PLAYERMAPV;
                    player.XV = 1;
                    right = 0;
                }
                if(down){
                    walkable = false;
                    player.realYV = PLAYERMAPV;
                    player.YV = 1;
                    down = 0;
                }
                if(player.x + player.XV >= 0 && player.x + player.XV <= mapmaxX - 1 && player.y + player.YV >= 0 && player.y + player.YV <= mapmaxY - 1){
                    if(map[player.x + player.XV][player.y].mapdata != 1 && map[player.x][player.y + player.YV].mapdata != 1 && map[player.x + player.XV][player.y + player.YV].mapdata != 1){
                        int nowalk;
                        nowalk = 0;
                        for(int i = 0; i < eventmanager.getEventNum(); i++){
                            if(player.x + player.XV == eventmanager.getEventX(i) && player.y + player.YV == eventmanager.getEventY(i) && eventmanager.getEventType(i) != 2){
                                if(eventmanager.getEventTrigger(i) == 1){
                                    eventattacked = 1;
                                    runeventnum = i;
                                }
                                nowalk = 1;
                            }
                        }
                        if(nowalk == 0){
                            player.x += player.XV;
                            player.y += player.YV;
                            player.realX += player.realXV;
                            player.realY += player.realYV;
                        }
                    }else{
                        for(int i = 0; i < eventmanager.getEventNum(); i++){
                            if(player.x + player.XV == eventmanager.getEventX(i) && player.y + player.YV == eventmanager.getEventY(i) && eventmanager.getEventType(i) == 1){
                                eventattacked = 1;
                                runeventnum = i;
                            }
                        }
                    }
                }

                //プレイヤーの向きを取得する
                if(player.realXV != 0.0 || player.realYV != 0.0){
                    if(player.XV == 0 && player.YV == 1)pvector = DOWNNUM;//下
                    if(player.XV == -1 && player.YV == 1)pvector = DLEFTNUM;//左下
                    if(player.XV == -1 && player.YV == 0)pvector = LEFTNUM;//左
                    if(player.XV == 1 && player.YV == 1)pvector = DRIGHTNUM;//右下
                    if(player.XV == 1 && player.YV == 0)pvector = RIGHTNUM;//右
                    if(player.XV == -1 && player.YV == -1)pvector = ULEFTNUM;//左上
                    if(player.XV == 0 && player.YV == -1)pvector = UPNUM;//上
                    if(player.XV == 1 && player.YV == -1)pvector = URIGHTNUM;//右上
                }

                //プレイヤー速度の初期化
                player.XV = 0;
                player.YV = 0;
                if((int)player.realX == player.x*MAPCHIP + MAPCHIP/2 && (int)player.realY == player.y*MAPCHIP + MAPCHIP/2){
                    walkable = 1;
                    player.realXV = 0.0;
                    player.realYV = 0.0;
                }

                //カメラ制御
                if(eventmanager.isEventRunning() == 0){
                    if(player.realX > WinX/2 + 1 && player.realX < mapmaxX * MAPCHIP - WinX/2 - 1){
                        cameraX = player.realX;
                    }else{
                        if(player.realX <= WinX/2 + 1) cameraX = WinX/2;
                        if(player.realX >= mapmaxX * MAPCHIP - WinX/2 - 1) cameraX = mapmaxX * MAPCHIP - WinY/2 - MAPCHIP * 2;
                    }
                    if(player.realY > WinY/2 + 1 && player.realY < mapmaxY * MAPCHIP - WinY/2 - 1){
                        cameraY = player.realY;
                    }else{
                        if(player.realY <= WinY/2 + 1) cameraY = WinY/2;
                        if(player.realY >= mapmaxY * MAPCHIP - WinY/2 - 1) cameraY = mapmaxY * MAPCHIP - WinY/2;
                    }
                }

                //触れたときのイベントの発生
                if(eventattacked){
                    eventmanager.runEventInit(runeventnum, pvector);
                    eventattacked = 0;
                }

                //Zキーを押したときのイベントの発生
                if(!checkInput(KEY_INPUT_Z))oktrigger = true;
                if(checkInput(KEY_INPUT_Z) && oktrigger &&eventmanager.isEventRunning() == false){
                    for(int i = 0; i < eventmanager.getEventNum(); i++){
                        if(eventmanager.getEventType(i) == 2){
                            if(player.x == eventmanager.getEventX(i) && player.y == eventmanager.getEventY(i) && eventmanager.getEventTrigger(i) == 0){
                                runeventnum = i;
                                eventmanager.runEventInit(runeventnum, pvector);
                            }
                        }else{
                            int xvec, yvec;
                            xvec = 0;
                            yvec = 0;
                            switch(pvector){
                                case UPNUM : yvec = -1;break;
                                case DOWNNUM : yvec = 1;break;
                                case LEFTNUM : xvec = -1;break;
                                case RIGHTNUM : xvec = 1;break;
                                case ULEFTNUM : xvec = -1; yvec = -1;break;
                                case URIGHTNUM : xvec = 1; yvec = -1;break;
                                case DLEFTNUM : xvec = -1; yvec = 1;break;
                                case DRIGHTNUM : xvec = 1; yvec = 1;break;
                            }
                            if(player.x + xvec == eventmanager.getEventX(i) && player.y + yvec == eventmanager.getEventY(i) && eventmanager.getEventTrigger(i) == 0){
                                runeventnum = i;
                                eventmanager.runEventInit(runeventnum, pvector);
                            }

                        }
                    }
                    oktrigger = false;
                }

                //イベント中の処理
                if(eventmanager.isEventRunning()){
                    eventmanager.runEvent(oktrigger);
                }
            }

            //メニュー画面表示
            if(!checkInput(KEY_INPUT_ESCAPE))menu.setMenuTrigger(false);
            if(checkInput(KEY_INPUT_ESCAPE) && menu.isInMenu() == false && menu.getMenuTrigger() == false && eventmanager.isEventRunning() == false){
                PlaySoundMem(music_handle[50], DX_PLAYTYPE_BACK);
                menu.setMenuTrigger(true);
                menu.menuInit();
            }
            if(menu.isInMenu()){
                menu.menu();
            }

        }else if(gamescene == battle){
            if(initflag){
                // bdata.backPicHandle =
            }
            battleScene.update();
            battleScene.draw();
        }

        ScreenFlip();
    }

    DxLib_End();

    return 0;
}

exception initialize(){
    char title[128];

    SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);
	SetGraphMode(WinX, WinY, 32);

    sprintf(title, "%s ver.%s", GAMETITLE, VERSION);
	SetWindowText(title);
	if(DxLib_Init() == -1){
        return failure;
    }
	SetWindowSizeChangeEnableFlag(TRUE);
	//SetWindowIconID(1);
	SetMouseDispFlag(TRUE);
    SetAlwaysRunFlag(TRUE);
    SetBackgroundColor(0, 0, 0);

    font_mes = CreateFontToHandle("ＭＳ　ゴシック", 35, -1, DX_FONTTYPE_ANTIALIASING);

    return success;
}
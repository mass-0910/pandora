#include "event.h"
#include "note.h"
#include "makewindow.h"
#include "DxLib/DxLib.h"
#include "input.h"

extern int font_mes;
extern bool initflag;

EventManager::EventManager(float *camX, float *camY, PLAYER *p, float *shadow, int *MH, int *WH, int *SH, Note *IB, int *II, int *AG, char *MN){
    cameraX = camX;
    cameraY = camY;
    player = p;
    shadowrate = shadow;
    musicHandle = MH;
    windowGraphic = WH;
    spriteHandle = SH;
    itemdatabuf = IB;
    itemicon = II;
    arrowGraphic = AG;
    mapname = MN;
}

void removespace(char *str){
    int i = 0, j = 0;
    while(str[i] != 0){
        if(str[i] != ' '){
            str[j] = str[i];
            j++;
        }
        i++;
    }
    str[j] = '\0';
}

char *enterinput(char *word){
    static char returnstr[1024];
	memset(returnstr, 0, 1024);
	for(int i = 0; i < strlen(word); i++){
		if(word[i] == '/'){
			snprintf(returnstr, 1024, "%s\n", returnstr);
		}else{
			returnstr[i] = word[i];
		}
    }
	return returnstr;
}

int getLinenum(char *str){
    int linenum = 1, i = 0;
    while(str[i] != '\0'){
        if(str[i] == '\n'){
            linenum++;
        }
        i++;
    }
    return linenum;
}

int searchtag(Note& text, char *tag){
    int nextlinestart = 0;
    int indentnum;
    int linenum;
    bool tagfound;
    bool ignore, notag;
    char tagline[64];
    char textline[128];
    char textline2[128];
    char param1[64];
    for(int cnt = 0; cnt < getLinenum(tag); cnt++){
        tagfound = false;
        getstr(tagline, tag, cnt, '\n');
        indentnum = cnt;
        for(int ccnt = nextlinestart; ccnt < text.notemax(); ccnt++){
            ignore = false;
            notag = false;
            strncpy(textline, text.noteget(ccnt), 128);
            for(int cccnt = 0; cccnt < indentnum; cccnt++){
                if(textline[cccnt] != ' '){
                    notag = true;
                    break;
                }
            }
            if(textline[indentnum] == ' ')ignore = true;
            if(notag)break;
            if(ignore)continue;
            strncpy(textline2, textline + indentnum, 128);
            getstr(param1, textline2, 0);
            if(strncmp(param1, tagline, 64) == 0){
                tagfound = true;
                linenum = ccnt;
                nextlinestart = linenum + 1;
                break;
            }
        }
        if(tagfound == false){
            linenum = -1;
            break;
        }
    }
    return linenum;
}

int searchLabel(Note& text, char *label){
    char atlabel[64];
    snprintf(atlabel, 64, "@%s", label);
    for(int i = 0; i < text.notemax(); i++){
        if(strstr(text.noteget(i), atlabel) != NULL){
            return i;
        }
    }
    return -1;
}

void EventManager::eventLoad(char *mapname){
    char eventtextname[64];
    char line[1024];
    char tmpstr[64];
    char tag[64];

    eventnum = 0;

    //イベント情報の読み込み
    snprintf(eventtextname, 64, "data/%s.event", mapname);
    printf("mapname : %s\n", mapname);
    eventtext.noteload(eventtextname);
    puts("\t\tイベント情報の読み込み完了");

    //イベントの各情報を配列に入れる
    for(int cnt = 0;; cnt++){
        sprintf(tag, "eve%d", cnt);
        strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
        //printf("line : %s\n", line);
        getstr(event[cnt].name, line, 1);
        if(strncmp(event[cnt].name, "NOP", EVENTNAMEMAX) == 0)break;

        sprintf(tag, "eve%d\nposx", cnt);
        strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
        getstr(tmpstr, line, 1);
        event[cnt].x = atoi(tmpstr);
        event[cnt].realX = event[cnt].x * MAPCHIP;

        sprintf(tag, "eve%d\nposy", cnt);
        strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
        getstr(tmpstr, line, 1);
        event[cnt].y = atoi(tmpstr);
        event[cnt].realY = event[cnt].y * MAPCHIP;

        sprintf(tag, "eve%d\ntype", cnt);
        strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
        getstr(tmpstr, line, 1);
        event[cnt].type = atoi(tmpstr);

        sprintf(tag, "eve%d\ntrigger", cnt);
        strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
        getstr(tmpstr, line, 1);
        event[cnt].trigger = atoi(tmpstr);

        sprintf(tag, "eve%d\nvector", cnt);
        strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
        getstr(tmpstr, line, 1);
        event[cnt].vector = atoi(tmpstr);

        sprintf(tag, "eve%d\nspritenum", cnt);
        strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
        getstr(tmpstr, line, 1);
        event[cnt].spritenum = atoi(tmpstr);

        sprintf(tag, "eve%d\nanime", cnt);
        strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
        getstr(tmpstr, line, 1);
        event[cnt].anime = atoi(tmpstr);

        sprintf(tag, "eve%d\npagenum", cnt);
        strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
        getstr(tmpstr, line, 1);
        event[cnt].pagenum = atoi(tmpstr);
        
        sprintf(tag, "eve%d\nflagnum", cnt);
        strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
        for(int ccnt = 0; ccnt < event[cnt].pagenum; ccnt++){
            getstr(tmpstr, line, ccnt + 1);
            event[cnt].flagnum[ccnt] = atoi(tmpstr);
        }

        eventnum++;
    }
    puts("\t\tイベントの各情報を配列に格納完了");
}

void EventManager::flagCheck(){
    char eventtextname[64];
    char line[1024];
    char tmpstr[64];
    char tag[64];

    for(int cnt = 0; cnt < eventnum; cnt++){
        if(event[cnt].pagenum != 1){
            bool changed;
            changed = false;
            for(int ccnt = 0; ccnt < event[cnt].pagenum - 1; ccnt++){
                if((event[cnt].flagnum[ccnt] > 0 && gameFlag[event[cnt].flagnum[ccnt]] == true) || (event[cnt].flagnum[ccnt] < 0 && gameFlag[event[cnt].flagnum[ccnt]] == false)){
                    printf("page change : %d\n", ccnt + 2);
                    event[cnt].page = ccnt + 2;
                    snprintf(tag, 64, "eve%d\npage%d", cnt, ccnt+2); //???
                    strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
                    getstr(tmpstr, line, 1);
                    event[cnt].spritenum = atoi(tmpstr);
                    getstr(tmpstr, line, 2);
                    event[cnt].type = atoi(tmpstr);
                    getstr(tmpstr, line, 3);
                    event[cnt].trigger = atoi(tmpstr);
                    changed = true;
                }
            }
            if(changed == false){
                puts("page reset");
                event[cnt].page = 1;
                sprintf(tag, "eve%d\ntype", cnt);
                strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
                getstr(tmpstr, line, 1);
                event[cnt].type = atoi(tmpstr);
                sprintf(tag, "eve%d\ntrigger", cnt);
                strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
                getstr(tmpstr, line, 1);
                event[cnt].trigger = atoi(tmpstr);
                sprintf(tag, "eve%d\nspritenum", cnt);
                strncpy(line, eventtext.noteget(searchtag(eventtext, tag)), 1024);
                getstr(tmpstr, line, 1);
                event[cnt].spritenum = atoi(tmpstr);
            }
        }else{
            event[cnt].page = 1;
        }
    }
}

void EventManager::runEventInit(int eventID, int pvector){
    char tag[64];

    runeventnum = eventID;
    sprintf(tag, "eve%d\npage%d", runeventnum, event[runeventnum].page);
    readinglinenum = searchtag(eventtext, tag) + 1;
    eventrunning = true;
    printf("page : %d\n", event[runeventnum].page);

    if(event[runeventnum].type == 0){
        switch(pvector){
            case UPNUM : event[runeventnum].vector = DOWNNUM ; break;
		    case DOWNNUM : event[runeventnum].vector = UPNUM ; break;
		    case LEFTNUM : event[runeventnum].vector = RIGHTNUM ; break;
		    case RIGHTNUM : event[runeventnum].vector = LEFTNUM ; break;
		    case ULEFTNUM : event[runeventnum].vector = DRIGHTNUM ; break;
		    case URIGHTNUM : event[runeventnum].vector = DLEFTNUM ; break;
		    case DLEFTNUM : event[runeventnum].vector = URIGHTNUM ; break;
		    case DRIGHTNUM : event[runeventnum].vector = ULEFTNUM ; break;
        }
    }
}

void EventManager::runEvent(bool& oktrigger){
    char line[1024];
    char tmpstr[64];

    strncpy(line, eventtext.noteget(readinglinenum), 1024);
    removespace(line);
    getstr(tmpstr, line, 0);

    if(strncmp(tmpstr, "mes", 64) == 0){//メッセージの表示
        if(eveinit){
            memset(outstr, 0, sizeof(outstr));
            getstr(tmpstr, line, 1);
            strncpy(endstr, enterinput(tmpstr), 1024);
            mesframe = 0;
            eveinit = false;
        }

        if(strcmp(outstr, endstr) != 0 && mesframe % 2 == 0){
            if(mesframe % 4 == 0)PlaySoundMem(musicHandle[57], DX_PLAYTYPE_BACK);
            snprintf(outstr, 1024, "%s%c", outstr, endstr[mesframe / 2]);
        }
        mesframe++;
        msgwindow(outstr, 0, 19, 5, windowGraphic);
        if(!checkInput(KEY_INPUT_Z)) oktrigger = true;
        if(checkInput(KEY_INPUT_Z) && oktrigger && strncmp(outstr, endstr, 1024) != 0){
            strncpy(outstr, endstr, 1024);
            oktrigger = false;
        }
        if(checkInput(KEY_INPUT_Z) && oktrigger && strncmp(outstr, endstr, 1024) == 0){
            mesframe = 0;
            PlaySoundMem(musicHandle[51], DX_PLAYTYPE_BACK);
            nextline = true;
            oktrigger = false;
        }
    }else if(strncmp(tmpstr, "yesno", 64) == 0){
        int result;
        static bool selectbeep;
        if(eveinit){
            memset(outstr, 0, sizeof(outstr));
            getstr(tmpstr, line, 1);
            strncpy(endstr, enterinput(tmpstr), 1024);
            mesframe = 0;
            eveinit = false;
            selectbeep = true;
        }

        if(strcmp(outstr, endstr) != 0 && mesframe % 2 == 0){
            if(mesframe % 4 == 0)PlaySoundMem(musicHandle[57], DX_PLAYTYPE_BACK);
            snprintf(outstr, 1024, "%s%c", outstr, endstr[mesframe / 2]);
        }
        mesframe++;
        msgwindow(outstr, 0, 19, 5, windowGraphic);
        if(!checkInput(KEY_INPUT_Z)) oktrigger = true;
        if(checkInput(KEY_INPUT_Z) && oktrigger && strncmp(outstr, endstr, 1024) != 0){
            strncpy(outstr, endstr, 1024);
            oktrigger = false;
        }
        if(strncmp(outstr, endstr, 1024) == 0){
            if(selectbeep){
                PlaySoundMem(musicHandle[56], DX_PLAYTYPE_BACK);
                selectbeep = false;
            }
            if((result = selectYes_No(true)) != 0){
                if(result == 1) {
                    readinglinenum =  searchLabel(eventtext, getstr(tmpstr, line, 2));
                    printf("tagnum : %d\n", readinglinenum);
                }
                oktrigger = false;
                nextline = true;
            }
        }
    }else if(strncmp(tmpstr, "jump", 64) == 0){
        readinglinenum = searchLabel(eventtext, getstr(tmpstr, line, 1));
        nextline = true;
    }else if(strncmp(tmpstr, "cammove", 64) == 0){//カメラ位置移動
        if(eveinit == true){
            getstr(cammovevector, line, 1);
            getstr(tmpstr, line, 2);
            cammovenum = atoi(tmpstr);
            getstr(tmpstr, line, 3);
            cammovespeed = atoi(tmpstr);
            movednum = 0;
            eveinit = false;
        }
        if(strncmp(cammovevector, "x", 4) == 0){
            if(cammovenum >= 0){
                printf("%f\n", *cameraX);
				if(*cameraX + cammovespeed > WinX/2 + 2 && *cameraX + cammovespeed < (float)(MAPMAX * MAPCHIP - WinX/2 - 2)) (*cameraX) += (float)(cammovespeed);
				movednum++;
			}else{
                printf("%f\n", *cameraX);
				if(*cameraX - cammovespeed > WinX/2 + 2 && *cameraX - cammovespeed < (float)(MAPMAX * MAPCHIP - WinX/2) - 2) (*cameraX) -= (float)(cammovespeed);
				movednum--;
			}
        }
        if(strncmp(cammovevector, "y", 4) == 0){
            if(cammovenum >= 0){
				(*cameraY) += (float)(cammovespeed);
				movednum++;
			}else{
				(*cameraY) -= (float)(cammovespeed);
				movednum--;
			}
        }
        if(movednum == cammovenum){
            nextline = true;
        }
    }else if(strncmp(tmpstr, "camreset", 64) == 0){//カメラ位置を元に戻す
        *cameraX = player->realX;
        *cameraY = player->realY;
        nextline = true;
    }else if(strncmp(tmpstr, "evemove", 64) == 0){//イベントの移動 evemove,name,x/y,移動量,スピード
        static int movingEvent, movingEventOriginX, movingEventOriginY;
        static float movingEventOriginRealX, movingEventOriginRealY;
        if(eveinit == true){
            for(int i = 0; i < EVENTMAX; i++){
                if(strcmp(event[i].name, getstr(tmpstr, line, 1)) == 0){
                    movingEvent = i;
                    movingEventOriginX = event[i].x;
                    movingEventOriginY = event[i].y;
                    movingEventOriginRealX = event[i].realX;
                    movingEventOriginRealY = event[i].realY;
                    if(strcmp(getstr(tmpstr, line, 2), "x") == 0 && atoi(getstr(tmpstr, line, 4)) > 0)event[i].vector = RIGHTNUM;
                    else if(strcmp(getstr(tmpstr, line, 2), "x") == 0 && atoi(getstr(tmpstr, line, 4)) < 0)event[i].vector = LEFTNUM;
                    else if(strcmp(getstr(tmpstr, line, 2), "y") == 0 && atoi(getstr(tmpstr, line, 4)) > 0)event[i].vector = DOWNNUM;
                    else if(strcmp(getstr(tmpstr, line, 2), "y") == 0 && atoi(getstr(tmpstr, line, 4)) < 0)event[i].vector = UPNUM;
                    break;
                }
            }
            eveinit = false;
        }
        if(strcmp(getstr(tmpstr, line, 2), "x") == 0){
            event[movingEvent].realX += (float)atoi(getstr(tmpstr, line, 4));
            if(atoi(getstr(tmpstr, line, 4)) > 0 && event[movingEvent].realX > movingEventOriginRealX + (float)(atoi(getstr(tmpstr, line, 3)) * MAPCHIP)){
                event[movingEvent].realX = movingEventOriginRealX + (float)(atoi(getstr(tmpstr, line, 3)) * MAPCHIP);
                event[movingEvent].x = movingEventOriginX + atoi(getstr(tmpstr, line, 3));
                nextline = true;
            }else if(atoi(getstr(tmpstr, line, 4)) < 0 && event[movingEvent].realX < movingEventOriginRealX - (float)(atoi(getstr(tmpstr, line, 3)) * MAPCHIP)){
                event[movingEvent].realX = movingEventOriginRealX - (float)(atoi(getstr(tmpstr, line, 3)) * MAPCHIP);
                event[movingEvent].x = movingEventOriginX - atoi(getstr(tmpstr, line, 3));
                nextline = true;
            }
        }else if(strcmp(getstr(tmpstr, line, 2), "y") == 0){
            event[movingEvent].realY += (float)atoi(getstr(tmpstr, line, 4));
            if(atoi(getstr(tmpstr, line, 4)) > 0 && event[movingEvent].realY > movingEventOriginRealY + (float)(atoi(getstr(tmpstr, line, 3)) * MAPCHIP)){
                event[movingEvent].realY = movingEventOriginRealY + (float)(atoi(getstr(tmpstr, line, 3)) * MAPCHIP);
                event[movingEvent].y = movingEventOriginY + atoi(getstr(tmpstr, line, 3));
                nextline = true;
            }else if(atoi(getstr(tmpstr, line, 4)) < 0 && event[movingEvent].realY < movingEventOriginRealY - (float)(atoi(getstr(tmpstr, line, 3)) * MAPCHIP)){
                event[movingEvent].realY = movingEventOriginRealY - (float)(atoi(getstr(tmpstr, line, 3)) * MAPCHIP);
                event[movingEvent].x = movingEventOriginY - atoi(getstr(tmpstr, line, 3));
                nextline = true;
            }
        }
    }else if(strncmp(tmpstr, "evevec", 64) == 0){//イベントの向き evevec,name,vector
        for(int i = 0; i < EVENTMAX; i++){
            if(strcmp(event[i].name, getstr(tmpstr, line, 1)) == 0){
                if(strcmp(getstr(tmpstr, line, 2), "right") == 0)event[i].vector = RIGHTNUM;
                else if(strcmp(getstr(tmpstr, line, 2), "left") == 0)event[i].vector = LEFTNUM;
                else if(strcmp(getstr(tmpstr, line, 2), "down") == 0)event[i].vector = DOWNNUM;
                else if(strcmp(getstr(tmpstr, line, 2), "up") == 0)event[i].vector = UPNUM;
                break;
            }
        }
        nextline = true;
    }else if(strncmp(tmpstr, "fadein", 64) == 0){//フェードイン
        if(eveinit){
            getstr(tmpstr, line, 1);
            fadeend = (float)(atoi(tmpstr));
            getstr(tmpstr, line, 2);
            fadespeed = (float)(atoi(tmpstr));
            eveinit = false;
        }
        *shadowrate -= fadespeed;
        if(*shadowrate <= fadeend){
            *shadowrate = fadeend;
            nextline = true;
        }
    }else if(strncmp(tmpstr, "fadeout", 64) == 0){//フェードアウト
        if(eveinit){
            getstr(tmpstr, line, 1);
            fadeend = (float)(atoi(tmpstr));
            getstr(tmpstr, line, 2);
            fadespeed = (float)(atoi(tmpstr));
            eveinit = false;
        }
        *shadowrate += fadespeed;
        if(*shadowrate >= fadeend){
            *shadowrate = fadeend;
            nextline = true;
        }
    }else if(strncmp(tmpstr, "fadeflush", 64) == 0){//フェードの取り消し
        *shadowrate = 0.0;
        nextline = true;
    }else if(strncmp(tmpstr, "egflush", 64) == 0){//イベント画面を消す
        nextline = true;
    }else if(strncmp(tmpstr, "shop", 64) == 0){//ショップ
        shop(line);
        oktrigger = false;
    }else if(strncmp(tmpstr, "flag", 64) == 0){//フラグの切り替え
        int flagnum, value;
        getstr(tmpstr, line, 1);
        flagnum = atoi(tmpstr);
        getstr(tmpstr, line, 2);
        value = atoi(tmpstr);
        gameFlag[flagnum] = (value == 0) ? false : true;
        flagCheck();
        nextline = true;
    }else if(strncmp(tmpstr, "mapchange", 64) == 0){//マップ移動
        if(eveinit == true){
            snprintf(mapname, 128, "%s", getstr(tmpstr, line, 1));
            eveinit = false;
        }
        *shadowrate += 4.0;
        if(*shadowrate >= 255.0){
            *shadowrate = 255.0;
            initflag = true;
            eventrunning = false;
            player->x = atoi(getstr(tmpstr, line, 2));
            player->y = atoi(getstr(tmpstr, line, 3));
            player->realX = float(player->x * MAPCHIP + MAPCHIP/2);
            player->realY = float(player->y * MAPCHIP + MAPCHIP/2);
            oktrigger = false;
            nextline = true;
        }
    }else if(strncmp(tmpstr, "end", 64) == 0){//イベントの終了
        eventrunning = false;
        oktrigger = false;
        nextline = true;
    }else{
        nextline = true; 
    }

    if(nextline){
        nextline = false;
        eveinit = true;
        readinglinenum++;
    }
}

void EventManager::draw(){
    for(int i = 0; i < eventnum; i++){
        if((event[i].x + 1) * (*cameraX) - WinX/2 && (event[i].x - 1) * MAPCHIP <= (*cameraX) + WinX/2 && (event[i].y + 1) * MAPCHIP >= (*cameraY) - WinY/2 && (event[i].y - 1) * MAPCHIP <= (*cameraY) + WinY/2){
            int posX, posY;
            posX = event[i].realX - ((*cameraX) - WinX/2) + MAPCHIP/2;
            posY = event[i].realY - ((*cameraY) - WinY/2) + MAPCHIP/2;
            if(event[i].anime == 1){
                event[i].animeframe++;
                if(event[i].animeframe == 20){
                    event[i].animeframe = 0;
                    event[i].animepage++;
                    event[i].animepage = event[i].animepage % 4;
                }
                if(event[i].animepage == 3){
                    DrawGraph(posX - 16, posY - 24, spriteHandle[event[i].spritenum * 24 + event[i].vector * 3 + 1], TRUE);
                }else{
                    DrawGraph(posX - 16, posY - 24, spriteHandle[event[i].spritenum * 24 + event[i].vector * 3 + event[i].animepage], TRUE);
                }
            }else{
                DrawGraph(posX - 16, posY - 24, spriteHandle[event[i].spritenum * 24 + event[i].vector * 3 + 1], TRUE);
            }
        }
    }
}

void EventManager::numberSelect(int& selnum, int& selend, char *text){
    makewindow(900, 570, 4, 3, windowGraphic);
    DrawFormatStringToHandle(980 + 32, 615 + 32 + 20, GetColor(255, 255, 255), font_mes, "%d", selnum);
    DrawStringToHandle(980 - 30, 615 - 10, text, GetColor(255, 255, 255), font_mes);
    DrawGraph(980 + 90, 615 + 40, arrowGraphic[2], TRUE);
    DrawGraph(980 - 48, 615 + 40, arrowGraphic[3], TRUE);
    if(leftbutton && selnum >= 2){
        PlaySoundMem(musicHandle[54], DX_PLAYTYPE_BACK);
        selnum--;
    }
    if(rightbutton && selnum < selnummax){
        PlaySoundMem(musicHandle[54], DX_PLAYTYPE_BACK);
        selnum++;
    }
    if(zbutton){
        PlaySoundMem(musicHandle[51], DX_PLAYTYPE_BACK);
        sb.init(leftbutton, rightbutton, upbutton, downbutton, zbutton, xbutton);
        selend = 1;
    }
    if(xbutton){
        PlaySoundMem(musicHandle[52], DX_PLAYTYPE_BACK);
        sb.init(leftbutton, rightbutton, upbutton, downbutton, zbutton, xbutton);
        selnum = 0;
        selend = 1;
    }
}

int EventManager::selectYes_No(bool initialize){
    static int yesnoline = 0;
    static bool init = true;
    static SysButton sbt;
    if(init){
        yesnoline = initialize ? 0 : 1;
        init = false;
    }
    makewindow(900, 570, 4, 3, windowGraphic);
    DrawStringToHandle(980, 615, "はい", GetColor(255, 255, 255), font_mes);
    DrawStringToHandle(980, 615 + 64, "いいえ", GetColor(255, 255, 255), font_mes);
    DrawGraph(955 - 32, 635 + yesnoline * 64 - 32, arrowGraphic[2], TRUE);
    sbt.getSysButton(leftbutton, rightbutton, upbutton, downbutton, zbutton, ztrigger, xbutton, xtrigger);
    yesnoline = (yesnoline - upbutton + downbutton) % 2;
    if(upbutton || downbutton)PlaySoundMem(musicHandle[54], DX_PLAYTYPE_BACK);
    if(yesnoline < 0)yesnoline = 1;

    if(zbutton){
        PlaySoundMem(musicHandle[51 + yesnoline], DX_PLAYTYPE_BACK);
        sbt.init(leftbutton, rightbutton, upbutton, downbutton, zbutton, xbutton);
        init = true;
        return yesnoline == 0 ? 1 : -1;
    }

    if(xbutton){
        PlaySoundMem(musicHandle[52], DX_PLAYTYPE_BACK);
        yesnoline = 1;
        sbt.init(leftbutton, rightbutton, upbutton, downbutton, zbutton, xbutton);
        init = true;
        return -1;
    }

    sbt.init(leftbutton, rightbutton, upbutton, downbutton, zbutton, xbutton);
    return 0;
}

void EventManager::shop(char *runeventline){
    char tmpstr[256];
    sb.init(leftbutton, rightbutton, upbutton, downbutton, zbutton, xbutton);
    if(shopping == false){
        for(int i = 0;i < SHOPITEMMAX; i++){
            shopitem[i].ID = 0;
            shopitem[i].money = 0;
            shopitem[i].buymoney = 0;
        }
        for(int i = 0; strncmp(getstr(tmpstr, runeventline, i + 1), "0", 128) != 0; i++){
            shopitem[i].ID = atoi(getstr(tmpstr, runeventline, i + 1));
            shopitem[i].icon = atoi(getstr(tmpstr, itemdatabuf->noteget(shopitem[i].ID), 1));
            shopitem[i].money = atoi(getstr(tmpstr, itemdatabuf->noteget(shopitem[i].ID), 5));
            shopitem[i].buymoney = atoi(getstr(tmpstr, itemdatabuf->noteget(shopitem[i].ID), 6));
            printf("ID:%d  icon:%d  money:%d  buy:%d\n", shopitem[i].ID, shopitem[i].icon, shopitem[i].money, shopitem[i].buymoney);
        }
        memset(shopitemname, 0, 512);
        memset(shopiteminfo, 0, 2048);
        for(int i = 0; shopitem[i].ID != 0; i++){
            strcat(shopitemname, strcat(getstr(tmpstr, itemdatabuf->noteget(shopitem[i].ID), 0), "\n"));
            strcat(shopiteminfo, strcat(getstr(tmpstr, itemdatabuf->noteget(shopitem[i].ID), 4), ","));
        }
        infosel = 0;
        shopsel = 0;
        infosel = 0;
		shopping = 1;
		shopsel = 0;
		shoporigin = 0;
		buy = 0;
		sell = 0;
        ztrigger = false;
		xtrigger = false;
		*shadowrate = 0.0;
    }
    //画面を暗くする
    if(*shadowrate != 130.0)*shadowrate += (130.0 - *shadowrate) * 0.1;
    //ウィンドウ表示
    makewindow(0, 0, WinX / 128, WinY / 64 - 11, windowGraphic);
    //選択欄の説明表示
	if(buy == 0 && sell == 0){
		msgwindow(getstr(tmpstr, (char *)shopinfostr, infosel), 0, 19, 4, windowGraphic);
	}
    //お金の表示
    makewindow(0, WinY - 11 * 64, 5, 3, windowGraphic);
    snprintf(tmpstr, 128, "所持金\n%dG", player->money);
    DrawStringToHandle(50, WinY - 11*64 + 50, tmpstr, GetColor(255, 255, 255), font_mes);
    //選択欄の色
    DrawBox(64, infosel * 64 + 32, 64 + 300, infosel * 64 + 32 + 50, GetColor(254, 254, 254), TRUE);
    //欄の項目表示
    for(int i = 0; i < 3; i++){
        if(infosel == i){
            DrawStringToHandle(64, i * 64 + 32, getstr(tmpstr, (char *)shopmenustr, i), GetColor(50, 50, 50), font_mes);
        }else{
            DrawStringToHandle(64, i * 64 + 32, getstr(tmpstr, (char *)shopmenustr, i), GetColor(255, 255, 255), font_mes);
        }
    }
    sb.getSysButton(leftbutton, rightbutton, upbutton, downbutton, zbutton, ztrigger, xbutton, xtrigger);
    if(downbutton && infosel != 2 && buy == 0 && sell == 0){
        PlaySoundMem(musicHandle[54], DX_PLAYTYPE_BACK);
        infosel++;
    }
    if(upbutton && infosel != 0 && buy == 0 && sell == 0){
        PlaySoundMem(musicHandle[54], DX_PLAYTYPE_BACK);
        infosel--;
    }
    if(zbutton && buy == 0 && sell == 0){
        switch(infosel){
            case 0:
                PlaySoundMem(musicHandle[51], DX_PLAYTYPE_BACK);
                buy = 1;
                sb.init(leftbutton, rightbutton, upbutton, downbutton, zbutton, xbutton);
                ztrigger = false;
                break;
            case 1:
                PlaySoundMem(musicHandle[51], DX_PLAYTYPE_BACK);
                sell = 1;
                sb.init(leftbutton, rightbutton, upbutton, downbutton, zbutton, xbutton);
                ztrigger = false;
                break;
            case 2:
                PlaySoundMem(musicHandle[52], DX_PLAYTYPE_BACK);
                nextline = true;
                shopping = false;
                *shadowrate = 0.0;
                break;
        }
    }
    if(xbutton && buy == 0 && sell == 0){    
        PlaySoundMem(musicHandle[52], DX_PLAYTYPE_BACK);
        nextline = true;
        shopping = false;
        *shadowrate = 0.0;
    }
    if(buy || sell){
        //ウィンドウ表示
        makewindow(WinX/2, 0, WinX / 128, WinY/64 - 6, windowGraphic);
        //選択アイテム欄の色変更
        DrawBox(70 + WinX/2, (shopsel - shoporigin) * 100 + 64 - 10, 70 + (WinX-150), (shopsel - shoporigin) * 100 + 64 - 10 + 64, GetColor(254, 254, 254), TRUE);
        if(buy){
            //アイテム名表示
            for(int i = 0; i < 5; i++){
                if(shopitem[i + shoporigin].ID != 0){
                    char itemname[64];
                    getstr(itemname, shopitemname, i + shoporigin, '\n');
                    DrawGraph(70 + WinX/2, i * 100 + 64 - 10, itemicon[shopitem[i + shoporigin].icon], TRUE);
                    if(shopsel == i + shoporigin){
                        DrawStringToHandle(140 + WinX/2, i * 100 + 64, itemname, GetColor(50, 50, 50), font_mes);
                        DrawFormatStringToHandle(140 + 300 + WinX/2 + 57 - GetDrawFormatStringWidthToHandle(font_mes, "%dG", shopitem[i + shoporigin].money), i * 100 + 64, GetColor(50, 50, 50), font_mes, "%dG", shopitem[i + shoporigin].money);
                    }else{
                        DrawStringToHandle(140 + WinX/2, i * 100 + 64, itemname, GetColor(255, 255, 255), font_mes);
                        DrawFormatStringToHandle(140 + 300 + WinX/2 + 57 - GetDrawFormatStringWidthToHandle(font_mes, "%dG", shopitem[i + shoporigin].money), i * 100 + 64, GetColor(255, 255, 255), font_mes, "%dG", shopitem[i + shoporigin].money);
                    }
                }
            }
            //矢印表示
            if(shopitem[shoporigin + 5].ID != 0){
                DrawGraph((WinX/4) * 3 - 45, (WinY/64 - 6) * 64 - 44 - 32 + 10, arrowGraphic[0], TRUE);
            }
            if(shoporigin > 0){
                DrawGraph((WinX/4) * 3 - 45, 64 - 20 - 32 - 10, arrowGraphic[1], TRUE);
            }
            //アイテムの説明表示
            msgwindow(enterinput(getstr(tmpstr, shopiteminfo, shopsel)), 0, 19, 4, windowGraphic);
            //買う個数の選択ウィンドウ表示
            static int selnum, selend;
            if(nownumsel){
                if(selend == 0)numberSelect(selnum, selend, (char *)"個数");
                if(selend == 1){
                    if(selnum != 0){
                        snprintf(tmpstr, 256, "%sを%d個で%d円払います。\nよろしいですか？", getstr(tmpstr, shopitemname, shopsel, '\n'), selnum, shopitem[shopsel].money * selnum);
                        msgwindow(tmpstr, 0, 19, 4, windowGraphic);
                        static bool inyesno = true;
                        static int yn;
                        if(inyesno)yn = selectYes_No(true);
                        if(yn == 1){
                            inyesno = false;
                            if(player->money >= shopitem[shopsel].money * selnum){
                                WaitTimer(180);
                                PlaySoundMem(musicHandle[55], DX_PLAYTYPE_BACK);
                                player->itemIncrease(shopitem[shopsel].ID, selnum);
                                player->money -= shopitem[shopsel].money * selnum;
                                nownumsel = false;
                                selend = 0;
                                inyesno = true;
                            }else{
                                static bool tmpztrigger = false;
                                msgwindow((char *)"お金が足りません。", 0, 19, 4, windowGraphic);
                                if(!checkInput(KEY_INPUT_Z))tmpztrigger = true;
                                if(checkInput(KEY_INPUT_Z) && tmpztrigger){
                                    PlaySoundMem(musicHandle[52], DX_PLAYTYPE_BACK);
                                    nownumsel = false;
                                    selend = 0;
                                    inyesno = true;
                                    tmpztrigger = false;
                                }
                            }
                        }else if(yn == -1){
                            selend = 0;
                        }
                    }else{
                        selend = 0;
                        nownumsel = 0;
                    }
                }
            }
            //操作部
            if(downbutton && shopitem[shopsel + 1].ID != 0 && nownumsel == false){
                PlaySoundMem(musicHandle[54], DX_PLAYTYPE_BACK);
                shopsel++;
            }
            if(upbutton && shopsel != 0 && nownumsel == false){
                PlaySoundMem(musicHandle[54], DX_PLAYTYPE_BACK);
                shopsel--;
            }
            if(shopsel >= shoporigin + 5){
                shoporigin++;
            }
            if(shopsel < shoporigin){
                shoporigin--;
            }
            if(zbutton && nownumsel == 0){
                PlaySoundMem(musicHandle[51], DX_PLAYTYPE_BACK);
                printf("%d : %d\n", shopitem[shopsel].ID, player->getItemNum(shopitem[shopsel].ID));
                selnummax = ITEMNUMMAX - player->getItemNum(shopitem[shopsel].ID);
                selnum = 1;
                nownumsel = 1;
            }
            if(xbutton && nownumsel == 0){
                PlaySoundMem(musicHandle[52], DX_PLAYTYPE_BACK);
                shopsel = 0;
                shoporigin = 0;
                buy = 0;
            }
        }
        if(sell){
            for(int i = 0; i < 5; i++){
                if(player->itemID[i + shoporigin] != 0){
                    DrawGraph(70 + WinX/2, i * 100 + 64 - 10, itemicon[atoi(getstr(tmpstr, itemdatabuf->noteget(player->itemID[i + shoporigin]), 1))], TRUE);
                    if(shopsel == i + shoporigin){
                        DrawFormatStringToHandle(140 + WinX/2, i * 100 + 64, GetColor(50, 50, 50), font_mes, "%s ×%d", getstr(tmpstr, itemdatabuf->noteget(player->itemID[i + shoporigin]), 0), player->itemNum[i + shoporigin]);
                        DrawFormatStringToHandle(140 + 300 + WinX/2 + 57 - GetDrawFormatStringWidthToHandle(font_mes, "%dG", atoi(getstr(tmpstr, itemdatabuf->noteget(player->itemID[i + shoporigin]), 6))), i * 100 + 64, GetColor(50, 50, 50), font_mes, "%dG", atoi(getstr(tmpstr, itemdatabuf->noteget(player->itemID[i + shoporigin]), 6)));
                    }else{
                        DrawFormatStringToHandle(140 + WinX/2, i * 100 + 64, GetColor(255, 255, 255), font_mes, "%s ×%d", getstr(tmpstr, itemdatabuf->noteget(player->itemID[i + shoporigin]), 0), player->itemNum[i + shoporigin]);
                        DrawFormatStringToHandle(140 + 300 + WinX/2 + 57 - GetDrawFormatStringWidthToHandle(font_mes, "%dG", atoi(getstr(tmpstr, itemdatabuf->noteget(player->itemID[i + shoporigin]), 6))), i * 100 + 64, GetColor(255, 255, 255), font_mes, "%dG", atoi(getstr(tmpstr, itemdatabuf->noteget(player->itemID[i + shoporigin]), 6)));
                    }
                }
            }
            //メッセージウィンドウ
            if(nownumsel == false)msgwindow((char *)"売却するアイテムを選択してください。", 0, 19, 4, windowGraphic);
            static int selnum, selend;
            if(nownumsel){
                if(selend == false)msgwindow((char *)"売却する個数を選択してください。", 0, 19, 4, windowGraphic);
                if(selend == 0)numberSelect(selnum, selend, (char *)"売却数");
                if(selend == 1){
                    if(selnum != 0){
                        snprintf(tmpstr, 256, "%sを%d個売って%d円もらいます。\nよろしいですか？", getstr(tmpstr, itemdatabuf->noteget(player->itemID[shopsel]), 0), selnum, atoi(getstr(tmpstr, itemdatabuf->noteget(player->itemID[shopsel]), 6)) * selnum);
                        msgwindow(tmpstr, 0, 19, 4, windowGraphic);
                        static bool inyesno = true;
                        static int yn;
                        if(inyesno)yn = selectYes_No(true);
                        if(yn == 1){
                            inyesno = false;
                            WaitTimer(180);
                            PlaySoundMem(musicHandle[55], DX_PLAYTYPE_BACK);
                            player->money += atoi(getstr(tmpstr, itemdatabuf->noteget(player->itemID[shopsel]), 6)) * selnum;
                            player->itemDecrease(player->itemID[shopsel], selnum);
                            // player->itemNum[shopsel] -= selnum;
                            // player->itemCheck();
                            if(player->itemID[shopsel] == 0 && shopsel >= 1)shopsel--;
                            printf("%d\n", player->itemID[shopsel]);
                            nownumsel = false;
                            selend = 0;
                            inyesno = true;
                        }else if(yn == -1){
                            selend = 0;
                        }
                    }else{
                        selend = 0;
                        nownumsel = 0;
                    }
                }
            }
            //操作部
            if(downbutton && player->itemID[shopsel + 1] != 0 && nownumsel == false){
                PlaySoundMem(musicHandle[54], DX_PLAYTYPE_BACK);
                shopsel++;
            }
            if(upbutton && shopsel != 0 && nownumsel == false){
                PlaySoundMem(musicHandle[54], DX_PLAYTYPE_BACK);
                shopsel--;
            }
            if(shopsel >= shoporigin + 5){
                shoporigin++;
            }
            if(shopsel < shoporigin){
                shoporigin--;
            }
            if(zbutton && nownumsel == 0 && player->itemID[shopsel] != 0){
                PlaySoundMem(musicHandle[51], DX_PLAYTYPE_BACK);
                printf("%d : %d\n", player->itemID[shopsel], player->itemNum[shopsel]);
                selnummax = player->itemNum[shopsel];
                selnum = 1;
                nownumsel = 1;
            }
            if(xbutton && nownumsel == 0){
                PlaySoundMem(musicHandle[52], DX_PLAYTYPE_BACK);
                shopsel = 0;
                shoporigin = 0;
                sell = 0;
            }
        }
    }
}
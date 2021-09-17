#include "menu.h"
#include "makewindow.h"
#include "input.h"

Menu::Menu(PLAYER *p, Note *idb, float *sr, int *WH, int *MBH, int *MH){
    player = p;
    itemdatabuf = idb;
    shadowrate = sr;
    font_menu = CreateFontToHandle("ＭＳ　ゴシック", 45, -1, DX_FONTTYPE_ANTIALIASING);
    window_graphic = WH;
    buffer_handle = MBH;
    music_handle = MH;
}

void Menu::menuInit(){
    for(int i = 0; i < 6; i++){
        menubarX[i] = -484.0;
    }

    linenum = 0;
    *shadowrate = 0.0;
    inmenu = true;
}

void Menu::menu(){
    char line[1024];
    for(int i = 0; i < 6; i++){
        DrawGraph(menubarX[i], i * 80 + 64, buffer_handle[4], TRUE);
        getstr(line, (char *)menustr, i, '\n');
        DrawStringToHandle(menubarX[i] + 20, i * 80 + 74, line, GetColor(100, 100, 100), font_menu);
        DrawStringToHandle(menubarX[i] + 20, i * 80 + 74, line, GetColor(255, 255, 255), font_menu);
    }

    getstr(line, (char *)menuinfo, linenum, '\n');
    msgwindow(line, 0, 14, 4, window_graphic);

    for(int i = 0; i < 6; i++){
        if(i != linenum){
            menubarX[i] += (64.0 - menubarX[i]) * 0.1;
        }else{
            menubarX[i] += (32.0 - menubarX[i]) * 0.1;
        }
    }

    sb.getSysButton(leftbutton, rightbutton, upbutton, downbutton, zbutton, ztrigger, xbutton, xtrigger);

    if(downbutton){
        linenum++;
        PlaySoundMem(music_handle[54], DX_PLAYTYPE_BACK);
    }
    if(upbutton){
        linenum--;
        PlaySoundMem(music_handle[54], DX_PLAYTYPE_BACK);
    }
    if(linenum < 0) linenum = 5;
    linenum = linenum % 6;




    *shadowrate += (130 - *shadowrate) * 0.1;

    if(checkInput(KEY_INPUT_ESCAPE) == 0)menutrigger = false;

    if((checkInput(KEY_INPUT_X) && menutrigger == false && xbutton) || (checkInput(KEY_INPUT_ESCAPE) && menutrigger == 0)){
        PlaySoundMem(music_handle[52], DX_PLAYTYPE_BACK);
        menutrigger = true;
        *shadowrate = 0.0;
        inmenu = false;
    }

    sb.init(leftbutton, rightbutton, upbutton, downbutton, zbutton, xbutton);

}
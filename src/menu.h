#ifndef MENU_H
#define MENU_H

#include <DxLib.h>
#include "struct.h"
#include "note.h"
#include "sysbutton.h"

class Menu{
    public:
        Menu(PLAYER *p, Note *idb, float *sr, int *WH, int *MBH, int *MH);
        void menuInit();
        void menu();
        bool isInMenu(){return inmenu;};
        void setMenuTrigger(bool arg){menutrigger = arg;};
        bool getMenuTrigger(){return menutrigger;};
    private:
        PLAYER *player;
        Note *itemdatabuf;
        SysButton sb;
        float menubarX[6];
        int linenum;
        int leftbutton, rightbutton, upbutton, downbutton, zbutton, xbutton;
        bool ztrigger, xtrigger;
        bool menutrigger;
        bool inmenu = false;
        float *shadowrate;
        const char *menustr = "アイテム\n装備\nクエスト\n強さ\nセーブ\nオプション";
        const char *menuinfo = "所持しているアイテムを使用します。\n装備している武器・防具を変更します。\n現在受けているクエストを確認します。\n戦闘におけるステータスの詳細を表示します。\n冒険の記録を行います。\nゲーム環境をいじります。";
        int font_menu;
        int *buffer_handle;
        int *window_graphic;
        int *music_handle;
};

#endif
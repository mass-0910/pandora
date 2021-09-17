#ifndef EVENT_H
#define EVENT_H

#include "define.h"
#include "struct.h"
#include "note.h"
#include "sysbutton.h"

typedef struct{
    char name[EVENTNAMEMAX];
    int x, y;
    float realX, realY;
    int type;
    int trigger;
    int vector;
    int spritenum;
    int anime;
    int animepage;
    int animeframe;
    int pagenum;
    int flagnum[PAGEMAX];
    int page;
}EVENT;

class EventManager{
    public:
        EventManager(float *camX, float *camY, PLAYER *p, float *shadow, int *MH, int *WH, int *SH, Note *IB, int *II, int *AG, char *MN);
        int getEventNum(){return eventnum;};
        int getEventX(int num){return event[num].x;};
        int getEventY(int num){return event[num].y;};
        int getEventType(int num){return event[num].type;};
        int getEventTrigger(int num){return event[num].trigger;};
        bool isEventRunning(){return eventrunning;};
        void eventLoad(char *mapname);
        void flagCheck();
        void runEventInit(int eventID, int pvector);
        void runEvent(bool& oktrigger);
        void draw();
    private:
        void shop(char *runeventline);
        void numberSelect(int& selnum, int& selend, char *text);
        int selectYes_No(bool initialize);
        EVENT event[EVENTMAX];
        Note eventtext;
        Note *itemdatabuf;
        PLAYER *player;
        SysButton sb;
        SHOPITEM shopitem[SHOPITEMMAX];
        int eventnum;
        int runeventnum;
        int readinglinenum;
        int mesframe = 0;
        int cammovenum, cammovespeed;
        int movednum;
        int infosel;
        int shopsel;
        int shoporigin;
        int buy;
        int sell;
        int leftbutton, rightbutton, upbutton, downbutton, zbutton, xbutton;
        int selnummax;
        const char *shopmenustr = "買う,売る,やめる";
        const char *shopinfostr = "商品を買います。,持っているアイテムを売ります。,買い物を終了します。";
        float *cameraX, *cameraY;
        int *musicHandle;
        int *windowGraphic;
        int *spriteHandle;
        int *itemicon;
        int *arrowGraphic;
        float *shadowrate;
        float fadeend, fadespeed;
        char outstr[1024];
        char endstr[1024];
        char cammovevector[4];
        char shopitemname[512];
        char shopiteminfo[2048];
        char *mapname;
        bool gameFlag[FLAGMAX] = {false};
        bool eventrunning = false;
        bool nextline = false;
        bool eveinit = true;
        bool ztrigger = false, xtrigger = false;
        bool shopping = false;
        bool nownumsel = false;
};

#endif
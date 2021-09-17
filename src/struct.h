#ifndef STRUCT_H
#define STRUCT_H

#include "define.h"

typedef enum{
    success,
    failure,
}exception;

typedef enum{
    mapwalk,
    battle
}gamemode;

typedef struct{
    int chipID;
    int mapdata;
}MAP;

typedef struct{
    int imageHandle;
    int attackImageHandle;
    int damageImageHandle;
}BATTLE_CHARAIMAGE;

typedef struct{
    int weaponID;
    int armorID;
}EQUIPMENTS;

typedef struct{
    int magicID[64];
    int skillID[64];
}SKILLS;

typedef struct{
    int level;
    int HP, maxHP;
    int MP, maxMP;
    int AT, DF, Speed;
}STATUS;

typedef struct{
    int ID;
    char name[ENEMYNAMEMAX];
    int picsize;
    int level;
    int type;
    int x, y, z;
    STATUS status;
    int EXP;
    int money;
}ENEMY;

typedef struct{
    BATTLE_CHARAIMAGE image;
    EQUIPMENTS equipments;
    STATUS status;
    SKILLS skill;
}PLAYER_CHARACTER;

typedef struct{
    void itemIncrease(int itemID, int num);
    void itemDecrease(int itemID, int num);
    int getItemNum(int ID);
    int x, y;
    float realX, realY;
    int XV, YV;
    int realXV, realYV;
    int money;
    int pagewait;
    int pagenum;
    int page;
    int itemID[ITEMMAX];
    int itemNum[ITEMMAX];
    PLAYER_CHARACTER character[4];
    int character_num;
    private:
    void itemCheck();
}PLAYER;

typedef struct{
    int ID;
    int icon;
    int money;
    int buymoney;
}SHOPITEM;

#endif
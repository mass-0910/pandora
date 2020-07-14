#include "struct.h"
#include "define.h"

void PLAYER::itemIncrease(int ID, int num){
    for(int i = 0; i < ITEMMAX; i++){
        if(itemID[i] == ID){
            itemNum[i] += num;
            break;
        }
        if(itemID[i] == 0){
            itemID[i] = ID;
            itemNum[i] = num;
            break;
        }
    }
    itemCheck();
}

void PLAYER::itemDecrease(int ID, int num){
    for(int i = 0; i < ITEMMAX; i++){
        if(itemID[i] == ID){
            itemNum[i] -= num;
            break;
        }
        if(itemID[i] == 0){
            break;
        }
    }
    itemCheck();
}

void PLAYER::itemCheck(){
    for(int i = 0; i < ITEMMAX; i++){
        if(itemNum[i] <= 0){
            itemID[i] = 0;
        }
    }

    for(int i = 0; i < ITEMMAX; i++){
        if(itemID[i] == 0){
            if(i < ITEMMAX - 1){
                itemID[i] = itemID[i + 1];
                itemNum[i] = itemNum[i + 1];
                itemID[i + 1] = 0;
                itemNum[i + 1] = 0;
            }
        }
    }
}

int PLAYER::getItemNum(int ID){
    for(int i = 0; i < ITEMMAX; i++){
        if(itemID[i] == ID){
            return itemNum[i];
        }
        if(itemID[i] == 0)return 0;
    }
    return 0;
}
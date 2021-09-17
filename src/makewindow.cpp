#include "makewindow.h"

void makewindow(int x, int y, int sizeX, int sizeY, int *graphicHandle){
    int curposX, posX, posY;
    curposX = x;
    posY = y;
    for(int yi = 0; yi < sizeY; yi++){
        posX = curposX;
        if(yi == 0){
            for(int xi = 0; xi < sizeX; xi++){
                if(xi == 0){
                    DrawGraph(posX, posY, graphicHandle[0], TRUE);
                }else if(xi == sizeX - 1){
                    DrawGraph(posX, posY, graphicHandle[1], TRUE);
                }else{
                    DrawGraph(posX, posY, graphicHandle[5], TRUE);
                }
                posX += 64;
            }
        }else if(yi == sizeY - 1){
            for(int xi = 0; xi < sizeX; xi++){
                if(xi == 0){
                    DrawGraph(posX, posY, graphicHandle[3], TRUE);
                }else if(xi == sizeX - 1){
                    DrawGraph(posX, posY, graphicHandle[2], TRUE);
                }else{
                    DrawGraph(posX, posY, graphicHandle[7], TRUE);
                }
                posX += 64;
            }
        }else{
            for(int xi = 0; xi < sizeX; xi++){
                if(xi == 0){
                    DrawGraph(posX, posY, graphicHandle[4], TRUE);
                }else if(xi == sizeX - 1){
                    DrawGraph(posX, posY, graphicHandle[6], TRUE);
                }else{
                    DrawGraph(posX, posY, graphicHandle[8], TRUE);
                }
                posX += 64;
            }
        }
        posY += 64;
    }
}

void msgwindow(char *word, int position, int sizeX, int sizeY, int *windowhandle){
    makewindow(0, WinY - 320, sizeX, sizeY, windowhandle);
    DrawStringToHandle(45, WinY - 287, word, GetColor(37, 45, 114), font_mes);
    DrawStringToHandle(40, WinY - 290, word, GetColor(255, 255, 255), font_mes);
}
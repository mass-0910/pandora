#ifndef MAKEWINDOW_H
#define MAKEWINDOW_H

#include "DxLib/DxLib.h"
#include "define.h"

extern int font_mes;

void makewindow(int x, int y, int sizeX, int sizeY, int *graphicHandle);
void msgwindow(char *word, int position, int sizeX, int sizeY, int *windowhandle);

#endif
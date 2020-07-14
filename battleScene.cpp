#include "battleScene.h"
#include "makewindow.h"

void BattleScene::init(BATTLEDATA bdata){
    this->bdata = bdata;
    this->phase = command;
}

void BattleScene::update(){

    //味方キャラの更新
    for(int i = 0; i < bdata.pdata->character_num; i++){
        character[i].update();
    }
}

void BattleScene::draw(){
    
    //背景画像
    DrawGraph(0, 0, bdata.backPicHandle, FALSE);

    //味方キャラの描画
    for(int i = 0; i < bdata.pdata->character_num; i++){
        character[i].draw();
    }

    //コマンド時ウインドウ表示
    if(phase == command){
        makewindow(200, WinY - 300, 19, 4);
    }
}
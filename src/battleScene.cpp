#include "battleScene.h"
#include "makewindow.h"

BattleScene::BattleScene(int *buffer_handle, int *music_handle, MAP map[MAPMAX + 2][MAPMAX + 2], int *windowGraphic){
    this->buffer_handle = buffer_handle;
    this->music_handle = music_handle;
    this->map = map;
    this->windowGraphic = windowGraphic;
}

void BattleScene::init(BATTLEDATA bdata){
    this->bdata = bdata;
    this->phase = phase_command;
    for(int i = 0; i < 4; i++){
        character[i] = new PlayerCharacter(bdata.pdata->character[i]);
    }
}

void BattleScene::update(){

    //味方キャラの更新
    for(int i = 0; i < bdata.pdata->character_num; i++){
        character[i]->update();
    }
}

void BattleScene::draw(){
    
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 0.5F);
    DrawGraph(0, 0, buffer_handle[101], FALSE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    //味方キャラの描画
    for(int i = 0; i < bdata.pdata->character_num; i++){
        character[i]->draw();
    }

    //コマンド時ウインドウ表示
    if(phase == phase_command){
        makewindow(200, WinY - 300, 19, 4, windowGraphic);
    }
}

void BattleScene::end(){
    for(int i = 0; i < 4; i++){
        delete character[i];
    }
}
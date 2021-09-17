#ifndef BATTLESCENE_H
#define BATTLESCENE_H

#include "struct.h"
#include "define.h"
#include <DxLib.h>

typedef enum{
    wait,
    attack,
    damage,
}CHARACTER_STATE;

typedef enum{
    phase_command,
    phase_attack,
    phase_result,
}BATTLE_PHASE;

//味方キャラクター情報をうまいことゲームに反映するクラス
class PlayerCharacter{
    public:
        PlayerCharacter(PLAYER_CHARACTER data){

            character_data = data;
            state = wait;
            GetGraphSize(character_data.image.imageHandle, &img_x, &img_y);
            img_x = img_y;

            //アニメ画像の読み込み(高さ1 x 幅2のアニメマップ画像)
            for(int i = 0; i < 4; i++){
                anime_image[i] = DerivationGraph(img_x * i, 0, img_x, img_y, character_data.image.imageHandle);
                anime_attackImage[i] = DerivationGraph(img_x * i, 0, img_x, img_y, character_data.image.attackImageHandle);
            }

        };

        //更新
        void update(){

            frame++;

        };

        //描画
        void draw(){

            if(state == wait){//待ち状態のとき
                DrawGraph(x - img_x / 2, y - img_y / 2, anime_image[(frame / 10) % 4], TRUE);
            }else if(state == attack){//攻撃状態のとき
                DrawGraph(x - img_x / 2, y - img_y / 2, character_data.image.attackImageHandle, TRUE);
            }else if(state == damage){//ダメージを受けているとき
                DrawGraph(x - img_x / 2, y - img_y / 2, character_data.image.damageImageHandle, TRUE);
            }

        };

        void end(){
            //アニメ画像削除
            for(int i = 0; i < 4; i++){
                DeleteGraph(anime_image[i]);
                DeleteGraph(anime_attackImage[i]);
            }
        }
    private:
        PLAYER_CHARACTER character_data;
        CHARACTER_STATE state;
        int x, y;
        int img_x, img_y;
        int anime_image[4];
        int anime_attackImage[4];
        int frame = 0;
};

typedef struct{
    int backPicHandle;
    int enemynum;
    int enemyID[8];
    PLAYER *pdata;
}BATTLEDATA;

class BattleScene{
    public:
        BattleScene(int *buffer_handle, int *music_handle, MAP map[MAPMAX + 2][MAPMAX + 2], int *windowGraphic);
        void init(BATTLEDATA bdata);
        void update();
        void draw();
        void end();
    private:
        BATTLEDATA bdata;
        PlayerCharacter *character[4];
        BATTLE_PHASE phase;
        int *buffer_handle, *music_handle;
        MAP (*map)[MAPMAX + 2];
        int *windowGraphic;
};

#endif
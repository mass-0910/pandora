#include "input.h"
#include <DxLib.h>

int checkInput(int inputType){
    if(GetJoypadNum() != 0){
        int input = GetJoypadInputState(DX_INPUT_KEY_PAD1);
        switch(inputType){
            case KEY_INPUT_UP:
                return input & PAD_INPUT_UP;
            case KEY_INPUT_DOWN:
                return input & PAD_INPUT_DOWN;
            case KEY_INPUT_LEFT:
                return input & PAD_INPUT_LEFT;
            case KEY_INPUT_RIGHT:
                return input & PAD_INPUT_RIGHT;
            case KEY_INPUT_Z:
                return input & PAD_INPUT_1;
            case KEY_INPUT_X:
                return input & PAD_INPUT_2;
            case KEY_INPUT_ESCAPE:
                return input & PAD_INPUT_9 || input & PAD_INPUT_8;
            default:
                return 0;
        }
    }else{
        return CheckHitKey(inputType);
    }
    return 0;
}
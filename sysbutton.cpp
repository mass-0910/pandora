#include "sysbutton.h"
#include "input.h"

void SysButton::getSysButton(int& leftbutton, int& rightbutton, int& upbutton, int& downbutton, int& zbutton, bool& ztrigger, int& xbutton, bool& xtrigger){
    int aing = 0;
    if(checkInput(KEY_INPUT_LEFT)){
        if(abuttontimer == 0){
            leftbutton = 1;
        }
        aing = 1;
    }else{
        leftbutton = 0;
    }
    if(checkInput(KEY_INPUT_UP)){
        if(abuttontimer == 0){
            upbutton = 1;
        }
        aing = 1;
    }else{
        upbutton = 0;
    }
    if(checkInput(KEY_INPUT_RIGHT)){
        if(abuttontimer == 0){
            rightbutton = 1;
        }
        aing = 1;
    }else{
        rightbutton = 0;
    }
    if(checkInput(KEY_INPUT_DOWN)){
        if(abuttontimer == 0){
            downbutton = 1;
        }
        aing = 1;
    }else{
        downbutton = 0;
    }

    if(aing){
        if(notfirst == 0){
            abuttontimer = 30;
        }
        if(abuttontimer == 0 && notfirst == 1)abuttontimer = 5;
        notfirst = 1;
        if(abuttontimer != 0)abuttontimer--;
    }else{
        abuttontimer = 0;
        notfirst = 0;
    }

    if(!checkInput(KEY_INPUT_Z))ztrigger = true;
    if(checkInput(KEY_INPUT_Z) && ztrigger){
        zbutton = 1;
        ztrigger = false;
    }
    if(!checkInput(KEY_INPUT_X))xtrigger = true;
    if(checkInput(KEY_INPUT_X) && xtrigger){
        xbutton = 1;
        xtrigger = false;
    }
}

void SysButton::init(int& leftbutton, int& rightbutton, int& upbutton, int& downbutton, int& zbutton, int& xbutton){
    leftbutton = 0;
    upbutton = 0;
    rightbutton = 0;
    downbutton = 0;
    zbutton = 0;
    xbutton = 0;
}
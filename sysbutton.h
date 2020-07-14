#ifndef SYSBUTTON_H
#define SYSBUTTON_H

#include "DxLib/DxLib.h"

class SysButton{
    public:
        void getSysButton(int& leftbutton, int& rightbutton, int& upbutton, int& downbutton, int& zbutton, bool& ztrigger, int& xbutton, bool& xtrigger);
        void init(int& leftbutton, int& rightbutton, int& upbutton, int& downbutton, int& zbutton, int& xbutton);
    private:
        int abuttontimer = 0;
        int notfirst = 0;
};

#endif
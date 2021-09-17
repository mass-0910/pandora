#ifndef NOTE_H
#define NOTE_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

char *getstr(char *buf, char *str, int num, char split_character = ',');

struct NOTELINE{
    char linebuf[1024];
};

class Note{
    public:
        int noteload(const char *filename);
        int notemax();
        char *noteget(int linenum);
        void delete_note();
        void print_note();
    private:
        std::vector<struct NOTELINE> note;
};

#endif
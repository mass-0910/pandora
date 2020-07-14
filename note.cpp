#include "note.h"
#include <string.h>
using namespace std;

int Note::noteload(const char *filename){
    ifstream ifs(filename);
    string line;
    struct NOTELINE tmp;

    if(ifs.fail()){
        return -1;
    }

    note.clear();

    while(getline(ifs, line)){
        strncpy(tmp.linebuf, line.c_str(), 1024);
        note.push_back(tmp);
    }

    ifs.close();
    return 0;
}

int Note::notemax(){
    return note.size();
}

char *Note::noteget(int linenum){
    return note.at(linenum).linebuf;
}

void Note::delete_note(){
    note.clear();
}

char *getstr(char *buf, char *str, int num, char split_character){
    int str_index = 0, buf_index = 0;
    while(num != 0){
        if(str[str_index] == split_character){
            num--;
        }
        str_index++;
    }
    while(str[str_index] != split_character && str[str_index] != '\0'){
        buf[buf_index] = str[str_index];
        buf_index++;
        str_index++;
    }
    buf[buf_index] = '\0';
    return buf;
}

void Note::print_note(){
    for(const NOTELINE& line : note){
        cout << line.linebuf << endl;
    }
}
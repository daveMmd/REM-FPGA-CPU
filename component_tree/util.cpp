//
// Created by 钟金诚 on 2020/8/13.
//

#include "util.h"
#include "../stdinc.h"

std::list<char *>* read_regexset(char* fname){
    FILE* file = fopen(fname, "r");
    if(file == nullptr) fatal((char*)"read_regexset: file NULL.");
    auto regex_list = new std::list<char *>();
    char tmp[1000];
    while(true){
        if(fgets(tmp, 1000, file)== nullptr) break;
        if(tmp[0]=='#') continue;
        int len=strlen(tmp);
        if(len == 1 && tmp[len-1] == '\n') continue; //jump empty line
        if(tmp[len-1] == '\n')
        {
            tmp[len-1]='\0';
        }
        char *re= (char*)malloc(1000);
        strcpy(re, tmp);
        regex_list->push_back(re);
    }
    fclose(file);
    return regex_list;
}

std::list<char *>* read_regexset_hex(char* fname){
    FILE* file = fopen(fname, "r");
    if(file == nullptr) fatal((char*)"read_regexset: file NULL.");
    auto regex_list = new std::list<char *>();
    char tmp[1000];
    while(true){
        if(fgets(tmp, 1000, file)== nullptr) break;
        if(tmp[0]=='#') continue;
        int len=strlen(tmp);
        if(len == 1 && tmp[len-1] == '\n') continue; //jump empty line
        if(tmp[len-1] == '\n')
        {
            tmp[len-1]='\0';
        }
        char *re= (char*)malloc(1000);
        //strcpy(re, tmp);
        for(int i=0; i<strlen(tmp); i+=2){
            char hexs[5];
            hexs[0] = tmp[i];
            hexs[1] = tmp[i+1];
            hexs[2] = '\0';
            int ascii_value = (int)strtol(hexs, nullptr, 16);
            char c;
            if(ascii_value < 128) c = ascii_value;
            else c = (ascii_value - 256);
            re[i/2] = c;
        }
        re[strlen(tmp)/2] = '\0';

        regex_list->push_back(re);
    }
    fclose(file);
    return regex_list;
}
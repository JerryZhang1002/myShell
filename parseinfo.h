#ifndef parseinfo_h
#define parseinfo_h

#include "parse.h"


#define IN_REDIRECT     1
#define OUT_REDIRECT    2
#define ERR_REDIRECT    4
#define IS_PIPED        8

typedef struct {
    int flag;
    char * in_file;       //remember to free
    char * out_file;      //remember to free
    char * err_file;
    char * command2;      //malloc new space
    params * parameters2; //malloc new space
} parse_info;

void parse_info_init(parse_info *info);
void parseInfo(params * parameter, parse_info * info);
void deleteParseInfo(parse_info *info);





#endif /* parseinfo_h */

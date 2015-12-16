#ifndef parse_h
#define parse_h
#include <string>
using namespace std;

typedef struct {
    char ** argv;
    size_t len;
} params;

string commandWithNoPreSpace(string inputCommand);
char * getCommandName (string inputCommane);
char * getProgramName (string inputCommand);
char * getParamsPartOfCommand (string inputCommand);
//char ** getParams (string inputCommand);
params * getParams (string inputCommand);
void deleteParam(params * parameter);
void printParam(params * parameter);
char * getDir(string inputCommand);
#endif /* parse_h */

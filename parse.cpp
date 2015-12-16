#include <stdio.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include "string.h"

#include "parse.h"
#include "findProgram.h"
using namespace std;

string commandWithNoPreSpace(string inputCommand) {
    size_t posNotSpace;
    size_t len = inputCommand.length();
    if (inputCommand == "") {
        return "";
    }
    posNotSpace = inputCommand.find_first_not_of(" ");
    if (posNotSpace == string::npos) {                         //if all whitespace, return NULL
        return "";
    }
    size_t commandLen = len - posNotSpace;
    string command(inputCommand, posNotSpace, commandLen);
    return command;
}

char * getCommandName (string inputCommane) {
    size_t posSpace;
    string command = commandWithNoPreSpace(inputCommane);
    if (command == "") {
        return NULL;
    }
    posSpace = command.find_first_of(" ");
    if (posSpace == string::npos) {
        return strdup(command.c_str());
    }
    else {
        string str(command, 0, posSpace);
        return strdup(str.c_str());
    }
}

char * getProgramName (string inputCommand) {
    size_t posSpace;
    size_t posSlash;
    string command = commandWithNoPreSpace(inputCommand);
    if (command == "") {
        return NULL;
    }
    posSpace = command.find_first_of(" ");
    if (posSpace == string::npos) {                           //no white space following the command, means no arguments
        posSlash = command.find_last_of("/");
        if (posSlash == string::npos) {
            return strdup(command.c_str());                            //remember to free the space
        }
        else {
            string commandName(command, posSlash + 1, command.length() - posSlash - 1);
            return strdup(commandName.c_str());
        }
    }
    else {
        string commandNamePart(command, 0, posSpace);
        posSlash = commandNamePart.find_last_of("/");
        if (posSlash == string::npos) {
            return strdup(commandNamePart.c_str());
        }
        else {
            string commandName(command, posSlash + 1, posSpace - posSlash - 1);
            return strdup(commandName.c_str());
        }
    }
    return NULL;
}

char * getParamsPartOfCommand (string inputCommand) {
    size_t posSpace;
    size_t posNotSpace;
    string command = commandWithNoPreSpace(inputCommand);
    if (command == "") {
        return NULL;
    }
    posSpace = command.find_first_of(" ");
    if (posSpace == string::npos) {                          //withoud param
        return NULL;
    }
    string paramsPart(command, posSpace + 1, command.length() - posSpace - 1);
    posNotSpace = paramsPart.find_first_not_of(" ");
    if (posNotSpace == string::npos) {                       //paramsPart are all spaces
        return NULL;
    }
    string params(paramsPart, posNotSpace, paramsPart.length() - posNotSpace);
    return strdup(params.c_str());
}

params * getParams (string inputCommand) {
    params * parameter = (params*)malloc(sizeof(*parameter));
    size_t argc = 1;
    size_t len = 0;
    char ** argv = NULL;
    char * start = NULL;
    char * cur = NULL;
    char * end = NULL;
    char temp = '\0';
    string param = "";
    argv = (char **)realloc(argv, argc * sizeof(*argv));
    argv[0] = NULL;
    char * commandName = getCommandName(inputCommand);
    if (strchr(commandName, '/') == NULL) {
        string path = "PATH";
        env * envVal = getEnv(path.c_str());
        argv[0] = findProgram(commandName, envVal);
        deleteEnv(envVal);
        free(commandName);
    }
    else {
        argv[0] = commandName;
    }
    char * paramsPart = getParamsPartOfCommand(inputCommand);                    //paramsPart in heap
    if (paramsPart == NULL) {
        parameter->argv = argv;
        parameter->len = argc;
        return parameter;
    }
    len = strlen(paramsPart);
    start = paramsPart;                                                          //initialize the pointers
    cur = start;
    end = start + len;
    while (cur < end) {
        if (strncmp(cur, "\\ ", 2) == 0) {
            temp = ' ';
            param = param + temp;
            cur = cur + 2;
        }
        else if(strncmp(cur, " ", 1) == 0) {
            argv = (char **)realloc(argv, (argc + 1) * sizeof(*argv));
            argv[argc++] = strdup(param.c_str());
            param = "";
            while (strncmp(cur, " ", 1) == 0 && cur < end) {
                cur++;
            }
        }
        else {
            temp = *cur;
            param = param + temp;
            cur = cur + 1;
        }
    }
    if (param != "") {
        argv = (char **)realloc(argv, (argc + 1) * sizeof(*argv));
        argv[argc++] = strdup(param.c_str());
        param = "";
    }
    free(paramsPart);
    parameter->argv = argv;
    parameter->len = argc;
    return parameter;
}

void deleteParam(params * parameter) {
    if (parameter == NULL) return;
    for (size_t i = 0; i < parameter->len; i++) {
        if (parameter->argv[i] != NULL) {
            free(parameter->argv[i]);
        }
    }
    free(parameter->argv);
    free(parameter);
}
void printParam(params * parameter) {
    if (parameter == NULL) {
        return;
    }
    for (size_t i = 0; i < parameter->len; i++) {
        if (parameter->argv[i] == NULL) {
            break;
        }
        cout <<"---"<<parameter->argv[i];
    }
    cout << endl;
}

char * getDir(string inputCommand) {
    size_t posSpace;
    size_t posNotSpace;
    size_t lastPosNotSpace;
    string command = commandWithNoPreSpace(inputCommand);
    posSpace = command.find_first_of(" ");
    if (posSpace == string::npos) {
        return NULL;                             //return NULL if no parameter input
    }
    string temp = command.substr(posSpace);
    posNotSpace = temp.find_first_not_of(" ");
    if (posNotSpace == string::npos) {
        return NULL;                             //parameter part is all whitespace
    }
    lastPosNotSpace = temp.find_last_not_of(" ");
    string dir = temp.substr(posNotSpace, lastPosNotSpace - posNotSpace + 1);
    return strdup(dir.c_str());
}

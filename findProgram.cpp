#include <stdio.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <string.h>

#include "findProgram.h"

using namespace std;

env * getEnv(const char * key) {
    char * colonPos = NULL;
    char * cur = NULL;
    env * envVal = (env *)malloc(sizeof(*envVal));
    envVal->len = 0;
    envVal->envs = NULL;
    char * val = getenv(key);
    if (val == NULL) {
        return envVal;
    }
    colonPos = strchr(val, ':');
    cur = val;
    while (colonPos != NULL) {
        char * environment = strndup(cur, colonPos - cur);             //'\0'is added automatically
        envVal->envs = (char**)realloc(envVal->envs, (envVal->len + 1) * sizeof(*(envVal->envs)));
        envVal->envs[(envVal->len)++] = environment;
        cur = colonPos + 1;
        if (cur == val + strlen(val)) {
            break;
        }
        colonPos = strchr(cur, ':');
    }
    return envVal;
}

    char * findProgram(const char * proName, env * envVal) {
    string program = "";
    string curDir = "";
    struct dirent * dirp;
    struct stat filestat;
    DIR * dp;
    for (size_t i = 0; i < envVal->len; i++) {
        dp = opendir(envVal->envs[i]);
        curDir = envVal->envs[i];
        if (dp == NULL) {
            cout << "Error opening " << envVal->envs[i] << endl;
            exit(1);
        }
        while ((dirp = readdir(dp))) {
            if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
                continue;
            }
            program = curDir + "/" + dirp->d_name;
            //If the file is invalid, just skip it
            if (lstat(program.c_str(), &filestat) < 0) {
                continue;
            }
            if (S_ISDIR(filestat.st_mode)) {
                continue;
            }
            if (strcmp(proName, dirp->d_name) == 0) {
                closedir(dp);
                return strdup(program.c_str());
            }
        }
        closedir(dp);
    }
    return NULL;
}

void deleteEnv(env * envVal) {
    if (envVal == NULL) {
        return;
    }
    for (size_t i = 0; i < envVal->len; i++) {
        free(envVal->envs[i]);
    }
    free(envVal->envs);
    free(envVal);
}

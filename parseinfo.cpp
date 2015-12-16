#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "parseinfo.h"
#include "parse.h"
#include "findProgram.h"

void parse_info_init(parse_info *info) {
    info->flag = 0;
    info->in_file = NULL;
    info->out_file = NULL;
    info->err_file = NULL;
    info->command2 = NULL;
    info->parameters2 = NULL;
}

void deleteParseInfo(parse_info *info) {
    if (info == NULL) return;
    free(info);
}

void parseInfo(params * parameter, parse_info * info) {
 //   printParam(parameter);
    env * envVal = NULL;
    string path = "PATH";
    size_t i = 0;
    parse_info_init(info);
    while (i < parameter->len) {
        if (i == 0 &&parameter->argv[i] == NULL) {
            i++;
            continue;
        }
        if (strcmp(parameter->argv[i], "<") == 0) {
            info->flag |= IN_REDIRECT;
            info->in_file = parameter->argv[i + 1];
            free(parameter->argv[i]);
            parameter->argv[i] = NULL;
            i += 2;
            continue;
        }
        else if (strcmp(parameter->argv[i], ">") == 0) {
            info->flag |= OUT_REDIRECT;
            info->out_file = parameter->argv[i + 1];
            free(parameter->argv[i]);
            parameter->argv[i] = NULL;
            i += 2;
            continue;
        }
        else if (strcmp(parameter->argv[i], "2>") == 0) {
            info->flag |= ERR_REDIRECT;
            info->err_file = parameter->argv[i + 1];
            free(parameter->argv[i]);
            parameter->argv[i] = NULL;
            i += 2;
            continue;
        }
        else if (strcmp(parameter->argv[i], "|") == 0) {
            info->flag |= IS_PIPED;
            free(parameter->argv[i]);
            parameter->argv[i] = NULL;
            info->command2 = parameter->argv[i + 1];
            if (strchr(info->command2, '/') == NULL) {
                envVal = getEnv(path.c_str());
                char * program = findProgram(parameter->argv[i + 1], envVal);
                deleteEnv(envVal);
                info->command2 = program;
            }
            else {
                info->command2 = strdup(parameter->argv[i + 1]);
            }
            free(parameter->argv[i + 1]);
            parameter->argv[i + 1] = NULL;
            // set parameters for command2
            info->parameters2 = (params *)malloc(sizeof(*(info->parameters2)));
            info->parameters2->len = 0;
            info->parameters2->argv = NULL;
            info->parameters2->argv = (char **)realloc(info->parameters2->argv, ((info->parameters2->len) + 1) * sizeof(*( info->parameters2->argv)));
            info->parameters2->argv[info->parameters2->len] = getProgramName(info->command2);
            info->parameters2->len += 1;
            i = i + 2;
            while (i < parameter->len) {
                info->parameters2->argv = (char **)realloc(info->parameters2->argv, ((info->parameters2->len) + 1) * sizeof(*(info->parameters2->argv)));
                info->parameters2->argv[info->parameters2->len] = strdup(parameter->argv[i]);
                info->parameters2->len += 1;
                i++;
            }
            continue;
        }
        i++;
    }
}

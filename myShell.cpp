#include <iostream>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdio.h>
#include <string>
#include <cstdlib>
#include "string.h"
#include "stack.h"
#include "parse.h"
#include "findProgram.h"
#include "parseinfo.h"
#include <fcntl.h>

using namespace std;

void runsource(int pfd[], char * command, char ** argv, parse_info *info, size_t len);
void runmiddle(int in_pfd[], int out_pfd[], char * command, char ** argv, parse_info *info, size_t len);
void rundest(int pfd[], char * command, char** argv, parse_info *info, size_t len);

int main () {
    string inputCommand;
    size_t pos;
    env * envVal = NULL;
    params * parameter = NULL;
    string path = "PATH";
    dirStack * s = initStack();
    parse_info *info = NULL;
    parse_info *info_dest = NULL;
    while (1) {
        char * temp = getcwd(NULL, 0);
        string curPath(temp);
        free(temp);
        cout << "myShell: " + curPath + " $ ";
        if (getline(cin, inputCommand)) {
            if (inputCommand == "exit") {
                break;
            }
            else if (inputCommand == "") {
                continue;
            }
            else if ((pos = inputCommand.find_first_not_of(" ")) == string::npos) {
                continue;
            }
        }
        else break;
        char * proName = getProgramName(inputCommand);
        if (strcmp(proName, "cd") == 0) {
            char * dir = getDir(inputCommand);
            if (dir == NULL) {
                cerr << "Error: Input directory part is empty." << endl;
            }
            else {
                if (chdir(dir) == -1) {
                    cerr << "Error: Could not open the input directory" << endl;
                }
                free(dir);
            }
            free(proName);
        }
        else if(strcmp(proName, "pushd") == 0) {
            char * dir = getDir(inputCommand);
            if (dir == NULL) {
                cout << "Error: Input directory part is empty." << endl;
                free(proName);
                continue;
            }
            else {
                if (chdir(dir) == -1) {
                    cerr << "Error: Could not open the input directory" << endl;
                    free(dir);
                    free(proName);
                }
                else {
                    free(dir);
                    dir = getcwd(NULL, 0);
                    push(s, dir);
                    free(proName);
                }
            }

        }        
/*        else if(strcmp(proName, "pushd") == 0) {
            char * dir = getDir(inputCommand);
            if (dir == NULL) {
                cout << "Error: Input directory part is empty." << endl;
            }
            else {
                if (chdir(dir) == -1) {
                    cout << "Error: Could not open the input directory." << endl;
                }
                else {
                    free(dir);
                    dir = getcwd(NULL, 0);
                    push(s, dir);
                }
            }
            free(proName);
        }    */
        else if (strcmp(proName, "popd") == 0) {
            char * temp = pop(s);
            if (temp == NULL) {
                cout << "Error: Dir Stack is empty." << endl;
            }
            else {
                if (chdir(temp) == -1) {
                    cout << "Error: Could not open the input directory." << endl;
                }
                free(temp);
            }
            free(proName);
        }
        else if (strcmp(proName, "dirstack") == 0) {
            int saved_stdout = dup(1);
            int saved_stdin = dup(0);
            parameter = getParams(inputCommand);
            if (info != NULL) deleteParseInfo(info);
            info = (parse_info *) malloc(sizeof(*info));
            parseInfo(parameter, info);
            if ((info->flag & OUT_REDIRECT) != 0) {
                int out_fd = open(info->out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                close(fileno(stdout));
                dup2(out_fd, fileno(stdout));
                close(out_fd);
                printStack(s);
                dup2(saved_stdin, 0);      //restore stdin
                close(saved_stdin);
                dup2(saved_stdout, 1);     //restore stdout
                close(saved_stdout);
            }
            else {
                printStack(s);  
            }
            deleteParam(parameter);
            parameter = NULL;
            deleteParseInfo(info);
            info = NULL;
 //           else printStack(s);
            free(proName);
        }
        else {
            free(proName);
            pid_t pid;
            parameter = getParams(inputCommand);
            if (parameter->argv[0] == NULL){
		deleteParam(parameter);
		parameter = NULL;	
	        continue;
            }
            if (info != NULL) deleteParseInfo(info);
            info = (parse_info *) malloc(sizeof(*info));
            parseInfo(parameter, info);
            if ((info->flag & IS_PIPED) == 0) {     
                pid = fork();
                if (pid < 0) {
                    cerr << "Error: fork() failed!" << endl;
                    exit(1);
                }
                if (pid == 0) {
                    char * commandName = getCommandName(inputCommand);
                    char ** argv = NULL;
                    size_t len = parameter->len;
                    size_t i = 0;
                    for (i = 0; i < parameter->len; i++) {
                        if(parameter->argv[i] != NULL) {
                            argv = (char**)realloc(argv, (i + 1) * sizeof(*argv));
                            argv[i] = strdup(parameter->argv[i]);
                        }
                        else break;
                    }
                    argv = (char**)realloc(argv, (i + 1) * sizeof(*argv));
                    argv[i] = NULL;
                    if (info->flag & IN_REDIRECT) {
                        int in_fd = open(info->in_file, O_RDONLY|O_CREAT, 0666);
                        close(fileno(stdin));
                        dup2(in_fd, fileno(stdin));
                        close(in_fd);
                    }
                    if ((info->flag & OUT_REDIRECT) != 0) {
//                        cout << "Output file is: " << info->out_file << endl;
                        int out_fd = open(info->out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                        close(fileno(stdout));
                        dup2(out_fd, fileno(stdout));
                        close(out_fd);
                    }
                    if ((info->flag & ERR_REDIRECT) != 0) {
                        int err_fd = open(info->err_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                        close(fileno(stderr));
                        dup2(err_fd, fileno(stderr));
                        close(err_fd);
                    }
                    deleteParam(parameter);
                    parameter = NULL;
                    if (strchr(commandName, '/') == NULL) {
                        envVal = getEnv(path.c_str());
                        char * program = findProgram(commandName, envVal);
                        deleteEnv(envVal);
                        envVal = NULL;
                        if (program == NULL) {
                            cerr << "Command " << commandName <<" not found" << endl;
                            free(commandName);
                            for (size_t i = 0; i <= len; i++) {
                                if (argv[i] != NULL) free(argv[i]);       
                                else break;
                            }
                            free(argv);
                            freeStack(s);
                            deleteParseInfo(info);
                            info = NULL;
                            return 1;
                        }
//                        cout << "Command would be run is:  "<< argv[0]<< endl;
                        execv(argv[0], argv);
                    }
                    else {
//                        cout << "Command would be run is:  "<<commandName << endl;
                        execv(commandName, argv);
                    }
                    free(commandName);
                    for (size_t i = 0; i < len; i++) {
                        if (argv[i] != NULL) free(argv[i]);
                    }
                    free(argv);
                    freeStack(s);
                    deleteParseInfo(info);
                    info = NULL;
                    return 1;
                }
                else {
                    int pid;
                    int status;
                    deleteParam(parameter);
                    parameter = NULL;
                    deleteParseInfo(info);
                    info = NULL;
                    while ((pid = wait(&status)) != -1) {
                        if ((WIFEXITED(status))) {
                            cout << "Program exited with status " << WEXITSTATUS(status) << endl;
                        }
                        else if (WIFSIGNALED(status)) {
                            cout << "Program was killed by signal" << WTERMSIG(status) << endl;
                        }
                    }
                }
            }
            else {     //piped
                int fd[2];
                int fd_2[2];
                bool switch_pipe = true;        //switch between fd[2] and fe_2[2]
                int pid;
                int status;

                char * commandName = getCommandName(inputCommand);
                char ** argv = NULL;
                size_t len = parameter->len;
                size_t i = 0;

                pipe(fd);
                pipe(fd_2);

                for (i = 0; i < parameter->len; i++) {
                    if(parameter->argv[i] != NULL) {
                        argv = (char**)realloc(argv, (i + 1) * sizeof(*argv));
                        argv[i] = strdup(parameter->argv[i]);
                    }
                    else break;
                }
                argv = (char**)realloc(argv, (i + 1) * sizeof(*argv));
                argv[i] = NULL;
                if (strchr(commandName, '/') == NULL) {
                    envVal = getEnv(path.c_str());
                    char * program = findProgram(commandName, envVal);
                    deleteEnv(envVal);
                    envVal = NULL;
                    if (program == NULL) {
                        cerr << "Command " << commandName <<" not found" << endl;
                        for (size_t j = 0; j <= len; j++) {
                            if (argv[j] != NULL) free(argv[j]);
                            else break;
                        }
                        free(argv);
                        deleteParseInfo(info);
                        info = NULL;
                        deleteParam(parameter);
                        parameter = NULL;
                        free(commandName);
                        continue;
                    }
                    else {
                        runsource(fd, program, argv, info, len);
                        free(program);
                    }
                }
                else runsource(fd, commandName, argv, info, len);

                //now ready to run dest program
                if (info_dest != NULL) deleteParseInfo(info_dest);
                info_dest = (parse_info *) malloc(sizeof(*info_dest));
                params * parameter_dest = info->parameters2;
                size_t len_dest = parameter_dest->len;
                parseInfo(parameter_dest, info_dest);
                char ** argv_dest = NULL;
                for (i = 0; i < parameter_dest->len; i++) {
                    if(parameter_dest->argv[i] != NULL) {
                        argv_dest = (char**)realloc(argv_dest, (i + 1) * sizeof(*argv_dest));
                        argv_dest[i] = strdup(parameter_dest->argv[i]);
                    }
                    else break;
                }
                argv_dest = (char**)realloc(argv_dest, (i + 1) * sizeof(*argv_dest));
                argv_dest[i] = NULL;
                char * destProgram = info->command2;
                while ((info_dest->flag & IS_PIPED) != 0) {
                    if (switch_pipe) pipe(fd_2);
                    else pipe(fd);
                    if (switch_pipe) runmiddle(fd, fd_2, destProgram, argv_dest, info_dest, len_dest);
                    else runmiddle(fd_2, fd, destProgram, argv_dest, info_dest, len_dest);

                    if (switch_pipe) {
                        close(fd[0]);
                        close(fd[1]);
                    }
                    else {
                        close(fd_2[0]);
                        close(fd_2[1]);
                    }
                    while ((pid = wait(&status)) != -1){
                        if ((WIFEXITED(status))) {
                            cout << "Program exited with status " << WEXITSTATUS(status) << endl;
                        }
                        else if (WIFSIGNALED(status)) {
                            cout << "Program was killed by signal" << WTERMSIG(status) << endl;
                        }
                    }

                    switch_pipe = !switch_pipe;

                    deleteParam(parameter_dest);
                    parameter_dest = NULL;
                    free(destProgram);

                    parameter_dest = info_dest->parameters2;
                    destProgram = info_dest->command2;

                    deleteParseInfo(info_dest);
                    info_dest = NULL;

                    for (size_t j = 0; j <= len_dest; j++) {
                        if (argv_dest[j] != NULL) free(argv_dest[j]);
                        else break;
                    }
                    free(argv_dest);
                    argv_dest = NULL;
                    if (info_dest != NULL) deleteParseInfo(info_dest);
                    info_dest = (parse_info *) malloc(sizeof(*info_dest));
                    parseInfo(parameter_dest, info_dest);
                    len_dest = parameter_dest->len;
                    for (i = 0; i < parameter_dest->len; i++) {
                        if(parameter_dest->argv[i] != NULL) {
                            argv_dest = (char**)realloc(argv_dest, (i + 1) * sizeof(*argv_dest));
                            argv_dest[i] = strdup(parameter_dest->argv[i]);
                        }
                        else break;
                    }
                    argv_dest = (char**)realloc(argv_dest, (i + 1) * sizeof(*argv_dest));
                    argv_dest[i] = NULL;
                }

                if (switch_pipe) rundest(fd, destProgram, argv_dest, info_dest, len_dest);
                else rundest(fd_2, destProgram, argv_dest, info_dest, len_dest);

                /* free memory */
                free(commandName);
                free(destProgram);
                for (size_t j = 0; j <= len; j++) {
                    if (argv[j] != NULL) free(argv[j]);
                    else break;
                }
                free(argv);
                for (size_t j = 0; j <= len_dest; j++) {
                    if (argv_dest[j] != NULL) free(argv_dest[j]);
                    else break;
                }
                free(argv_dest);
                deleteParam(parameter);
                parameter = NULL;
                deleteParam(parameter_dest);
                parameter_dest = NULL;
                deleteParseInfo(info);
                info = NULL;
                deleteParseInfo(info_dest);
                info_dest = NULL;
                close(fd[0]);
                close(fd[1]);
                close(fd_2[0]);
                close(fd_2[1]);
                while ((pid = wait(&status)) != -1){
                    if ((WIFEXITED(status))) {
                        cout << "Program exited with status " << WEXITSTATUS(status) << endl;
                    }
                    else if (WIFSIGNALED(status)) {
                        cout << "Program was killed by signal" << WTERMSIG(status) << endl;
                    }
                }
            }
        }
    }
    freeStack(s);         //free Stack;
    deleteEnv(envVal);
    deleteParam(parameter);
    deleteParseInfo(info);
    return 0;
}

void runsource(int pfd[], char * command, char ** argv, parse_info *info, size_t len) {
    int pid;
    size_t i;
    switch (pid = fork()) {
        case 0:      //child process
            if (info->flag & IN_REDIRECT) {
                int in_fd = open(info->in_file, O_RDONLY|O_CREAT, 0666);
                close(fileno(stdin));
                dup2(in_fd, fileno(stdin));
                close(in_fd);
            }
            if ((info->flag & ERR_REDIRECT) != 0) {
                int err_fd = open(info->err_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                close(fileno(stderr));
                dup2(err_fd, fileno(stderr));
                close(err_fd);
            }
            dup2(pfd[1], 1);    //this end of the pipe becomes the standard output
            close(pfd[0]);              //this process don't need the other end 
            deleteParam(info->parameters2);
            deleteParseInfo(info);
            execv(command, argv);
            cerr << "Error: fail to execv source program of pipe" << endl;
            free(command);
            for (i = 0; i <= len; i++) {
                if (argv[i] != NULL) free(argv[i]);
                else break;
            }
            free(argv);
            deleteParseInfo(info);
        default:
            break;
        case -1:
            cerr << "Error: forking source process failed!" << endl;
    }
}

void runmiddle(int in_pfd[], int out_pfd[], char * command, char ** argv, parse_info *info, size_t len) {
    int pid;
    size_t i;
    switch (pid = fork()) {
        case 0:      //child process
            if ((info->flag & ERR_REDIRECT) != 0) {
                int err_fd = open(info->err_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                close(fileno(stderr));
                dup2(err_fd, fileno(stderr));
                close(err_fd);
            }
            dup2(in_pfd[0], 0);
            close(in_pfd[1]);
            dup2(out_pfd[1], 1);    //this end of the pipe becomes the standard output 
            close(out_pfd[0]);              //this process don't need the other end
            deleteParam(info->parameters2);
            deleteParseInfo(info);
            execv(command, argv);
            cerr << "Error: fail to execv source program of pipe" << endl;
            free(command);
            for (i = 0; i <= len; i++) {
                if (argv[i] != NULL) free(argv[i]);
                else break;
            }
            free(argv);
            deleteParseInfo(info);
        default:
            break;
        case -1:
            cerr << "Error: forking source process failed!" << endl;
    }
}

void rundest(int pfd[], char * command, char** argv, parse_info *info, size_t len) {
    int pid;
    size_t i;
    switch (pid = fork()) {
        case 0:
            if ((info->flag & OUT_REDIRECT) != 0) {
//                cout << "Output file is: " << info->out_file << endl;
                int out_fd = open(info->out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                close(fileno(stdout));
                dup2(out_fd, fileno(stdout));
                close(out_fd);
            }
            if ((info->flag & ERR_REDIRECT) != 0) {
                int err_fd = open(info->err_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                close(fileno(stderr));
                dup2(err_fd, fileno(stderr));
                close(err_fd);
            }
            dup2(pfd[0], 0);    //this end of the pipe becomes the standard input
            close(pfd[1]);              //this process doesn't need the other end 
            execv(command, argv);
            cerr << "Error: fail to execv dest program of pipe" << endl;
            free(command);
            for (i = 0; i <= len; i++) {
                if (argv[i] != NULL) free(argv[i]);
                else break;
            }
            free(argv);
            deleteParseInfo(info);
        default:
            break;
        case -1:
            cerr << "Error: forking dest process failed!" << endl;
    }
}


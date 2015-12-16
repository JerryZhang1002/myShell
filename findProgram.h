#ifndef findProgram_h
#define findProgram_h

typedef struct {
    char ** envs;
    size_t len;
} env;

env * getEnv(const char * key);
char * findProgram(const char * proName, env * envVal);
void deleteEnv(env * envVal);

#endif /* findProgram_h */

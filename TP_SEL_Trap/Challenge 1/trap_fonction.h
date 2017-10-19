#include <sys/ptrace.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LEN   512

int attach(int pid);
int modifMem(int pid, const * char processus, const * char fct);

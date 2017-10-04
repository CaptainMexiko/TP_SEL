#include "trap_fonction.h"

int attach (int pid){
  long erreurAttach1 = ptrace(PTRACE_ATTACH, pid, 0, 0);
  if (erreurAttach1 != 0){
    printf("%d\n", pid);
    perror("PTRACE_ATTACH a l'erreur suivante ");
    return -1; 
  }
  return 0;
}


int main(int argc, char const *argv[]) {
  if (argc == 0){
    printf("Il manque le programme a surveiller en argument%s\n", "");
  }
char const *str= argv[1];
char cmd[MAX_LEN];
int testAppel = snprintf(cmd, sizeof("pgrep  > proc.txt") - 1 + sizeof(str),"pgrep %s > proc.txt", str);
if (testAppel < 0){
  perror("Erreur de la chaine str ");
  return -1;
}

int errPgrep= system(cmd);
errPgrep = WEXITSTATUS(errPgrep);
if (errPgrep == 1){
  perror("Le processus n'existe pas ");
  return -1;
}

int pid;

FILE * f = fopen("proc.txt", "r");
char numProc[MAX_LEN];
int sizePid = fread(numProc, 5, sizeof(char), f);
if (sizePid == 0){
  perror("Erreur de fread ");
  return -1;
}
fclose(f);
pid = atoi(numProc);

if (pid == 0){
  perror("erreur pid ");
  return -1;
}
attach(pid);
  return 0;
}

#include "trap_fonction.h"

int attach (int pid){
  long erreurAttach1 = ptrace(PTRACE_ATTACH, pid, 0, 0);
  if (erreurAttach1 != 0){
    perror("PTRACE_ATTACH a l'erreur suivante :");
    return -1;
  }
  return 0;
}


int main(int argc, char const *argv[]) {
  if (argc == 0){
    printf("Il manque le programme a surveiller en argument%s\n", "");
  }
char *str= argv[1];
char cmd[100];
int testAppel = snprintf(cmd, sizeof("pegrep") - 1 + sizeof(str),"pgrep %s", str);
if (testAppel != 0){
  perror("Erreur de la chaine str : ");
}
int pid = system(str);
if (pid < 1){
  perror("erreur pid : ");
}
attach(pid);
  return 0;
}

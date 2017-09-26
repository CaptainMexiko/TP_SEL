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
char cmd[100];
int testAppel = snprintf(cmd, sizeof("pegrep") - 1 + sizeof(str),"pgrep %s", str);
if (testAppel < 0){
  perror("Erreur de la chaine str ");
  return -1;
}
int pid = 0;
FILE * f = popen(cmd, "r");
if (f == NULL){
  perror("Erreur de popen ");
  return -1;
}
char cmax[MAX_LEN] = {0};
if(fgets(cmax, MAX_LEN, f) == NULL){
  perror("Erreur de fgets ");
  return -1;
}
if (pclose(f) == -1){
  perror("Erreur de pclose ");
  return -1;
}
pid = atoi(cmax);
if (pid == 0){
  perror("erreur pid ");
  return -1;
}
attach(pid);
  return 0;
}

#include "trap_fonction.h"

int attach(int pid) {
  long erreurAttach1 = ptrace(PTRACE_ATTACH, pid, 0, 0);
  if (erreurAttach1 == -1) {
    perror("PTRACE_ATTACH a l'erreur suivante ");
    return -1;
  }
  wait(&pid);
  return 0;
}

struct user_regs_struct getRegistry(int pid) {
  struct user_regs_struct regs;
  int pgReg = ptrace(PTRACE_GETREGS, pid, 0, &regs);
  if (pgReg == -1) {
    perror("Erreur de récupération des registres avec PTRACE_GETREGS : \n");
    exit(1);
  }
  return regs;
}

int modifMem(int pid, const char *processus, const char *fct, size_t sizeFct) {
  char prg[MAX_LEN];
  char cmd[MAX_LEN];
  char cmdCall[MAX_LEN];
  char add[MAX_LEN];
  char addCall[MAX_LEN];
  long addresse;
  long addrCall;
  int testAppel;
  int errPgrep;
  int errCall;
  int re;
  int reC;
  int wr;
  int errSeek;
  FILE *adr;
  FILE *adrC;
  FILE *f;
  unsigned int trap = 0xCC;
  unsigned int call = 0xFF;
  // 0x9A, 0xE8 ou 0xFF ?
  struct user_regs_struct gRegistre;
  char callHex[8];
  const char *fctCall = "appelMem";

  // On regarde la table des symboles du processus controle
  if (snprintf(cmd,
               sizeof("nm ") + sizeof(processus) + sizeof(" | grep \" ") +
                   sizeof(fct) + sizeof("\" > addrTrap.txt"),
               "nm %s | grep \" %s\" > addrTrap.txt", processus, fct) < 0) {
    perror("Erreur de la chaine nm processsus | grep \" fct\" > addrTrap.txt");
    return -1;
  }

  errPgrep = system(cmd);
  errPgrep = WEXITSTATUS(errPgrep);

  if (errPgrep < 0) {
    perror("nm n'as pas fonctionné\n");
    return -1;
  }

  // On lit l'adresse recuperee dans le fichier addrTrap.txt
  adr = fopen("addrTrap.txt", "r");

  if (adr == NULL) {
    perror("Erreur de fopen adr\n");
    return -1;
  }

  re = fread(add, 16, sizeof(char), adr);

  if (re == 0) {
    perror("erreur lecture adresse du fichier addrTrap.txt\n");
    return -1;
  }
  // On convertit les 16 charactere de la chaine correspondant à l'adresse
  // hexadecimale en Long
  addresse = strtol(add, NULL, 16);

  testAppel = snprintf(prg, sizeof("/proc/") + sizeof(pid) + sizeof("/mem"),
                       "/proc/%d/mem", pid);

  if (testAppel < 0) {
    perror("Erreur de la chaine /proc/pid/mem\n");
    return -1;
  }

  // On ouvre le fichier mem du processus attache
  f = fopen(prg, "w");

  if (f == NULL) {
    perror("Erreur de fopen /mem");
  }
  // On se place a l'adresse de la fonction a modifier
  errSeek = fseek(f, addresse, SEEK_SET);

  if (errSeek < 0) {
    perror("Erreur fseek\n");
    return -1;
  }

  // On recupere les registres du processus attache
  gRegistre = getRegistry(pid);
  printf("rax = %llX\n", gRegistre.rax);

  // On recupere l'adresse hexadecimale de la fonction que l'on veut forcer a
  // executer a la place Pour l'instant une fonction que l'on utilise pas dans
  // le programme principale puis plus tard posix_memalign
  if (snprintf(cmdCall,
               sizeof("nm ") + sizeof(processus) + sizeof(" | grep \" ") +
                   sizeof(fctCall) + sizeof("\" > addrCall.txt"),
               "nm %s | grep \" %s\" > addrCall.txt", processus, fctCall) < 0) {
    perror("Erreur de la chaine nm processus | grep \" fctCall\" > "
           "addrCall.txt \n");
    return -1;
  }

  errCall = system(cmdCall);
  errCall = WEXITSTATUS(errCall);

  if (errCall < 0) {
    perror("nm n'as pas fonctionné\n");
    return -1;
  }

  adrC = fopen("addrCall.txt", "r");

  if (adrC == NULL) {
    perror("Erreur de fopen adrC\n");
    return -1;
  }

  reC = fread(addCall, sizeof(char), 16, adrC);

  if (reC == 0) {
    perror("erreur lecture adresse du fichier addrCall.txt\n");
    return -1;
  }

  addrCall = strtol(addCall, NULL, 16);

  gRegistre.rax = addrCall;

  printf("appelMem: %lX\n", addrCall);

  // Creation de la ligne "call posix_memalign"
  if (snprintf(callHex, sizeof(callHex) + 1, "%X%llX", call, gRegistre.rax) <
      0) {
    perror("Erreur creation de la chaine callHex \n");

    printf("Appel complet: %s\n", callHex);
    wr = fwrite(&callHex, 1, sizeof(callHex), f);

    if (wr == 0) {
      perror("Erreur write call dans /mem\n");
      return -1;
    }

    // On ecrit un trap a l'adresse de la fonction pour l'arreter.
    wr = fwrite(&trap, 1, 1, f);

    if (wr == 0) {
      perror("Erreur write dans /mem\n");
      return -1;
    }

    if (fclose(adr) != 0) {
      perror("Erreur fermeture adr");
      return -1;
    }

    if (fclose(adrC) != 0) {
      perror("Erreur fermeture adrC");
      return -1;
    }

    if (fclose(f) != 0) {
      perror("Erreur fermeture f");
      return -1;
    }

    printf("----Succès de l'arrêt de la fonction.----\n");
    return 0;
  }

  int main(int argc, char const *argv[]) {

    if (argc == 0) {
      printf("Passez votre processus et la fonction a surveiller en "
             "paramètres%s\n",
             "");
    }

    char const *str = argv[1];
    char cmd[MAX_LEN];
    int testAppel = snprintf(cmd, sizeof("pgrep  > proc.txt") + sizeof(str),
                             "pgrep %s > proc.txt", str);

    if (testAppel < 0) {
      perror("Erreur de la chaine str ");
      return -1;
    }

    int errPgrep = system(cmd);
    errPgrep = WEXITSTATUS(errPgrep);

    if (errPgrep == 1) {
      perror("Le processus n'existe pas ");
      return -1;
    }

    int pid;
    FILE *f = fopen("proc.txt", "r");

    if (f == NULL) {
      perror("Erreur de fopen ");
    }

    char numProc[MAX_LEN];
    int sizePid = fread(numProc, 5, sizeof(char), f);

    if (sizePid == 0) {
      perror("Erreur de fread ");
      return -1;
    }

    fclose(f);
    pid = atoi(numProc);

    if (pid == 0) {
      perror("Erreur pid ");
      return -1;
    }

    if (attach(pid) == -1) {
      return -1;
    }

    size_t sizeFct = 0;

    if (modifMem(pid, argv[1], argv[2], sizeFct) == -1) {
      return -1;
    }

    return 0;
  }

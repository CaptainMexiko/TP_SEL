#include "trap_fonction.h"

// Fonction d'attachement au processus a observer
// Prend en paramètre le pid du procesus cible.
int attach(int pid) {
  long erreurAttach1 = ptrace(PTRACE_ATTACH, pid, 0, 0);
  if (erreurAttach1 == -1) {
    perror("PTRACE_ATTACH a l'erreur suivante ");
    return -1;
  }
  wait(&pid);
  return 0;
}

// getRegistry permet d'obtenir les registres du processus observe.
struct user_regs_struct getRegistry(int pid) {
  struct user_regs_struct regs;
  int pgReg = ptrace(PTRACE_GETREGS, pid, 0, &regs);
  if (pgReg == -1) {
    perror("Erreur de récupération des registres avec PTRACE_GETREGS : \n");
    exit(1);
  }
  return regs;
}

////////////////////////////////////// Fonctions Principales ///////////////////////////////////////


// Permet de remplacer le code binaire d'une fonction par l'appel a posix_memalign
// pid est le pid du processus a observer, *fct est le nom de la fonction que l'on veut remplacer
// sizeFct est la taille de la fonction que l'on veut mettre a la place.
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

  struct user_regs_struct gRegistre;

  // Representation hexadecimale de l'appel call indirecte suivit du code trap (0xCC)
  char callHex[3] = {0xFF, 0xD0, 0xCC};
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

  re = fread(add, sizeof(char), 16, adr);
  add[16] = 0;

  if (re == 0) {
    perror("erreur lecture adresse du fichier addrTrap.txt\n");
    return -1;
  }
  // On convertit les 16 charactere de la chaine correspondant à l'adresse
  // hexadecimale en Long
  addresse = strtol(add, NULL, 16);

  testAppel = snprintf(prg, sizeof("/proc/") + sizeof(pid) + sizeof("/mem"),"/proc/%d/mem", pid);

  if (testAppel < 0) {
    perror("Erreur de la chaine /proc/pid/mem\n");
    return -1;
  }

  // On ouvre le fichier mem du processus attache
  f = fopen(prg, "w");

  if (f == NULL) {
    perror("Erreur de fopen /mem");
  }


  // On recupere les registres du processus attache
  gRegistre = getRegistry(pid);

  // On recupere l'adresse hexadecimale de la fonction que l'on veut forcer a
  // executer a la place Pour l'instant une fonction que l'on utilise pas dans
  // le programme principale puis plus tard posix_memalign
  if (snprintf(cmdCall, sizeof("nm ") + sizeof(processus) + sizeof(" | grep \" ") + sizeof(fctCall) + sizeof("\" > addrCall.txt"),"nm %s | grep \" %s\" > addrCall.txt", processus, fctCall) < 0) {
    perror("Erreur de la chaine nm processus | grep \" fctCall\" > addrCall.txt \n");
    return -1;
  }

  errCall = system(cmdCall);
  errCall = WEXITSTATUS(errCall);

  if (errCall < 0) {
    perror("nm n'as pas fonctionne\n");
    return -1;
  }

  adrC = fopen("addrCall.txt", "r");

  if (adrC == NULL) {
    perror("Erreur de fopen adrC\n");
    return -1;
  }

  reC = fread(addCall, sizeof(char), 16, adrC);
  addCall[16] = 0;

  if (reC == 0) {
    perror("erreur lecture adresse du fichier addrCall.txt\n");
    return -1;
  }

  addrCall = strtol(addCall, NULL, 16);

  long int A = 523;

  gRegistre.rax = addrCall;   // Fonction a remplacer
  printf("raxSet = %llX\n", gRegistre.rax);
  gRegistre.rip = addresse;  // Adresse de la fonction a remplacer
  printf("ripSet = %llx\n", gRegistre.rip);

  gRegistre.rdi  = A;

  for(int i = 0; i < 3; i++){
	  printf("Bytes: %x\n", callHex[i]);
	}

  // On se place a l'adresse de la fonction a modifier
  errSeek = fseek(f, addresse, SEEK_SET);

  if (errSeek < 0) {
    perror("Erreur fseek\n");
    return -1;
  }

  wr = fwrite(callHex, sizeof(callHex), 1, f);
  if (wr == 0) {
    perror("Erreur write call dans /mem\n");
    return -1;
  }

  ptrace(PTRACE_SETREGS, pid, 0, &gRegistre);
  printf("appelMem: %lX\n", addrCall);

  ptrace(PTRACE_CONT, pid, NULL, NULL);

//wait(&pid);

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
    printf("Passez votre processus et la fonction a surveiller en paramètres%s\n", "");
  }

  char const *str = argv[1];
  char cmd[MAX_LEN];
  int testAppel = snprintf(cmd, sizeof("pgrep  > proc.txt") + sizeof(str), "pgrep %s > proc.txt", str);

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

  if(fclose(f) != 0){
    perror("Erreur fermeture fichier proc.txt");
  }
  pid = atoi(numProc);

  if (pid == 0) {
    perror("Erreur dans l'obtention du pid ");
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

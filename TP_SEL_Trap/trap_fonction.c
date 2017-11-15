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

struct user_regs_struct getRegistry(int pid){
	struct user_regs_struct regs;
	int pgReg = ptrace(PTRACE_GETREGS,pid,0,&regs);
	if(pgReg == -1){
		perror("Erreur de récupération des registres avec PTRACE_GETREGS : ");
		exit(1);
	}
	return regs;
}

int modifMem(int pid, const char * processus, const char * fct){
	char prg[MAX_LEN];
	char cmd[MAX_LEN];
	char add[MAX_LEN];
	long addresse;
	int testAppel;
	int errPgrep;
	int re;
	int wr;
	int errSeek;
	FILE * adr;
	char oct = {0xCC};
	struct user_regs_struct gRegistre;

	if(snprintf(cmd, sizeof("nm ") + sizeof(processus) + sizeof(" | grep ") + sizeof(fct) + sizeof(" > addr.txt"), "nm %s | grep %s > addr.txt", processus, fct) < 0){
		perror("Erreur de la chaine nm processsu | grep fct > addr.txt");
		return -1;
	}


	errPgrep = system(cmd);
	errPgrep = WEXITSTATUS(errPgrep);
	if (errPgrep < 0) {
		perror("nm n'as pas fonctionné\n");
		return -1;
	}

	adr = fopen("addr.txt", "r");
	if (adr == NULL) {
		perror("Erreur de fopen adr\n");
		return -1;
	}


	re = fread(add, 16, sizeof(char), adr);
	if(re == 0){
		perror("erreur lecture adresse du fichier addr.txt\n");
		return -1;
	}

	addresse = strtol(add, NULL, 16);



	testAppel = snprintf(prg, sizeof("/proc/") + sizeof(pid) + sizeof("/mem"), "/proc/%d/mem" , pid);
	if (testAppel < 0) {
		perror("Erreur de la chaine /proc/pid/mem\n");
		return -1;
	}


	FILE * f =  fopen(prg,"w");
	if (f == NULL) {
		perror("Erreur de fopen /mem");
	}

	errSeek = fseek(f, addresse, SEEK_SET);
	if(errSeek < 0){
		perror("Erreur fseek\n");
		return -1;
	}

	wr = fwrite(&oct,1,1,f);
	if(wr == 0){
		perror("Erreur write dans /mem\n");
		return -1;
	}

	gRegistre = getRegistry(pid);
	printf("%lld\n", gRegistre.rax );


	printf("----Succès de l'arrêt de la fonction.----\n");
	return 0;
}


int main(int argc, char const *argv[]) {
	if (argc == 0) {
		printf("Passez votre processus et la fonction a surveiller en paramètres%s\n", "");
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
	FILE * f = fopen("proc.txt", "r");
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
		perror("erreur pid ");
		return -1;
	}
	if(attach(pid) == -1){
		return -1;
	}

	if(modifMem(pid, argv[1] ,argv[2]) == -1){
		return -1;
	}


	return 0;
}

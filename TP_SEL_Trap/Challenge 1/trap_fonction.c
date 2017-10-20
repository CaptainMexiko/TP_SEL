#include "trap_fonction.h"

int attach(int pid) {
	long erreurAttach1 = ptrace(PTRACE_ATTACH, pid, 0, 0);
	if (erreurAttach1 != 0) {
		printf("%d\n", pid);
		perror("PTRACE_ATTACH a l'erreur suivante ");
		return -1;
	}
	wait(&pid);
	return 0;
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
	FILE * f;
	char oct = {0xCC};

	if(snprintf(cmd, sizeof("nm ") + sizeof(processus) + sizeof(" | grep ") + sizeof(fct) + sizeof(" > addr.txt"), "nm %s | grep %s > addr.txt", processus, fct) < 0){
		perror("Erreur de la chaine nm processus | grep fct > addr.txt");
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


	re = fread(add, sizeof(char), 16, adr);
	if(re == 0){
		perror("erreur lecture adresse du fichier addr.txt\n");
		return -1;
	}

	addresse = strtol(add, NULL, 16);
	
	if(fclose(adr) < 0){
		perror("Error fermeture addr.txt\n");
		return -1;
	}



	testAppel = snprintf(prg, sizeof("/proc/") + sizeof(pid) + sizeof("/mem"), "/proc/%d/mem" , pid);
	if (testAppel < 0) {
		perror("Erreur de la chaine /proc/pid/mem\n");
		return -1;
	}


	f =  fopen(prg,"w");
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

	printf("----Succès de la modification mémoire.----\n");
	
	if(fclose(f) < 0){
		perror("Erreur fermeture /proc/pid/mem\n");
		return -1;
	}
	
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
	int sizePid = fread(numProc, sizeof(char), 5, f);
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

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  FILE *fp;
  fp=fopen("try.txt","w");
  // fp=fopen("try.txt","r");
  unsigned int c = 0xff;
  unsigned int t = 0xefa49b4d;
  // long l;
  // char addCall[512];
  char str[10];

  // int reC = fread(addCall, 16, sizeof(char), fp);
  // if(reC == 0){
  //   perror("erreur lecture adresse du fichier try.txt\n");
  //   return -1;
  // }

  // l = strtol(addCall, NULL, 16);

  // snprintf(str, sizeof(str) + 1, "%ld", l);
  // printf("%s\n", str);

  snprintf(str, sizeof(str) + 1, "%x%x", c, t);
  fwrite(&str,1,sizeof(str),fp);
  fclose(fp);
	return 0;
}

#include <stdio.h>

int main(int argc, char const *argv[]) {
  FILE *fp;
  fp=fopen("try.txt","w");
  unsigned int c = 0xE8;
  unsigned int t = 0xefa49b4d;
  char str[10];
  snprintf(str, sizeof(str) + 1, "%02x%08x", c, t);
  fwrite(&str,1,sizeof(str),fp);
  fclose(fp);
	return 0;
}

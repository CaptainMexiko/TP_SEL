#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int c1() {
	sleep(5);
	printf("%d\n", 1);
	return 0;
}

int c2() {
	sleep(5);
	printf("%d\n", 2);
	return 0;
}

int c3() {
	sleep(5);
	printf("%d\n", 3);
	return 0;
}

int c4() {
	sleep(5);
	printf("%d\n", 4);
	return 0;
}

int c5() {
	sleep(5);
	printf("%d\n", 5);
	return 0;
}

int c6() {
	sleep(5);
	printf("%d\n", 6);
	return 0;
}

int c7() {
	sleep(5);
	printf("%d\n", 7);
	return 0;
}

int c8() {
	sleep(5);
	printf("%d\n", 8);
	return 0;
}

int c9() {
	sleep(5);
	printf("%d\n", 9);
	return 0;
}

int main(int argc, char const *argv[]) {
	while (1) {
		c1();
		c2();
		c3();
		c4();
		c5();
		c6();
		c7();
		c8();
		c9();
	}
	return 0;
}

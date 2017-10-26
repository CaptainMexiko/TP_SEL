#include <stdlib.h>
#include <stdio.h>

int c1() {
	printf("%d\n", 1);
	return 0;
}

int c2() {
	printf("%d\n", 2);
	return 0;
}

int c3() {
	printf("%d\n", 3);
	return 0;
}

int c4() {
	printf("%d\n", 4);
	return 0;
}

int c5() {
	printf("%d\n", 5);
	return 0;
}

int c6() {
	printf("%d\n", 6);
	return 0;
}

int c7() {
	printf("%d\n", 7);
	return 0;
}

int c8() {
	printf("%d\n", 8);
	return 0;
}

int c9() {
	printf("%d\n", 9);
	return 0;
}

int main(int argc, char const *argv[]) {
	while (1) {
		c1();
		sleep(5);
		c2();
		sleep(5);
		c3();
		sleep(5);
		c4();
		sleep(5);
		c5();
		sleep(5);
		c6();
		sleep(5);
		c7();
		sleep(5);
		c8();
		sleep(5);
		c9();
		sleep(5);
	}
	return 0;
}

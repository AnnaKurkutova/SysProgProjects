#include "1.h"

extern int a;

int fn2(){
	fn1();
	a = a + 7;
	printf("%d \n", a);
	return 0;
}

int main(){
	fn2();
	return 0;
}
#include<iostream>
int a[10][10];
int f(int x) {
	std::cout << x << std::endl;
	return x;
}

int main() {
	int a;
	scanf_s("%d", &a);
	if (a > 100) {
		printf("yes");
	}
	else {
		printf("no");
	}
	return 0;
}
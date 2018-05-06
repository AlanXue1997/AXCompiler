#include<iostream>
int a[10][10];
int f(int x) {
	int b;
	b = x;
	std::cout << (x+b) << std::endl;
	return x;
}

int main() {
	f(5);
	return 0;
}
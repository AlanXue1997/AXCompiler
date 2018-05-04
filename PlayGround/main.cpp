#include<iostream>
int a[10][10];
int f(int x) {
	std::cout << x << std::endl;
	return x;
}

int main() {
	a[f(1)][f(2)] = f(3)+f(4)-f(5)*f(6);
	return 0;
}
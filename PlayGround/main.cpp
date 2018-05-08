#include<cstdlib>
#include<iostream>
#include<string>

int main() {
	std::string a = "%d\n%dasdfasdfa\n";
	std::string b = "chr$(";
	int k;
	k = a.find_first_of("\n");
	bool more_than_one = false;
	while (k >= 0) {
		if (more_than_one) b += ", ";
		else more_than_one = true;
		b += "\"" + a.substr(0, k) + "\", 0AH, 0DH";
		a = a.substr(k + 1, a.length() - k);
		k = a.find_first_of("\n");
	}
	if (a != "") {
		if (more_than_one) b += ", ";
		b += "\"" + a + "\"";
	}
	b += ")";
	std::cout << b << std::endl;
	system("pause");
	return 0;
}
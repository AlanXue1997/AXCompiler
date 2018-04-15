#include "parser.h"

static int table[N_STATE][N_TOTAL];

production pros[N_PRODUCTION];
static std::stack<item> s;

void new_pro(int len, ...) {
	static int num = 0;

	va_list args;
	va_start(args, len);
	pros[num].L = va_arg(args, int);
	pros[num].len = len;
	if (len > 0)pros[num].R = new int[len];
	for (int i = 0; i < len; ++i) {
		pros[num].R[i] = va_arg(args, int);
	}
	va_end(args);
	num++;
}

void add_var(name2int &n2i, int2name &i2n) {
	std::ifstream fin("productions.txt");
	int n = i2n.size() + N_TYPE + 1;
	int start = n+1;
	n2i["S0"] = n; i2n[n] = "S0"; n++;
	std::string st;
	fin >> st;
	n2i[st] = n; i2n[n] = st; n++;
	int flag = 0;
	while (fin >> st) {
		if (n2i.find(st) == n2i.end()) {
			n2i[st] = n; i2n[n] = st; n++;
		}
		int L = n2i[st];
		int R[20];
		int len;
		fin >> st;
#ifdef LOG
		if (st != ":")std::cout << "Wrong! no ':'" << std::endl;
#endif
		do {
			len = 0;
			fin >> st;
			while (st != "|" && st != ";") {
				if (n2i.find(st) == n2i.end()) {
					n2i[st] = n; i2n[n] = st; n++;
					//std::cout << st << std::endl;
				}
				R[len++] = n2i[st];
				fin >> st;
			}
			pros[flag].L = L;
			pros[flag].len = len;
			pros[flag].R = new int[len];
			for (int i = 0; i < len; ++i) pros[flag].R[i] = R[i];
			flag++;
			std::cout << flag << std::endl;
		} while (st != ";");
	}
}

void init_parse() {
	s.push(item{ 0, END });//bottom of stack, END denotes '#'
	name2int n2i = getName2int();
	int2name i2n = getInt2name();
	add_var(n2i, i2n);

	/*
	std::ifstream f("table.dat", std::ios::binary);
	for (int i = 0; i < N_STATE; ++i) {
		for (int j = 0; j < N_TOTAL; ++j) {
			f.read((char*)&table[i][j], sizeof(int));
		}
	}
	f.close();
	*/
}

int parse(int t) {
	int k = table[s.top().state][t];
	while (k >= N_STATE) {//R
		k %= N_STATE;
		production &p = pros[k];
		for (int i = p.len - 1; i >= 0; i--) {
			if (s.top().symble == p.R[i]) {
				s.pop();
			}
			else {
				std::cout << "Error when using r" << std::endl;
			}
		}
		s.push(item{ table[s.top().state][p.L] % N_STATE,p.L });
		k = table[s.top().state][t];
	}
	if (k != ACC && k != NONE) s.push(item{ k, t });
	return k;
}

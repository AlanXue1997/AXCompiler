#include "pre_parser.h"

#define OUTPUT_ITEM_SET(s) for(ITEM_SET::iterator it=s.begin();it!=s.end();it++)std::cout<<(*it)<<std::endl;

#define isTerm(x) (x<TERMINATION)
#define notTerm(x) (x>=TERMINATION)

#define ITEM_SET std::set<LR_Item>
#define TERM_SET std::set<int>

static int table[N_FAMILY][N_TOTAL];

static TERM_SET first[N_TOTAL];
static production pros[N_PRODUCTION];
static std::vector<ITEM_SET> family(N_FAMILY);

static name2int n2i;
static int2name i2n;

std::ostream& operator<<(std::ostream& os, const LR_Item &item) {
	os << "[" << item.L << "]->";
	for (int i = 0; i < item.alen; ++i) os << "[" << item.alpha[i] << "]";
	os << ".";
	for (int i = 0; i < item.blen; ++i) os << "[" << item.beta[i] << "]";
	os << ", [" << item.exp << "]";
	return os;
}

void get_first() {
	for (int x = 0; x < N_TOTAL; ++x) {
#ifdef LOG
		std::cout << "[" << (x + 1) << "/" << N_TOTAL << "]" << std::endl;
#endif
		if (isTerm(x)) {
			first[x].insert(x);
		}
		else {
			for (int i = 0; i < N_PRODUCTION; ++i) {
				if (pros[i].L == x) {
					if (pros[i].len == 0) first[x].insert(NONE);
					else if (isTerm(pros[i].R[0])) first[x].insert(pros[i].R[0]);
				}
			}
		}
	}
	bool p = true;
	while (p) {
		p = false;
		for (int i = 0; i <N_PRODUCTION; ++i) {
			if (pros[i].len == 0)continue;
			if (isTerm(pros[i].R[0]))continue;
			int j = 0;
			while (j<pros[i].len) {
				for (TERM_SET::iterator it = first[pros[i].R[j]].begin(); it != first[pros[i].R[j]].end(); it++) {
					if (*it != NONE) {
						std::pair<TERM_SET::iterator,bool> ret = first[pros[i].L].insert(*it);
						p = p || ret.second;
					}
				}
				if (first[pros[i].R[j]].find(NONE) != first[pros[i].R[j]].end()) {
					j++;
				}
				else {
					break;
				}
			}
		}
	}
	/*
	for (int i = 0; i < N_TOTAL; ++i) {
		for (TERM_SET::iterator it = first[i].begin(); it != first[i].end(); ++it) {
			printf("%d ", *it);
		}
		printf("\n");
	}*/
	
}

TERM_SET str_first(int *a, int n) {
	TERM_SET s = first[a[0]];
	if (s.find(NONE) != s.end()) s.erase(NONE);
	int k = 1;
	while (k < n && first[a[k - 1]].find(NONE) != first[a[k - 1]].end()) {
		for (auto it = first[a[k]].cbegin(); it != first[a[k]].cend(); it++) {
			if (*it != NONE) {
				s.insert(*it);
			}
		}
		k++;
	}
	if (k == n && first[a[k - 1]].find(NONE) != first[a[k - 1]].end()) {
		s.insert(NONE);
	}
	return s;
}

void closure(ITEM_SET &s) {
	bool p = true;
	while (p) {
		p = false;
		for (auto it = s.begin(); it != s.end(); ++it) {
			if (it->blen>0 && notTerm(it->beta[0])) {
				for (int i = 0; i < N_PRODUCTION; ++i) {
					if (pros[i].L == it->beta[0]) {
						int *t = new int[it->blen];
						for (int j = 1; j < it->blen; ++j) {
							t[j - 1] = it->beta[j];
						}
						t[it->blen - 1] = it->exp;
						TERM_SET ts = str_first(t, it->blen);
						delete[] t;
						for (auto jt = ts.cbegin(); jt != ts.cend(); jt++) {
							LR_Item l;
							l.L = pros[i].L;
							l.alen = 0;
							l.blen = pros[i].len;
							l.beta = new int[l.blen];
							for (int k = 0; k < l.blen; ++k) {
								l.beta[k] = pros[i].R[k];
							}
							l.exp = *jt;
							//std::cout << l << std::endl;
							std::pair<ITEM_SET::iterator, bool> ret = s.insert(l);
							if (ret.second) {
								p = true;
							}
							else {
								delete[] l.beta;
							}
						}
					}
				}
			}
		}
	}
}

ITEM_SET go_set(const ITEM_SET &s, int x) {
	ITEM_SET s1;
	for (auto it = s.cbegin(); it != s.cend(); ++it) {
		if (it->blen > 0 && it->beta[0]==x) {
			LR_Item l;
			l.L = it->L;
			l.alen = it->alen + 1;
			l.alpha = new int[l.alen];
			for (int i = 0; i < l.alen - 1; ++i) {
				l.alpha[i] = it->alpha[i];
			}
			l.alpha[l.alen - 1] = it->beta[0];
			l.blen = it->blen - 1;
			if (l.blen > 0) {
				l.beta = new int[l.blen];
				for (int i = 0; i < l.blen; ++i) {
					l.beta[i] = it->beta[i+1];
				}
			}
			l.exp = it->exp;
			s1.insert(l);
		}
	}
	closure(s1);
	return s1;
}

void get_family() {
	ITEM_SET s;
	LR_Item l;
	l.L = n2i["S0"];
	l.alen = 0;
	l.blen = 1;
	l.beta = new int[1];
	l.beta[0] = n2i["S0"]+1;
	l.exp = END;
	s.insert(l);
	closure(s);

	int flag = 0;
	family[flag++] = s;
	bool p = true;
	while (p) {
		p = false;
		for (int i = 0; i < flag; ++i) {
			for (int j = 0; j < N_TOTAL; ++j) {
				s = go_set(family[i],j);
				if (s.size() > 0) {
					int k = 0;
					while (k < flag) {
						if (family[k] == s)break;
						k++;
					}
					if (k == flag) {
#ifdef LOG
						std::cout << "f[" << (flag + 1) << "/" << N_FAMILY << "]" << std::endl;
#endif
						family[flag++] = s;
						p = true;
					}
				}
			}
		}
	}
	std::cout <<flag<< std::endl;
}

void get_table() {
	for (int i = 0; i < N_FAMILY; ++i)
		for (int j = 0; j < N_TOTAL; ++j)
			table[i][j] = NONE;
	for (int k = 0; k < N_FAMILY; ++k) {
#ifdef LOG
		std::cout << "t[" << (k + 1) << "/" << N_FAMILY << "]" << std::endl;
#endif
		for (auto it = family[k].cbegin(); it != family[k].cend(); ++it) {
			if (it->L == n2i["S0"] && it->alen == 1 && it->alpha[0] == n2i["S0"]+1 && it->blen == 0 && it->exp == END) {
				table[k][END] = ACC;
			}
			else if (it->blen > 0 && isTerm(it->beta[0])) {
				auto s = go_set(family[k], it->beta[0]);
				int j = 0;
				while (!(s == family[j]))j++;
				table[k][it->beta[0]] = j;//Sj
			}
			else if (it->blen == 0) {
				for (int j = 0; j < N_PRODUCTION; ++j) {
					if (pros[j].L == it->L && pros[j].len == it->alen) {
						int m=0;
						while (m < it->alen && pros[j].R[m] == it->alpha[m])m++;
						if (m == it->alen) {
							table[k][it->exp] = j + N_FAMILY;//rj
							break;
						}
					}
				}
			}
		}
		for (int i = TERMINATION; i < N_TOTAL; ++i) {
			auto s = go_set(family[k], i);
			if (s.size() > 0) {
				int j = 0;
				while (!(s == family[j]))j++;
				table[k][i] = j;
			}
		}
	}
}

void init_pre_parse() {
	
	n2i = getName2int();
	i2n = getInt2name();
	add_var(n2i, i2n, pros);

#ifdef LOG
	std::cout << "Generating First" << std::endl;
#endif
	get_first();
#ifdef LOG
	std::cout << "Generating Family" << std::endl;
#endif
	get_family();
#ifdef LOG
	std::cout << "Generating Table" << std::endl;
#endif
	get_table();

	
	std::ofstream f("../AXCompiler/table.dat", std::ios::binary);
	for (int i = 0; i < N_FAMILY; ++i) {
		for (int j = 0; j < N_TOTAL; ++j) {
			f.write((char*)&table[i][j], sizeof(int));
			//std::cout << table[i][j] << '\t';
		}
		//std::cout << std::endl;
	}
	f.close();
}


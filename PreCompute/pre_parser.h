#pragma once

#include "../AXCompiler/scanner.h"
#include "../AXCompiler/tokens.h"
#include "../AXCompiler/parser.h"


#include <set>
#include <vector>
#include <iostream>
#include <fstream>

#define LOG

#define N_FAMILY N_STATE

void init_pre_parse();

class LR_Item {
public:
	int L;
	int *alpha;
	int alen;
	int *beta;
	int blen;
	int exp;

	~LR_Item() {
		delete[] alpha;
		delete[] beta;
	}

	bool operator < (const LR_Item &item2) const{
		if (L != item2.L) return L<item2.L;
		else if (alen != item2.alen) return alen < item2.alen;
		else if (blen != item2.blen) return blen < item2.blen;
		else if (exp != item2.exp) return exp < item2.exp;
		for (int i = 0; i < alen; ++i) {
			if (alpha[i] != item2.alpha[i]) {
				return alpha[i] < item2.alpha[i];
			}
		}
		for (int i = 0; i < blen; ++i) {
			if (beta[i] != item2.beta[i]) {
				return beta[i] < item2.beta[i];
			}
		}
		return false;
	}

	bool operator ==(const LR_Item &item2) const {
		if (L != item2.L) return false;
		else if (alen != item2.alen) return false;
		else if (blen != item2.blen) return false;
		else if (exp != item2.exp) return false;
		for (int i = 0; i < alen; ++i) {
			if (alpha[i] != item2.alpha[i]) {
				return false;
			}
		}
		for (int i = 0; i < blen; ++i) {
			if (beta[i] != item2.beta[i]) {
				return false;
			}
		}
		return true;
	}
};
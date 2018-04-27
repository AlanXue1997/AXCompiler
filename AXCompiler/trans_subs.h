#pragma once

#include <string>
#include <stack>
#include <map>

#include "scanner.h"

#define VARIABLE_LIST std::map<std::string,std::string>

struct Identifier {
	std::string name;
};

struct Int {
	std::string type;
};

struct TypeSpecifier {
	std::string type;
};

struct DeclarationSpecifiers {
	std::string type;
};

struct Declaration {
	std::string type;
	std::string name;
};

struct DirectDeclarator {
	std::string name;
};

struct Declarator {
	std::string name;
};

struct InitDeclarator {
	std::string name;
};

struct InitDeclaratorList {
	std::string name;
};

struct ExternalDeclaration {
	int declaration_type;//reducted from declaration(1) or function(0)
	std::string type;
	std::string name;
};

struct TranslateUnit {
	VARIABLE_LIST variable_list;
};

void init_trans_subs();
void trans_add(TOKEN* token, int2name& i2n);
void trans_reduction(int L, int sub_index, int2name& i2n);

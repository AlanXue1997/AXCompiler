#pragma once

#include <string>
#include <stack>
#include <map>
#include <iostream>

#include "scanner.h"

//#define VARIABLE_LIST std::map<std::string,std::string>

struct VARIABLE {
	std::string type;
	int size;
};

class VARIABLE_LIST {
private:
	std::map<std::string, VARIABLE> dict;
	VARIABLE_LIST* father;
public:
	VARIABLE_LIST() {
		father = NULL;
	}
	int insert(const std::string &st, const VARIABLE var) {
		VARIABLE_LIST *p = this;
		while (p != NULL && p->dict.find(st) == p->dict.end()) p = p->father;
		if (p == NULL) {
			dict[st] = var;
			return 1;//insert succeeded
		}
		else {
			return 0;//insert failed
		}
	}

	//ATTENTION, This method doesn't ensure the result of dict if insert failed
	const std::string* insert(const VARIABLE_LIST &list) {
		for (auto p = list.dict.cbegin(); p != list.dict.cend(); ++p) {
			if (this->find(p->first) != NULL) return &p->first;
			dict[p->first] = p->second;
		}
		return NULL;
	}

	const VARIABLE *find(const std::string &st) {
		VARIABLE_LIST *p = this;
		while (p != NULL) {
			if (p->dict.find(st) != p->dict.end()) return &p->dict[st];
			else p = p->father;
		}
		return NULL;
	}

	std::ostream& operator<<(std::ostream& os) {
		auto p = this;
		int k = 1;
		while (p != NULL) {
			os << "+-------+-------+-------+" << std::endl;
			os << "|" << k++ << " level\t\t|" << std::endl;
			os << "+-------+-------+-------+" << std::endl;
			for (auto it = p->dict.cbegin(); it != p->dict.cend(); ++it) {
				os << "|" << it->first << "\t|" << it->second.type << "\t|" << it->second.size << "\t|" << std::endl;
			}
			p = p->father;
		}
		os << "+-------+-------+-------+";
		return os;
	}
};

struct Identifier {
	std::string name;
};

struct TypeSpecifier {
	VARIABLE var;
};

struct DeclarationSpecifiers {
	VARIABLE var;
};

struct DeclarationList {
	VARIABLE_LIST variable_list;
};

struct Declaration {
	VARIABLE var;
	std::string name;
};

struct DirectDeclarator {
	std::string name;
	VARIABLE_LIST variable_list;
};

struct Declarator {
	std::string name;
	VARIABLE_LIST variable_list;
};

struct InitDeclarator {
	std::string name;
};

struct InitDeclaratorList {
	std::string name;
};

struct ExternalDeclaration {
	int declaration_type;//reducted from declaration(1) or function(0)
	VARIABLE var;
	std::string name;
};

struct TranslateUnit {
	VARIABLE_LIST variable_list;
};

struct TrandeclarationSpecifiers {
	VARIABLE var;
	std::string name;
};

struct ParameterDeclaration {
	VARIABLE var;
	std::string name;
};

struct ParameterList {
	VARIABLE_LIST variable_list;
};

struct ParameterTypeList {
	VARIABLE_LIST variable_list;
};

struct CompoundStatement {
	VARIABLE_LIST variable_list;
};

void init_trans_subs();
int trans_add(TOKEN* token, int2name& i2n);
int trans_reduction(int L, int sub_index, int2name& i2n);

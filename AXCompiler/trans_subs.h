#pragma once

#include <string>
#include <stack>
#include <map>
#include <list>
#include <iostream>
#include <iomanip>

#include "scanner.h"

#define SIZE_INT 4
#define SIZE_DOUBLE 8

//#define VARIABLE_LIST std::map<std::string,std::string>

//#define VARIABLE_LIST_MAP std::map<std::string, VARIABLE_LIST*>
#define FUNCTION_LIST std::map<std::string, FUNCTION>
#define INT_LIST std::map<std::string, int>
#define QUADRUPLE_LIST std::list<QUADRUPLE>

struct ARRAY_LINK {
	std::string vari_or_cons_name;
	ARRAY_LINK* next;
};

struct QUADRUPLE {
	std::string op;
	std::string arg1;
	std::string arg2;
	std::string result;
	std::string mark = "";
};

struct VARIABLE {
	std::string type;
	int size;
	int array;
	void* pointer;
};

class VARIABLE_LIST {
private:
	std::map<std::string, VARIABLE> dict;
	VARIABLE_LIST* father;
	int temp;
public:
	VARIABLE_LIST() {
		father = NULL;
		temp = 0;
	}

	std::map<std::string, VARIABLE>* get_dict() {
		return &dict;
	}

	//Generate a new temp variable
	std::string new_temp(std::string type) {
		if (type == "int") {
			std::string name = "@" + std::to_string(temp++);
			insert(name, { "int", SIZE_INT });
			return name;
		}
		else {
			std::cout << "[Wrong]\n\tUnsupported temp variable type: " << type << std::endl;
			return "";
		}
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

	VARIABLE *find(const std::string &st) {
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
		os << "+-------+-------+-------+-------+" << std::endl;
		os << "|name\t|type\t|size\t|array\t|" << std::endl;
		while (p != NULL) {
			os << "+-------+-------+-------+-------+" << std::endl;
			os << "|" << k++ << " level\t\t\t|" << std::endl;
			os << "+-------+-------+-------+-------+" << std::endl;
			for (auto it = p->dict.cbegin(); it != p->dict.cend(); ++it) {
				os << "|" << it->first << "\t|";
				os << it->second.type << "\t|";
				os << it->second.size << "\t|";
				os << it->second.array << "\t|";
				if (it->second.array) {
					ARRAY_LINK* p = (ARRAY_LINK*)it->second.pointer;
					while (p != NULL) {
						os << "[" << p->vari_or_cons_name << "]";
						p = p->next;
					}
				}
				os << std::endl;
			}
			p = p->father;
		}
		os << "+-------+-------+-------+-------+";
		return os;
	}
};

struct FUNCTION {
	VARIABLE var;
	VARIABLE_LIST * parameter_variables;
	VARIABLE_LIST * local_variable_list;
	QUADRUPLE_LIST* quadruple_list;
};

struct Identifier {
	std::string name;
};

struct Constant {
	std::string name;
};

struct TypeSpecifier {
	VARIABLE var;
};

struct DeclarationSpecifiers {
	VARIABLE var;
};

struct DeclarationList {
	VARIABLE_LIST *variable_list;
};

struct Declaration {
	VARIABLE var;
	std::string name;
};

struct DirectDeclarator {
	std::string name;
	VARIABLE_LIST *variable_list;
	ARRAY_LINK* array_link= NULL;
};

struct Declarator {
	std::string name;
	VARIABLE_LIST *variable_list;
	ARRAY_LINK* array_link = NULL;
};

struct InitDeclarator {
	std::string name;
	ARRAY_LINK* array_link = NULL;
};

struct InitDeclaratorList {
	std::string name;
	ARRAY_LINK* array_link = NULL;
};

struct ExternalDeclaration {
	int declaration_type;//reducted from declaration(1) or function(0)
	VARIABLE var;
	std::string name;
	FUNCTION func;
};

struct TranslateUnit {
	VARIABLE_LIST *variable_list = NULL;
	FUNCTION_LIST *function_list = NULL;
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
	VARIABLE_LIST *variable_list;
};

struct ParameterTypeList {
	VARIABLE_LIST *variable_list;
};

struct CompoundStatement {
	VARIABLE_LIST *variable_list = NULL;
	QUADRUPLE_LIST *quadruple_list;
};

struct FunctionDefinition {
	std::string name;
	FUNCTION func;
};

struct AssignmentExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct ConditionalExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct LogicalOrExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct LogicalAndExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct InclusiveOrExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct ExclusiveOrExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct AndExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct EqualityExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct RelationalExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct ShiftExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct AdditiveExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct MultiplicativeExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct CastExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct UnaryExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct PostfixExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
	ARRAY_LINK* array_link = NULL;
};

struct PrimaryExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct AssignmentOperator {
	std::string operator_type;
};

struct Expression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct ExpressionStatement {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct Statement {
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct StatementList {
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct ConstantExpression {
	std::string vari_or_cons_name;
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct SelectionStatement {
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct JumpStatement {
	QUADRUPLE_LIST *quadruple_list = NULL;
};

struct IterationStatement {
	QUADRUPLE_LIST *quadruple_list = NULL;
};

void init_trans_subs();
int trans_add(TOKEN* token, int2name& i2n);
int trans_reduction(int L, int sub_index, int2name& i2n);

void output_global_variables();
void output_local_variables();
void output_all_quadruple();
void output_int_list();
void output_func_list();

VARIABLE_LIST *get_global_variables();
INT_LIST *get_int_list();
FUNCTION_LIST *get_function_list();
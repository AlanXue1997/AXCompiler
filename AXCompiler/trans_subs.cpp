#include "trans_subs.h"

std::stack<void*> trans_stack;

void init_trans_subs(TOKEN& token, int2name& i2n) {
	if (i2n[token.code] == "INT") {
		trans_stack.push((void*)new Int{ "int" });
	}
	else if (i2n[token.code] == "IDENTIFIER") {
		trans_stack.push((void*)new Identifier{ *(std::string*)token.p });
	}
}

void trans_add() {

}

void trans_reduction(int L, int sub_index, int2name& i2n) {
	if (i2n[L] == "type_specifier") {
		if (sub_index == 3) {
			TypeSpecifier *typeSpecifier = new TypeSpecifier{ ((Int*)trans_stack.top())->type };
			delete (Int*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)typeSpecifier);
		}
	}
	else if (i2n[L] == "declaration_specifiers") {
		if (sub_index == 2) {
			DeclarationSpecifiers *declarationSpecifiers = new DeclarationSpecifiers{ ((TypeSpecifier*)trans_stack.top())->type };
			delete (TypeSpecifier*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)declarationSpecifiers);
		}
	}
}
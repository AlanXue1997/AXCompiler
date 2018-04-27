#include "trans_subs.h"

std::stack<void*> trans_stack;

void init_trans_subs() {
	
}

void trans_add(TOKEN* token, int2name& i2n) {
	if (i2n[token->code] == "INT") {
		trans_stack.push((void*)new Int{ "int" });
	}
	else if (i2n[token->code] == "IDENTIFIER") {
		trans_stack.push((void*)new Identifier{ *(std::string*)token->p });
		printf("%s", ((Identifier*)trans_stack.top())->name);
	}
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
	else if (i2n[L] == "direct_declarator") {
		if (sub_index == 0) {
			DirectDeclarator *directDeclarator = new DirectDeclarator{ ((Identifier*)trans_stack.top())->name };
			delete (Identifier*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)directDeclarator);
		}
	}
	else if (i2n[L] == "declarator") {
		if (sub_index == 1) {
			Declarator *declarator = new Declarator{ ((DirectDeclarator*)trans_stack.top())->name };
			delete (DirectDeclarator*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)declarator);
		}
	}
	else if (i2n[L] == "init_declarator") {
		if (sub_index == 0) {
			InitDeclarator *initDeclarator = new InitDeclarator{ ((Declarator*)trans_stack.top())->name };
			delete (Declarator*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)initDeclarator);
		}
	}
	else if (i2n[L] == "init_declarator_list") {
		if (sub_index == 0) {
			InitDeclaratorList *initDeclaratorList = new InitDeclaratorList{ ((InitDeclarator*)trans_stack.top())->name };
			delete (InitDeclarator*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)initDeclaratorList);
		}
	}
	else if (i2n[L] == "declaration") {
		if (sub_index == 1) {
			Declaration *declaration = new Declaration;
			//get type first
			declaration->name = { ((InitDeclaratorList*)trans_stack.top())->name };
			delete (InitDeclaratorList*)trans_stack.top();
			trans_stack.pop();

			//get name then
			declaration->type = { ((DeclarationSpecifiers*)trans_stack.top())->type };
			delete (DeclarationSpecifiers*)trans_stack.top();
			trans_stack.pop();

			//push to stack
			trans_stack.push((void*)declaration);
		}
	}
	else if (i2n[L] == "external_declaration") {
		if (sub_index == 1) {
			ExternalDeclaration *externalDeclaration = new ExternalDeclaration;
			externalDeclaration->declaration_type = 1;

			externalDeclaration->name = { ((Declaration*)trans_stack.top())->name };
			externalDeclaration->type = { ((Declaration*)trans_stack.top())->type };
			delete (Declaration*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)externalDeclaration);
		}
	}
	else if (i2n[L] == "translation_unit") {
		if (sub_index == 0) {
			TranslateUnit *translateUnit = new TranslateUnit;

			translateUnit->variable_list[((ExternalDeclaration*)trans_stack.top())->name] = ((ExternalDeclaration*)trans_stack.top())->type;

			delete (ExternalDeclaration*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)translateUnit);
		}
		else if (sub_index == 1) {
			TranslateUnit *translateUnit = new TranslateUnit;

			translateUnit->variable_list[((ExternalDeclaration*)trans_stack.top())->name] = ((ExternalDeclaration*)trans_stack.top())->type;
			delete (ExternalDeclaration*)trans_stack.top();
			trans_stack.pop();

			VARIABLE_LIST & sub_list = ((TranslateUnit*)trans_stack.top())->variable_list;
			for (auto it = sub_list.begin(); it != sub_list.end(); ++it) {
				translateUnit->variable_list[it->first] = it->second;
			}
			delete (TranslateUnit*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)translateUnit);
			for (auto it = translateUnit->variable_list.cbegin(); it != translateUnit->variable_list.cend(); ++it) {
				printf("%s %s \n", it->first, it->second);
			}
		}
	}
}
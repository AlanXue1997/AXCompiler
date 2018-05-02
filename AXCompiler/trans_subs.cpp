#include "trans_subs.h"

#define SIZE_INT 4
#define SIZE_DOUBLE 8

std::stack<void*> trans_stack;

#define UPEXPECTED_PRODUCTION printf("[Wrong] %dth production of %s not defined\n", sub_index, i2n[L].c_str()); return 1;
#define UNDEFINE_CASE printf("[Wrong] not defined case in %dth production of %s\n", sub_index, i2n[L].c_str()); return 1;

void init_trans_subs() {
	
}

int trans_add(TOKEN* token, int2name& i2n) {
	if (i2n[token->code] == "INT") {
		trans_stack.push((void*)new VARIABLE{ "int", SIZE_INT });
	}
	else if(i2n[token->code] == "DOUBLE") {
		trans_stack.push((void*)new VARIABLE{ "int", SIZE_DOUBLE });
	}
	else if (i2n[token->code] == "IDENTIFIER") {
		trans_stack.push((void*)new Identifier{ *(std::string*)token->p });
		//printf("%s", ((Identifier*)trans_stack.top())->name);
	}
	else {
		//printf("[Wrong] Unexpected TOKEN: %s", i2n[token->code]);
		//return 1;
	}
	return 0;
}

int trans_reduction(int L, int sub_index, int2name& i2n) {
	if (i2n[L] == "type_specifier") {
		if (sub_index <= 6) {
			TypeSpecifier *typeSpecifier = new TypeSpecifier{ *(VARIABLE*)trans_stack.top() };
			delete (VARIABLE*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)typeSpecifier);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "declaration_specifiers") {
		if (sub_index == 2) {
			DeclarationSpecifiers *declarationSpecifiers = new DeclarationSpecifiers{ ((TypeSpecifier*)trans_stack.top())->var };
			delete (TypeSpecifier*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)declarationSpecifiers);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "direct_declarator") {
		if (sub_index == 0) {
			DirectDeclarator *directDeclarator = new DirectDeclarator;
			directDeclarator->name = ((Identifier*)trans_stack.top())->name ;
			delete (Identifier*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)directDeclarator);
		}
		else if (sub_index == 4) {
			DirectDeclarator *directDeclarator = new DirectDeclarator;

			directDeclarator->variable_list.insert(((ParameterList*)trans_stack.top())->variable_list);
			delete (ParameterList*)trans_stack.top();
			trans_stack.pop();

			directDeclarator->name = ((DirectDeclarator*)trans_stack.top())->name;
			delete (DirectDeclarator*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)directDeclarator);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "declarator") {
		if (sub_index == 1) {
			Declarator *declarator = new Declarator;
			declarator->name = ((DirectDeclarator*)trans_stack.top())->name;
			declarator->variable_list.insert(((DirectDeclarator*)trans_stack.top())->variable_list);
			delete (DirectDeclarator*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)declarator);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "init_declarator") {
		if (sub_index == 0) {
			InitDeclarator *initDeclarator = new InitDeclarator{ ((Declarator*)trans_stack.top())->name };
			delete (Declarator*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)initDeclarator);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "init_declarator_list") {
		if (sub_index == 0) {
			InitDeclaratorList *initDeclaratorList = new InitDeclaratorList{ ((InitDeclarator*)trans_stack.top())->name };
			delete (InitDeclarator*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)initDeclaratorList);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "declaration_list") {
		if (sub_index == 0) {
			DeclarationList *declarationList = new DeclarationList;
			declarationList->variable_list.insert(((Declaration*)trans_stack.top())->name, ((Declaration*)trans_stack.top())->var);
			delete (Declaration*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)declarationList);
		}
		else if (sub_index == 1) {
			DeclarationList *declarationList = new DeclarationList;

			//insert current variable
			declarationList->variable_list.insert(((Declaration*)trans_stack.top())->name, ((Declaration*)trans_stack.top())->var);
			delete (Declaration*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)declarationList);

			//insert elder variables
			declarationList->variable_list.insert(((DeclarationList*)trans_stack.top())->variable_list);
			delete (DeclarationList*)trans_stack.top();
			trans_stack.pop();//delete old (temp) variable list

			trans_stack.push((void*)declarationList);
		}
		else {
			UPEXPECTED_PRODUCTION
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
			declaration->var = { ((DeclarationSpecifiers*)trans_stack.top())->var };
			delete (DeclarationSpecifiers*)trans_stack.top();
			trans_stack.pop();

			//push to stack
			trans_stack.push((void*)declaration);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "external_declaration") {
		if (sub_index == 1) {
			ExternalDeclaration *externalDeclaration = new ExternalDeclaration;
			externalDeclaration->declaration_type = 1;

			externalDeclaration->name = { ((Declaration*)trans_stack.top())->name };
			externalDeclaration->var = { ((Declaration*)trans_stack.top())->var };
			delete (Declaration*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)externalDeclaration);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "translation_unit") {
		if (sub_index == 0) {
			TranslateUnit *translateUnit = new TranslateUnit;
			if (((ExternalDeclaration*)trans_stack.top())->declaration_type == 1) {
				translateUnit->variable_list.insert(((ExternalDeclaration*)trans_stack.top())->name, ((ExternalDeclaration*)trans_stack.top())->var);
			}
			else {
				UNDEFINE_CASE
			}
			delete (ExternalDeclaration*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)translateUnit);
		}
		else if (sub_index == 1) {
			TranslateUnit *translateUnit = new TranslateUnit;

			//insert current variable
			translateUnit->variable_list.insert(((ExternalDeclaration*)trans_stack.top())->name, ((ExternalDeclaration*)trans_stack.top())->var);
			delete (ExternalDeclaration*)trans_stack.top();
			trans_stack.pop();

			//insert elder variables
			translateUnit->variable_list.insert(((TranslateUnit*)trans_stack.top())->variable_list);
			delete (TranslateUnit*)trans_stack.top();
			trans_stack.pop();//delete old (temp) variable list

			trans_stack.push((void*)translateUnit);
			translateUnit->variable_list << std::cout << std::endl;
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "trandeclaration_specifiers") {
		if (sub_index == 0) {
			TrandeclarationSpecifiers *trandeclarationSpecifiers = new TrandeclarationSpecifiers;
			trandeclarationSpecifiers->name = ((Declarator*)trans_stack.top())->name;
			delete (Declarator*)trans_stack.top();
			trans_stack.pop();

			trandeclarationSpecifiers->var = ((DeclarationSpecifiers*)trans_stack.top())->var;
			delete (DeclarationSpecifiers*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)trandeclarationSpecifiers);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "parameter_declaration") {
		if (sub_index == 0) {
			ParameterDeclaration *parameterDeclaration = new ParameterDeclaration;

			parameterDeclaration->name = ((Declarator*)trans_stack.top())->name;
			delete (Declarator*)trans_stack.top();
			trans_stack.pop();

			parameterDeclaration->var = ((DeclarationSpecifiers*)trans_stack.top())->var;
			delete (DeclarationSpecifiers*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)parameterDeclaration);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "parameter_list") {
		if (sub_index == 0) {
			ParameterList *parameterList = new ParameterList;
			parameterList->variable_list.insert(((ParameterDeclaration*)trans_stack.top())->name, ((ParameterDeclaration*)trans_stack.top())->var);
			delete (ParameterDeclaration*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)parameterList);
		}
		else if (sub_index == 1) {
			ParameterList *parameterList = new ParameterList;

			//insert current variable
			parameterList->variable_list.insert(((ParameterDeclaration*)trans_stack.top())->name, ((ParameterDeclaration*)trans_stack.top())->var);
			delete (ParameterDeclaration*)trans_stack.top();
			trans_stack.pop();

			//insert elder variables
			parameterList->variable_list.insert(((ParameterList*)trans_stack.top())->variable_list);
			delete (ParameterList*)trans_stack.top();
			trans_stack.pop();//delete old (temp) variable list

			trans_stack.push((void*)parameterList);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "parameter_type_list") {
		if (sub_index == 0) {
			ParameterTypeList *parameterTypeList = new ParameterTypeList{ ((ParameterList*)trans_stack.top())->variable_list };
			delete (ParameterList*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)parameterTypeList);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "compound_statement") {
		if (sub_index == 2) {
			CompoundStatement *compoundStatement = new CompoundStatement;
			compoundStatement->variable_list.insert(((DeclarationList*)trans_stack.top())->variable_list);
			delete (DeclarationList*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)compoundStatement);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else {
		UPEXPECTED_PRODUCTION
		//printf("[Wrong] productions of %s not defined\n", i2n[L]); return 1;
	}
	return 0;
}
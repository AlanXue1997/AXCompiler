#include "trans_subs.h"

#define SIZE_INT 4
#define SIZE_DOUBLE 8

std::stack<void*> trans_stack;

VARIABLE_LIST *global_variables;
VARIABLE_LIST *local_variables;
VARIABLE_LIST *parameter_variables;

#define UPEXPECTED_PRODUCTION printf("[Wrong] %dth production of %s not defined\n", sub_index, i2n[L].c_str()); return 1;
#define UNDEFINE_CASE printf("[Wrong] not defined case in %dth production of %s\n", sub_index, i2n[L].c_str()); return 1;

void init_trans_subs() {
	
}

void output_global_variables() {
	(*global_variables) << std::cout << std::endl;
}

void output_local_variables() {
	(*local_variables) << std::cout << std::endl;
}

int trans_add(TOKEN* token, int2name& i2n) {
	if (i2n[token->code] == "INT") {
		trans_stack.push((void*)new VARIABLE{ "int", SIZE_INT });
	}
	else if(i2n[token->code] == "DOUBLE") {
		trans_stack.push((void*)new VARIABLE{ "double", SIZE_DOUBLE });
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

			directDeclarator->variable_list = ((ParameterList*)trans_stack.top())->variable_list;
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
			declarator->variable_list = ((DirectDeclarator*)trans_stack.top())->variable_list;
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
			declarationList->variable_list = new VARIABLE_LIST;
			if (!declarationList->variable_list->insert(((Declaration*)trans_stack.top())->name, ((Declaration*)trans_stack.top())->var)) {
				printf("[Wrong] variable \"%s\" exists", ((Declaration*)trans_stack.top())->name.c_str());
				return 1;
			}
			delete (Declaration*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)declarationList);
			local_variables = declarationList->variable_list;
		}
		else if (sub_index == 1) {
			DeclarationList *declarationList = new DeclarationList;

			//insert current variable
			std::string name = ((Declaration*)trans_stack.top())->name;
			VARIABLE var = ((Declaration*)trans_stack.top())->var;
			//declarationList->variable_list.insert(((Declaration*)trans_stack.top())->name, ((Declaration*)trans_stack.top())->var);
			delete (Declaration*)trans_stack.top();
			trans_stack.pop();

			//insert elder variables
			declarationList->variable_list = ((DeclarationList*)trans_stack.top())->variable_list;
			if (!declarationList->variable_list->insert(name, var)) {
				printf("[Wrong] variable \"%s\" exists", name.c_str());
				return 1;
			}
			delete (DeclarationList*)trans_stack.top();
			trans_stack.pop();//delete old (temp) variable list

			trans_stack.push((void*)declarationList);
			local_variables = declarationList->variable_list;
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
		if (sub_index == 0) {
			ExternalDeclaration *externalDeclaration = new ExternalDeclaration;
			externalDeclaration->declaration_type = 0;//It's a function

			externalDeclaration->func = { ((FunctionDefinition*)trans_stack.top())->func };
			delete (FunctionDefinition*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)externalDeclaration);
		}
		else if (sub_index == 1) {
			ExternalDeclaration *externalDeclaration = new ExternalDeclaration;
			externalDeclaration->declaration_type = 1;//It's a definition

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
				translateUnit->variable_list = new VARIABLE_LIST;
				if (!translateUnit->variable_list->insert(((ExternalDeclaration*)trans_stack.top())->name, ((ExternalDeclaration*)trans_stack.top())->var)) {
					printf("[Wrong] variable \"%s\" exists", ((ExternalDeclaration*)trans_stack.top())->name.c_str());
					return 1;
				}
				global_variables = translateUnit->variable_list;
			}
			else if (((ExternalDeclaration*)trans_stack.top())->declaration_type == 0) {
				translateUnit->function_list = new FUNCTION_LIST;
				translateUnit->function_list->insert(
					std::pair<std::string, FUNCTION>(
						((ExternalDeclaration*)trans_stack.top())->name,
						((ExternalDeclaration*)trans_stack.top())->func
					)
				);//Too long...
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
			
			std::string name;
			VARIABLE var;
			FUNCTION func;
			int flag = ((ExternalDeclaration*)trans_stack.top())->declaration_type;

			if (flag == 1) {
				//insert current variable
				name = ((ExternalDeclaration*)trans_stack.top())->name;
				var = ((ExternalDeclaration*)trans_stack.top())->var;
				//translateUnit->variable_list.insert(((ExternalDeclaration*)trans_stack.top())->name, ((ExternalDeclaration*)trans_stack.top())->var);
			}
			else if (flag == 0) {
				//insert current function
				name = ((ExternalDeclaration*)trans_stack.top())->name;
				func = ((ExternalDeclaration*)trans_stack.top())->func;
			}
			else {
				UNDEFINE_CASE
			}
			delete (ExternalDeclaration*)trans_stack.top();
			trans_stack.pop();

			//insert elder variables && functions
			translateUnit->variable_list = ((TranslateUnit*)trans_stack.top())->variable_list;
			if (translateUnit->variable_list == NULL) translateUnit->variable_list = new VARIABLE_LIST;
			//translateUnit->variable_list->insert(name, var);
			translateUnit->function_list = ((TranslateUnit*)trans_stack.top())->function_list;
			if (translateUnit->function_list == NULL) translateUnit->function_list = new FUNCTION_LIST;
			//translateUnit->function_list->insert(std::pair<std::string, FUNCTION>(name,func));

			if (flag == 1) {
				if (!translateUnit->variable_list->insert(name, var)) {
					printf("[Wrong] variable \"%s\" exists", name.c_str());
					return 1;
				}
				
			}
			else if (flag == 0) {
				translateUnit->function_list->insert(std::pair<std::string, FUNCTION>(name, func));
			}
			else {
				UNDEFINE_CASE
			}

			delete (TranslateUnit*)trans_stack.top();
			trans_stack.pop();//delete old (temp) variable list

			trans_stack.push((void*)translateUnit);
			global_variables = translateUnit->variable_list;
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
			parameterList->variable_list = new VARIABLE_LIST;
			parameterList->variable_list->insert(((ParameterDeclaration*)trans_stack.top())->name, ((ParameterDeclaration*)trans_stack.top())->var);
			delete (ParameterDeclaration*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)parameterList);
		}
		else if (sub_index == 1) {
			ParameterList *parameterList = new ParameterList;

			std::string name;
			VARIABLE var;
			//insert current variable
			name = ((ParameterDeclaration*)trans_stack.top())->name;
			var = ((ParameterDeclaration*)trans_stack.top())->var;
			//parameterList->variable_list.insert(((ParameterDeclaration*)trans_stack.top())->name, ((ParameterDeclaration*)trans_stack.top())->var);
			delete (ParameterDeclaration*)trans_stack.top();
			trans_stack.pop();

			//insert elder variables
			parameterList->variable_list = ((ParameterList*)trans_stack.top())->variable_list;
			if (!parameterList->variable_list->insert(name, var)) {
				printf("[Wrong] variable \"%s\" exists", name.c_str());
				return 1;
			}
			
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
			parameter_variables = parameterTypeList->variable_list;
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "compound_statement") {
		if (sub_index == 2) {
			CompoundStatement *compoundStatement = new CompoundStatement;
			compoundStatement->variable_list = ((DeclarationList*)trans_stack.top())->variable_list;
			delete (DeclarationList*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)compoundStatement);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "function_definition") {
		if (sub_index == 1) {
			FunctionDefinition *functionDefinition = new FunctionDefinition;

			delete (CompoundStatement*)trans_stack.top();
			trans_stack.pop();

			functionDefinition->func.name = ((Declarator*)trans_stack.top())->name;
			functionDefinition->func.parameter_variables = ((Declarator*)trans_stack.top())->variable_list;
			delete (Declarator*)trans_stack.top();
			trans_stack.pop();

			functionDefinition->func.var = ((DeclarationSpecifiers*)trans_stack.top())->var;
			delete (DeclarationSpecifiers*)trans_stack.top();
			trans_stack.pop();
			
			trans_stack.push((void*)functionDefinition);
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
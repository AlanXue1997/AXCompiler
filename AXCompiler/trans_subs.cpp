#include "trans_subs.h"

std::stack<void*> trans_stack;

VARIABLE_LIST *global_variables;
VARIABLE_LIST *local_variables;
VARIABLE_LIST *parameter_variables;
INT_LIST *int_list;
QUADRUPLE_LIST *all_quadruple;
FUNCTION_LIST *function_list;

#define UPEXPECTED_PRODUCTION printf("[Wrong]\n\t %dth production of %s not defined\n", sub_index, i2n[L].c_str()); return 1;
#define UNDEFINE_CASE printf("[Wrong]\n\t not defined case in %dth production of %s\n", sub_index, i2n[L].c_str()); return 1;

void init_trans_subs() {
	int_list = new INT_LIST;
}

VARIABLE_LIST *get_global_variables() {
	return global_variables;
}
INT_LIST *get_int_list() {
	return int_list;
}
FUNCTION_LIST *get_function_list() {
	return function_list;
}

void output_global_variables() {
	(*global_variables) << std::cout << std::endl;
}

void output_local_variables() {
	(*local_variables) << std::cout << std::endl;
}

void output_int_list() {
	std::cout << "+-------+-------+" << std::endl;
	std::cout << "|name\t|value\t|" << std::endl;
	std::cout << "+-------+-------+" << std::endl;
	for (auto it = int_list->cbegin(); it != int_list->cend(); ++it) {
		std::cout << "|" << it->first << "\t|" << it->second << "\t|" << std::endl;
	}
	std::cout << "+-------+-------+" << std::endl;
}

void output_quadruple(QUADRUPLE_LIST* quadruple_list) {
	std::cout << "+--------+--------+--------+--------+--------+" << std::endl;
	std::cout << "|op      |arg1    |arg2    |output  |mark    |" << std::endl;
	std::cout << "+--------+--------+--------+--------+--------+" << std::endl;
	if (quadruple_list != NULL) {
		std::cout.fill(' ');
		for (auto it = quadruple_list->cbegin(); it != quadruple_list->cend(); ++it) {
			std::cout << "|" << std::setw(8) << std::left << it->op;
			std::cout << "|" << std::setw(8) << std::left << it->arg1;
			std::cout << "|" << std::setw(8) << std::left << it->arg2;
			std::cout << "|" << std::setw(8) << std::left << it->result;
			std::cout << "|" << std::setw(8) << std::left << it->mark;
			std::cout << "|" << std::endl;
		}
		std::cout << "+--------+--------+--------+--------+--------+" << std::endl;
	}
}

void output_all_quadruple() {
	output_quadruple(all_quadruple);
}

void output_func_list() {
	if (function_list != NULL) {
		for (auto it = function_list->cbegin(); it != function_list->cend(); ++it) {
			std::cout << "|" << std::setw(8) << std::left << it->first;
			std::cout << "|" << std::setw(8) << std::left << it->second.var.type;
			std::cout << "|" << std::endl;
		}
	}
}

std::string new_if_mark() {
	static int n = 0;
	return "IF" + std::to_string(n++);
}

void new_const_int(const std::string num) {
	int_list->insert(std::pair<std::string, int>(num, atoi(num.c_str())));
}

void new_const_int(int num) {
	int_list->insert(std::pair<std::string, int>(std::to_string(num), num));
}

QUADRUPLE new_quadruple(std::string arg1, std::string op, std::string arg2) {
	QUADRUPLE quadruple;
	quadruple.op = op;
	quadruple.arg1 = arg1;
	quadruple.arg2 = arg2;
	if (op == "" || op[0] == 'J') {
		quadruple.result = "";
	}
	else {
		if (local_variables == NULL) local_variables = new VARIABLE_LIST;
		quadruple.result = local_variables->new_temp("int");
	}
	return quadruple;
}

QUADRUPLE new_quadruple(std::string arg1, std::string op, std::string arg2, std::string mark) {
	QUADRUPLE quadruple = new_quadruple(arg1, op, arg2);
	quadruple.mark = mark;
	return quadruple;
}

QUADRUPLE_LIST* new_quadruple_list(QUADRUPLE_LIST* quadruple_list_l, QUADRUPLE_LIST* quadruple_list_r) {
	QUADRUPLE_LIST* quadruple_list;
	if (quadruple_list_l != NULL) {
		//(sub)additivie_expression HAS quadruple list
		quadruple_list = quadruple_list_l;
		if (quadruple_list_r != NULL) {
			quadruple_list->splice(quadruple_list->end(), *quadruple_list_r);
			delete quadruple_list_r;
		}
	}
	else {
		//(sub)additive_expression does NOT have quadruple list
		if (quadruple_list_r != NULL) {
			quadruple_list = quadruple_list_r;
		}
		else {
			quadruple_list = new QUADRUPLE_LIST;
		}
	}
	return quadruple_list;
}

QUADRUPLE_LIST* new_quadruple_list(QUADRUPLE_LIST* quadruple_list_l, QUADRUPLE_LIST* quadruple_list_r, const QUADRUPLE quadruple) {
	QUADRUPLE_LIST* quadruple_list;
	quadruple_list = new_quadruple_list(quadruple_list_l, quadruple_list_r);
	quadruple_list->push_back(quadruple);
	return quadruple_list;
}

int trans_add(TOKEN* token, int2name& i2n) {
	if (i2n[token->code] == "INT") {
		trans_stack.push((void*)new VARIABLE{ "int", SIZE_INT, 0, NULL });
	}
	else if (i2n[token->code] == "DOUBLE") {
		trans_stack.push((void*)new VARIABLE{ "double", SIZE_DOUBLE, 0, NULL });
	}
	else if (i2n[token->code] == "IDENTIFIER") {
		trans_stack.push((void*)new Identifier{ *(std::string*)token->p });
		//printf("%s", ((Identifier*)trans_stack.top())->name);
	}
	else if (i2n[token->code] == "CONSTANT") {
		new_const_int(*(std::string*)token->p);
		trans_stack.push((void*)new Constant{ *(std::string*)token->p });
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
		else if (sub_index == 2) {
			DirectDeclarator *directDeclarator = new DirectDeclarator;

			std::string num = ((Constant*)trans_stack.top())->name;
			delete (Constant*)trans_stack.top();
			trans_stack.pop();

			if (int_list->find(num) == int_list->end()) {
				std::cout << "Only CONST INT is acceptable in \"[]\"" << std::endl;
				return 1;
			}
			directDeclarator->array_link = new ARRAY_LINK;
			directDeclarator->array_link->vari_or_cons_name = num;

			directDeclarator->array_link->next = ((DirectDeclarator*)trans_stack.top())->array_link;
			directDeclarator->name = ((DirectDeclarator*)trans_stack.top())->name;
			directDeclarator->variable_list = ((DirectDeclarator*)trans_stack.top())->variable_list;

			delete (DirectDeclarator*)trans_stack.top();
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
		else if (sub_index == 6) {
			DirectDeclarator *directDeclarator = new DirectDeclarator;

			directDeclarator->variable_list = NULL;
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
			declarator->array_link = ((DirectDeclarator*)trans_stack.top())->array_link;
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
			InitDeclarator *initDeclarator = new InitDeclarator;
			initDeclarator->name = ((Declarator*)trans_stack.top())->name;
			initDeclarator->array_link = ((Declarator*)trans_stack.top())->array_link;
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
			InitDeclaratorList *initDeclaratorList = new InitDeclaratorList;
			initDeclaratorList->name = ((InitDeclarator*)trans_stack.top())->name;
			initDeclaratorList->array_link = ((InitDeclarator*)trans_stack.top())->array_link;
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
			//get name first
			ARRAY_LINK* array_link = ((InitDeclaratorList*)trans_stack.top())->array_link;
			declaration->name = { ((InitDeclaratorList*)trans_stack.top())->name };
			delete (InitDeclaratorList*)trans_stack.top();
			trans_stack.pop();

			//get type then
			declaration->var = { ((DeclarationSpecifiers*)trans_stack.top())->var };
			declaration->var.pointer = array_link;
			declaration->var.array = array_link != NULL;
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
			externalDeclaration->name = ((FunctionDefinition*)trans_stack.top())->name;
			externalDeclaration->func = ((FunctionDefinition*)trans_stack.top())->func;
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
			if (((ExternalDeclaration*)trans_stack.top())->declaration_type == 1) {//It's a definition
				translateUnit->variable_list = new VARIABLE_LIST;
				if (!translateUnit->variable_list->insert(((ExternalDeclaration*)trans_stack.top())->name, ((ExternalDeclaration*)trans_stack.top())->var)) {
					printf("[Wrong] variable \"%s\" exists", ((ExternalDeclaration*)trans_stack.top())->name.c_str());
					return 1;
				}
				global_variables = translateUnit->variable_list;
			}
			else if (((ExternalDeclaration*)trans_stack.top())->declaration_type == 0) {//It's a function
				translateUnit->function_list = new FUNCTION_LIST;
				translateUnit->function_list->insert(
					std::pair<std::string, FUNCTION>(
						((ExternalDeclaration*)trans_stack.top())->name,
						((ExternalDeclaration*)trans_stack.top())->func
					)
				);//Too long...
				function_list = translateUnit->function_list;
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
			function_list = translateUnit->function_list;
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
		if (sub_index == 1) {
			CompoundStatement *compoundStatement = new CompoundStatement;
			compoundStatement->quadruple_list = ((StatementList*)trans_stack.top())->quadruple_list;
			delete (StatementList*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)compoundStatement);
			all_quadruple = compoundStatement->quadruple_list;
		}
		else if (sub_index == 2) {
			CompoundStatement *compoundStatement = new CompoundStatement;
			compoundStatement->variable_list = ((DeclarationList*)trans_stack.top())->variable_list;
			delete (DeclarationList*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)compoundStatement);
		}
		else if (sub_index == 3) {
			CompoundStatement *compoundStatement = new CompoundStatement;
			compoundStatement->quadruple_list = ((StatementList*)trans_stack.top())->quadruple_list;
			delete (StatementList*)trans_stack.top();
			trans_stack.pop();

			compoundStatement->variable_list = ((DeclarationList*)trans_stack.top())->variable_list;
			delete (DeclarationList*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)compoundStatement);
			all_quadruple = compoundStatement->quadruple_list;
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "function_definition") {
		if (sub_index == 1) {
			FunctionDefinition *functionDefinition = new FunctionDefinition;
			
			functionDefinition->func.quadruple_list = ((CompoundStatement*)trans_stack.top())->quadruple_list;
			functionDefinition->func.local_variable_list = ((CompoundStatement*)trans_stack.top())->variable_list;
			delete (CompoundStatement*)trans_stack.top();
			trans_stack.pop();
			
			functionDefinition->name = ((Declarator*)trans_stack.top())->name;
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
	else if (i2n[L] == "assignment_expression") {
		if (sub_index == 0) {
			AssignmentExpression *assignmentExpression = new AssignmentExpression;
			assignmentExpression->vari_or_cons_name = ((ConditionalExpression*)trans_stack.top())->vari_or_cons_name;
			assignmentExpression->quadruple_list = ((ConditionalExpression*)trans_stack.top())->quadruple_list;
			delete (ConditionalExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)assignmentExpression);
		}
		else if (sub_index == 1) {
			AssignmentExpression *assignmentExpression = new AssignmentExpression;
			
			QUADRUPLE_LIST* quadruple_list_r = ((AssignmentExpression*)trans_stack.top())->quadruple_list;
			QUADRUPLE quadruple;
			quadruple.arg1 = ((AssignmentExpression*)trans_stack.top())->vari_or_cons_name;
			quadruple.arg2 = "";
			delete (AssignmentExpression*)trans_stack.top();
			trans_stack.pop();

			quadruple.op = ((AssignmentOperator*)trans_stack.top())->operator_type;
			delete ((AssignmentOperator*)trans_stack.top());
			trans_stack.pop();

			QUADRUPLE_LIST* quadruple_list_l = ((UnaryExpression*)trans_stack.top())->quadruple_list;
			quadruple.result = ((UnaryExpression*)trans_stack.top())->vari_or_cons_name;
			delete (UnaryExpression*)trans_stack.top();
			trans_stack.pop();
			
			assignmentExpression->vari_or_cons_name = quadruple.result;

			//ATTENTION, l and r exchanged because right expression should be calculated firstly in assignment expression
			assignmentExpression->quadruple_list = new_quadruple_list(quadruple_list_r, quadruple_list_l, quadruple);

			trans_stack.push((void*)assignmentExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "conditional_expression") {
		if (sub_index == 0) {
			ConditionalExpression *conditionalExpression = new ConditionalExpression;
			conditionalExpression->vari_or_cons_name = ((LogicalOrExpression*)trans_stack.top())->vari_or_cons_name;
			conditionalExpression->quadruple_list = ((LogicalOrExpression*)trans_stack.top())->quadruple_list;
			delete (LogicalOrExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)conditionalExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "logical_or_expression") {
		if (sub_index == 0) {
			LogicalOrExpression *logicalOrExpression = new LogicalOrExpression;
			logicalOrExpression->vari_or_cons_name = ((LogicalAndExpression*)trans_stack.top())->vari_or_cons_name;
			logicalOrExpression->quadruple_list = ((LogicalAndExpression*)trans_stack.top())->quadruple_list;
			delete (LogicalAndExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)logicalOrExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "logical_and_expression") {
		if (sub_index == 0) {
			LogicalAndExpression *logicalAndExpression = new LogicalAndExpression;
			logicalAndExpression->vari_or_cons_name = ((InclusiveOrExpression*)trans_stack.top())->vari_or_cons_name;
			logicalAndExpression->quadruple_list = ((InclusiveOrExpression*)trans_stack.top())->quadruple_list;
			delete (InclusiveOrExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)logicalAndExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "inclusive_or_expression") {
		if (sub_index == 0) {
			InclusiveOrExpression *inclusiveOrExpression = new InclusiveOrExpression;
			inclusiveOrExpression->vari_or_cons_name = ((ExclusiveOrExpression*)trans_stack.top())->vari_or_cons_name;
			inclusiveOrExpression->quadruple_list = ((ExclusiveOrExpression*)trans_stack.top())->quadruple_list;
			delete (ExclusiveOrExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)inclusiveOrExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "exclusive_or_expression") {
		if (sub_index == 0) {
			ExclusiveOrExpression *exclusiveOrExpression = new ExclusiveOrExpression;
			exclusiveOrExpression->vari_or_cons_name = ((AndExpression*)trans_stack.top())->vari_or_cons_name;
			exclusiveOrExpression->quadruple_list = ((AndExpression*)trans_stack.top())->quadruple_list;
			delete (AndExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)exclusiveOrExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "and_expression") {
		if (sub_index == 0) {
			AndExpression *andExpression = new AndExpression;
			andExpression->vari_or_cons_name = ((EqualityExpression*)trans_stack.top())->vari_or_cons_name;
			andExpression->quadruple_list = ((EqualityExpression*)trans_stack.top())->quadruple_list;
			delete (EqualityExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)andExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "equality_expression") {
		if (sub_index == 0) {
			EqualityExpression *equalityExpression = new EqualityExpression;
			equalityExpression->vari_or_cons_name = ((RelationalExpression*)trans_stack.top())->vari_or_cons_name;
			equalityExpression->quadruple_list = ((RelationalExpression*)trans_stack.top())->quadruple_list;
			delete (RelationalExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)equalityExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "relational_expression") {
		if (sub_index == 0) {
			RelationalExpression *relationalExpression = new RelationalExpression;
			relationalExpression->vari_or_cons_name = ((ShiftExpression*)trans_stack.top())->vari_or_cons_name;
			relationalExpression->quadruple_list = ((ShiftExpression*)trans_stack.top())->quadruple_list;
			delete (ShiftExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)relationalExpression);
		}

		else if (sub_index == 1 || sub_index == 2 || sub_index == 3 || sub_index == 4) {
			RelationalExpression *relationalExpression = new RelationalExpression;
			QUADRUPLE_LIST *quadruple_list_r = ((ShiftExpression*)trans_stack.top())->quadruple_list;

			//QUADRUPLE quadruple;
			std::string op;
			if (sub_index == 1) {
				op = "<";
			}
			else if (sub_index == 2) {
				op = ">";
			}
			else if(sub_index==3){
				op = "<=";
			}
			else {
				op = ">=";
			}
			std::string arg2 = ((ShiftExpression*)trans_stack.top())->vari_or_cons_name;
			delete (ShiftExpression*)trans_stack.top();
			trans_stack.pop();

			QUADRUPLE_LIST *quadruple_list_l = ((RelationalExpression*)trans_stack.top())->quadruple_list;
			std::string arg1 = ((RelationalExpression*)trans_stack.top())->vari_or_cons_name;
			if (local_variables == NULL)local_variables = new VARIABLE_LIST;
			QUADRUPLE quadruple = new_quadruple(arg1, op, arg2);
			relationalExpression->vari_or_cons_name = quadruple.result;
			delete (MultiplicativeExpression*)trans_stack.top();
			trans_stack.pop();

			relationalExpression->quadruple_list = new_quadruple_list(quadruple_list_l, quadruple_list_r, quadruple);

			trans_stack.push((void*)relationalExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "shift_expression") {
		if (sub_index == 0) {
			ShiftExpression *shiftExpression = new ShiftExpression;
			shiftExpression->vari_or_cons_name = ((AdditiveExpression*)trans_stack.top())->vari_or_cons_name;
			shiftExpression->quadruple_list = ((AdditiveExpression*)trans_stack.top())->quadruple_list;
			delete (AdditiveExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)shiftExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "additive_expression") {
		if (sub_index == 0) {
			AdditiveExpression *additiveExpression = new AdditiveExpression;
			additiveExpression->vari_or_cons_name = ((MultiplicativeExpression*)trans_stack.top())->vari_or_cons_name;
			additiveExpression->quadruple_list = ((MultiplicativeExpression*)trans_stack.top())->quadruple_list;
			delete (MultiplicativeExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)additiveExpression);
		}
		else if (sub_index == 1 || sub_index == 2) {
			AdditiveExpression *additiveExpression = new AdditiveExpression;
			QUADRUPLE_LIST *quadruple_list_r = ((MultiplicativeExpression*)trans_stack.top())->quadruple_list;

			std::string op = sub_index==1?"+":"-";
			std::string arg2 = ((MultiplicativeExpression*)trans_stack.top())->vari_or_cons_name;
			delete (MultiplicativeExpression*)trans_stack.top();
			trans_stack.pop();

			QUADRUPLE_LIST *quadruple_list_l = ((AdditiveExpression*)trans_stack.top())->quadruple_list;
			std::string arg1 = ((AdditiveExpression*)trans_stack.top())->vari_or_cons_name;
			if (local_variables == NULL)local_variables = new VARIABLE_LIST;
			QUADRUPLE quadruple = new_quadruple(arg1, op, arg2);
			additiveExpression->vari_or_cons_name = quadruple.result;
			delete (AdditiveExpression*)trans_stack.top();
			trans_stack.pop();

			additiveExpression->quadruple_list = new_quadruple_list(quadruple_list_l, quadruple_list_r, quadruple);

			trans_stack.push((void*)additiveExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "multiplicative_expression") {
		if (sub_index == 0) {
			MultiplicativeExpression *multiplicativeExpression = new MultiplicativeExpression;
			multiplicativeExpression->vari_or_cons_name = ((CastExpression*)trans_stack.top())->vari_or_cons_name;
			multiplicativeExpression->quadruple_list = ((CastExpression*)trans_stack.top())->quadruple_list;
			delete (CastExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)multiplicativeExpression);
		}
		else if (sub_index == 1 || sub_index == 2 || sub_index == 3) {
			MultiplicativeExpression *multiplicativeExpression = new MultiplicativeExpression;
			QUADRUPLE_LIST *quadruple_list_r = ((CastExpression*)trans_stack.top())->quadruple_list;

			//QUADRUPLE quadruple;
			std::string op;
			if (sub_index == 1) {
				op = "*";
			}
			else if (sub_index == 2) {
				op = "/";
			}
			else {
				op = "%";
			}
			std::string arg2 = ((CastExpression*)trans_stack.top())->vari_or_cons_name;
			delete (CastExpression*)trans_stack.top();
			trans_stack.pop();

			QUADRUPLE_LIST *quadruple_list_l = ((MultiplicativeExpression*)trans_stack.top())->quadruple_list;
			std::string arg1 = ((MultiplicativeExpression*)trans_stack.top())->vari_or_cons_name;
			if (local_variables == NULL)local_variables = new VARIABLE_LIST;
			QUADRUPLE quadruple = new_quadruple(arg1, op, arg2);
			multiplicativeExpression->vari_or_cons_name = quadruple.result;
			delete (MultiplicativeExpression*)trans_stack.top();
			trans_stack.pop();

			multiplicativeExpression->quadruple_list = new_quadruple_list(quadruple_list_l, quadruple_list_r, quadruple);

			trans_stack.push((void*)multiplicativeExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "cast_expression") {
		if (sub_index == 0) {
			CastExpression *castExpression = new CastExpression;
			castExpression->vari_or_cons_name = ((UnaryExpression*)trans_stack.top())->vari_or_cons_name;
			castExpression->quadruple_list = ((UnaryExpression*)trans_stack.top())->quadruple_list;
			delete (UnaryExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)castExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "unary_expression") {
		if (sub_index == 0) {
			UnaryExpression *unaryExpression = new UnaryExpression;
			unaryExpression->vari_or_cons_name = ((PostfixExpression*)trans_stack.top())->vari_or_cons_name;
			unaryExpression->quadruple_list = ((PostfixExpression*)trans_stack.top())->quadruple_list;

			//It's an array
			if (((PostfixExpression*)trans_stack.top())->array_link != NULL) {
				ARRAY_LINK *p_use = ((PostfixExpression*)trans_stack.top())->array_link;
				VARIABLE *variable = local_variables->find(unaryExpression->vari_or_cons_name);
				if (NULL == variable) {
					variable = global_variables->find(unaryExpression->vari_or_cons_name);
				}
				if (!variable->array) {
					std::cout << "[Wrong]\n\t\"" << unaryExpression->vari_or_cons_name << "\" is NOT an ARRAY" << std::endl;
				}
				ARRAY_LINK *p_define = (ARRAY_LINK*)variable->pointer;
				int multi = int_list->at(p_define->vari_or_cons_name);
				int sum = 0;
				std::string last_variable = p_use->vari_or_cons_name;
				p_use = p_use->next;
				p_define = p_define->next;
				while (p_use != NULL && p_define != NULL) {
					new_const_int(multi);

					QUADRUPLE quadruple1 = new_quadruple(std::to_string(multi), "*", p_use->vari_or_cons_name);
					unaryExpression->quadruple_list->push_back(quadruple1);

					QUADRUPLE quadruple2 = new_quadruple(last_variable, "+", quadruple1.result);
					unaryExpression->quadruple_list->push_back(quadruple2);
					
					last_variable = quadruple2.result;
					p_use = p_use->next;
					p_define = p_define->next;
				}
				if (p_use != NULL || p_define != NULL) {
					std::cout << "[Wrong]\n\tDimension of array " << unaryExpression->vari_or_cons_name << " doesn't match" << std::endl;
				}
				int size = variable->size;
				new_const_int(size);
				QUADRUPLE quadruple_result = new_quadruple(std::to_string(size), "*", last_variable);

				unaryExpression->quadruple_list->push_back(quadruple_result);
				unaryExpression->vari_or_cons_name += "+" + quadruple_result.result;
			}
			delete (PostfixExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)unaryExpression);

			//output_quadruple(unaryExpression->quadruple_list);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "postfix_expression") {
		if (sub_index == 0) {
			PostfixExpression *postfixExpression = new PostfixExpression;
			postfixExpression->vari_or_cons_name = ((PrimaryExpression*)trans_stack.top())->vari_or_cons_name;
			delete (PrimaryExpression*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)postfixExpression);
		}
		else if (sub_index == 1) {
			PostfixExpression *postfixExpression = new PostfixExpression;
			postfixExpression->array_link = new ARRAY_LINK;
			postfixExpression->array_link->vari_or_cons_name = ((Expression*)trans_stack.top())->vari_or_cons_name;
			QUADRUPLE_LIST *quadruple_list_r = ((Expression*)trans_stack.top())->quadruple_list;
			delete (Expression*)trans_stack.top();
			trans_stack.pop();

			postfixExpression->array_link->next = ((PostfixExpression*)trans_stack.top())->array_link;
			postfixExpression->vari_or_cons_name = ((PostfixExpression*)trans_stack.top())->vari_or_cons_name;
			QUADRUPLE_LIST *quadruple_list_l = ((PostfixExpression*)trans_stack.top())->quadruple_list;
			delete (PostfixExpression*)trans_stack.top();
			trans_stack.pop();

			postfixExpression->quadruple_list = new_quadruple_list(quadruple_list_l, quadruple_list_r);
			
			trans_stack.push((void*)postfixExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "primary_expression") {
		if (sub_index == 0) {
			PrimaryExpression *primaryExpression = new PrimaryExpression;
			primaryExpression->vari_or_cons_name = ((Constant*)trans_stack.top())->name;
			delete (Identifier*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)primaryExpression);
		}
		else if (sub_index == 1) {
			PrimaryExpression *primaryExpression = new PrimaryExpression;
			primaryExpression->vari_or_cons_name = ((Constant*)trans_stack.top())->name;
			delete (Constant*)trans_stack.top();
			trans_stack.pop();
			trans_stack.push((void*)primaryExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "assignment_operator") {
		if (sub_index == 0) {
			AssignmentOperator *assignmentOperator = new AssignmentOperator{ "=" };
			trans_stack.push((void*)assignmentOperator);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "expression") {
		if (sub_index == 0) {
			Expression *expression = new Expression;
			expression->quadruple_list = ((AssignmentExpression*)trans_stack.top())->quadruple_list;
			expression->vari_or_cons_name = ((AssignmentExpression*)trans_stack.top())->vari_or_cons_name;
			delete (AssignmentExpression*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)expression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "expression_statement") {
		if (sub_index == 1) {
			ExpressionStatement *expressionStatement = new ExpressionStatement;
			expressionStatement->quadruple_list = ((Expression*)trans_stack.top())->quadruple_list;
			delete (Expression*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)expressionStatement);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "statement") {
		if (sub_index == 1) {
			Statement *statement = new Statement;
			statement->quadruple_list = ((CompoundStatement*)trans_stack.top())->quadruple_list;
			delete (CompoundStatement*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)statement);
		}
		else if (sub_index == 2) {
			Statement *statement = new Statement;
			statement->quadruple_list = ((ExpressionStatement*)trans_stack.top())->quadruple_list;
			delete (ExpressionStatement*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)statement);
		}
		else if (sub_index == 3) {
			Statement *statement = new Statement;
			statement->quadruple_list = ((SelectionStatement*)trans_stack.top())->quadruple_list;
			delete (SelectionStatement*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)statement);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "statement_list") {
		if (sub_index == 0) {
			StatementList *statementList = new StatementList;
			statementList->quadruple_list = ((Statement*)trans_stack.top())->quadruple_list;
			delete (Statement*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)statementList);
		}
		else if (sub_index == 1) {
			StatementList *statementList = new StatementList;
			QUADRUPLE_LIST *quadruple_list_r = ((Statement*)trans_stack.top())->quadruple_list;
			delete (Statement*)trans_stack.top();
			trans_stack.pop();

			QUADRUPLE_LIST *quadruple_list_l = ((StatementList*)trans_stack.top())->quadruple_list;
			delete (StatementList*)trans_stack.top();
			trans_stack.pop();
			
			statementList->quadruple_list = new_quadruple_list(quadruple_list_l, quadruple_list_r);
			trans_stack.push((void*)statementList);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "constant_expression") {
		if (sub_index == 0) {
			ConstantExpression *constantExpression = new ConstantExpression;
			constantExpression->vari_or_cons_name = ((ConditionalExpression*)trans_stack.top())->vari_or_cons_name;
			constantExpression->quadruple_list = ((ConditionalExpression*)trans_stack.top())->quadruple_list;
			delete (ConditionalExpression*)trans_stack.top();
			trans_stack.pop();

			trans_stack.push((void*)constantExpression);
		}
		else {
			UPEXPECTED_PRODUCTION
		}
	}
	else if (i2n[L] == "selection_statement") {
		if (sub_index == 0) {
			std::string mark = new_if_mark();
			std::string mark_end = mark+"_END";
			SelectionStatement *selectionStatement = new SelectionStatement;

			QUADRUPLE_LIST *quadruple_list_r = ((Statement*)trans_stack.top())->quadruple_list;
			delete (Statement*)trans_stack.top();
			trans_stack.pop();

			QUADRUPLE_LIST *quadruple_list_l = ((Expression*)trans_stack.top())->quadruple_list;
			if (quadruple_list_l == NULL)quadruple_list_l = new QUADRUPLE_LIST;
			quadruple_list_l->push_back(new_quadruple(((Expression*)trans_stack.top())->vari_or_cons_name, "JZ", mark_end));
			delete (Expression*)trans_stack.top();
			trans_stack.pop();

			selectionStatement->quadruple_list = new_quadruple_list(quadruple_list_l, quadruple_list_r, new_quadruple("", "", "", mark_end));
			trans_stack.push((void*)selectionStatement);
		}
		else if (sub_index == 1) {
			std::string mark = new_if_mark();
			std::string mark_end = mark + "_END";
			std::string mark_else = mark + "_ELSE";
			SelectionStatement *selectionStatement = new SelectionStatement;

			QUADRUPLE_LIST *quadruple_list_r = ((Statement*)trans_stack.top())->quadruple_list;
			delete (Statement*)trans_stack.top();
			trans_stack.pop();

			QUADRUPLE_LIST *quadruple_list_m = ((Statement*)trans_stack.top())->quadruple_list;
			delete (Statement*)trans_stack.top();
			trans_stack.pop();

			QUADRUPLE_LIST *quadruple_list_l = ((Expression*)trans_stack.top())->quadruple_list;
			if (quadruple_list_l == NULL)quadruple_list_l = new QUADRUPLE_LIST;
			quadruple_list_l->push_back(new_quadruple(((Expression*)trans_stack.top())->vari_or_cons_name, "JZ", mark_else));
			delete (Expression*)trans_stack.top();
			trans_stack.pop();

			quadruple_list_l = new_quadruple_list(quadruple_list_l, quadruple_list_m, new_quadruple("", "JMP", mark_end));
			quadruple_list_l->push_back(new_quadruple("", "", "", mark_else));
			selectionStatement->quadruple_list = new_quadruple_list(quadruple_list_l, quadruple_list_r, new_quadruple("", "", "", mark_end));

			trans_stack.push((void*)selectionStatement);
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
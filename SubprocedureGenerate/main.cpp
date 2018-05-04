#include <iostream>
#include <fstream>
#include <string>
std::ofstream fout("output.txt");

std::string st[] = {
	"assignment",
	"conditional",
	"logical_or",
	"logical_and",
	"inclusive_or",
	"exclusive_or",
	"and",
	"equality",
	"relational",
	"shift",
	"additive",
	"multiplicative",
	"cast",
	"unarry",
	"postfix"
};

std::string st_struct[] = {
	"Assignment",
	"Conditional",
	"LogicalOr",
	"LogicalAnd",
	"InclusiveOr",
	"ExclusiveOr",
	"And",
	"Equality",
	"Relational",
	"Shift",
	"Additive",
	"Multiplicative",
	"Cast",
	"Unary",
	"Postfix"
};
std::string st_vari[] = {
	"assignment",
	"conditional",
	"logicalOr",
	"logicalAnd",
	"inclusiveOr",
	"exclusiveOr",
	"and",
	"equality",
	"relational",
	"shift",
	"additive",
	"multiplicative",
	"cast",
	"unary",
	"postfix"
};

int main() {
	for (int i = 0; i < 16; ++i) {
		fout << "struct " << st_struct[i] << "Expression {" << std::endl;
		fout << "\tstd::string vari_or_cons_name;" << std::endl;
		fout << "\tQUADRUPLE_LIST *quadruple_list = NULL;" << std::endl;
		fout << "};" << std::endl;
		fout << std::endl;
	}
	for (int i = 0; i < 16-1; ++i) {
		fout << "else if (i2n[L] == \"" << st[i] << "_expression\") {" << std::endl;
		fout << "\tif (sub_index == 0) {" << std::endl;
		fout << "\t\t" << st_struct[i] << "Expression *" << st_vari[i] << "Expression = new " << st_struct[i] << "Expression;" << std::endl;
		fout << "\t\t" << st_vari[i] << "Expression->vari_or_cons_name = ((" << st_struct[i + 1] << "Expression*)trans_stack.top())->vari_or_cons_name" << " ; " << std::endl;
		fout << "\t\t" << st_vari[i] << "Expression->quadruple_list = ((" << st_struct[i + 1] << "Expression*)trans_stack.top())->quadruple_list" << " ; " << std::endl;
		fout << "\t\tdelete (" << st_struct[i + 1] << "Expression*)trans_stack.top();" << std::endl;
		fout << "\t\ttrans_stack.pop();" << std::endl;
		fout << "\t\ttrans_stack.push((void*)" << st_vari[i] << "Expression);" << std::endl;
		fout << "\t}" << std::endl;
		fout << "\telse {" << std::endl;
		fout << "\t\tUPEXPECTED_PRODUCTION" << std::endl;
		fout << "\t}" << std::endl;
		fout << "}" << std::endl;
	}
	return 0;
}
#include "CodeGenerator.h"

#define INSTRUCTOR_WIDTH 13

std::ofstream fout("generatedCode.asm");

INT_LIST *const_int_list;
VARIABLE_LIST* global_variable_list;
VARIABLE_LIST* local_variable_list;
FUNCTION_LIST *func_list;
ASM_VARIABLE_LIST *asm_variable_list;

void init_code_generator() {

}

std::string new_common_label() {
	static int n;
	return "A" + std::to_string(n++);
}

void writeLabel(std::string label) {
	fout << "\t";
	fout << label << ":" << std::endl;
}

void writeHead() {
	fout << "INCLUDE	ASM32.INC" << std::endl;
	fout << std::endl;
}

void writeData() {
	int wid = 8;
	if (global_variable_list != NULL) {
		fout << ".DATA" << std::endl;
		auto list = global_variable_list->get_dict();
		for (auto it = list->cbegin(); it != list->cend(); it++) {
			fout << "\t";
			fout << std::setw(wid) << std::left << ("g_" + it->first);
			if (it->second.type == "int") {
				fout << std::setw(wid) << std::left << "SDWORD";
			}
			else {
				fout << std::setw(wid) << std::left << "(!)UNKNOW_TYPE: " << it->second.type;
			}
			if (it->second.array) {
				auto p = (ARRAY_LINK*)it->second.pointer;
				int n = 1;
				while (p != NULL) {
					n *= const_int_list->at(p->vari_or_cons_name);
					p = p->next;
				}
				fout << n << std::setw(wid) << std::left << " DUP(0)";
			}
			else {
				fout << std::setw(wid) << std::left << "0";
			}
			fout << std::endl;
		}
		fout << std::endl;
	}
}

ASM_VARIABLE_LIST* convertVariable(VARIABLE_LIST* variable_list) {
	ASM_VARIABLE_LIST* asm_variable_list = new ASM_VARIABLE_LIST;
	int n = 8;
	if (variable_list != NULL) {
		auto dict = variable_list->get_dict();
		for (auto it = dict->cbegin(); it != dict->cend(); ++it) {
			std::string name = it->first;
			int size = it->second.size;
			std::string type;
			if (it->second.type == "int") {
				type = "SDWORD";
			}
			else {
				std::cout << "[Wrong]\n\tUnsupported type " << it->second.type;
			}
			if (it->second.array) {
				auto p = (ARRAY_LINK*)it->second.pointer;
				while (p != NULL) {
					size *= const_int_list->at(p->vari_or_cons_name);
					p = p->next;
				}
			}
			n += size;
			asm_variable_list->insert(std::pair<std::string, ASM_VARIABLE>(name, { type,n }));
		}
	}
	asm_variable_list->insert(std::pair<std::string, ASM_VARIABLE>("@ALL", { "",n }));
	return asm_variable_list;
}

void modify_name(std::string &name);

void inst2(std::string instructor, std::string destination, std::string source) {
	int wid = INSTRUCTOR_WIDTH;
	modify_name(destination);
	modify_name(source);
	fout << "\t";
	fout << std::setw(wid) << std::left << instructor;
	fout << std::setw(wid) << std::left << (destination+",");
	fout << std::setw(wid) << std::left << source;
	fout << std::endl;
}

void inst1(std::string instructor, std::string source) {
	int wid = INSTRUCTOR_WIDTH;
	modify_name(source); 
	fout << "\t";
	fout << std::setw(wid) << std::left << instructor;
	fout << std::setw(wid) << std::left << source;
	fout << std::endl;
}

void modify_name(std::string &name) {
	if (global_variable_list != NULL && global_variable_list->find(name) != NULL) {
		name = "g_" + name;
	}
	else if (local_variable_list != NULL && local_variable_list->find(name) != NULL) {
		name = "[EBP-" + std::to_string(asm_variable_list->at(name).offset) + "]";
	}
	else if (name.find('%') != std::string::npos) {
		int k = name.find_first_of('%');
		std::string base = name.substr(0, k);
		std::string index = name.substr(k + 1, name.length() - k - 1);
		if (global_variable_list != NULL && global_variable_list->find(base) != NULL) {
			base = "g_" + base;
			inst2("MOV", "EBX", "OFFSET " + base);
		}
		else if (local_variable_list != NULL && local_variable_list->find(base) != NULL) {
			inst2("MOV", "EBX", "EBP");
			inst2("SUB", "EBX", std::to_string(asm_variable_list->at(base).offset));
		}
		inst2("MOV", "ESI", index);
		name = "[EBX+ESI*4]";
	}
}

void writeInstructor(QUADRUPLE *quadruple) {
	if (quadruple->mark != "") {
		fout << quadruple->mark << ":" << std::endl;;
	}
	if (quadruple->op == "+") {
		inst2("MOV", "EAX", quadruple->arg1);
		inst2("ADD", "EAX", quadruple->arg2);
		inst2("MOV", quadruple->result, "EAX");
	}
	else if (quadruple->op == "=") {
		inst2("MOV", "EAX", quadruple->arg1);
		inst2("MOV", quadruple->result, "EAX");
	}
	else if (quadruple->op == "*") {
		inst2("MOV", "EAX", quadruple->arg1);
		inst2("MOV", "EBX", quadruple->arg2);
		inst1("IMUL", "EBX");
		inst2("MOV", quadruple->result, "EAX");
	}
	else if (quadruple->op == "JZ") {
		inst2("MOV", "EAX", quadruple->arg1);
		inst2("CMP", "EAX", "0");
		inst1("JZ", quadruple->arg2);
	}
	else if (quadruple->op == "JMP") {
		inst1("JMP", quadruple->arg2);
	}
	else if (quadruple->op == "<") {
		inst2("MOV", "EAX", "1");
		inst2("MOV", quadruple->result, "EAX");
		inst2("MOV", "EAX", quadruple->arg1);
		inst2("CMP", "EAX", quadruple->arg2);
		std::string label = new_common_label();
		inst1("JL", label);
		inst2("MOV", "EAX", "0");
		inst2("MOV", quadruple->result, "EAX");
		writeLabel(label);
	}
	else if (quadruple->op == "") {

	}
	else {
		fout << "\t(!)Undefined operator " << quadruple->op << std::endl;
	}
}

void writeProc() {
	fout << ".CODE" << std::endl;
	for (auto it = func_list->cbegin(); it != func_list->cend(); ++it) {
		std::string name = it->first;
		//!!cannot get local variable correctly
		local_variable_list = it->second.local_variable_list;
		//it->second.parameter_variables
		std::string return_type = it->second.var.type;
		QUADRUPLE_LIST *quadruple_list = it->second.quadruple_list;
		if (name == "main") {
			fout << name << " PROC" << std::endl;
		}
		else {
			fout << "(!)Undefined Function name type" << std::endl;
		}
		asm_variable_list = convertVariable(local_variable_list);
		if (asm_variable_list->at("@ALL").offset > 0) {
			fout << "\t";
			fout << std::setw(INSTRUCTOR_WIDTH) << std::left << "SUB";
			fout << std::setw(INSTRUCTOR_WIDTH) << std::left << "EBP,";
			fout << std::setw(INSTRUCTOR_WIDTH) << std::left << asm_variable_list->at("@ALL").offset;
			fout << std::endl;
		}
		for (auto quadruple = quadruple_list->begin(); quadruple != quadruple_list->end(); ++quadruple) {
			writeInstructor(&*quadruple);//Magical converting...
		}

		fout << "\t";
		fout << std::setw(INSTRUCTOR_WIDTH) << std::left << "INVOKE";
		fout << std::setw(INSTRUCTOR_WIDTH) << std::left << "ExitProcess,";
		fout << std::setw(INSTRUCTOR_WIDTH) << std::left << "0";
		fout << std::endl;
		fout << name << " ENDP" << std::endl;
		fout << std::endl;
	}
	fout << "END main" << std::endl;
}

void generate_code(VARIABLE_LIST* _global_variable_list, INT_LIST* _int_list, FUNCTION_LIST* function_list) {
	const_int_list = _int_list;
	global_variable_list = _global_variable_list;
	func_list = function_list;

	writeHead();
	writeData();
	writeProc();

	fout.close();
}
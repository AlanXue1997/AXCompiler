#pragma once

#include <map>
#include <string>

#define S_TABLE std::map<std::string,std::string>

class SymbolTable {
	S_TABLE table;
	SymbolTable *f;
	
	SymbolTable() {
		f = NULL;
	}
	SymbolTable(SymbolTable *s) {
		f = s;
	}
};
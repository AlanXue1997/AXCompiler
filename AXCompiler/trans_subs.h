#pragma once

#include <string>
#include <stack>

#include "scanner.h"

struct Identifier {
	std::string name;
};

struct Int {
	std::string type;
};

struct TypeSpecifier {
	std::string type;
};

struct DeclarationSpecifiers {
	std::string type;
};

struct Declaration {
	std::string type;
	std::string name;
};

struct DirectDeclarator {
	std::string name;
};

struct Declarator {
	std::string name;
};

struct InitDeclarator {
	std::string name;
};

struct InitDeclaratorList {
	std::string name;
};
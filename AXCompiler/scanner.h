#pragma once 

#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unordered_map>
#include <map>

#include "tokens.h"

#define DIC_NAME "dictionary.txt"
#define CACHE_SIZE 128

#define S_WRONG -1
#define S_EMPTY 0
#define S_ST_d 1
#define S_ST_do 2
#define S_INT 3
#define S_TOKEN 4
#define S_OP 5	//operator
#define S_DOUBLE 6
#define S_STRING 7
#define S_CHAR 8
#define S_COMMENT 9

typedef std::map<std::string, int> word2int;
typedef std::map<int, std::string> int2name;
typedef std::map<std::string, int> name2int;

word2int getWord2int();
int2name getInt2name();
name2int getName2int();

struct TOKEN {
	int code;
	void * p;
};

TOKEN * tokenScan(FILE * file);
int initScanner();
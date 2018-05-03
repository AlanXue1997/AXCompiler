#include "scanner.h"

#define MATCH(STRING,CLASS) if(strcmp(str, STRING) == 0){ words = new TOKEN{CLASS,words}; flush = 1; }

word2int w;

word2int getWord2int() {
	std::ifstream fin(DIC_NAME);
	word2int w;
	std::string str1, str2;
	int n = N_TYPE + 1;
	while (fin >> str1 >> str2) {
		w[str2] = n++;
	}
	return w;
}

int2name getInt2name() {
	std::ifstream fin(DIC_NAME);
	int2name w;
	std::string str1, str2;
	w[IDENTIFIER] = "IDENTIFIER";
	w[CONST_DOUBLE] = "CONST_DOUBLE";
	w[CONSTANT] = "CONSTANT";
	w[STRING_LITERAL] = "STRING_LITERAL";
	w[CONST_CHAR] = "CONST_CHAR";
	w[END] = "END";
	int n = N_TYPE + 1;
	while (fin >> str1 >> str2) {
		w[n++] = str1;
	}
	return w;
}

name2int getName2int() {
	std::ifstream fin(DIC_NAME);
	name2int w;
	std::string str1, str2;
	w["IDENTIFIER"] = IDENTIFIER;
	w["CONST_DOUBLE"] = CONST_DOUBLE;
	w["CONSTANT"] = CONSTANT;
	w["STRING_LITERAL"] = STRING_LITERAL;
	w["CONST_CHAR"] = CONST_CHAR;
	w["END"] = END;
	int n = N_TYPE + 1;
	while (fin >> str1 >> str2) {
		w[str1] = n++;
	}
	return w;
}

int initScanner() {
	w = getWord2int();
	return 0;
}

TOKEN * tokenScan(FILE * file) {
	static char str[CACHE_SIZE] = { 0 };
	static int flush = 0;
	static int eof = 0;
	int flag;
	int state;
	
	TOKEN * words = NULL;

	flag = 0;
	state = S_EMPTY;
	while (!eof) {
		if (!flush) {
			eof = fscanf(file, "%c", &str[flag]) != 1;
		}
		char ch = str[flag];
		flush = 0;
		if (state == S_EMPTY) {
			if (ch >= '0' && ch <= '9') { state = S_INT; }
			else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') { state = S_TOKEN; }
			else if (ch == '?' || ch == '(' || ch == ')' ||
				ch == '[' || ch == ']' || ch == '{' ||
				ch == '}' || ch == ';' || ch == ',' || 
				ch == ':' || ch == '~'
				) { words = new TOKEN{ w[str],NULL }; flush = 1; ch = 10; }
			else if (ch == ' ' || ch == 10 || ch == '\t') { flush = 1; }
			else if (ch == '"') { state = S_STRING; }
			else if (ch == '"') { state = S_CHAR; }
			//else if (ch == '/') { state = S_COMMENT; }
			else { state = S_OP; }
		}
		else if (state == S_INT) {
			if (ch == '.') {
				state = S_DOUBLE;
			}
			else
			if (ch < '0' || ch > '9') {
				str[flag] = 0;
				words = new TOKEN{ CONSTANT,words };
				flush = 1;
			}
		}
		else if (state == S_DOUBLE) {
			if (ch < '0' || ch > '9') {
				words = new TOKEN{ CONSTANT,words };
				flush = 1;
			}
		}
		else if (state == S_TOKEN) {
			if (!((ch >= '0'&&ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')) {
				str[flag] = 0;
				if (w.find(str) != w.end()) {
					words = new TOKEN{ w[str],words }; flush = 1;
				}

				if (flush == 0) {
					words = new TOKEN{ IDENTIFIER , words }; flush = 1;
				}
			}
		}
		else if (state == S_STRING) {
			if (ch == '"' && str[flag-1] != '\\') {
				words = new TOKEN{ STRING_LITERAL,words };
				flush = 1;
				ch = 10;
			}
		}
		else if (state == S_CHAR) {
			if (ch == '\'' && str[flag - 1] != '\\') {
				words = new TOKEN{ CONST_CHAR,words };
				flush = 1;
				ch = 10;
			}
		}
		else if (state == S_OP) {
			switch (str[0]) {
			case '|':
				if (ch == '|' || ch == '=') { ch = 10;  words = new TOKEN{ w[str],words }; flush = 1; }
				else { words = new TOKEN{ w[std::string(1,str[0])],words }; flush = 1; }
				break;
			case '>':
				if (ch == '=') { ch = 10;  words = new TOKEN{ w[str],words }; flush = 1; }
				else if (ch == '>') {
					eof = fscanf(file, "%c", &str[++flag]) != 1;
					ch = str[flag];
					if(ch == '=') { ch = 10;  words = new TOKEN{ w[str],words }; flush = 1; }
					else { words = new TOKEN{ w[">>"],words }; flush = 1; }
				}
				else { words = new TOKEN{ w[std::string(1,str[0])],words }; flush = 1; }
				break;
			case '<':
				if (ch == '=') { ch = 10;  words = new TOKEN{ w[str],words }; flush = 1; }
				else if (ch == '<') {
					eof = fscanf(file, "%c", &str[++flag]) != 1;
					ch = str[flag];
					if (ch == '=') { ch = 10;  words = new TOKEN{ w[str],words }; flush = 1; }
					else { words = new TOKEN{ w["<<"],words }; flush = 1; }
				}
				else { words = new TOKEN{ w[std::string(1,str[0])],words }; flush = 1; }
				break;
			case '%':
			case '^':
			case '*':
			case '!':
			case '=':
				if (ch == '=') { ch = 10;  words = new TOKEN{ w[str],words }; flush = 1; }
				else { words = new TOKEN{ w[std::string(1,str[0])],words }; flush = 1; }
				break;
			case '/':
				if (ch == '=') { ch = 10;  words = new TOKEN{ w[str],words }; flush = 1; }
				else if (ch == '/') {
					while (fscanf(file, "%c", &ch) == 1 && ch != '\n');
					flush = 1;
				}
				else if (ch == '*') {
					do {
						while (fscanf(file, "%c", &ch) == 1 && ch != '*');
					} while (fscanf(file, "%c", &ch) == 1 && ch != '/');
					flush = 1; ch = '\n';
				}
				else { words = new TOKEN{ w[std::string(1,str[0])],words }; flush = 1; }
				break;
			case '+':
				if (ch == '+' || ch == '=') { ch = 10;  words = new TOKEN{ w[str],words }; flush = 1; }
				else { words = new TOKEN{ w[std::string(1,str[0])],words }; flush = 1; }
				break;
			case '-':
				if (ch == '>' || ch == '-' || ch == '=') { ch = 10;  words = new TOKEN{ w[str],words }; flush = 1; }
				else { words = new TOKEN{ w[std::string(1,str[0])],words }; flush = 1; }
				break;
			case '&':
				if (ch == '&' || ch == '=') { ch = 10;  words = new TOKEN{ w[str],words }; flush = 1; }
				else { words = new TOKEN{ w[std::string(1,str[0])],words }; flush = 1; }
				break;
			case '.':
				if (ch == '.') {
					eof = fscanf(file, "%c", &str[++flag]) != 1;
					ch = str[flag];
					if (ch == '.') { ch = 10;  words = new TOKEN{ w[str],words }; flush = 1; }
					else { words = new TOKEN{ SCAN_WRONG,words }; flush = 1; }
				}
				else { words = new TOKEN{ w[std::string(1,str[0])],words }; flush = 1; }
				break;
			default:
				words = new TOKEN{ w[std::string(1,str[0])],words }; flush = 1;
			}
		}
		if (flush) {
			state = S_EMPTY;
			if (ch != ' ' && ch != 10 && ch != '\t') {
				str[flag] = 0;
				if(words != NULL) words->p = new std::string(str);
				memset(str, 0, sizeof(str));
				str[0] = ch;
				flush = 1;//stop read new word one time
			}
			else {
				if(words != NULL) words->p = new std::string(str);
				memset(str, 0, sizeof(str));
				flush = 0;
			}
			flag = 0;
			if (words != NULL) {
				return words;
			}
		}
		else {
			flag++;
		}
	}
	return words == NULL ? new TOKEN{ END, NULL } : words;
}
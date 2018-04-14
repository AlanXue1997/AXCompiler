#include "scanner.h"

#define FLUSH2(CHAR,CLASS) case CHAR: words = new TOKEN{CLASS,words}; state = S_EMPTY; break;
#define CHANGE2(CHAR,STATE) case CHAR: state = STATE; break;

#define MATCH(STRING,CLASS) if(strcmp(str, STRING) == 0){ words = new TOKEN{CLASS,words}; flush = 1; }

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
			else if (ch == '.') { words = new TOKEN{ DOT,NULL }; flush = 1; ch = 10; }
			else if (ch == '(') { words = new TOKEN{ LP,NULL }; flush = 1; ch = 10; }
			else if (ch == ')') { words = new TOKEN{ RP,NULL }; flush = 1; ch = 10;	}
			else if (ch == '[') { words = new TOKEN{ LB,NULL }; flush = 1; ch = 10;	}
			else if (ch == ']') { words = new TOKEN{ RB,NULL }; flush = 1; ch = 10;	}
			else if (ch == '{') { words = new TOKEN{ LC,NULL }; flush = 1; ch = 10; }
			else if (ch == '}') { words = new TOKEN{ RC,NULL }; flush = 1; ch = 10; }
			else if (ch == ';') { words = new TOKEN{ SEMI,NULL }; flush = 1; ch = 10; }
			else if (ch == ',') { words = new TOKEN{ COMMA,NULL }; flush = 1; ch = 10; }
			else if (ch == ' ' || ch == 10 || ch == '\t') { flush = 1; }
			else if (ch == '"') { state = S_STRING; }
			else if (ch == '"') { state = S_CHAR; }
			//else if (ch == '/') { state = S_COMMENT; }
			else { state = S_OP; }
		}
		else if (state == S_INT) {
			/*if (ch == '.') {
				state = S_DOUBLE;
			}
			else*/ 
			if (ch < '0' || ch > '9') {
				words = new TOKEN{ CONST_INT,words };
				flush = 1;
			}
		}
		else if (state == S_DOUBLE) {
			if (ch < '0' || ch > '9') {
				words = new TOKEN{ CONST_DOUBLE,words };
				flush = 1;
			}
		}
		else if (state == S_TOKEN) {
			if (!((ch >= '0'&&ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')) {
				str[flag] = 0;
				if (flag == 2) {
					MATCH("do", DO)
				}
				else if (flag == 3) {
					MATCH("int", INT)
				}
				else if (flag == 4) {
					MATCH("else", ELSE)
					else MATCH("char", CHAR)
				}
				else if (flag == 5) {
					MATCH("while", WHILE);
				}
				else if (flag == 6) {
					MATCH("return", RETURN)
					else MATCH("struct", STRUCT)
					else MATCH("double", DOUBLE)
				}

				if (flush == 0) {
					words = new TOKEN{ ID, words }; flush = 1;
				}
			}
		}
		else if (state == S_STRING) {
			if (ch == '"' && str[flag - 1] != '\\') {
				words = new TOKEN{ CONST_STRING,words };
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
			if (ch != '=' && ch != '&' && ch != '|' && ch != '/'&&ch!='*') {
				str[flag] = 0;
				MATCH(">", GT)
				else MATCH("<", LT)
				else MATCH("+", PLUS)
				else MATCH("-", MINUS)
				else MATCH("*", STAR)
				else MATCH("/", DIV)
				else MATCH("!", NOT)
				else MATCH("=", ASSIGNOP)
			}
			else {
				ch = 10;
				MATCH(">=", GE)
				else MATCH("<=", LE)
				else MATCH("==", EQ)
				else MATCH("!=", NE)
				else MATCH("&&", AND)
				else MATCH("||", OR)
				else if (strcmp(str, "//") == 0) {
					while (fscanf(file, "%c", &ch) == 1 && ch != '\n');
					flush = 1;
				}
				else if (strcmp(str, "/*") == 0) {
					do {
						while (fscanf(file, "%c", &ch) == 1 && ch != '*');
					} while (fscanf(file, "%c", &ch) == 1 && ch != '/');
					flush = 1;
				}
			}
		}/*
		else if (state == S_COMMENT) {
			if (ch == '*') {
				do {
					while (fscanf(file, "%c", &ch) == 1 && ch != '*');
				} while (fscanf(file, "%c", &ch) == 1 && ch != '/');
				flush = 1;
			}
			else if (ch == '/') {
				while (fscanf(file, "%c", &ch) == 1 && ch != '\n') {
				//	int abc;
					//scanf("%d", &abc);
					//printf("%c\n", ch);
				}
				flush = 1;
			}
			else {
				state = S_WRONG;
			}
		}*/
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
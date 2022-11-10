#include <iostream>
#include <cctype>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>

#ifndef PARSER_H
#define PARSER_H

using namespace std;

/* My var */
struct token_tb{
    int nbrToken;
    string lexeme;
    int ass_unknown_flag;
};
vector<token_tb> token_table;
token_tb tokenTuple; //Global Variablie
int nbrIdent;
int nbrConst;
int nbrOp;
int parsingResult;
int prev_sp, cur_sp; //stack pointer
int ass_unknown_flag = 0;
vector<int> tempValue_stk; // value of const operated
vector<token_tb> value_stk; // push the value into the var(IDENT); nbrToken == Value, lexeme == lexeme
set <string> ident_set; //for printing final result : 이후 들어오는 같은 Ident의 중복 출력을 방지하기 위함.
string errorMsg;
string warningMsg;


/* Global declarations */
/* Variables */
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp, *fopen();

/* Function declarations - lex */
void addChar();
void getChar();
void getNonBlank();
void pushToken();
int lex();

/* Function declarations - parser */
void program();
void statements();
void statement();
void expr();
void term();
void factor();
void error();

/* Function declarations - Else */
void printResult(int);
void initCnt();




/* Character Classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define CONST 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MUL_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define SEMI_COLON 27

#endif

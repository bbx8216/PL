#include "parser.h"

/* Main Driver */
int main(int argc, char *argv[]){
    if(argc != 2){
        cout << "ERROR - No File Entered" << endl;
    }
    if ((in_fp  = fopen(argv[1], "r")) == NULL){
        cout << "ERROR - Cannot Open Input File" << endl;
    }
    else{
        getChar();
        lex();
        program();
    }
    return 0;
}

/* Lookup Function */
int lookup(char ch){
    switch (ch){
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            nbrOp++;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            nbrOp++;
            break;
        case '*':
            addChar();
            nextToken = MUL_OP;
            nbrOp++;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            nbrOp++;
            break;
        case ';':
            addChar();
            nextToken = SEMI_COLON;
            break;
        case '=':
            addChar();
            nextToken = ASSIGN_OP;
            break;
        default:
            addChar();
            nextToken = EOF;
            break;
    }
    return nextToken;
}

/* addChar */
void addChar(){
    if (lexLen <= 98){
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
    else{
        cout << "ERROR - Lexeme is Too Long"<<endl;
    }
}

/* getChar */
void getChar(){
    if ((nextChar = getc(in_fp)) != EOF){
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else
            charClass = UNKNOWN;
    }
    else
        charClass = EOF;
}

/* getNonBlank */
void getNonBlank(){
    while (isspace(nextChar))
        getChar();
}

void pushToken(){
    tokenTuple.nbrToken = nextToken;
    tokenTuple.lexeme = lexeme;
    token_table.push_back(tokenTuple);
}

/* lex */
int lex(){
    lexLen = 0;
    getNonBlank();
    switch(charClass){
        case LETTER:
            nbrIdent++;
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT){
                addChar();
                getChar();
            }
            nextToken = IDENT;
            pushToken();
            ident_set.insert(lexeme); // set에 IDENT 모으기 (중복제거)
            break;
        case DIGIT:
            nbrConst++;
            addChar();
            getChar();
            while (charClass == DIGIT){
                addChar();
                getChar();
            }
            nextToken = CONST;
            pushToken();
            break;
        case UNKNOWN:
            //assign operation 은 따로 빼서
            if (nextChar == ':'){
                addChar();
                getChar();
                if (nextChar == '=')
                {
                    addChar();
                    getChar();
                    nextToken = ASSIGN_OP;
                    pushToken();
                }
            }
            else
            {
                lookup(nextChar);
                getChar();
                pushToken();
            }
            break;
        case EOF:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            break;
    }
    return nextToken;
}

/* Parser */

void program(){
    statements();
    printResult(1);
}

void statements(){
    statement();
    while(nextToken == SEMI_COLON){
        lex();
        statements();
    }
}
void statement(){
    prev_sp = token_table.size() - 1;
    if (nextToken == IDENT){
        token_tb valueTuple;
        valueTuple.lexeme = lexeme;
        valueTuple.ass_unknown_flag = 0;
        lex();
        if (nextToken == ASSIGN_OP){
            lex();
            expr();
            valueTuple.nbrToken = tempValue_stk.back();
            if (ass_unknown_flag == 1){
                valueTuple.ass_unknown_flag = 1;
            }
            value_stk.push_back(valueTuple);
        }
    }
    else{
        error();
        errorMsg ="\"Parsing Error - in Statement\"";
    }
    printResult(0);
    initCnt();
}

void expr(){
    term();
    while(nextToken == ADD_OP || nextToken == SUB_OP) {
        int checkOp;
        checkOp = nextToken;
        lex();
        term();
        /* Operating poped value have stored in tempValueStack */
        int value_tmp;
        if (checkOp == 21){
            value_tmp = tempValue_stk.back();
            tempValue_stk.pop_back();
            value_tmp += tempValue_stk.back();
            tempValue_stk.pop_back();
            tempValue_stk.push_back(value_tmp);

        }
        else if (checkOp == 22){
            value_tmp = tempValue_stk.back();
            tempValue_stk.pop_back();
            value_tmp -= tempValue_stk.back();
            tempValue_stk.pop_back();
            tempValue_stk.push_back(value_tmp);
        }
    }
}

void term(){
    factor();
    while (nextToken == MUL_OP || nextToken == DIV_OP){
        int checkOp;
        checkOp = nextToken;
        lex();
        factor();
        /*Operate value poped by tempValueStack*/
        int value_tmp;
        if (checkOp == 23){
            value_tmp = tempValue_stk.back();
            tempValue_stk.pop_back();
            value_tmp *= tempValue_stk.back();
            tempValue_stk.pop_back();
            tempValue_stk.push_back(value_tmp);
        }
        else if (checkOp == 24){
            value_tmp = tempValue_stk.back();
            tempValue_stk.pop_back();
            value_tmp /= tempValue_stk.back();
            tempValue_stk.pop_back();
            tempValue_stk.push_back(value_tmp);
        }
    }

}

void factor(){
    if(nextToken == IDENT || nextToken == CONST){
        if(nextToken == IDENT){
            /* Checking Error */
            int flag = 1;
            for (int i = 0 ; i < token_table.size() - 1 ; i++){
                if (token_table[i].lexeme == lexeme){
                    flag = 0;
                }
            }
            if (flag == 1) {
                error();
                errorMsg = "\"정의되지 않은 변수 (";
                errorMsg.append(lexeme);
                errorMsg.append(") 가 참조됨\"");
                ass_unknown_flag = 1;
            }
            /* Operating Variable Value1 */
            for (int i = 0 ; i < value_stk.size() ; i++){
                if (value_stk[i].lexeme == lexeme){
                    int value_tmp;
                    value_tmp = value_stk[i].nbrToken;
                    tempValue_stk.push_back(value_tmp);
                }
            }
        }
        else if(nextToken == CONST){
            // 여기에서는 스택에 담기만 하기!!!!!
            int value_tmp;
            value_tmp = atoi(lexeme);
            /* Storing Value into the tempValue Stack */
            tempValue_stk.push_back(value_tmp);
        }
        lex();
    }
    else{
        if(nextToken == LEFT_PAREN){
            lex();
            expr();
            if (nextToken == RIGHT_PAREN){
                lex();
            }
            else{
                error();
                errorMsg = "\"Parsing Error - in factor\"";
            }
        }
        else if (token_table.back().nbrToken == nextToken){
            parsingResult = 1;
            nbrOp--;
            warningMsg = " \"중복연산자 (";
            warningMsg.append(lexeme);
            warningMsg.append(") 제거\"");
            token_table.erase(token_table.end()-1);
            lex();
            factor(); //오류 복구를 해야하므로 factor() 함수를 다시 들어가야함.
        }
        else{
            parsingResult = 2;
        }
    }
}

void error(){
    parsingResult = 2;
}
void initCnt(){
    nbrIdent = 0;
    nbrConst = 0;
    nbrOp = 0;
    parsingResult = 0;
    tempValue_stk.clear();
};


void printResult(int mode){
    if (mode == 1){
        cout << "Result ==> "; // IDENT들 전부 꺼내서 결과 계산해주기
        set<string>::iterator iter;
        for(iter = ident_set.begin(); iter!= ident_set.end(); iter++){
            cout << *iter;
            int flag = 0;
            for(int i = 0; i < value_stk.size(); i++){
                if (value_stk[i].lexeme == *iter){
                    if(value_stk[i].ass_unknown_flag == 0){
                        flag = 1;
                        cout << ": " << value_stk[i].nbrToken <<"; ";
                    }
                }
            }
            if(flag == 0){
                cout << ": Unknown; ";
            }
        }
        cout <<endl;
    }

    else{
        // 읽은 문장 출력하기
        for (int i = prev_sp; i <token_table.size() ; i++){
            cout << token_table[i].lexeme;
        }
        cout << endl;
        // 파싱 결과 출력
        cout << "ID: " << nbrIdent << "; CONST: " << nbrConst << "; OP: "<< nbrOp <<";"<< endl;
        if (parsingResult == 2){
            cout << "(Error)" << errorMsg << endl;
        }
        else if (parsingResult == 1){
            cout << "(Warning)" << warningMsg << endl;
        }
        else if (parsingResult == 0){
            cout << "(OK)"<<endl;
        }
        errorMsg.clear();
        warningMsg.clear();
    }
};

// 입력 케이스 2번째, operand2 , target 또한 unknown으로 지정하기
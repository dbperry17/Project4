/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "parser.h"

using namespace std;

bool testing = false;
bool testParse = false;
bool testParseAll = false;
bool testStore = false;
bool testPrint = true;
bool testMatch = true;

void tester()
{
    if(testing)
    {
        cout << "\n\nSTARTING PROGRAM" << endl;
    }
    else
    {
        testParse = false;
        testParseAll = false;
        testStore = false;
        testPrint = false;
        testMatch = false;
    }
}

struct Parser::idListNode
{
    string id;
    idListNode *next;
};

struct Parser::Symbol
{
    string id;
    TokenType flag;
    int type;
    bool declared; //0 for implicit, 1 for explicit
    bool printed = 0;
};

//So that I don't have to remember what type number each one is
enum
{
    myBool = 0, myInt, myLong, myReal, myString
};

vector<Parser::Symbol> symTable;
int typeNum = 5;

/****
 *  NOTE: While dealing with testing for 1.4 errors
 *  I realized it would probably be more practical
 *  to make a function.
 *
 *  I decided against it because I was almost done
 *  pasting the code into the relevant functions,
 *  and I didn't want to redo stuff just for an
 *  asthetic difference
 */



/***********************
 * Teacher's functions *
 ***********************/

void Parser::syntax_error()
{
    cout << "Syntax Error\n";
    exit(1);
}

Token Parser::expect(TokenType expected_type)
{
    if(testParse && (expected_type != END_OF_FILE))
        cout << "\nExpecting a Terminal" << endl;


    else if(testParse)
        cout << "End of file" << endl;

    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    else if(testParse && (expected_type != END_OF_FILE))
        cout << "Terminal found: " << t.lexeme << endl;
    return t;
}

Token Parser::peek()
{
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    return t;
}

// Parsing
/*
 * program	    ->	var_section body
 * var_section	->	id_list SEMICOLON
 * id_list	    ->	ID COMMA id_list | ID
 * body	        ->	LBRACE stmt_list RBRACE
 * stmt_list	->	stmt stmt_list | stmt
 * stmt	        ->	assign_stmt | print_stmt | while_stmt | if_stmt | switch_stmt | for_stmt
 * assign_stmt	->	ID EQUAL primary SEMICOLON
 * assign_stmt	->	ID EQUAL expr SEMICOLON
 * expr	        ->	primary op primary
 * primary	    ->	ID | NUM
 * op       	->	PLUS | MINUS | MULT | DIV
 * print_stmt	->	print ID SEMICOLON
 * while_stmt	->	WHILE condition body
 * if_stmt  	->	IF condition body
 * condition	->	primary relop primary
 * relop	    ->	GREATER | LESS | NOTEQUAL
 * switch_stmt	->	SWITCH ID LBRACE case_list RBRACE
 * switch_stmt	->	SWITCH ID LBRACE case_list default_case RBRACE
 * for_stmt 	->	FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
 * case_list	->	case case_list | case
 * case	        ->	CASE NUM COLON body
 * default_case	->	DEFAULT COLON body
 */

//program -> var_section body
void Parser::parse_program()
{
    if(testParse)
        cout << "\nParsing: " << "program" << endl;

    loadDefaultSyms(); //my function
    //program -> var_section body
    parse_var_section();
    parse_body();
    if(testParse)
        cout << "Done Parsing: " << "program" << endl;
}

//var_section -> id_list SEMICOLON
void Parser::parse_var_section()
{

}

//id_list -> ID COMMA id_list | ID
Parser::idListNode* Parser::parse_id_list()
{
    if(testParseAll)
        cout << "\nParsing: " << "id_list" << endl;

    // id_list -> ID
    // id_list -> ID COMMA id_list
    Token t1 = peek();
    idListNode *result = new idListNode;
    expect(ID);
    Token t2 = lexer.GetToken();
    if (t2.token_type == COMMA)
    {
        // id_list -> ID COMMA id_list
        //General case
        result->id = t1.lexeme;
        result->next = parse_id_list();
        return result;
    }
    else if (t2.token_type == COLON)
    {
        // id_list -> ID
        lexer.UngetToken(t2);
        result->id = t1.lexeme;
        result->next = NULL;
        return result;
    }
    else
    {
        syntax_error();
    }

    if(testParseAll)
        cout << "Done Parsing: " << "id_list" << endl;

    return NULL; //I'm tired of getting warnings when I compile
}

//body -> LBRACE stmt_list RBRACE
void Parser::parse_body()
{
    if(testParseAll)
        cout << "\nParsing: " << "body" << endl;

    // body -> LBRACE stmt_list RBRACE
    expect(LBRACE);
    parse_stmt_list();
    expect(RBRACE);

    if(testParse)
        cout << "Done Parsing: " << "body" << endl;
}

//stmt_list -> stmt stmt_list | stmt
void Parser::parse_stmt_list()
{
    if(testParseAll)
        cout << "\nParsing: " << "stmt_list" << endl;

    // stmt_list -> stmt
    // stmt_list -> stmt stmt_list
    parse_stmt();
    Token t = peek();
    if (t.token_type == WHILE || t.token_type == ID ||
        t.token_type == SWITCH || t.token_type == DO)
    {
        // stmt_list -> stmt stmt_list
        parse_stmt_list();
    }
    else if (t.token_type == RBRACE)
    {
        // stmt_list -> stmt
    }
    else
    {
        syntax_error();
    }

    if(testParseAll)
        cout << "Done Parsing: " << "stmt_list" << endl;
}

//stmt -> assign_stmt
//stmt -> print_stmt
//stmt -> while_stmt
//stmt -> if_stmt
//stmt -> switch_stmt
//stmt -> for_stmt
void Parser::parse_stmt()
{
    if(testParseAll)
        cout << "\nParsing: " << "stmt" << endl;

    //stmt -> assign_stmt
    //stmt -> print_stmt
    //stmt -> while_stmt
    //stmt -> if_stmt
    //stmt -> switch_stmt
    //stmt -> for_stmt
    Token t = peek();
    if (t.token_type == ID)
    {
        // stmt -> assign_stmt
        parse_assign_stmt();
    }
    else if (t.token_type == PRINT)
    {
        //stmt -> print_stmt
        parse_print_stmt();
    }
    else if (t.token_type == WHILE)
    {
        // stmt -> while_stmt
        parse_while_stmt();
    }
    else if (t.token_type == IF)
    {
        //stmt -> if_stmt
        parse_if_stmt();
    }
    else if (t.token_type == SWITCH)
    {
        // stmt -> switch_stmt
        parse_switch_stmt();
    }
    else if (t.token_type == FOR)
    {
        //stmt -> for_stmt
        parse_for_stmt();
    }
    else
    {
        syntax_error();
    }

    if(testParseAll)
        cout << "Done Parsing: " << "stmt" << endl;
}

//assign_stmt -> ID EQUAL primary SEMICOLON
//assign_stmt -> ID EQUAL expr SEMICOLON
//TODO: assign_stmt -> ID EQUAL primary SEMICOLON
//TODO: get rid of type mismatch errors?
void Parser::parse_assign_stmt()
{
    if(testParse)
        cout << "\nParsing: " << "assign_stmt" << endl;


    Token t = peek();
    Symbol checkSym = declCheck(t.lexeme);

    //Programmer-defined type used as variable (error code 1.4)
    //If a previously declared type appears in the body of the program, the
    //type is used as a variable.
    if (checkSym.flag == TYPE)
        errorCode(1, 4, checkSym.id);
    if(checkSym.type == -1)
    {
        checkSym.flag = VAR;
        checkSym.type = typeNum;
        typeNum++;
        symTable.push_back(checkSym);
    }

    expect(ID);
    expect(EQUAL);
    int y = parse_expr();
    int z = unify(checkSym.type, y);
    //C1: The left hand side of an assignment should have the same
    //type as the right hand side of that assignment
    if(z == -1)
        typeMismatch(t.line_no, "C1");

    expect(SEMICOLON);

    if(testParse)
        cout << "Done Parsing: " << "assign_stmt" << endl;
}

//expr -> primary op primary
//old:
//expr -> term PLUS expr | term
//TODO: expr -> primary op primary
//TODO: get rid of type mismatch errors?
int Parser::parse_expr()
{
    if (testParse)
        cout << "\nParsing: " << "expr" << endl;

    int x = parse_term();

    Token t = lexer.GetToken();
    if (t.token_type == PLUS)
    {
        //expr -> term PLUS expr
        int y = parse_expr();
        int z = unify(x, y);
        //C2: The operands of an operation ( PLUS , MULT , and DIV )
        //should have the same type (it can be any type, including STRING and
        //BOOLEAN )
        if(z == -1)
            typeMismatch(t.line_no, "C2");
        return z;

    }
    else if ((t.token_type == SEMICOLON) || (t.token_type == RPAREN))
    {
        //expr -> term
        lexer.UngetToken(t);
        return x;
    }
    else
        syntax_error();

    if (testParse)
        cout << "Done Parsing: " << "expr" << endl;

    return -1;
}

//primary -> ID | NUM
//TODO: get rid of error code?
int Parser::parse_primary()
{
    if(testParse)
        cout << "\nParsing: " << "primary" << endl;

    Token t = lexer.GetToken();
    if((t.token_type == ID) || (t.token_type == NUM))
    {
        //primary -> ID
        if(t.token_type == ID)
        {
            Symbol checkSym = declCheck(t.lexeme);
            //Programmer-defined type used as variable (error code 1.4)
            //If a previously declared type appears in the body of the
            //program, the type is used as a variable.
            if(checkSym.flag == TYPE)
                errorCode(1, 4, checkSym.id);
            if(checkSym.type == -1)
            {
                checkSym.flag = VAR;
                checkSym.type = typeNum;
                typeNum++;
                symTable.push_back(checkSym);
            }

            return checkSym.type;
        }
            //primary -> NUM
        else if(t.token_type == NUM)
            return myInt;
    }
    else
        syntax_error();

    if(testParse)
        cout << "Done Parsing: " << "primary" << endl;

    return -1;
}

//op -> PLUS | MINUS | MULT | DIV
int Parser::parse_op()
{

}

//print_stmt -> print ID SEMICOLON
void Parser::parse_print_stmt()
{

}

//while_stmt -> WHILE condition body
//TODO: Eliminate error codes?
void Parser::parse_while_stmt()
{
    if(testParse)
        cout << "\nParsing: " << "while_stmt" << endl;

    expect(WHILE);

    Token t = peek(); //to get the line number
    int x = parse_condition();
    //C4: condition should be of type BOOLEAN
    if(x != myBool)
        typeMismatch(t.line_no, "C4");

    parse_body();

    if(testParse)
        cout << "Done Parsing: " << "while_stmt" << endl;
}

//if_stmt -> IF condition body
void Parser::parse_if_stmt()
{

}

//condition -> primary relop primary
//TODO: eliminate errors?
int Parser::parse_condition()
{
    if(testParse)
        cout << "\nParsing: " << "condition" << endl;

    Token t = lexer.GetToken();
    if((t.token_type == NUM) || (t.token_type == REALNUM))
    {
        //condition -> primary relop primary
        int x;
        if(t.token_type == NUM)
            x = myInt;
        else
            x = myReal;
        parse_relop();
        int y = parse_primary();
        //C3: The operands of a relational operator (see relop in
        //grammar) should have the same type (it can be any type,
        //including STRING and BOOLEAN )
        if(unify(x, y) == -1)
            typeMismatch(t.line_no, "C3");
        else
            return myBool;
    }
    else
        syntax_error();

    if(testParse)
        cout << "Done Parsing: " << "condition" << endl;

    return -1;
}

//relop -> GREATER | LESS | NOTEQUAL
void Parser::parse_relop()
{
    if(testParse)
        cout << "\nParsing: " << "relop" << endl;

    Token t = lexer.GetToken();
    if(testMatch)
        cout << t.lexeme << endl;
    if((t.token_type == GREATER) || (t.token_type == LESS) || (t.token_type == NOTEQUAL))
    {
        //relop-> GREATER
        //relop-> LESS
        //relop-> NOTEQUAL
    }
    else
        syntax_error();

    if(testParse)
        cout << "Done Parsing: " << "relop" << endl;
}

//switch_stmt -> SWITCH ID LBRACE case_list RBRACE
//switch_stmt -> SWITCH ID LBRACE case_list default_case RBRACE
//TODO: switch_stmt -> SWITCH ID LBRACE case_list default_case RBRACE
//TODO: eliminate errors?
void Parser::parse_switch_stmt()
{
    if(testParse)
        cout << "\nParsing: " << "switch_stmt" << endl;

    //type mismatch check
    expect(SWITCH);

    Token t = lexer.GetToken();
    if(t.token_type == ID)
    {
        Symbol checkSym = declCheck(t.lexeme);
        if (checkSym.type == -1) //symbol has been implicitly declared as INT
        {
            checkSym.type = myInt;
            checkSym.flag = VAR;
            symTable.push_back(checkSym);
        }
        else if (checkSym.type > 4) //symbol is of unknown type that is equivalent to INT
            unify(checkSym.type, myInt);
        else if (checkSym.type != myInt) //Symbol is built-in type that is not an INT.
        {
            //C5: The variable that follows the SWITCH keyword in switch_stmt should be
            //of type INT
            typeMismatch(t.line_no, "C5");
        }
    }

    expect(LBRACE);
    parse_case_list();
    expect(RBRACE);

    if(testParse)
        cout << "Done Parsing: " << "switch_stmt" << endl;
}

//for_stmt -> FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
void Parser::parse_for_stmt()
{

}

//case_list -> case case_list | case
void Parser::parse_case_list()
{
    if(testParse)
        cout << "\nParsing: " << "case_list" << endl;

    parse_case();
    Token t = peek();
    if(t.token_type == CASE)
    {
        //case_list -> case case_list
        parse_case_list();
    }
    else if(t.token_type == RBRACE)
    {
        //case_list -> case
    }
    else
        syntax_error();

    if(testParse)
        cout << "Done Parsing: " << "case_list" << endl;

}

//case -> CASE NUM COLON body
void Parser::parse_case()
{
    if(testParse)
        cout << "\nParsing: " << "case" << endl;

    expect(CASE);
    expect(NUM);
    expect(COLON);
    parse_body();

    if(testParse)
        cout << "Done Parsing: " << "case" << endl;
}

//default_case -> DEFAULT COLON body
void Parser::parse_default_case()
{

}


/************************************
 * Functions I created from scratch *
 ************************************/

//Loads int, real, boolean, string, and long into symbol table
void Parser::loadDefaultSyms()
{
    //Listed in different order than in spec
    //so as to make outputting to spec easier
    Symbol tempSym;
    tempSym.flag = TYPE;
    tempSym.declared = 0; //counting defaults as implicit declarations
    tempSym.id = "BOOLEAN";
    tempSym.type = myBool;
    symTable.push_back(tempSym);

    tempSym.id = "INT";
    tempSym.type = myInt;
    symTable.push_back(tempSym);

    tempSym.id = "LONG";
    tempSym.type = myLong;
    symTable.push_back(tempSym);

    tempSym.id = "REAL";
    tempSym.type = myReal;
    symTable.push_back(tempSym);

    tempSym.id = "STRING";
    tempSym.type = myString;
    symTable.push_back(tempSym);

}

//outputs error code
//cat = category 1 or 2, spec = specific error
void Parser::errorCode(int cat, int spec, string symbol)
{
    cout << "ERROR CODE " << cat << "." << spec << " " << symbol << endl;
    exit(1);
}

//outputs type mismatch error
void Parser::typeMismatch(int lineNo, string constraint)
{
    cout << "TYPE MISMATCH " << lineNo << " " << constraint << endl;
    exit(1);
}

//Check to see if item is in symbol table
Parser::Symbol Parser::declCheck(string name)
{
    Symbol notFound;
    notFound.id = name;
    notFound.flag = ERROR;
    notFound.type = -1;
    notFound.declared = 0;

    for(int iter = 0; iter < (int)symTable.size(); iter++)
    {
        //Remember, string comparison returns 0 if strings are equal
        if(name.compare((symTable[iter]).id) == 0)
        {
            return symTable[iter];
        }
    }

    return notFound;
}


//Print types and variables
void Parser::print()
{
    if(testPrint)
    {
        cout << "\nList of IDs:" << endl;
        cout << "NAME \t\tFLAG \tTYPE \tEXP_DECL" << endl;
        for (int i = 0; i < (int)symTable.size(); i++)
        {
            if((i == myBool))
                cout << symTable[i].id << " \t";
            else
                cout << symTable[i].id << " \t\t";

            if (symTable[i].flag == TYPE)
                cout << "TYPE \t";
            else if (symTable[i].flag == VAR)
                cout << "VAR \t";
            else
                cout << "ERROR :\t";
            cout << symTable[i].type << " \t";
            cout << boolalpha << symTable[i].declared << endl;

        }
        cout << "End of list\n" << endl;
    }

    /*
    //From PDF file
    for each built-in type T:
    {
        output T
        output all names that are type-equivalent with T in order of their appearance
        mark outputted names to avoid re-printing them later
        output "#\n"
    }
    if there are unprinted names left:
    {
        for each unprinted name N in order of appearance:
        {
            output N
            output all other names that are type-equivalent with N in order of their appearance
            output "#\n"
        }
    }
    */

    for(int i = 0; i < 5; i++)
    {
        cout << symTable[i].id << " ";
        for(int j = 5; j < (int)symTable.size(); j++)
        {
            if(symTable[j].type == i)
            {
                cout << symTable[j].id << " ";
                symTable[j].printed = true;
            }
        }
        cout << "#" << endl;
    }

    for(int i = 5; i < (int)symTable.size(); i++)
    {
        if(!symTable[i].printed)
        {
            for(int j = 5; j < (int)symTable.size(); j++) //can start with i because all priors will have gone through
            {
                if(symTable[j].type == symTable[i].type)
                {
                    cout << symTable[j].id << " ";
                    symTable[j].printed = true;
                }
            }
            cout << "#" << endl;
        }
    }

}

//Determine which common type (if possible) typeNum1 and typeNum2 can be equal to
int Parser::unify(int typeNum1, int typeNum2)
{
    int newType;

    if(typeNum1 == typeNum2) //both types are the same
        newType = typeNum1;
    else if((typeNum1 < 5) && (typeNum2 < 5)) //not same type, are different built-in types
        newType = -1; //-1 for type mismatch
    else if(typeNum1 < 5) //first is built in, second is not
    {
        //change all symbols of typeNum2 to that of typeNum1
        for(int i = 0; i < (int)symTable.size(); i++)
        {
            if(symTable[i].type == typeNum2)
                symTable[i].type = typeNum1;
        }
        newType = typeNum1;
    }
    else //either the second is built in and the first is not, or neither is built in
    {
        //change all symbols of typeNum1 to that of typeNum2
        for(int i = 0; i < (int)symTable.size(); i++)
        {
            if(symTable[i].type == typeNum1)
                symTable[i].type = typeNum2;
        }
        newType = typeNum2;
    }

    return newType;
}


//Teacher's function
void Parser::ParseInput()
{
    parse_program();
    expect(END_OF_FILE);
}

int main()
{
    tester();

    Parser parser;

    parser.ParseInput();

    parser.print();
}
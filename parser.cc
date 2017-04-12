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
    bool printed = 0;
};


vector<Parser::Symbol> symTable;


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
    if(testStore)
        cout << "\nParsing: " << "var_decl" << endl;

    //var_section -> id_list SEMICOLON
    // var_decl -> id_list COLON type_name SEMICOLON
    idListNode *head = parse_id_list();

    //check to see if any items in list are already used
    idListNode *current = head;
    vector<string> idVec;
    Symbol tmpSym;


    while(current != NULL)
    {
        idVec.push_back(current->id);

        //putting variables in symbol table
        tmpSym.id = current->id;
        symTable.push_back(tmpSym);

        current = current->next;
    }

    expect(SEMICOLON);

    if(testStore)
        cout << "Done Parsing: " << "var_decl" << endl;
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
    if ((t.token_type == WHILE) || (t.token_type == ID) ||
        (t.token_type == SWITCH) || (t.token_type == PRINT) ||
        (t.token_type == FOR) || (t.token_type == IF))
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
void Parser::parse_assign_stmt()
{
    if(testParse)
        cout << "\nParsing: " << "assign_stmt" << endl;


    Token t = peek();

    //Checking for implicit declaration
    Symbol checkSym;
    checkSym.id = t.lexeme;
    if(declCheck(checkSym.id) == -1)
        symTable.push_back(checkSym);

    expect(ID);
    expect(EQUAL);
    t = peek();
    parse_primary();
    Token t2 = peek();
    if((t2.token_type == PLUS) || (t2.token_type == MINUS) ||
       (t2.token_type == MULT) || (t2.token_type == DIV))
    {
        //assign_stmt -> ID EQUAL expr SEMICOLON
        lexer.UngetToken(t);
        parse_expr();
    }
    else
    {
        //assign_stmt -> ID EQUAL primary SEMICOLON
    }

    expect(SEMICOLON);

    if(testParse)
        cout << "Done Parsing: " << "assign_stmt" << endl;
}

//expr -> primary op primary
void Parser::parse_expr()
{
    parse_primary();
    parse_op();
    parse_primary();
}

//primary -> ID | NUM
void Parser::parse_primary()
{
    if(testParse)
        cout << "\nParsing: " << "primary" << endl;

    Token t = lexer.GetToken();
    //primary -> ID
    if(t.token_type == ID)
    {
        //check for implicity declared variables
        if(declCheck(t.lexeme) == -1)
        {
            Symbol checkSym;
            checkSym.id = t.lexeme;
            symTable.push_back(checkSym);
        }
    }
    else if(t.token_type == NUM)
    {
        //primary -> NUM
    }
    else
        syntax_error();

    if(testParse)
        cout << "Done Parsing: " << "primary" << endl;

}

//op -> PLUS | MINUS | MULT | DIV
void Parser::parse_op()
{
    Token t = lexer.GetToken();
    if((t.token_type == PLUS) || (t.token_type == MINUS) ||
       (t.token_type == MULT) || (t.token_type == DIV))
    {
        //op -> PLUS | MINUS | MULT | DIV
    }
    else
        syntax_error();
}

//print_stmt -> print ID SEMICOLON
void Parser::parse_print_stmt()
{
    expect(PRINT);
    expect(ID);
    expect(SEMICOLON);
}

//while_stmt -> WHILE condition body
void Parser::parse_while_stmt()
{
    if(testParse)
        cout << "\nParsing: " << "while_stmt" << endl;

    expect(WHILE);
    parse_condition();
    parse_body();

    if(testParse)
        cout << "Done Parsing: " << "while_stmt" << endl;
}

//if_stmt -> IF condition body
void Parser::parse_if_stmt()
{
    expect(IF);
    parse_condition();
    parse_body();
}

//condition -> primary relop primary
void Parser::parse_condition()
{
    if(testParse)
        cout << "\nParsing: " << "condition" << endl;

    parse_primary();
    parse_relop();
    parse_primary();

    if(testParse)
        cout << "Done Parsing: " << "condition" << endl;
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
//TODO: eliminate errors
void Parser::parse_switch_stmt()
{
    if(testParse)
        cout << "\nParsing: " << "switch_stmt" << endl;

    //type mismatch check
    expect(SWITCH);

    Token t = lexer.GetToken();
    if(t.token_type == ID)
    {
        if (declCheck(t.lexeme) == -1) //symbol has been implicitly declared as INT
        {
            Symbol checkSym;
            checkSym.id = t.lexeme;
            symTable.push_back(checkSym);
        }
    }
    else
        syntax_error();

    expect(LBRACE);
    parse_case_list();

    t = peek();


    if(t.token_type = DEFAULT)
    {
        parse_default_case();
        //switch_stmt -> SWITCH ID LBRACE case_list default_case RBRACE
    }
    else
    {
        //switch_stmt -> SWITCH ID LBRACE case_list RBRACE
    }

    expect(RBRACE);

    if(testParse)
        cout << "Done Parsing: " << "switch_stmt" << endl;
}

//for_stmt -> FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
void Parser::parse_for_stmt()
{
    expect(FOR);
    expect(LPAREN);
    parse_assign_stmt();
    parse_condition();
    expect(SEMICOLON);
    parse_assign_stmt();
    expect(RPAREN);
    parse_body();
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
    expect(DEFAULT);
    expect(COLON);
    parse_body();
}


/************************************
 * Functions I created from scratch *
 ************************************/


//Check to see if item is in symbol table
int Parser::declCheck(string name)
{
    for(int iter = 0; iter < (int)symTable.size(); iter++)
    {
        //Remember, string comparison returns 0 if strings are equal
        if(name.compare((symTable[iter]).id) == 0)
        {
            return iter;
        }
    }

    return -1;
}


//Print types and variables
void Parser::print()
{
    for(int i = 5; i < (int)symTable.size(); i++)
    {
        if(!symTable[i].printed)
        {
                cout << symTable[i].id << " ";
                symTable[i].printed = true;
        }
    }
    cout << "#" << endl;
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
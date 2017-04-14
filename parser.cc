/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "parser.h"
#include "compiler.h"

using namespace std;

vector<ValueNode> symTable;


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
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();

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
    //program -> var_section body
    parse_var_section();
    parse_body();
}

//var_section -> id_list SEMICOLON
void Parser::parse_var_section()
{
    parse_id_list();
    expect(SEMICOLON);
}

//id_list -> ID COMMA id_list | ID
void Parser::parse_id_list()
{
    // id_list -> ID
    // id_list -> ID COMMA id_list
    ValueNode tmpSym;
    Token t = peek();
    expect(ID);
    tmpSym.name = t.lexeme;
    symTable.push_back(tmpSym);
    t = lexer.GetToken();
    if (t.token_type == COMMA)
    {
        // id_list -> ID COMMA id_list
        //General case
        parse_id_list();
    }
    else if (t.token_type == SEMICOLON)
    {
        // id_list -> ID
        lexer.UngetToken(t);
    }
    else
        syntax_error();
}

//body -> LBRACE stmt_list RBRACE
void Parser::parse_body()
{
    // body -> LBRACE stmt_list RBRACE
    expect(LBRACE);
    parse_stmt_list();
    expect(RBRACE);
}

//stmt_list -> stmt stmt_list | stmt
void Parser::parse_stmt_list()
{
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
}

//stmt -> assign_stmt
//stmt -> print_stmt
//stmt -> while_stmt
//stmt -> if_stmt
//stmt -> switch_stmt
//stmt -> for_stmt
void Parser::parse_stmt()
{
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
}

//assign_stmt -> ID EQUAL primary SEMICOLON
//assign_stmt -> ID EQUAL expr SEMICOLON
void Parser::parse_assign_stmt()
{
    expect(ID);
    expect(EQUAL);
    Token t = peek();
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
    Token t = lexer.GetToken();
    if(t.token_type == ID)
    {
        //primary -> ID
    }
    else if(t.token_type == NUM)
    {
        //primary -> NUM
    }
    else
        syntax_error();
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
    expect(WHILE);
    parse_condition();
    parse_body();
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
    parse_primary();
    parse_relop();
    parse_primary();
}

//relop -> GREATER | LESS | NOTEQUAL
void Parser::parse_relop()
{
    Token t = lexer.GetToken();
    if((t.token_type == GREATER) || (t.token_type == LESS) || (t.token_type == NOTEQUAL))
    {
        //relop-> GREATER
        //relop-> LESS
        //relop-> NOTEQUAL
    }
    else
        syntax_error();
}

//switch_stmt -> SWITCH ID LBRACE case_list RBRACE
//switch_stmt -> SWITCH ID LBRACE case_list default_case RBRACE
void Parser::parse_switch_stmt()
{
    expect(SWITCH);
    expect(ID);
    expect(LBRACE);
    parse_case_list();

    Token t = peek();


    if(t.token_type == DEFAULT)
    {
        parse_default_case();
        //switch_stmt -> SWITCH ID LBRACE case_list default_case RBRACE
    }
    else
    {
        //switch_stmt -> SWITCH ID LBRACE case_list RBRACE
    }

    expect(RBRACE);
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
}

//case -> CASE NUM COLON body
void Parser::parse_case()
{
    expect(CASE);
    expect(NUM);
    expect(COLON);
    parse_body();
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
        if(name.compare((symTable[iter]).name) == 0)
        {
            return iter;
        }
    }

    return -1; //to prevent warnings
}


//Print types and variables
void Parser::print()
{
    for(int i = 0; i < (int)symTable.size(); i++)
    {
        cout << symTable[i].name << " ";

    }
    cout << "#" << endl;
}


//Teacher's function
void Parser::ParseInput()
{
    parse_program();
    expect(END_OF_FILE);
}
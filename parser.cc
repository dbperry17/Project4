/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <algorithm>
#include "parser.h"

using namespace std;

vector<ValueNode*> symTable;
struct Parser::ExprNode
{
    ValueNode* op1;
    ValueNode* op2;
    ArithmeticOperatorType arith;
};

/**********************
 *  HELPER FUNCTIONS  *
 **********************/

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

//Check to see if item is in symbol table
//Returns position of item if found
//Returns -1 if not found
ValueNode* Parser::symLookup(string name)
{
    ValueNode* tempNode;
    int iter = 0;
    bool found = false;
    for(iter = 0; iter < (int)symTable.size(); iter++)
    {
        //Remember, string comparison returns 0 if strings are equal
        if(name.compare((symTable[iter])->name) == 0)
        {
            tempNode = symTable[iter];
            found = true;
        }
    }

    //Symbol not in table
    if(!found)
        tempNode = addValNode(name);

    return tempNode;
}

//Adds new ValueNode to symbol table
ValueNode* Parser::addValNode(string name)
{
    ValueNode* temp;
    temp->name = "constant";
    symTable.push_back(temp);
    temp = symLookup(name); //yay recursion
    return temp;
}

//Turns a constant into a value node
ValueNode* Parser::constNode(int val)
{
    ValueNode* temp;
    temp->name = "constant";
    temp->value = val;
    return temp;
}


//Print types and variables
//for use of testing parsing only
void Parser::print()
{
    for(int i = 0; i < (int)symTable.size(); i++)
    {
        cout << symTable[i]->name << " ";

    }
    cout << "#" << endl;
}


/*************
 *  PARSING  *
 *************/

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
 * while_stmt	->	WHILE condition body
 * if_stmt  	->	IF condition body
 * for_stmt 	->	FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
 * condition	->	primary relop primary
 * relop	    ->	GREATER | LESS | NOTEQUAL
 * switch_stmt	->	SWITCH ID LBRACE case_list RBRACE
 * switch_stmt	->	SWITCH ID LBRACE case_list default_case RBRACE
 * case_list	->	case case_list | case
 * case	        ->	CASE NUM COLON body
 * default_case	->	DEFAULT COLON body
 * print_stmt	->	print ID SEMICOLON
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
    ValueNode* tmpSym;
    Token t = peek();
    expect(ID);
    tmpSym->name = t.lexeme;
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
StatementNode * Parser::parse_stmt()
{
    StatementNode* stmt;
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
        stmt->type = ASSIGN_STMT;
        stmt->assign_stmt = parse_assign_stmt();
    }
    else if (t.token_type == PRINT)
    {
        //stmt -> print_stmt
        stmt->type = PRINT_STMT;
        stmt->print_stmt = parse_print_stmt();
    }
    else if (t.token_type == WHILE)
    {
        // stmt -> while_stmt
        stmt->type = IF_STMT;
        stmt->if_stmt = parse_while_stmt();
    }
    else if (t.token_type == IF)
    {
        //stmt -> if_stmt
        stmt->type = IF_STMT;
        stmt->if_stmt = parse_if_stmt();
    }
    else if (t.token_type == SWITCH)
    {
        // stmt -> switch_stmt
        stmt->type = IF_STMT;
        stmt->if_stmt = parse_switch_stmt();
    }
    else if (t.token_type == FOR)
    {
        //stmt -> for_stmt
        stmt->type = IF_STMT;
        stmt->if_stmt = parse_for_stmt();
    }
    else
    {
        syntax_error();
    }

    return stmt;
}

//assign_stmt -> ID EQUAL primary SEMICOLON
//assign_stmt -> ID EQUAL expr SEMICOLON
AssignmentStatement* Parser::parse_assign_stmt()
{
    AssignmentStatement* stmt;
    ValueNode* tempNode;

    Token t = expect(ID);
    tempNode = symLookup(t.lexeme);
    stmt->left_hand_side = tempNode;
    expect(EQUAL);
    t = peek();
    tempNode = parse_primary();
    Token t2 = peek();

    if((t2.token_type == PLUS) || (t2.token_type == MINUS) ||
       (t2.token_type == MULT) || (t2.token_type == DIV))
    {
        //assign_stmt -> ID EQUAL expr SEMICOLON
        lexer.UngetToken(t);
        ExprNode* exprNode;
        exprNode = parse_expr();
        stmt->op = exprNode->arith;
        stmt->operand1 = exprNode->op1;
        stmt->operand2 = exprNode->op2;
    }
    else
    {
        //assign_stmt -> ID EQUAL primary SEMICOLON
        stmt->operand1 = tempNode;
        stmt->op = OPERATOR_NONE;
        stmt->operand2 = NULL;
    }

    expect(SEMICOLON);

    return stmt;
}

//expr -> primary op primary
ExprNode* Parser::parse_expr()
{
    Parser::ExprNode* expr;

    expr->op1 = parse_primary();
    expr->arith = parse_op();
    expr->op2 = parse_primary();

    return expr;
}

//primary -> ID | NUM
ValueNode* Parser::parse_primary()
{
    ValueNode* node;
    Token t = lexer.GetToken();
    if(t.token_type == ID)
    {
        //primary -> ID

        node = symLookup(t.lexeme);
    }
    else if(t.token_type == NUM)
    {
        //primary -> NUM
        node = constNode(stoi(t.lexeme));
    }
    else
        syntax_error();

    return node;
}

//op -> PLUS | MINUS | MULT | DIV
ArithmeticOperatorType Parser::parse_op()
{
    Token t = lexer.GetToken();
    if(t.token_type == PLUS)
    {
        //op -> PLUS
        return OPERATOR_PLUS;
    }
    else if(t.token_type == MINUS)
    {
        //op -> MINUS
        return OPERATOR_MINUS;
    }
    else if(t.token_type == MULT)
    {
        //op -> MULT
        return OPERATOR_MULT;
    }
    else if(t.token_type == DIV)
    {
        //op -> DIV
        return OPERATOR_DIV;
    }
    else
        syntax_error();
}

//while_stmt -> WHILE condition body
IfStatement* Parser::parse_while_stmt()
{
    IfStatement* stmt;

    expect(WHILE);
    parse_condition();
    parse_body();

    return stmt;
}

//if_stmt -> IF condition body
IfStatement * Parser::parse_if_stmt()
{
    IfStatement* stmt;

    expect(IF);
    parse_condition();
    parse_body();

    return stmt;
}

//for_stmt -> FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
IfStatement* Parser::parse_for_stmt()
{
    IfStatement* stmt;

    expect(FOR);
    expect(LPAREN);
    parse_assign_stmt();
    parse_condition();
    expect(SEMICOLON);
    parse_assign_stmt();
    expect(RPAREN);
    parse_body();

    return stmt;
}

//condition -> primary relop primary
void Parser::parse_condition()
{
    parse_primary();
    parse_relop();
    parse_primary();
}

//relop -> GREATER | LESS | NOTEQUAL
ConditionalOperatorType Parser::parse_relop()
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
IfStatement* Parser::parse_switch_stmt()
{
    IfStatement* stmt;

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

    return stmt;
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

//print_stmt -> print ID SEMICOLON
PrintStatement* Parser::parse_print_stmt()
{
    PrintStatement* stmt;
    expect(PRINT);
    expect(ID);
    expect(SEMICOLON);

    return stmt;
}

//Teacher's function
void Parser::ParseInput()
{
    parse_program();
    expect(END_OF_FILE);
}


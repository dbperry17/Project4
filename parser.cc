/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <algorithm>
#include "parser.h"

using namespace std;
bool errorFind = false;
bool testIf = false;

vector<ValueNode*> symTable;
struct Parser::ExprNode
{
    ValueNode* op1;
    ValueNode* op2;
    ArithmeticOperatorType arith;
};

struct Parser::CondNode
{
    ValueNode* op1;
    ValueNode* op2;
    ConditionalOperatorType condType;
};

/********************************************************************
 *                                                                  *
 * NOTE: I am aware we "can assume that there are no syntax or      *
 *       semantic errors in the input program." However, I found    *
 *       checking for syntax errors to be useful while altering     *
 *       the parsing functions so that I could make sure I didn't   *
 *       mess anything up, so I have chosen to keep that bit of     *
 *       code.                                                      *
 *                                                                  *
 ********************************************************************/

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

//Check to see if item is in symbol table
ValueNode* Parser::symLookup(string name)
{
    if(errorFind)
        cout << "Starting " << "symLookup" << endl;
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

    //Symbol not in table, so add it
    if(!found)
        tempNode = addValNode(name);

    if(errorFind)
        cout << "Finished " << "symLookup" << endl;

    return tempNode;
}

//Adds new ValueNode to symbol table
ValueNode* Parser::addValNode(string name)
{
    if(errorFind)
        cout << "Starting " << "addValNode" << endl;

    ValueNode* temp = new ValueNode;
    temp->name = "constant";
    symTable.push_back(temp);
    temp = symLookup(name); //yay recursion

    if(errorFind)
        cout << "Finished " << "addValNode" << endl;

    return temp;
}

//Turns a constant into a value node
ValueNode* Parser::constNode(int val)
{
    if(errorFind)
        cout << "Starting " << "constNode" << endl;

    ValueNode* temp = new ValueNode;
    temp->name = "";
    temp->value = val;

    if(errorFind)
        cout << "Finished " << "constNode" << endl;

    return temp;
}


void Parser::printStatementList(StatementNode* head)
{
    cout << "\nStatement List: " << endl;

    StatementNode* current = head;
    while (current != NULL)
    {
        if (current->type == ASSIGN_STMT)
        {
            cout << "Assign Statement -> ";
        }
        else if (current->type == IF_STMT)
        {
            cout << "If Statement -> " << endl;
            cout << "\tTrue path: ";
            printStatementList(head->if_stmt->true_branch);
            cout << "\n" << endl;
        }
        else if (current->type == NOOP_STMT)
        {
            cout << "No-Op Statement -> ";
        }
        else if (current->type == PRINT_STMT)
        {
            cout << "Print Statement -> ";
        }
        else if (current->type == GOTO_STMT)
        {
            cout << "Go-To Statement -> ";
        }

        current = current->next;
    }


    cout << "NULL" << endl;
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
 * if_stmt  	->	IF condition body
 * while_stmt	->	WHILE condition body
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
StatementNode* Parser::parse_program()
{
    if(errorFind)
        cout << "Starting " << "parse_program" << endl;

    //program -> var_section body
    parse_var_section();
    StatementNode* node = parse_body();

    if(testIf)
    {
        printStatementList(node);
    }

    if(errorFind)
        cout << "Finished " << "parse_program" << endl;

    return node;
}

//var_section -> id_list SEMICOLON
//TODO: See if this needs anything
void Parser::parse_var_section()
{
    if(errorFind)
        cout << "Starting " << "parse_var_section" << endl;

    parse_id_list();
    expect(SEMICOLON);

    if(errorFind)
        cout << "Finished " << "parse_var_section" << endl;
}

//id_list -> ID COMMA id_list | ID
//TODO: See if this needs anything
void Parser::parse_id_list()
{
    if(errorFind)
        cout << "Starting " << "parse_id_list" << endl;

    // id_list -> ID
    // id_list -> ID COMMA id_list
    ValueNode* tmpSym = new ValueNode;
    Token t = expect(ID);
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

    if(errorFind)
        cout << "Finished " << "parse_id_list" << endl;
}

//body -> LBRACE stmt_list RBRACE
StatementNode* Parser::parse_body()
{
    if(errorFind)
        cout << "\n\nStarting " << "parse_body" << endl;

    // body -> LBRACE stmt_list RBRACE
    expect(LBRACE);
    StatementNode* node = parse_stmt_list();
    expect(RBRACE);

    if(errorFind)
        cout << "Finished " << "parse_body" << endl;

    return node;
}

//stmt_list -> stmt stmt_list | stmt
StatementNode* Parser::parse_stmt_list()
{
    if(errorFind)
        cout << "Starting " << "parse_stmt_list" << endl;

    // stmt_list -> stmt
    // stmt_list -> stmt stmt_list
    StatementNode* node = parse_stmt();
    StatementNode* nodeList;

    Token t = peek();
    if ((t.token_type == WHILE) || (t.token_type == ID) ||
        (t.token_type == SWITCH) || (t.token_type == PRINT) ||
        (t.token_type == FOR) || (t.token_type == IF))
    {
        // stmt_list -> stmt stmt_list
        nodeList = parse_stmt_list();

        if(node->type == IF_STMT)
        {
            node->next = node->if_stmt->false_branch;
            node->if_stmt->false_branch->next = nodeList;
        }
        else
            node->next = nodeList;
    }
    else if (t.token_type == RBRACE)
    {
        // stmt_list -> stmt
        if(node->type == IF_STMT)
        {
            node->next = node->if_stmt->false_branch;
            node->if_stmt->false_branch->next = NULL;
        }
        else
            node->next = NULL;
    }
    else
    {
        syntax_error();
    }

    if(errorFind)
    {
        cout << "Finished " << "parse_stmt_list" << endl;
    }

    return node;
}

//stmt -> assign_stmt
//stmt -> print_stmt
//stmt -> while_stmt
//stmt -> if_stmt
//stmt -> switch_stmt
//stmt -> for_stmt
//TODO: Check back after finishing parse_while_stmt
//TODO: Check back after finishing parse_switch_stmt
//TODO: Check back after finishing parse_for_stmt
StatementNode * Parser::parse_stmt()
{
    if(errorFind)
        cout << "Starting " << "parse_stmt" << endl;

    StatementNode* stmt = new StatementNode;

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
        if(errorFind)
            cout << "\nStatement type: Assign" << endl;
        stmt->type = ASSIGN_STMT;
        stmt->assign_stmt = parse_assign_stmt();
    }
    else if (t.token_type == PRINT)
    {
        //stmt -> print_stmt
        if(errorFind)
            cout << "\nStatement type: Print" << endl;
        stmt->type = PRINT_STMT;
        stmt->print_stmt = parse_print_stmt();
    }
    else if (t.token_type == WHILE)
    {
        // stmt -> while_stmt
        if(errorFind)
            cout << "\nStatement type: While" << endl;
        stmt->type = IF_STMT;
        stmt->if_stmt = parse_while_stmt();
    }
    else if (t.token_type == IF)
    {
        //stmt -> if_stmt
        if(errorFind)
            cout << "\nStatement type: If" << endl;
        stmt->type = IF_STMT;
        stmt->if_stmt = parse_if_stmt();
    }
    else if (t.token_type == SWITCH)
    {
        // stmt -> switch_stmt
        if(errorFind)
            cout << "\nStatement type: Switch" << endl;
        stmt->type = IF_STMT;
        stmt->if_stmt = parse_switch_stmt();
    }
    else if (t.token_type == FOR)
    {
        //stmt -> for_stmt
        if(errorFind)
            cout << "\nStatement type: For" << endl;
        stmt->type = IF_STMT;
        stmt->if_stmt = parse_for_stmt();
    }
    else
    {
        syntax_error();
    }

    if(errorFind)
        cout << "Finished " << "parse_stmt" << endl;

    return stmt;
}

//assign_stmt -> ID EQUAL primary SEMICOLON
//assign_stmt -> ID EQUAL expr SEMICOLON
AssignmentStatement* Parser::parse_assign_stmt()
{
    if(errorFind)
        cout << "Starting " << "parse_assign_stmt" << endl;

    AssignmentStatement* stmt = new AssignmentStatement;
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

    if(errorFind)
        cout << "Finished " << "parse_assign_stmt" << endl;

    return stmt;
}

//expr -> primary op primary
Parser::ExprNode* Parser::parse_expr()
{
    if(errorFind)
        cout << "Starting " << "parse_expr" << endl;

    ExprNode* expr = new ExprNode;

    expr->op1 = parse_primary();
    expr->arith = parse_op();
    expr->op2 = parse_primary();

    if(errorFind)
        cout << "Finished " << "parse_expr" << endl;

    return expr;
}

//primary -> ID | NUM
ValueNode* Parser::parse_primary()
{
    if(errorFind)
        cout << "Starting " << "parse_primary" << endl;

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

    if(errorFind)
        cout << "Finished " << "parse_primary" << endl;

    return node;
}

//op -> PLUS | MINUS | MULT | DIV
ArithmeticOperatorType Parser::parse_op()
{
    if(errorFind)
        cout << "Starting " << "parse_op" << endl;

    Token t = lexer.GetToken();
    ArithmeticOperatorType op;
    op = OPERATOR_NONE;

    if(t.token_type == PLUS)
    {
        //op -> PLUS
        op = OPERATOR_PLUS;
    }
    else if(t.token_type == MINUS)
    {
        //op -> MINUS
        op = OPERATOR_MINUS;
    }
    else if(t.token_type == MULT)
    {
        //op -> MULT
        op = OPERATOR_MULT;
    }
    else if(t.token_type == DIV)
    {
        //op -> DIV
        op = OPERATOR_DIV;
    }
    else
        syntax_error();

    if(errorFind)
        cout << "Finished " << "parse_op" << endl;

    return op;
}

//if_stmt -> IF condition body
IfStatement * Parser::parse_if_stmt()
{
    IfStatement* stmt = new IfStatement;
    StatementNode* noOpNode = new StatementNode;
    noOpNode->type = NOOP_STMT;

    CondNode* condNode;

    expect(IF);
    condNode = parse_condition();
    stmt->condition_operand1 = condNode->op1;
    stmt->condition_op = condNode->condType;
    stmt->condition_operand2 = condNode->op2;
    stmt->true_branch = parse_body();

    StatementNode* current = stmt->true_branch;
    //Find end of if's body
    while(current->next != NULL)
    {
        current = current->next;
    }

    //append no-op node to end of if's body
    current->next = noOpNode;
    stmt->false_branch = noOpNode;

    return stmt;
}


//while_stmt -> WHILE condition body
//TODO: Work on parse_while_stmt
IfStatement* Parser::parse_while_stmt()
{
    IfStatement* stmt = new IfStatement;

    expect(WHILE);
    parse_condition();
    parse_body();

    return stmt;
}

//for_stmt -> FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
//TODO: Work on parse_for_stmt
IfStatement* Parser::parse_for_stmt()
{
    IfStatement* stmt = new IfStatement;

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
Parser::CondNode* Parser::parse_condition()
{
    CondNode* node = new CondNode;

    node->op1 = parse_primary();
    node->condType = parse_relop();
    node->op2 = parse_primary();

    return node;
}

//relop -> GREATER | LESS | NOTEQUAL
ConditionalOperatorType Parser::parse_relop()
{
    if(errorFind)
        cout << "Starting " << "parse_relop" << endl;

    ConditionalOperatorType relop;

    Token t = lexer.GetToken();
    if (t.token_type == GREATER)
    {
        //relop-> GREATER
        relop = CONDITION_GREATER;
    }
    else if (t.token_type == LESS)
    {
        //relop-> LESS
        relop = CONDITION_LESS;
    }
    else if (t.token_type == NOTEQUAL)
    {
        //relop-> NOTEQUAL
        relop = CONDITION_NOTEQUAL;
    }
    else
        syntax_error();

    if(errorFind)
        cout << "Finished " << "parse_relop" << endl;

    return relop;
}

//switch_stmt -> SWITCH ID LBRACE case_list RBRACE
//switch_stmt -> SWITCH ID LBRACE case_list default_case RBRACE
//TODO: Work on parse_switch_stmt
IfStatement* Parser::parse_switch_stmt()
{
    IfStatement* stmt = new IfStatement;

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
//TODO: Work on parse_case_list
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
//TODO: Work on parse_case
void Parser::parse_case()
{
    expect(CASE);
    expect(NUM);
    expect(COLON);
    parse_body();
}

//default_case -> DEFAULT COLON body
//TODO: Work on parse_default_case
void Parser::parse_default_case()
{
    expect(DEFAULT);
    expect(COLON);
    parse_body();
}

//print_stmt -> print ID SEMICOLON
PrintStatement* Parser::parse_print_stmt()
{
    if(errorFind)
        cout << "Starting parse_print_stmt" << endl;

    PrintStatement* stmt = new PrintStatement;

    expect(PRINT);
    Token t = expect(ID);
    stmt->id = symLookup(t.lexeme);
    expect(SEMICOLON);

    if(errorFind)
        cout << "Finished parse_print_stmt" << endl;

    return stmt;
}

//Teacher's function
//TODO: See if this needs anything
StatementNode* Parser::ParseInput()
{
    if(errorFind)
        cout << "Starting " << "parse_input" << endl;

    StatementNode* startNode = parse_program();
    expect(END_OF_FILE);

    if(errorFind)
        cout << "Finished " << "parse_input" << endl;

    return startNode;
}



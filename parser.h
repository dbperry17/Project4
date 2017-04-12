/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"

class Parser {
  public:
    void ParseInput();
    struct idListNode;
    struct Symbol;
    void print();
    void loadDefaultSyms();

  private:

    LexicalAnalyzer lexer;

    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();

//program -> var_section body
//var_section -> id_list SEMICOLON
//id_list -> ID COMMA id_list | ID
//body -> LBRACE stmt_list RBRACE
//stmt_list -> stmt stmt_list | stmt
//stmt -> assign_stmt | print_stmt | while_stmt | if_stmt | switch_stmt | for_stmt
//assign_stmt -> ID EQUAL primary SEMICOLON
//assign_stmt -> ID EQUAL expr SEMICOLON
//expr -> primary op primary
//primary -> ID | NUM
//op -> PLUS | MINUS | MULT | DIV
//print_stmt -> print ID SEMICOLON
//while_stmt -> WHILE condition body
//if_stmt -> IF condition body
//condition -> primary relop primary
//relop -> GREATER | LESS | NOTEQUAL
//switch_stmt -> SWITCH ID LBRACE case_list RBRACE
//switch_stmt -> SWITCH ID LBRACE case_list default_case RBRACE
//for_stmt -> FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
//case_list -> case case_list | case
//case -> CASE NUM COLON body
//default_case -> DEFAULT COLON body

    void parse_program();
    void parse_var_section();
    idListNode* parse_id_list();
    void parse_body();
    void parse_stmt_list();
    void parse_stmt();
    void parse_assign_stmt();
    int parse_expr();
    int parse_primary();
    int parse_op();
    void parse_print_stmt();
    void parse_while_stmt();
    void parse_if_stmt();
    int parse_condition();
    void parse_relop();
    void parse_switch_stmt();
    void parse_for_stmt();
    void parse_case_list();
    void parse_case();
    void parse_default_case();

    //mine
    void errorCode(int cat, int spec, std::string symbol); //cat = category 1 or 2, spec = specific error
    void typeMismatch(int lineNo, std::string constraint);
    Symbol declCheck(std::string name);
    int unify(int typeNum1, int typeNum2);
};

#endif


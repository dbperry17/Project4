/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <cstdlib>
#include "lexer.h"
#include "compiler.h"

class Parser {
  private:
    struct ExprNode;
    struct CondNode;
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();
    ValueNode* symLookup(std::string name);
    ValueNode* addValNode(std::string name);
    ValueNode* constNode(int val);
    void printStatementList(StatementNode* head);

public:
    void print();
    StatementNode* ParseInput();
    StatementNode* parse_program();
    void parse_var_section();
    void parse_id_list();
    StatementNode* parse_body();
    StatementNode* parse_stmt_list();
    StatementNode* parse_stmt();
    AssignmentStatement* parse_assign_stmt();
    ExprNode* parse_expr();
    ValueNode* parse_primary();
    ArithmeticOperatorType parse_op();
    PrintStatement* parse_print_stmt();
    IfStatement* parse_while_stmt();
    IfStatement* parse_if_stmt();
    CondNode* parse_condition();
    ConditionalOperatorType parse_relop();
    IfStatement* parse_switch_stmt();
    IfStatement* parse_for_stmt();
    void parse_case_list();
    void parse_case();
    void parse_default_case();
};

#endif


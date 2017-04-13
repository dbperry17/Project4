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
    LexicalAnalyzer lexer;

    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();
    void parse_program();
    void parse_var_section();
    void parse_id_list();
    void parse_body();
    void parse_stmt_list();
    void parse_stmt();
    void parse_assign_stmt();
    void parse_expr();
    void parse_primary();
    void parse_op();
    void parse_print_stmt();
    void parse_while_stmt();
    void parse_if_stmt();
    void parse_condition();
    void parse_relop();
    void parse_switch_stmt();
    void parse_for_stmt();
    void parse_case_list();
    void parse_case();
    void parse_default_case();

    //mine
    int declCheck(std::string name);
};

#endif


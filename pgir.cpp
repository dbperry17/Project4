#include <iostream>
#include "compiler.h"
#include "parser.h"



struct StatementNode * parse_generate_intermediate_representation()
{
    Parser parser;
    parser.ParseInput();
    parser.print();
}
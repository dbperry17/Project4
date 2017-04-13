#include <iostream>
#include "compiler.h"
#include "parser.h"
#include "lexer.h"

using namespace std;

bool testing = true;
bool testParse = true;

void tester()
{
    if(testing && !testParse)
    {
        cout << "\n\nSTARTING PROGRAM" << endl;
    }
    else
    {
        testParse = false;
    }
}


vector<ValueNode> vars;
vector<ValueNode> cons;

struct StatementNode * parse_generate_intermediate_representation()
{
    if(testing)
    {
        Parser parser;

        parser.ParseInput();

        parser.print();
    }
}
#include <iostream>
#include "compiler.h"
#include "parser.h"
//#include "lexer.h" //only needed if not including parser.h

using namespace std;

bool testing = false;
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

/**********
 *  PGIR  *
 **********/

struct StatementNode * parse_generate_intermediate_representation()
{
    tester();
    Parser parser;
    parser.ParseInput();
    //parser.print();

    /**
     * Return symbol table???
     */
    //parser.parse_var_section();

}


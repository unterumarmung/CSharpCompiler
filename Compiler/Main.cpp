#include <cstdio>
#include <iostream>

#include "Parser.tab.h"

extern FILE* yyin;
struct Program* treeRoot;

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        const auto errorCode = fopen_s(&yyin, argv[1], "r");
    }
    else
    {
        yyin = stdin;
    }

    yyparse();
    std::cin.get();
}
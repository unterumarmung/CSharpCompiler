#include <cstdio>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "Parser.tab.h"
#include "Dot.h"

extern FILE* yyin;
struct Program* treeRoot;

int main(const int argc, char** argv)
{
    if (argc > 1) { const auto errorCode = fopen_s(&yyin, argv[1], "r"); }
    else { yyin = stdin; }

    yyparse();
    using namespace std::filesystem;
    const auto dotFile = current_path() / "Output" / "Tree.dot";
    create_directory(current_path() / "Output");
    {
        std::fstream treeOut;
        treeOut.open(dotFile, std::ios_base::out);
        ToDot(treeRoot, treeOut);
    }

    RunDot("../ThirdParty/Bin/dot/dot.exe", dotFile.string());
    std::cin.get();
}

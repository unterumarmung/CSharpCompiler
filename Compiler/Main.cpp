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
    if (argc > 1)
    {
        std::cout << "Opening file " << argv[1] << std::endl;
        const auto errorCode = fopen_s(&yyin, argv[1], "r");
    }
    else { yyin = stdin; }

    std::cout << "Building syntax tree" << std::endl;
    yyparse();

    {
        std::cout << "Generating dot file for syntax tree" << std::endl;
        using namespace std::filesystem;
        const auto dotFile = current_path() / "Output" / "Tree.dot";
        create_directory(current_path() / "Output");
        {
            std::fstream treeOut;
            treeOut.open(dotFile, std::ios_base::out);
            ToDot(treeRoot, treeOut);
        }
        std::cout << "Generating picture of syntax tree" << std::endl;
        RunDot("../ThirdParty/Bin/dot/dot.exe", dotFile.string());
    }

    std::cout << "To close the program press Enter" << std::endl;
    std::cin.get();
}

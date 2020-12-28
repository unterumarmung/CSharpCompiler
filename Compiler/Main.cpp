#include <cstdio>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "Parser.tab.h"
#include "Dot.h"
#include "Semantic/Semantic.h"

extern FILE* yyin;
struct Program* treeRoot;

void MakeTreeImage(std::string_view filename)
{
    {
        std::cout << "Generating dot file for " << filename << std::endl;
        using namespace std::filesystem;
        const auto dotFile = current_path() / "Output" / filename;
        create_directory(current_path() / "Output");
        {
            std::fstream treeOut;
            treeOut.open(dotFile, std::ios_base::out);
            ToDot(treeRoot, treeOut);
        }
        std::cout << "Generating picture" << std::endl;
        RunDot("../ThirdParty/Bin/dot/dot.exe", dotFile.string());
    }
}

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

    MakeTreeImage("TreeBeforeSemantic.dot");

    Semantic semantic(treeRoot);
    semantic.Analyze();

    std::cout << std::endl << "Errors: " << std::endl;
    for (auto const& error : semantic.Errors) { std::cout << error << std::endl; }
    std::cout << std::endl;

    MakeTreeImage("TreeAfterSemantic.dot");

    semantic.Generate();

    std::cout << "To close the program press Enter" << std::endl;
    std::cin.get();
}

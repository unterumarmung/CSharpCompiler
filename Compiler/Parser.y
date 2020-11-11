%{
// Remove warnings from bison
#ifdef _MSC_VER
#pragma warning( push, 0 )
#pragma warning( disable : 6011 )
#pragma warning( disable : 26819 )
#pragma warning( disable : 26812 )
#pragma warning( disable : 6387 )
#endif // _MSC_VER

#include <cstdio>
#include <iostream>

void __cdecl yyerror(const char* s) {
	std::cerr << s << std::endl;
}

template <char Separator = ' ', typename... Args>
void Print(Args&&... args) {
    ((std::cout << args << Separator), ...);
    std::cout << std::endl;
}

int yylex();
int yyparse();
extern FILE* yyin;

%}

%union {
    int _integer;
    char* _string;
    char* _identifier;
    double _floatingPoint;
    char _character;
}

%token LESS
%token GREATER
%token EQUAL
%token NOT_EQUAL
%token GREATER_OR_EQUAL
%token LESS_OR_EQUAL
%token OR
%token AND

%token <_identifier> IDENTIFIER
%token <_integer> INTEGER
%token <_floatingPoint> FLOATING_POINT
%token <_string> STRING
%token <_character> CHARACTER
%token RETURN
%token IF
%token ELSE
%token WHILE
%token DO
%token FOR
%token CHAR_KW
%token INT_KW
%token STRING_KW
%token BOOL_KW
%token FLOAT_KW

%right '='
%left OR
%left AND
%left '<' '>' EQUAL NOT_EQUAL LESS_OR_EQUAL GREATER_OR_EQUAL
%left '+' '-'
%left '*' '/'
%right '!'
%left UNARY_MINUS
%left '.' ']' '['
%nonassoc ')'

%%

expr: IDENTIFIER                        { Print("Found id:", $1); }
    | INTEGER                           { Print("Found integer:", $1); }
    | FLOATING_POINT                    { Print("Found floating point:", $1); }
    | STRING                            { Print("Found string:", $1); }
    | CHARACTER                         { Print("Found character:", $1); }
    | expr '+' expr                     { Print("Found binary expression +"); }
    | expr '-' expr                     { Print("Found binary expression -"); }
    | expr '*' expr                     { Print("Found binary expression *"); }
    | expr '/' expr                     { Print("Found binary expression /"); }
    | expr '=' expr                     { Print("Found binary expression ="); }
    | expr '<' expr                     { Print("Found binary expression <"); }
    | expr '>' expr                     { Print("Found binary expression >"); }
    | expr EQUAL expr                   { Print("Found binary expression =="); }
    | expr NOT_EQUAL expr               { Print("Found binary expression !="); }
    | expr LESS_OR_EQUAL expr           { Print("Found binary expression <="); }
    | expr GREATER_OR_EQUAL expr        { Print("Found binary expression >="); }
    | expr AND expr                     { Print("Found binary expression &&"); }
    | expr OR expr                      { Print("Found binary expression ||"); }
    | '!' expr                          { Print("Found unary expression !"); }
    | '-' expr %prec UNARY_MINUS        { Print("Found unary expression -"); }
;


%%

#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER

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
}
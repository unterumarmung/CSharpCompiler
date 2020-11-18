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
%token NEW
%token NULL_KW
%token TRUE_KW
%token FALSE_KW
%token PUBLIC
%token STATIC
%token CLASS

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

%start program

%%

program: class_decl
;

value_expr: IDENTIFIER                        { Print("Found id:", $1); }
    | INTEGER                           { Print("Found integer:", $1); }
    | FLOATING_POINT                    { Print("Found floating point:", $1); }
    | STRING                            { Print("Found string:", $1); }
    | CHARACTER                         { Print("Found character:", $1); }
    | TRUE_KW
    | FALSE_KW
    | value_expr '.' IDENTIFIER
;       

expr: value_expr
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
    | new_expr
    | NULL_KW
    | '(' expr ')'
    | method_call_expr
;

method_call_expr: IDENTIFIER '(' expr_seq_optional ')'  { Print("Found method call with name:", $1); }

expr_optional: 
              | expr
;

new_expr: NEW type '(' expr_seq_optional ')'
        | NEW type '[' expr ']'
        | NEW type '[' expr ']' '{' expr_seq_optional '}'
        | NEW type '[' ']' '{' expr_seq_optional '}'
        | NEW '[' ']' '{' expr_seq_optional '}'
;

expr_seq: expr
        | expr_seq ',' expr
;

expr_seq_optional:
                 | expr_seq
;

stmt: ';'
    | expr ';'
    | var_decl ';'
    | var_decl_with_init ';'
    | while_stmt
    | do_while_stmt
    | for_stmt
    | if_stmt
    | '{' stmt_seq_optional '}'
;

stmt_seq: stmt              { Print("Found stmt"); }
        | stmt_seq stmt     { Print("Found stmt"); }
;

stmt_seq_optional:
                | stmt_seq
;

while_stmt: WHILE '(' expr ')' stmt         { Print("Found while"); }
;

do_while_stmt: DO stmt WHILE '(' expr ')'';'         { Print("Found do while"); }
;

for_stmt: FOR '(' var_decl ';' expr_optional ';' expr_optional ')' stmt         { Print("Found for"); }
;

if_stmt: IF '(' expr ')' stmt               { Print("found if"); }
        | IF '(' expr ')' stmt ELSE stmt    { Print("found if/else"); }
;

standard_type: CHAR_KW
             | INT_KW
             | BOOL_KW
             | FLOAT_KW
;

type: standard_type             { Print("Found standard type"); }
    | IDENTIFIER
    | type '.' IDENTIFIER
;

array_type: type '[' ']' 
;

var_decl: type IDENTIFIER                   { Print("Variable decl"); }
        | array_type IDENTIFIER             { Print("Array decl"); }      
;

var_decl_with_init: type IDENTIFIER '=' expr          { Print("Variable decl with init"); }
                    | array_type IDENTIFIER '=' expr    { Print("Array decl with init"); }
;

method_arguments: var_decl
                | method_arguments var_decl
;

method_arguments_optional:
                         | method_arguments
;

method_decl: PUBLIC type IDENTIFIER '(' method_arguments_optional ')' '{' stmt_seq_optional '}'                 { Print("Found method decl"); }
           | PUBLIC array_type IDENTIFIER '(' method_arguments_optional ')' '{' stmt_seq_optional '}'           { Print("Found method decl with array return type"); }
           | PUBLIC STATIC type IDENTIFIER '(' method_arguments_optional ')' '{' stmt_seq_optional '}'          { Print("Found static method decl"); }
           | PUBLIC STATIC array_type IDENTIFIER '(' method_arguments_optional ')' '{' stmt_seq_optional '}'    { Print("Found static method decl with array return type"); }
;

field_decl: PUBLIC var_decl ';'
          | PUBLIC var_decl_with_init ';'
;

method_field_seq: method_decl
                | field_decl
                | method_field_seq method_decl
                | method_field_seq field_decl
;

method_field_seq_optional: 
                         | method_field_seq
;

class_decl: PUBLIC CLASS IDENTIFIER '{' method_field_seq_optional '}'  { Print("Found class declaration with name:", $3); }
                

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
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

/*
A[5].C[10]
*/

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
%token VOID_KW
%token NEW
%token NULL_KW
%token TRUE_KW
%token FALSE_KW
%token PUBLIC
%token PROTECTED
%token PRIVATE
%token STATIC
%token CLASS
%token ENUM
%token USING
%token NAMESPACE
%token FOREACH
%token IN_KW
%token OBJECT

%right '='
%left OR
%left AND
%left '<' '>' EQUAL NOT_EQUAL LESS_OR_EQUAL GREATER_OR_EQUAL
%left '+' '-'
%left '*' '/'
%right '!'
%left UNARY_MINUS
%left '.' ']' '['
%nonassoc '(' ')'

%start program

%%

program: class_decl
;

access_expr:  '(' expr ')'
            |  access_expr '[' expr ']'
            |  access_expr '[' ']'
            | INTEGER                           { Print("Found integer:", $1); }
            | FLOATING_POINT                    { Print("Found floating point:", $1); }
            | STRING                            { Print("Found string:", $1); }
            | CHARACTER                         { Print("Found character:", $1); }
            | TRUE_KW
            | FALSE_KW
            | IDENTIFIER
            | IDENTIFIER '(' expr_seq_optional ')'
            | access_expr '.' IDENTIFIER
            | access_expr '.' IDENTIFIER       '(' expr_seq_optional ')'          { Print("Found method call"); }
;


expr: expr '+' expr                     { Print("Found binary expression +"); }
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
    | NULL_KW
    | access_expr 
    | NEW standard_type 
    | NEW standard_type '{' expr_seq_optional '}'
    | NEW '[' ']' '{' expr_seq_optional '}'

;

expr_optional: 
              | expr
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
    | foreach_stmt
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

for_stmt: FOR '(' var_decl ';' expr_optional ';' expr_optional ')' stmt                     { Print("Found for"); }
        |  FOR '(' var_decl_with_init ';' expr_optional ';' expr_optional ')' stmt           { Print("Found for with init"); }
        |  FOR '(' expr_optional ';' expr_optional ';' expr_optional ')' stmt               { Print("Found for with expr"); }
;

if_stmt: IF '(' expr ')' stmt               { Print("found if"); }
        | IF '(' expr ')' stmt ELSE stmt    { Print("found if/else"); }
;

foreach_stmt: FOREACH '(' var_decl IN_KW expr ')' stmt        { Print("Found foreach"); }
;

standard_type: CHAR_KW
             | INT_KW
             | BOOL_KW
             | FLOAT_KW
             
;

standard_array_type: standard_type '[' ']'
                   | standard_array_type '[' ']'
;

type: standard_type
    | standard_array_type
    | access_expr
;

var_decl: type IDENTIFIER                   { Print("Variable decl with name", $2); }
;

var_decl_with_init: type IDENTIFIER '=' expr          { Print("Variable decl with init"); }
;

method_arguments: var_decl
                | method_arguments ',' var_decl
;

method_arguments_optional:
                         | method_arguments
;

visibility_modifier: PUBLIC
                   | PROTECTED
                   | PRIVATE
;

method_decl: visibility_modifier type IDENTIFIER '(' method_arguments_optional ')' '{' stmt_seq_optional '}'                 { Print("Found method decl with name:", $3); }
           | visibility_modifier VOID_KW IDENTIFIER '(' method_arguments_optional ')' '{' stmt_seq_optional '}'              { Print("Found void method decl with name:", $3); }
;

field_decl: visibility_modifier var_decl ';'                         { Print("Found field decl"); }
          | visibility_modifier var_decl_with_init ';'               { Print("Found field decl with init"); }
;

class_members: method_decl
                | field_decl
                | class_members method_decl
                | class_members field_decl
;

class_members_optional: 
                         | class_members
;

enumerators: IDENTIFIER
            | enumerators ',' IDENTIFIER
;

enum_decl: PUBLIC ENUM IDENTIFIER '{' enumerators '}' { Print("Found enum declaration with name:", $3); }

class_decl: PUBLIC CLASS IDENTIFIER '{' class_members_optional '}'  { Print("Found class declaration with name:", $3); }
          | PUBLIC CLASS IDENTIFIER ':' using_arg '{' class_members_optional '}'  { Print("Found class declaration with inheritance with name:", $3); }
          | PUBLIC CLASS IDENTIFIER ':' OBJECT '{' class_members_optional '}'  { Print("Found class declaration with inheritance with name:", $3); }
;
                
namespace_members: enum_decl
                | class_decl
                | namespace_members enum_decl
                | namespace_members class_decl
;

namespace_members_optional:
                            | namespace_members
;

namespace_decl: NAMESPACE IDENTIFIER '{' namespace_members_optional '}' { Print("Found namespace declaration with name:", $2); }
;

namespace_decl_seq: namespace_decl
                   | namespace_decl_seq namespace_decl
;

using_arg: IDENTIFIER
         | using_arg '.' IDENTIFIER
;

using_directive: USING using_arg ';'  { Print("Found using"); }
;

using_directives:  using_directive
                | using_directives using_directive
;

using_directives_optional: 
                        | using_directives
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
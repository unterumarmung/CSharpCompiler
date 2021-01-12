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

#include "Tree/Program.h"

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
extern struct Program* treeRoot;
%}

%union {
    int _integer;
    char* _string;
    char* _identifier;
    double _floatingPoint;
    char _character;

    struct AccessExpr* _accessExpr;
    struct ExprNode* _expr;
    struct ExprSeqNode* _exprSeq;
    
    enum class StandardType _standardType;
    struct StandardArrayType* _standardArrayType;
    struct TypeNode* _type;
    
    struct VarDeclNode* _varDecl;
    struct WhileNode* _while;
    struct DoWhileNode* _doWhile;
    struct ForNode* _for;
    struct ForEachNode* _foreach;
    struct StmtSeqNode* _stmtSeq;
    struct IfNode* _if;
    struct StmtNode* _stmt;
    
    enum class VisibilityModifier _visibiltyModifier;
    struct FieldDeclNode* _fieldDecl;
    struct MethodArguments* _methodArguments;
    struct MethodDeclNode* _methodDecl;
    struct ClassMembersNode* _classMembers;
    struct ClassDeclNode* _classDecl;

    struct IdentifierList* _enumerators;
    struct EnumDeclNode* _enumDecl;

    struct IdentifierList* _usingArg;
    struct NamespaceMembersNode* _namespaceMembers;
    struct NamespaceDeclNode* _namespaceDecl;
    struct UsingDirectiveNode* _usingDirective;
    struct UsingDirectives* _usingDirectives;
    struct NamespaceDeclSeq* _namespaceDeclSeq;
}

%type <_accessExpr> access_expr
%type <_expr> expr expr_optional
%type <_exprSeq> expr_seq expr_seq_optional

%type <_standardType> standard_type
%type <_standardArrayType> standard_array_type
%type <_type> type;

%type <_varDecl> var_decl var_decl_with_init
%type <_while> while_stmt
%type <_doWhile> do_while_stmt
%type <_for> for_stmt
%type <_foreach> foreach_stmt
%type <_stmtSeq> stmt_seq stmt_seq_optional
%type <_if> if_stmt
%type <_stmt> stmt

%type <_visibiltyModifier> visibility_modifier
%type <_fieldDecl> field_decl
%type <_methodArguments> method_arguments method_arguments_optional
%type <_methodDecl> method_decl operator_overload
%type <_classMembers> class_members class_members_optional
%type <_classDecl> class_decl

%type <_enumerators> enumerators
%type <_enumDecl> enum_decl

%type <_usingArg> using_arg
%type <_namespaceMembers> namespace_members namespace_members_optional
%type <_namespaceDecl> namespace_decl
%type <_usingDirective> using_directive
%type <_usingDirectives> using_directives using_directives_optional
%type <_namespaceDeclSeq> namespace_decl_seq


%token LESS
%token GREATER
%token EQUAL
%token NOT_EQUAL
%token GREATER_OR_EQUAL
%token LESS_OR_EQUAL
%token OR
%token AND

%token PLUS_ASSIGN
%token MINUS_ASSIGN
%token MULTIPLY_ASSIGN
%token DIVISION_ASSIGN
%token INCREMENT
%token DECREMENT

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
%token OPERATOR
%token VAR

%right '=' PLUS_ASSIGN MINUS_ASSIGN MULTIPLY_ASSIGN DIVISION_ASSIGN
%left OR
%left AND
%left '<' '>' EQUAL NOT_EQUAL LESS_OR_EQUAL GREATER_OR_EQUAL
%left '+' '-'
%left '*' '/'
%right '!' INCREMENT DECREMENT
%left UNARY_MINUS UNARY_PLUS
%left '.' ']' '['
%nonassoc '(' ')'

%start program

%%

program: using_directives_optional namespace_decl_seq { treeRoot = new Program($1, $2); }
;

access_expr:  '(' expr ')'                                                  { $$ = AccessExpr::FromExpr($2); }
            |  access_expr '[' expr ']'                                     { $$ = AccessExpr::FromBrackets($1, $3); }
            |  access_expr '[' ']'                                          { $$ = AccessExpr::FromBrackets($1); }
            | INTEGER                                                       { $$ = AccessExpr::FromInt($1); }
            | FLOATING_POINT                                                { $$ = AccessExpr::FromFloat($1); }
            | STRING                                                        { $$ = AccessExpr::FromString($1); }
            | CHARACTER                                                     { $$ = AccessExpr::FromChar($1); }
            | TRUE_KW                                                       { $$ = AccessExpr::FromBool(true); }
            | FALSE_KW                                                      { $$ = AccessExpr::FromBool(false); }
            | IDENTIFIER                                                    { $$ = AccessExpr::FromId($1); }
            | IDENTIFIER '(' expr_seq_optional ')'                          { $$ = AccessExpr::FromCall($1, $3); }
            | access_expr '.' IDENTIFIER                                    { $$ = AccessExpr::FromDot($1, $3); }
            | access_expr '.' IDENTIFIER '(' expr_seq_optional ')'          { $$ = AccessExpr::FromDot($1, $3, $5); }
;


expr: expr '+' expr                             { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::BinPlus, $1, $3); }
    | expr '-' expr                             { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::BinMinus, $1, $3); }
    | expr '*' expr                             { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::Multiply, $1, $3); }
    | expr '/' expr                             { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::Divide, $1, $3); }
    | expr '=' expr                             { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::Assign, $1, $3); }
    | expr PLUS_ASSIGN expr                     { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::Plus_assign, $1, $3); }
    | expr MINUS_ASSIGN expr                    { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::Minus_assign, $1, $3); }
    | expr MULTIPLY_ASSIGN expr                 { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::Multiply_assign, $1, $3); }
    | expr DIVISION_ASSIGN expr                 { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::Division_assign, $1, $3); }
    | expr '<' expr                             { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::Less, $1, $3); }
    | expr '>' expr                             { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::Greater, $1, $3); }
    | expr EQUAL expr                           { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::Equal, $1, $3); }
    | expr NOT_EQUAL expr                       { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::NotEqual, $1, $3); }
    | expr LESS_OR_EQUAL expr                   { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::LessOrEqual, $1, $3); }
    | expr GREATER_OR_EQUAL expr                { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::GreaterOrEqual, $1, $3); }
    | expr AND expr                             { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::And, $1, $3); }
    | expr OR expr                              { $$ = ExprNode::FromBinaryExpression(ExprNode::TypeT::Or, $1, $3); }
    | '!' expr                                  { $$ = ExprNode::FromUnaryExpression(ExprNode::TypeT::Not, $2); }
    | INCREMENT expr                            { $$ = ExprNode::FromUnaryExpression(ExprNode::TypeT::Increment, $2); }
    | DECREMENT expr                            { $$ = ExprNode::FromUnaryExpression(ExprNode::TypeT::Decrement, $2); }
    | '+' expr %prec UNARY_PLUS                 { $$ = ExprNode::FromUnaryExpression(ExprNode::TypeT::UnaryPlus, $2); }
    | '-' expr %prec UNARY_MINUS                { $$ = ExprNode::FromUnaryExpression(ExprNode::TypeT::UnaryMinus, $2); }
    | NULL_KW                                   { $$ = ExprNode::FromNull(); }
    | access_expr                               { $$ = ExprNode::FromAccessExpr($1); }
    | NEW type                                  { $$ = ExprNode::FromNew($2); }
    | NEW type '{' expr_seq_optional '}'        { $$ = ExprNode::FromNew($2, $4); }
    | NEW '[' ']' '{' expr_seq_optional '}'     { $$ = ExprNode::FromNew(nullptr, $5); }
    | '(' standard_type ')' expr                { $$ = ExprNode::FromCast($2, $4); }
    | NEW standard_type '[' expr ']'            { $$ = ExprNode::FromNew($2, $4); }
;

expr_optional:                  { $$ = nullptr; }
              | expr            { $$ = $1; }
;

expr_seq: expr                  { $$ = new ExprSeqNode($1); }
        | expr_seq ',' expr     { $$ -> Add($3); }
;

expr_seq_optional:              { $$ = ExprSeqNode::MakeEmpty(); }
                 | expr_seq     { $$ = $1; }
;

stmt: ';'                           { $$ = new StmtNode(); }
    | expr ';'                      { $$ = new StmtNode($1, /* isReturn= */ false); }
    | var_decl ';'                  { $$ = new StmtNode($1); }
    | var_decl_with_init ';'        { $$ = new StmtNode($1); }
    | while_stmt                    { $$ = new StmtNode($1); }
    | do_while_stmt                 { $$ = new StmtNode($1); }
    | for_stmt                      { $$ = new StmtNode($1); }
    | if_stmt                       { $$ = new StmtNode($1); }
    | foreach_stmt                  { $$ = new StmtNode($1); }
    | '{' stmt_seq_optional '}'     { $$ = new StmtNode($2); }
    | RETURN expr_optional ';'      { $$ = new StmtNode($2, /* isReturn= */ true); }
;

stmt_seq: stmt              { $$ = new StmtSeqNode($1); }
        | stmt_seq stmt     { $$ -> Add($2); }
;
stmt_seq_optional:          { $$ = StmtSeqNode::MakeEmpty(); }
                | stmt_seq  { $$ = $1; }
;

while_stmt: WHILE '(' expr ')' stmt                 { $$ = new WhileNode($3, $5); }
;
do_while_stmt: DO stmt WHILE '(' expr ')'';'        { $$ = new DoWhileNode($5, $2); }
;

for_stmt: FOR '(' var_decl ';' expr_optional ';' expr_optional ')' stmt                     { $$ = new ForNode($3, $5, $7, $9); }
        |  FOR '(' var_decl_with_init ';' expr_optional ';' expr_optional ')' stmt          { $$ = new ForNode($3, $5, $7, $9); }
        |  FOR '(' expr_optional ';' expr_optional ';' expr_optional ')' stmt               { $$ = new ForNode($3, $5, $7, $9); }
;

if_stmt: IF '(' expr ')' stmt               { $$ = new IfNode($3, $5); }
        | IF '(' expr ')' stmt ELSE stmt    { $$ = new IfNode($3, $5, $7); }
;

foreach_stmt: FOREACH '(' var_decl IN_KW expr ')' stmt      { $$ = new ForEachNode($3, $5, $7); }
;

standard_type: CHAR_KW      { $$ = StandardType::Char; }
             | INT_KW       { $$ = StandardType::Int; }
             | BOOL_KW      { $$ = StandardType::Bool; }
             | FLOAT_KW     { $$ = StandardType::Float; }
             | STRING_KW    { $$ = StandardType::String; }
;

standard_array_type: standard_type '[' ']'          { $$ = new StandardArrayType{ $1, 1 }; }
                   | standard_array_type '[' ']'    { $$ -> Arity += 1; }
;

type: standard_type         { $$ = new TypeNode($1); }
    | standard_array_type   { $$ = new TypeNode(* $1); delete $1; }
    | access_expr           { $$ = new TypeNode($1); }
;

var_decl: type IDENTIFIER                           { $$ = new VarDeclNode($1, $2, nullptr); }
        | VAR IDENTIFIER                            { $$ = new VarDeclNode(nullptr, $2, nullptr, true); }
;
var_decl_with_init: type IDENTIFIER '=' expr        { $$ = new VarDeclNode($1, $2, $4); }
                    | VAR IDENTIFIER '=' expr       { $$ = new VarDeclNode(nullptr, $2, $4, true); }
;

method_arguments: var_decl                          { $$ = new MethodArguments($1); }
                | method_arguments ',' var_decl     { $$ -> Add($3); }
;
method_arguments_optional:                          { $$ = MethodArguments::MakeEmpty(); }
                         | method_arguments         { $$ = $1; }
;

visibility_modifier: PUBLIC         { $$ = VisibilityModifier::Public; }
                   | PROTECTED      { $$ = VisibilityModifier::Protected; }
                   | PRIVATE        { $$ = VisibilityModifier::Private; }
;

operator_overload:    visibility_modifier STATIC type OPERATOR '+'              '(' var_decl ',' var_decl ')' '{' stmt_seq_optional '}'  { $$ = new MethodDeclNode($1, $3, OperatorType::Plus,              $7, $9, $12); }
                    | visibility_modifier STATIC type OPERATOR '-'              '(' var_decl ',' var_decl ')' '{' stmt_seq_optional '}'  { $$ = new MethodDeclNode($1, $3, OperatorType::Minus,             $7, $9, $12); }
                    | visibility_modifier STATIC type OPERATOR '*'              '(' var_decl ',' var_decl ')' '{' stmt_seq_optional '}'  { $$ = new MethodDeclNode($1, $3, OperatorType::Multiply,          $7, $9, $12); }
                    | visibility_modifier STATIC type OPERATOR '/'              '(' var_decl ',' var_decl ')' '{' stmt_seq_optional '}'  { $$ = new MethodDeclNode($1, $3, OperatorType::Divide,            $7, $9, $12); }
                    | visibility_modifier STATIC type OPERATOR '<'              '(' var_decl ',' var_decl ')' '{' stmt_seq_optional '}'  { $$ = new MethodDeclNode($1, $3, OperatorType::Less,              $7, $9, $12); }
                    | visibility_modifier STATIC type OPERATOR '>'              '(' var_decl ',' var_decl ')' '{' stmt_seq_optional '}'  { $$ = new MethodDeclNode($1, $3, OperatorType::Greater,           $7, $9, $12); }
                    | visibility_modifier STATIC type OPERATOR EQUAL            '(' var_decl ',' var_decl ')' '{' stmt_seq_optional '}'  { $$ = new MethodDeclNode($1, $3, OperatorType::Equal,             $7, $9, $12); }
                    | visibility_modifier STATIC type OPERATOR NOT_EQUAL        '(' var_decl ',' var_decl ')' '{' stmt_seq_optional '}'  { $$ = new MethodDeclNode($1, $3, OperatorType::NotEqual,          $7, $9, $12); }
                    | visibility_modifier STATIC type OPERATOR LESS_OR_EQUAL    '(' var_decl ',' var_decl ')' '{' stmt_seq_optional '}'  { $$ = new MethodDeclNode($1, $3, OperatorType::LessOrEqual,       $7, $9, $12); }
                    | visibility_modifier STATIC type OPERATOR GREATER_OR_EQUAL '(' var_decl ',' var_decl ')' '{' stmt_seq_optional '}'  { $$ = new MethodDeclNode($1, $3, OperatorType::GreaterOrEqual,    $7, $9, $12); }

method_decl: visibility_modifier type IDENTIFIER '(' method_arguments_optional ')' '{' stmt_seq_optional '}'                { $$ = new MethodDeclNode($1, $2, $3, $5, $8); }
           | visibility_modifier VOID_KW IDENTIFIER '(' method_arguments_optional ')' '{' stmt_seq_optional '}'             { $$ = new MethodDeclNode($1, nullptr, $3, $5, $8); }
           | visibility_modifier STATIC VOID_KW IDENTIFIER '(' method_arguments_optional ')' '{' stmt_seq_optional '}'      { $$ = new MethodDeclNode($1, nullptr, $4, $6, $9, /* isStatic = */ true); }
           | STATIC visibility_modifier VOID_KW IDENTIFIER '(' method_arguments_optional ')' '{' stmt_seq_optional '}'      { $$ = new MethodDeclNode($2, nullptr, $4, $6, $9, /* isStatic = */ true); }
;

field_decl: visibility_modifier var_decl ';'              { $$ = new FieldDeclNode($1, $2); }
          | visibility_modifier var_decl_with_init ';'    { $$ = new FieldDeclNode($1, $2); }
;
    
class_members: method_decl                          { $$ = new ClassMembersNode(); $$ -> Add($1); }
                | field_decl                        { $$ = new ClassMembersNode(); $$ -> Add($1); }
                | operator_overload                 { $$ = new ClassMembersNode(); $$ -> Add($1); }
                | class_members method_decl         { $$ -> Add($2); }
                | class_members field_decl          { $$ -> Add($2); }
                | class_members operator_overload   { $$ -> Add($2); }
;

class_members_optional:                     { $$ = new ClassMembersNode(); }
                         | class_members    { $$ = $1; }
;

enumerators: IDENTIFIER                     { $$ = new IdentifierList(); $$ -> Add($1); }
            | enumerators ',' IDENTIFIER    { $$ -> Add($3); }
;

enum_decl: PUBLIC ENUM IDENTIFIER '{' enumerators '}' { Print("Found enum declaration with name:", $3); }

class_decl: PUBLIC CLASS IDENTIFIER '{' class_members_optional '}'                  { $$ = new ClassDeclNode($3, nullptr, $5); }
          | PUBLIC CLASS IDENTIFIER ':' using_arg '{' class_members_optional '}'    { $$ = new ClassDeclNode($3, $5, $7); }
          | PUBLIC CLASS IDENTIFIER ':' OBJECT '{' class_members_optional '}'       { $$ = new ClassDeclNode($3, nullptr, $7); }
;
                
namespace_members: enum_decl                    { $$ = new NamespaceMembersNode(); $$ -> Add($1); }           
                | class_decl                    { $$ = new NamespaceMembersNode(); $$ -> Add($1); }           
                | namespace_members enum_decl   { $$ -> Add($2); }           
                | namespace_members class_decl  { $$ -> Add($2); }
;

namespace_members_optional:                         { $$ = new NamespaceMembersNode(); }
                            | namespace_members     { $$ = $1; }
;

namespace_decl: NAMESPACE IDENTIFIER '{' namespace_members_optional '}' { $$ = new NamespaceDeclNode($2, $4);  }
;

namespace_decl_seq: namespace_decl                      { $$ = new NamespaceDeclSeq($1); }
                   | namespace_decl_seq namespace_decl  { $$ -> Add($2); }
;

using_arg: IDENTIFIER                   { $$ = new IdentifierList(); $$ -> Add($1); }
         | using_arg '.' IDENTIFIER     { $$ -> Add($3); }
;

using_directive: USING using_arg ';'    { $$ = new UsingDirectiveNode($2); }
;

using_directives:  using_directive                  { $$ = new UsingDirectives($1); }
                | using_directives using_directive  { $$ -> Add($2); }
;

using_directives_optional:                  { $$ = UsingDirectives::MakeEmpty();  }
                        | using_directives  { $$ = $1; }
;

%%

#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER


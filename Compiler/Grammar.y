%{

%}

%start program

%token UsingTok
%token <Identifier> IdentifierTok
%token NamespaceTok
%token ClassTok
%token PublicTok
%token IntTok
%token StringTok
%token CharTok
%token DoubleTok
%token ReturnTok

%%

program: using_seq_optional namespace_seq_optional ENDOFFILE
;

using_seq_optional:
                    | using_seq
;

using_seq: UsingTok using_arg ';'
            |  using_seq UsingTok using_arg ';'
;

using_arg: IdentifierTok
            | using_arg '.' IdentifierTok
;

namespace_seq_optional:
                        | namespace_seq
;

namespace_seq: namespace_decl
                | namespace_seq namespace_decl
;

namespace_decl: NamespaceTok IdentifierTok '{' using_seq_optional namespace_seq_optional class_seq_optional '}'
;

class_seq_optional:
                    | class_seq
;

class_seq: class_decl
            | class_seq class_decl
;

class_decl: ClassTok IdentifierTok inheritance_optional '{' class_members_seq_optional '}'
;

inheritance_optional:
                        | inheritance
;

inheritance: ':' complex_type
;

type: simple_type
        | complex_type
;

simple_type: IntTok
                | CharTok
                | StringTok
                | DoubleTok
;

complex_type: IdentifierTok
                | complex_type '.' IdentifierTok
;

class_members_seq_optional: 
                            | class_members_seq
;  

class_members_seq: method_decl
                    | field_decl
                    | class_members_seq methon_decl
                    | class_members_seq field_decl
;

field_decl: PublicTok variable_decl ';'
;

variable_decl: type IdentifierTok
;

variable_decl_with_init: variable_decl '=' expr;

method_decl: PublicTok complex_type IdentifierTok '(' method_decl_arg_seq_optional ')' '{' stmt_seq_optional '}'
;

method_decl_arg_seq_optional:
                            | method_decl_arg_seq
;

method_decl_arg_seq: variable_decl
                    | method_decl_arg_seq ',' variable_decl
;

stmt_seq_optional:
                   | stmt_seq
;

stmt_seq: stmt
            | stmt_seq stmt
;

stmt: ';'
        | expr_stmt
        | variable_decl ';'
        | variable_decl_with_init ';'
        | if_stmt
        | while_stmt
        | do_while_stmt
        | for_stmt
        | '{' stmt_seq_optional '}'
        | ReturnTok expr ';'
        | ReturnTok ';'
;

expr_stmt: method_call_expr ';'
            | new_expr ';'
            | assigment_expr ';'
            | inc_dec_expr ';'
;

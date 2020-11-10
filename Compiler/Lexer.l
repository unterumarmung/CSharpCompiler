%option noyywrap 
%option never-interactive
%option yylineno

%{
/* Note: There's no octal integer literals in C# */

// Remove warnings from flex
#ifdef _MSC_VER
#pragma warning( push, 0 )
#pragma warning( disable : 6011 )
#pragma warning( disable : 26819 )
#pragma warning( disable : 6387 )
#endif // _MSC_VER

#include <string>
#include <algorithm>
#include <charconv>
#include <iostream>
#include <cstdio>

#include "enable_unicode_console.hpp"

void Print(const std::string& message)
{
    std::cout << message << std::endl;
}

void PrintError(const std::string& errorMessage, const int line = -1) 
{
    std::cerr << errorMessage << std::endl;
    if (line >= 0)
        std::cerr << "Line: " << line << std::endl;
    std::cerr << std::endl;
}

void PrintComment(const std::string& comment) 
{
    Print("Found multiline comment:");
    Print("\"" + comment + "\"");
}

void PrintStringLiteral(const std::string& literal) {
    Print("Found string literal:");
    Print("\"" + literal + "\"");
}

void PrintKeyword(const std::string& keyword) {
    Print("Found keyword: " + keyword);
}

void PrintLogicalOperator(const std::string& o) {
    Print("Found logic operator: " + o);
}

void PrintComparisonOperator(const std::string& c) {
    Print("Found comparison operator: " + c);
}

void PrintAriphmeticOperator(const std::string& o) {
    Print("Found ariphmetic operator: " + o);
}

void PrintIntegerLiteral(const std::string& type, const int integer) {
    Print("Found " + type + " integer literal: " + std::to_string(integer));
}

void PrintFloatingPointLiteral(const double floatingPoint) {
    Print("Found floating point literal: " + std::to_string(floatingPoint));
}

void PrintCharacterLiteral(const char literal) {
    using namespace std::string_literals;
    Print("Found character: \'"s + literal + "\'"s);
}

void PrintIdentifier(const std::string& identifier) {
    Print("Found identifier: " + identifier);
}

void RemoveCharacterFromString(std::string& str, char ch) {
    const auto newLast = std::remove(str.begin(), str.end(), ch);
    str.erase(newLast, str.end());
}

%}

DEC_DIGIT [0-9]
HEX_DIGIT [0-9A-Fa-f]
BIN_DIGIT [01]
DEC_NUMBER {DEC_DIGIT}(_*{DEC_DIGIT})*
EXPONENT [eE][+-]?{DEC_NUMBER}

%x MULTILINE_COMMENT
%x STRING_LITERAL
%x VERBATIM_STRING_LITERAL
%x CHARACTER_LITERAL

%%

%{
std::string currentStringLiteral;
std::string currentComment;
int currentInteger;
std::string currentCharacter;
double currentFloatingPoint;
char buffer[10];
%}

"/*"                                    {
                                            currentComment.clear();
                                            BEGIN(MULTILINE_COMMENT);
                                        }

<MULTILINE_COMMENT>[^*\n]*              {
                                            currentComment += yytext;
                                        }

<MULTILINE_COMMENT>\*+[^*/\n]*          {
                                            currentComment += yytext;
                                        }

<MULTILINE_COMMENT>\n                   {
                                            currentComment += yytext;
                                        }

<MULTILINE_COMMENT>\*+\/                {
                                            PrintComment(currentComment);
                                            BEGIN(INITIAL);
                                        }

<MULTILINE_COMMENT><<EOF>>              {
                                            PrintError("Error! Expected end of multiline comment but found end of file!", yylineno);
                                            BEGIN(INITIAL);
                                        }
                                        
\/\/.*                                  { Print("Found single-line comment: " + std::string{yytext}); }

\"                                      {
                                            currentStringLiteral.clear();
                                            BEGIN(STRING_LITERAL);
                                        }

<STRING_LITERAL>[^\\\"\\\n]+            {
                                            currentStringLiteral += yytext;
                                        }
<STRING_LITERAL>\\x[0-9a-fA-F]{1,4}     {
                                            std::fill(std::begin(buffer), std::end(buffer), 0);
                                            std::copy(yytext, yytext + yyleng, buffer);
                                            std::sscanf(buffer + 2, "%x", &currentInteger);
                                            currentStringLiteral = static_cast<char>(currentInteger);
                                        }
<STRING_LITERAL>\\n                     {
                                            currentStringLiteral += "\n";
                                        }
<STRING_LITERAL>\\t                     {
                                            currentStringLiteral += "\t";
                                        }
<STRING_LITERAL>\\a                     {
                                            currentStringLiteral += "\a";
                                        }
<STRING_LITERAL>\\b                     {
                                            currentStringLiteral += "\b";
                                        }
<STRING_LITERAL>\\f                     {
                                            currentStringLiteral += "\f";
                                        }
<STRING_LITERAL>\n                      {
                                            PrintError("Error! Expected end of string literal but found end of line!", yylineno);
                                            BEGIN(INITIAL);
                                        }
<STRING_LITERAL><<EOF>>                 {
                                            PrintError("Error! expected end of string literal but found end of file!", yylineno);
                                            BEGIN(INITIAL);
                                        }
<STRING_LITERAL>\\r                     {
                                            currentStringLiteral += "\r";
                                        }
<STRING_LITERAL>\\v                     {
                                            currentStringLiteral += "\v";
                                        }
<STRING_LITERAL>\\?                     {
                                            currentStringLiteral += "\?";
                                        }
<STRING_LITERAL>\\0                     {
                                            currentStringLiteral += "\0";
                                        }
<STRING_LITERAL>\\\\                    {
                                            currentStringLiteral += "\\";
                                        }
<STRING_LITERAL>\\\"                    {
                                            currentStringLiteral += "\"";
                                        }
<STRING_LITERAL>\\\'                    {
                                            currentStringLiteral += "\'";
                                        }
<STRING_LITERAL>\"                      {
                                            PrintStringLiteral(currentStringLiteral);
                                            BEGIN(INITIAL);
                                        }
@\"                                     {
                                            currentStringLiteral.clear();
                                            BEGIN(VERBATIM_STRING_LITERAL);
                                        }
<VERBATIM_STRING_LITERAL>\"\"           {
                                            currentStringLiteral += "\"";
                                        }
<VERBATIM_STRING_LITERAL>[^"\n]+        {
                                            currentStringLiteral += yytext;
                                        }
<VERBATIM_STRING_LITERAL>\n             {
                                            currentStringLiteral += "\n";
                                        }
<VERBATIM_STRING_LITERAL>\"             {
                                            PrintStringLiteral(currentStringLiteral);
                                            BEGIN(INITIAL);
                                        }
<VERBATIM_STRING_LITERAL><<EOF>>        {
                                            PrintError("Error! expected end of string literal but found end of file!", yylineno);
                                            BEGIN(INITIAL);
                                        }
\'                                      {
                                            currentCharacter.clear();
                                            BEGIN(CHARACTER_LITERAL);
                                        }
<CHARACTER_LITERAL>\'                   {
                                            if (currentCharacter.size() == 0) 
                                            {
                                                PrintError("Error! Found empty character literal");
                                            }
                                            else if (currentCharacter.size() > 1)
                                            {
                                                PrintError("Error! Character literal is not allowed to have more then one character");
                                            }
                                            else 
                                            {
                                                PrintCharacterLiteral(currentCharacter[0]);
                                            }
                                            BEGIN(INITIAL);
                                        }
<CHARACTER_LITERAL>[^\'\\]+             {

                                                currentCharacter += yytext;                                                    
                                        }
<CHARACTER_LITERAL>\\x[0-9a-fA-F]{1,4}  {
                                            std::fill(std::begin(buffer), std::end(buffer), 0);
                                            std::copy(yytext + 2, yytext + yyleng, buffer);
                                            std::sscanf(buffer, "%x", &currentInteger);
                                            currentCharacter += static_cast<char>(currentInteger);
                                        }
<CHARACTER_LITERAL>\\n                  {
                                            currentCharacter += '\n';
                                        }
<CHARACTER_LITERAL>\\t                  {
                                            currentCharacter += '\t';
                                        }
<CHARACTER_LITERAL>\\a                  {
                                            currentCharacter += '\a';
                                        }
<CHARACTER_LITERAL>\\b                  {
                                            currentCharacter += '\b';
                                        }
<CHARACTER_LITERAL>\\f                  {
                                            currentCharacter += '\f';
                                        }
<CHARACTER_LITERAL>\\r                  {
                                            currentCharacter += '\r';
                                        }
<CHARACTER_LITERAL>\\v                  {
                                            currentCharacter += '\v';
                                        }
<CHARACTER_LITERAL>\\?                  {
                                            currentCharacter += '\?';
                                        }
<CHARACTER_LITERAL>\\0                  {
                                            currentCharacter += '\0';
                                        }
<CHARACTER_LITERAL>\\\\                 {
                                            currentCharacter += '\\';
                                        }
<CHARACTER_LITERAL>\\\"                 {      
                                            currentCharacter += '\"';
                                        }
<CHARACTER_LITERAL>\\\'                 {
                                            currentCharacter += '\'';
                                        }



abstract                                { PrintKeyword(yytext); } 
as                                      { PrintKeyword(yytext); } 
base                                    { PrintKeyword(yytext); } 
bool                                    { PrintKeyword(yytext); } 
break                                   { PrintKeyword(yytext); } 
byte                                    { PrintKeyword(yytext); } 
case                                    { PrintKeyword(yytext); } 
catch                                   { PrintKeyword(yytext); } 
char                                    { PrintKeyword(yytext); } 
checked                                 { PrintKeyword(yytext); } 
class                                   { PrintKeyword(yytext); } 
const                                   { PrintKeyword(yytext); } 
continue                                { PrintKeyword(yytext); } 
decimal                                 { PrintKeyword(yytext); } 
default                                 { PrintKeyword(yytext); } 
delegate                                { PrintKeyword(yytext); } 
do                                      { PrintKeyword(yytext); } 
double                                  { PrintKeyword(yytext); } 
else                                    { PrintKeyword(yytext); } 
enum                                    { PrintKeyword(yytext); } 
event                                   { PrintKeyword(yytext); } 
explicit                                { PrintKeyword(yytext); } 
extern                                  { PrintKeyword(yytext); } 
false                                   { PrintKeyword(yytext); } 
finally                                 { PrintKeyword(yytext); } 
fixed                                   { PrintKeyword(yytext); } 
float                                   { PrintKeyword(yytext); } 
for                                     { PrintKeyword(yytext); } 
foreach                                 { PrintKeyword(yytext); } 
goto                                    { PrintKeyword(yytext); } 
if                                      { PrintKeyword(yytext); } 
implicit                                { PrintKeyword(yytext); } 
in                                      { PrintKeyword(yytext); } 
int                                     { PrintKeyword(yytext); } 
interface                               { PrintKeyword(yytext); } 
internal                                { PrintKeyword(yytext); } 
is                                      { PrintKeyword(yytext); } 
lock                                    { PrintKeyword(yytext); } 
long                                    { PrintKeyword(yytext); } 
namespace                               { PrintKeyword(yytext); } 
new                                     { PrintKeyword(yytext); } 
null                                    { PrintKeyword(yytext); } 
object                                  { PrintKeyword(yytext); } 
operator                                { PrintKeyword(yytext); } 
out                                     { PrintKeyword(yytext); } 
override                                { PrintKeyword(yytext); } 
params                                  { PrintKeyword(yytext); } 
private                                 { PrintKeyword(yytext); } 
protected                               { PrintKeyword(yytext); } 
public                                  { PrintKeyword(yytext); } 
readonly                                { PrintKeyword(yytext); } 
ref                                     { PrintKeyword(yytext); } 
return                                  { PrintKeyword(yytext); } 
sbyte                                   { PrintKeyword(yytext); } 
sealed                                  { PrintKeyword(yytext); } 
short                                   { PrintKeyword(yytext); } 
sizeof                                  { PrintKeyword(yytext); } 
stackalloc                              { PrintKeyword(yytext); } 
static                                  { PrintKeyword(yytext); } 
string                                  { PrintKeyword(yytext); } 
struct                                  { PrintKeyword(yytext); } 
switch                                  { PrintKeyword(yytext); } 
this                                    { PrintKeyword(yytext); } 
throw                                   { PrintKeyword(yytext); } 
true                                    { PrintKeyword(yytext); } 
try                                     { PrintKeyword(yytext); } 
typeof                                  { PrintKeyword(yytext); } 
uint                                    { PrintKeyword(yytext); } 
ulong                                   { PrintKeyword(yytext); } 
unchecked                               { PrintKeyword(yytext); } 
unsafe                                  { PrintKeyword(yytext); } 
ushort                                  { PrintKeyword(yytext); } 
using                                   { PrintKeyword(yytext); } 
virtual                                 { PrintKeyword(yytext); } 
void                                    { PrintKeyword(yytext); } 
volatile                                { PrintKeyword(yytext); } 
while                                   { PrintKeyword(yytext); } 
get                                     { PrintKeyword(yytext); }
set                                     { PrintKeyword(yytext); }

"+"                                     { PrintAriphmeticOperator(yytext); }
"-"                                     { PrintAriphmeticOperator(yytext); }
"*"                                     { PrintAriphmeticOperator(yytext); }
"/"                                     { PrintAriphmeticOperator(yytext); }
"%"                                     { PrintAriphmeticOperator(yytext); }

"=="                                    { PrintComparisonOperator(yytext); }
"!="                                    { PrintComparisonOperator(yytext); }
"<="                                    { PrintComparisonOperator(yytext); }
">="                                    { PrintComparisonOperator(yytext); }
">"                                     { PrintComparisonOperator(yytext); }
"<"                                     { PrintComparisonOperator(yytext); }

"&&"                                    { PrintLogicalOperator(yytext); }
"||"                                    { PrintLogicalOperator(yytext); }
"!"                                     { PrintLogicalOperator(yytext); }

"="                                     { Print("Found assignment operation"); }

":"                                     { Print("Found inheritance operation"); }

";"                                     { Print("Found semicolon"); }

","                                     { Print("Found comma"); }
"."                                     { Print("Found dot"); }

\{                                      { Print("Found left curly brace"); }
\}                                      { Print("Found right curly brace"); }

\(                                      { Print("Found left parenthesis"); }
\)                                      { Print("Found right parenthesis"); }

\[                                      { Print("Found left bracket"); }
\]                                      { Print("Found right bracket"); }

{DEC_NUMBER}{EXPONENT}                                    {
                                                               std::string number_string = yytext;
                                                               RemoveCharacterFromString(number_string, '_');
                                                               std::from_chars(number_string.c_str(), number_string.c_str() + number_string.size(), currentFloatingPoint);
                                                               PrintFloatingPointLiteral(currentFloatingPoint); 
                                                          }
({DEC_NUMBER}?\.{DEC_NUMBER}){EXPONENT}?                  {
                                                               std::string number_string = yytext;
                                                               RemoveCharacterFromString(number_string, '_');
                                                               std::from_chars(number_string.c_str(), number_string.c_str() + number_string.size(), currentFloatingPoint);
                                                               PrintFloatingPointLiteral(currentFloatingPoint); 
                                                          }

{DEC_NUMBER}                            {
                                            std::string number_string = yytext;
                                            RemoveCharacterFromString(number_string, '_');
                                            std::from_chars(number_string.c_str(), number_string.c_str() + number_string.size(), currentInteger);
                                            PrintIntegerLiteral("decimal", currentInteger); 
                                        }
0x(_|{HEX_DIGIT})*{HEX_DIGIT}+          { 
                                            currentInteger = 0;
                                            std::string number_string = yytext;
                                            RemoveCharacterFromString(number_string, '_');
                                            std::from_chars(number_string.c_str() + 2, number_string.c_str() + number_string.size(), currentInteger, 16);
                                            PrintIntegerLiteral("hexadecimal", currentInteger); 
                                        }
0b(_|{BIN_DIGIT})*{BIN_DIGIT}+          { 
                                            currentInteger = 0;
                                            std::string number_string = yytext;
                                            RemoveCharacterFromString(number_string, '_');
                                            std::from_chars(number_string.c_str() + 2, number_string.c_str() + number_string.size(), currentInteger, 2);
                                            PrintIntegerLiteral("binary", currentInteger); 
                                        }




[[:alpha:]_][[:alnum:]_]*               {
                                            PrintIdentifier(yytext);
                                        }
[[:space:]]+                            { /* ignoring */ }

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

    yylex();
}
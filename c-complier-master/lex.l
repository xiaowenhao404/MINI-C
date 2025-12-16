%{
    #include "yacc.tab.h"
	void comment(void);
	struct Tree* terminator(char* name, int number, ...);
%}
%option yylineno
delim   [ \t\n]
whitespace  {delim}+
digit   [0-9]

int8    (0([0-7])+)
int10   ([1-9]|[1-9]([0-9])+|0)
int16   (0[xX]([A-Fa-f0-9])+)
string  ["](\"|.)*?["]

letter  [A-Za-z_]
id  {letter}({digit}|{letter})*

%%
void	{ yylval.tree = terminator("VOID", yylineno); return VOID;}
main    { yylval.tree = terminator("MAIN", yylineno); return MAIN;}
return  { yylval.tree = terminator("RETURN", yylineno); return RET;}

const   { yylval.tree = terminator("CONST", yylineno); return CONST;}
static  { yylval.tree = terminator("STATIC", yylineno); return STATIC;}
auto    { yylval.tree = terminator("AUTO", yylineno); return AUTO;}

if      { yylval.tree = terminator("IF", yylineno); return IF;}
else    { yylval.tree = terminator("ELSE", yylineno); return ELSE;}
for     { yylval.tree = terminator("FOR", yylineno); return FOR;}
while   { yylval.tree = terminator("WHILE", yylineno); return WHILE;}
do      { yylval.tree = terminator("DO", yylineno); return DO;}
break   { yylval.tree = terminator("BREAK", yylineno); return BREAK;}
continue	{ yylval.tree = terminator("CONTINUE", yylineno); return CONTINUE;}
switch  { yylval.tree = terminator("SWITCH", yylineno); return SWITCH;}
case    { yylval.tree = terminator("CASE", yylineno); return CASE;}
default { yylval.tree = terminator("DEFAULT", yylineno); return DEFAULT;}

sizeof  	{ yylval.tree = terminator("SIZEOF", yylineno); return SIZEOF;}
typedef 	{ yylval.tree = terminator("TYPEDEF", yylineno); return TYPEDEF;}
volatile    { yylval.tree = terminator("VOL", yylineno); return VOLATILE;}
goto    	{ yylval.tree = terminator("GOTO", yylineno); return GOTO;}

int			{ yylval.tree = terminator("INT", yylineno); return INT;}
input_int   { yylval.tree = terminator("INPUT", yylineno); return INPUT;}
output_int  { yylval.tree = terminator("OUTPUT", yylineno);return OUTPUT;}

{whitespace}    {}
{int8}  	{ yylval.tree = terminator("INT8", yylineno); return INT8;}
{int10}   	{ yylval.tree = terminator("INT10", yylineno); return INT10;}
{int16} 	{ yylval.tree = terminator("INT16", yylineno); return INT16;}
{id} 		{ yylval.tree = terminator("ID", yylineno); return ID;}
{string}    { yylval.tree = terminator("STR", yylineno); return STR;}

"//"[^\n]*  {}
"/*"    {comment();}

"("     { yylval.tree = terminator("LP", yylineno); return '(';}
")"     { yylval.tree = terminator("RP", yylineno); return ')';}
"["     { yylval.tree = terminator("LSB", yylineno); return '[';}
"]"     { yylval.tree = terminator("RSB", yylineno); return ']';}
"{"     { yylval.tree = terminator("LCB", yylineno); return '{';}
"}"     { yylval.tree = terminator("RCB", yylineno); return '}';}

"<"     { yylval.tree = terminator("LT", yylineno); return '<';}
"<="    { yylval.tree = terminator("LE", yylineno); return LE;}
">"     { yylval.tree = terminator("GT", yylineno); return '>';}
">="    { yylval.tree = terminator("GE", yylineno); return GE;}

"&"     { yylval.tree = terminator("&", yylineno); return '&';}
"|"     { yylval.tree = terminator("|", yylineno); return '|';}
"&&"    { yylval.tree = terminator("AND", yylineno); return AND;}
"||"    { yylval.tree = terminator("OR", yylineno); return OR;}

"+"     { yylval.tree = terminator("ADD", yylineno); return '+';}
"-"     { yylval.tree = terminator("SUB", yylineno); return '-';}
"*"     { yylval.tree = terminator("MUL", yylineno); return '*';}
"/"     { yylval.tree = terminator("DIV", yylineno); return '/';}
"%"     { yylval.tree = terminator("MOD", yylineno); return '%';}
"^"     { yylval.tree = terminator("POW", yylineno); return '^';}
"!"     { yylval.tree = terminator("NOT", yylineno); return '!';}

"="     { yylval.tree = terminator("ASSIGN", yylineno); return '=';}
"+="    { yylval.tree = terminator("ADD_ASSIGN", yylineno); return ADD_ASSIGN;}
"-="    { yylval.tree = terminator("SUB_ASSIGN", yylineno); return SUB_ASSIGN;}
"*="    { yylval.tree = terminator("MUL_ASSIGN", yylineno); return MUL_ASSIGN;}
"/="    { yylval.tree = terminator("DIV_ASSIGN", yylineno); return DIV_ASSIGN;}
"%="    { yylval.tree = terminator("MOD_ASSIGN", yylineno); return MOD_ASSIGN;}
"++"    { yylval.tree = terminator("INC", yylineno); return INC;}
"--"    { yylval.tree = terminator("DEC", yylineno); return DEC;}
"=="    { yylval.tree = terminator("EQ", yylineno); return EQUAL;}
"!="    { yylval.tree = terminator("NE", yylineno); return NE;}
","     { yylval.tree = terminator("COMMA", yylineno); return ',';}
";"     { yylval.tree = terminator("SEMICOLON", yylineno); return ';';}

"->"    { yylval.tree = terminator("PTR", yylineno); return PTR;}
.       {fprintf(yyout, "Not include: %s\n", yytext); }
%%

void comment(void)
{
	char c, prev = 0;
  
	while ((c = input()) != 0)      /* (EOF maps to 0) */
	{
		if (c == '/' && prev == '*')
			return;
		prev = c;
	}
	/* perror("unterminated comment"); */
    printf("Error: unterminated comment\n");
}
int yywrap() 
{ 
	return 1; 
}

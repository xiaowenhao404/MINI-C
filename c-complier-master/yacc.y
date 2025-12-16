%{
    #include "tree.h"
    #include "hashMap.h"
    #include "inner.h"
    #include <stdio.h>
    #include <string.h>
    void output();
    int yylex();
    void yyerror(const char *s);
    extern int yylineno;
    extern FILE* yyout;
    FILE *out,*outInner;
    
    HashMap* hashMap = NULL;    // 符号表
    int scope = 0;
    struct Declator* declator;
    int type;
    int preType;

    Tree* root;
    extern Node *head;
    //中间代码生成
    int line_count=1;
   
%}
%union{
    struct Tree* tree;
}
%token <tree> INT8 INT10 INT16
%token <tree> ID
%token <tree> INT FLOAT CHAR
%token <tree> VOID MAIN RET CONST STATIC AUTO IF ELSE WHILE DO BREAK CONTINUE SWITCH CASE
%token <tree> STRUCT 
%token <tree> DEFAULT SIZEOF TYPEDEF VOLATILE GOTO INPUT OUTPUT
%token <tree> LE GE AND OR ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN 
%token <tree> INC DEC EQUAL NE PTR FOR STR
%token <tree> '(' ')' '[' ']' '{' '}' '!' '^' '&' '*' '-' '+' '=' '/' '%' ',' ';' '<' '>'
%type <tree> type
%type <tree> constant primary_expression postfix_expression unary_expression cast_expression
multiplicative_expression additive_expression relational_expression equality_expression
logical_and_expression logical_or_expression assignment_expression operate_expression declare_expression
nullable_expression while_expression for_expression funcion_expression if_expression if_identifier return_expression null unary_operator
main_function sentence statement assignment_operator single_expression
function_definition parameter_list parameter_declaration external_declaration argument_list assignment_expression_list
struct_definition struct_member_list struct_member_declaration

%precedence ')'
%precedence ELSE

%%


project:
    external_declaration
    {      
        root = createTree("Project", 1, $1);

        //重新赋值行号
        int seek=1;
        line_count=1;
        if(root->code){
            while(seek){
                seek = swap(root->code,"#",lineToString(line_count++));
            }
            fprintf(outInner,"%s",root->code);
        }
    }
    | project external_declaration
    {
        root = createTree("Project", 2, $1, $2);

        //重新赋值行号
        int seek=1;
        line_count=1;
        if(root->code){
            while(seek){
                seek = swap(root->code,"#",lineToString(line_count++));
            }
            fprintf(outInner,"%s",root->code);
        }
    }
;

/* 外部声明：可以是函数定义、main函数或结构体定义 */
external_declaration
    : function_definition
    | main_function
    | struct_definition
;

/* 结构体定义 */
struct_definition
    : STRUCT ID '{' struct_member_list '}' ';'
    {
        $$ = createTree("STRUCT_DEF", 3, $2, $4, NULL);
        $$->line = yylineno;
    }
;

/* 结构体成员列表 */
struct_member_list
    : struct_member_declaration
    {
        $$ = $1;
    }
    | struct_member_list struct_member_declaration
    {
        $$ = createTree("MEMBER_LIST", 2, $1, $2);
        $$->line = yylineno;
    }
;

/* 结构体成员声明 */
struct_member_declaration
    : type ID ';'
    {
        $$ = createTree("MEMBER", 2, $1, $2);
        $$->line = yylineno;
    }
;

main_function
    : VOID MAIN '(' ')' '{' sentence '}'
    {
        $$ = createTree("Main Func", 7, $1, $2, $3, $4, $5, $6, $7);
    }
    | type MAIN '(' ')' '{' sentence '}'
    {
        $$ = createTree("Main Func", 7, $1, $2, $3, $4, $5, $6, $7);
    }
;

sentence
    : statement sentence
    {
        $$ = createTree("sentence", 2, $1, $2);
    }
    | statement

;
constant
    : INT8
    | INT10 
    | INT16
;

primary_expression
    : ID
    {
        if(type == 0){
            Data* data = toData(type, $1->content, NULL, scope);
            HashNode* hashNode = get(hashMap, data);
            if(hashNode == NULL){
                char* error = (char*)malloc(50);
                sprintf(error, "\"%s\" is undefined", $1->content);
                yyerror(error);
                free(error);
            }
        }
    }
    | constant
    | '(' operate_expression ')'
    {
        $$ = createTree("primary_expression", 3, $1, $2, $3);
    }
;

postfix_expression
    : primary_expression
    | postfix_expression '[' operate_expression ']'
    {
        $$ = addDeclator("Array", $1, $3);
        //$$->code ti = $1-> inner * $3->inner + $3->inner
    }
    | postfix_expression INC
    {
        $$ = createTree("postfix_expression", 2, $1, $2);
    }
    | postfix_expression DEC
    {
        $$ = createTree("postfix_expression", 2, $1, $2);
    }
    | ID '(' argument_list ')'
    {
        $$ = createTree("FUNC_CALL", 2, $1, $3);
        $$->line = yylineno;
    }
    | ID '(' ')'
    {
        $$ = createTree("FUNC_CALL", 1, $1);
        $$->line = yylineno;
    }
    | postfix_expression '.' ID
    {
        $$ = createTree("MEMBER_ACCESS", 2, $1, $3);
        $$->line = yylineno;
    }
    | postfix_expression PTR ID
    {
        $$ = createTree("PTR_MEMBER_ACCESS", 2, $1, $3);
        $$->line = yylineno;
    }
;

/* 参数列表（函数调用时使用）*/
argument_list
    : operate_expression
    {
        $$ = $1;
    }
    | argument_list ',' operate_expression
    {
        $$ = createTree("ARG_LIST", 2, $1, $3);
        $$->line = yylineno;
    }
;

unary_operator
    : '+'
    | '-'
    | '*' {  type = preType; }
    | '&'
    | '!'
;

unary_expression
    : postfix_expression
    | INC postfix_expression
    {
        $$ = unaryOpr("unary_expression", $1, $2);
    }
    | DEC postfix_expression
    {
        $$ = unaryOpr("unary_expression", $1, $2);
    }
    | unary_operator cast_expression
    {
        if(!strcmp($1->content, "*")){
            // 在表达式中，* 是解引用运算符
            $$ = createTree("DEREF", 1, $2);
            $$->line = yylineno;
        }else if(!strcmp($1->content, "&")){
            // & 是取地址运算符
            $$ = createTree("ADDR_OF", 1, $2);
            $$->line = yylineno;
        }else{
            $$ = unaryOpr("unary_expression", $1, $2);
        }
    }
;

cast_expression
    : unary_expression

;

multiplicative_expression
    : cast_expression
    | multiplicative_expression '*' cast_expression
    {
        $$ = binaryOpr("multiplicative_expression", $1, $2, $3);
    }
    | multiplicative_expression '/' cast_expression
    {
        $$ = binaryOpr("multiplicative_expression", $1, $2, $3);
    }
    | multiplicative_expression '%' cast_expression
    {
        $$ = binaryOpr("multiplicative_expression", $1, $2, $3);
    }
    | multiplicative_expression '^' cast_expression
    {
        $$ = binaryOpr("multiplicative_expression", $1, $2, $3);
    }
;

additive_expression
    : multiplicative_expression

    | additive_expression '+' multiplicative_expression
    {
        $$ = binaryOpr("additive_expression", $1, $2, $3);
    }
    | additive_expression '-' multiplicative_expression
    {
        $$ = binaryOpr("additive_expression", $1, $2, $3);
    }
;

relational_expression
    : additive_expression
    | relational_expression '<' additive_expression
    {
        $$ = binaryOpr("relational_expression", $1, $2, $3);
    }
    | relational_expression '>' additive_expression
    {
        $$ = binaryOpr("relational_expression", $1, $2, $3);   
    }
    | relational_expression LE additive_expression
    {
        $$ = binaryOpr("relational_expression", $1, $2, $3);
    }
    | relational_expression GE additive_expression
    {
        $$ = binaryOpr("relational_expression", $1, $2, $3);
    }
;

equality_expression
    : relational_expression
    | equality_expression EQUAL relational_expression
    {
        $$ = binaryOpr("equality_expression", $1, $2, $3);
    }
    | equality_expression NE relational_expression
    {
        $$ = binaryOpr("equality_expression", $1, $2, $3);
    }
;

logical_and_expression
    : equality_expression
    | logical_and_expression AND equality_expression
    {
        $$ = binaryOpr("logical_and_expression", $1, $2, $3);
    }
;

logical_or_expression
    : logical_and_expression
    | logical_or_expression OR logical_and_expression
    {
        $$ = binaryOpr("logical_or_expression", $1, $2, $3);
    }
;

assignment_operator
    : '='
    | MUL_ASSIGN
    | DIV_ASSIGN
    | MOD_ASSIGN
    | ADD_ASSIGN
    | SUB_ASSIGN
;

assignment_expression
    : logical_or_expression
    | unary_expression assignment_operator assignment_expression
    {
        $$ = assignOpr("assignment_expression", $1, $2, $3);
    }
;

operate_expression
    : assignment_expression
    | operate_expression ',' assignment_expression
    {
        $$ = createTree("operate_expression", 3, $1, $2, $3);
    }
;

null:{$$ = createTree("null", 0, yylineno);};

nullable_expression
    : null
    | operate_expression
;

declare_expression
    : type operate_expression
    {
        $$ = createTree("declare_expression", 2, $1, $2);
        if($2){
            putTree(hashMap, $2);
            preType = type = 0;
        }
    }
    | type ID '[' INT10 ']'
    {
        $$ = createTree("ARRAY_DECL", 3, $1, $2, $4);
        $$->line = yylineno;
        putTree(hashMap, $2);
        preType = type = 0;
    }
    | type ID '[' INT10 ']' '[' INT10 ']'
    {
        $$ = createTree("ARRAY_2D_DECL", 4, $1, $2, $4, $7);
        $$->line = yylineno;
        putTree(hashMap, $2);
        preType = type = 0;
    }
    | type ID '[' INT10 ']' '=' '{' assignment_expression_list '}'
    {
        $$ = createTree("ARRAY_DECL_INIT", 4, $1, $2, $4, $8);
        $$->line = yylineno;
        putTree(hashMap, $2);
        preType = type = 0;
    }
    | type '*' ID
    {
        $$ = createTree("POINTER_DECL", 2, $1, $3);
        $$->line = yylineno;
        putTree(hashMap, $3);
        preType = type = 0;
    }
    | type '*' ID '=' operate_expression
    {
        $$ = createTree("POINTER_DECL_INIT", 3, $1, $3, $5);
        $$->line = yylineno;
        putTree(hashMap, $3);
        preType = type = 0;
    }
;

/* 初始化列表（用于数组初始化）*/
assignment_expression_list
    : assignment_expression
    | assignment_expression_list ',' assignment_expression
    {
        $$ = createTree("INIT_LIST", 2, $1, $3);
        $$->line = yylineno;
    }
;

single_expression
    : operate_expression ';'
    | declare_expression ';'
    | if_expression
    | while_expression
    | for_expression
    | funcion_expression ';'
    | return_expression ';'
    | ';'
;

if_expression 
    : if_identifier ')' statement
    {
        int headline = $1->headline;
        int nextline = line_count;
        $$ = ifOpr("if_expression",headline,nextline, $1, $3);
    }
    | if_identifier ')' statement ELSE {$1->nextline = line_count++;} statement
    {
        int headline = $1->headline;
        int next1 = $1->nextline;
        int next2 = line_count;
        $$ = ifelseOpr("if_else_expression",headline,next1,next2, $1, $3, $6);
    }
;
if_identifier
    : IF  '(' operate_expression
    {
        $$ = $3;
        $$->headline = line_count;
        line_count += 2;
    }
;


statement
    : '{' sentence '}'
    {
        $$ = createTree("statement", 3, $1, $2, $3);
    }
    | '{' '}'
    {
        $$ = createTree("statement", 2, $1, $2);
    }
    | single_expression  {head = NULL;}
;

for_expression
    : FOR '(' nullable_expression {
        $1->headline = line_count;
    } ';'  nullable_expression ';' {
        $6->headline = line_count;
    } nullable_expression ')' {
        
    } statement
    {
        line_count += 2;
        int head1 = $1->headline;
        int head2 = $6->headline;
        int nextline = line_count++;
        $$ = forOpr("for_expression",head1, head2, nextline, $3, $6, $9, $12);
    }
;

while_expression
    : WHILE {
        $1->headline = line_count;
    }'(' operate_expression ')' {
        $4->headline = line_count;
        line_count += 2;
    } statement{
        int head1 = $1->headline;
        int head2 = $4->headline;
        int nextline = line_count++;
        $$ = whileOpr("while_expression",head1, head2, nextline, $4, $7);
    }
;

funcion_expression
    : INPUT '(' operate_expression ')'
    {
        $$ = unaryFunc("input", $1, $3);
        line_count+=2;
    }
    | OUTPUT '(' operate_expression ')'
    {
        $$ = unaryFunc("output", $1, $3);
        line_count+=2;
    }
;

return_expression
    : RET
    {
        $$ = retNull("return_expression", $1);
    }
    | RET operate_expression
    {
        $$ = retOpr("return_expression", $1, $2);
    }
;

/* ==================== 函数定义规则（2.0版本）==================== */

/* 参数声明 */
parameter_declaration
    : type ID
    {
        $$ = createTree("PARAM", 2, $1, $2);
        $$->line = yylineno;
    }
    | type '*' ID
    {
        $$ = createTree("PARAM_PTR", 2, $1, $3);
        $$->line = yylineno;
    }
    | type ID '[' ']'
    {
        $$ = createTree("PARAM_ARRAY", 2, $1, $2);
        $$->line = yylineno;
    }
;

/* 参数列表 */
parameter_list
    : parameter_declaration
    {
        $$ = $1;
    }
    | parameter_list ',' parameter_declaration
    {
        $$ = createTree("PARAM_LIST", 2, $1, $3);
        $$->line = yylineno;
    }
;

/* 函数定义 */
function_definition
    : type ID '(' parameter_list ')' '{' sentence '}'
    {
        $$ = createTree("FUNC_DEF", 5, $1, $2, $4, $6, $8);
        $$->line = yylineno;
    }
    | type ID '(' ')' '{' sentence '}'
    {
        $$ = createTree("FUNC_DEF", 4, $1, $2, $6, $7);
        $$->line = yylineno;
    }
    | VOID ID '(' parameter_list ')' '{' sentence '}'
    {
        $$ = createTree("FUNC_DEF", 5, $1, $2, $4, $6, $8);
        $$->line = yylineno;
    }
    | VOID ID '(' ')' '{' sentence '}'
    {
        $$ = createTree("FUNC_DEF", 4, $1, $2, $6, $7);
        $$->line = yylineno;
    }
;

type
    : INT {
        $$ = createTree("type", 1, $1);
        type = INT;
    }
    | FLOAT {
        $$ = createTree("type", 1, $1);
        type = FLOAT;
    }
    | CHAR {
        $$ = createTree("type", 1, $1);
        type = CHAR;
    }
    | VOID {
        $$ = createTree("type", 1, $1);
        type = VOID;
    }
%%




void yyerror(const char* s){
    printf("Error: %s\tline: %d\n", s, yylineno);
}

int main(int argc, char* argv[]){
    const char* outFile="Lexical";
    const char* outFile2="Grammatical";
    const char* outFile3="Innercode";
    extern FILE* yyin, *yyout;	//yyin和yyout都是FILE*类型
    type = 0;
    hashMap = createHashMap(2);
	yyin = fopen(argv[1], "r");
    yyout = fopen(outFile, "w");
    out = fopen(outFile2,"w");
    outInner = fopen(outFile3,"w");
    int i = 0;
    fprintf(yyout, "%-15s\t%-15s\t%s\n", "单词", "词素", "属性");
	if(!yyparse()){
        //正常解读文件
        printf("read successfully\n");
        printTree(root);
    }
	else{
        printf("something error\n");
    }
    fclose(out);
    fclose(outInner);
	fclose(yyin);
    fclose(yyout);
    destoryHashMap(hashMap);
	return 0;
}


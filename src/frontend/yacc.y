%{
    #include "../utils/tree.h"
    #include "../utils/hashMap.h"
    #include "../utils/inner.h"
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
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
%token <tree> INT8 INT10 INT16 FLOAT10
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
%type <tree> project

%precedence ')'
%precedence ELSE

%%


project:
    external_declaration
    {      
        root = createTree("Project", 1, $1);
        $$ = root;

        // 仅在第一个声明时重置行号，后续声明继续编号
        // 使用静态变量跟踪是否是第一次
        static int first_decl = 1;
        if (first_decl) {
            line_count = 1;
            first_decl = 0;
        }

        //重新赋值行号
        int seek=1;
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
        $$ = root;

        // 不重置行号，继续编号
        int seek=1;
        // 只处理第二个 external_declaration 的代码
        if($2 && $2->code){
            while(seek){
                seek = swap($2->code,"#",lineToString(line_count++));
            }
            fprintf(outInner,"%s",$2->code);
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
        // 结构体定义不生成运行时代码，只注册类型信息（3个参数）
        $$->code = mergeCode(3, "#STRUCT_DEF ", $2->content, "\n");
    }
;

/* 结构体成员列表 */
struct_member_list
    : struct_member_declaration
    {
        $$ = $1;
        $$->code = $1->code ? $1->code : "";
    }
    | struct_member_list struct_member_declaration
    {
        $$ = createTree("MEMBER_LIST", 2, $1, $2);
        $$->line = yylineno;
        $$->code = mergeCode(2, $1->code ? $1->code : "", $2->code ? $2->code : "");
    }
;

/* 结构体成员声明 */
struct_member_declaration
    : type ID ';'
    {
        $$ = createTree("MEMBER", 2, $1, $2);
        $$->line = yylineno;
        // 记录成员信息（3个参数）
        $$->code = mergeCode(3, "#MEMBER ", $2->content, "\n");
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
    | FLOAT10
;

primary_expression
    : ID
    {
        // 不进行变量检查，因为：
        // 1. 在声明语句中（type != 0），右侧的变量还未被添加到符号表
        // 2. 语义检查应该在单独的语义分析阶段进行
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
        $$ = createTree("ARRAY_ACCESS", 2, $1, $3);
        // 生成数组访问的中间代码
        // t1 = index * 4
        // t2 = arr + t1
        // t3 = load t2
        char* temp_offset = mergeCode(2, "t", toString(inner_count++));
        char* temp_addr = mergeCode(2, "t", toString(inner_count++));
        $$->inner = mergeCode(2, "t", toString(inner_count++));
        // 19个字符串参数
        $$->code = mergeCode(19,
            $1->code ? $1->code : "",
            $3->code ? $3->code : "",
            "#", temp_offset, " = ", $3->inner, " * 4\n",
            "#", temp_addr, " = ", $1->inner, " + ", temp_offset, "\n",
            "#", $$->inner, " = load ", temp_addr, "\n");
        line_count += 3;
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
        // 生成函数调用的中间代码: args... + result = call func_name
        $$->inner = mergeCode(2, "t", toString(inner_count++));
        $$->code = mergeCode(6, $3->code ? $3->code : "", 
            "#", $$->inner, " = call ", $1->content, "\n");
        line_count++;
    }
    | ID '(' ')'
    {
        $$ = createTree("FUNC_CALL", 1, $1);
        $$->line = yylineno;
        // 生成无参数函数调用的中间代码
        $$->inner = mergeCode(2, "t", toString(inner_count++));
        $$->code = mergeCode(5, "#", $$->inner, " = call ", $1->content, "\n");
        line_count++;
    }
    | postfix_expression '.' ID
    {
        $$ = createTree("MEMBER_ACCESS", 2, $1, $3);
        $$->line = yylineno;
        // 生成结构体成员访问的中间代码
        // TODO: 需要从符号表获取成员偏移量
        char* temp_addr = mergeCode(2, "t", toString(inner_count++));
        $$->inner = mergeCode(2, "t", toString(inner_count++));
        // 11个字符串参数
        $$->code = mergeCode(11,
            $1->code ? $1->code : "",
            "#", temp_addr, " = ", $1->inner, " + 0\n",
            "#", $$->inner, " = load ", temp_addr, "\n");
        line_count += 2;
    }
    | postfix_expression PTR ID
    {
        $$ = createTree("PTR_MEMBER_ACCESS", 2, $1, $3);
        $$->line = yylineno;
        // 生成指针成员访问的中间代码 (ptr->member)
        char* temp_addr = mergeCode(2, "t", toString(inner_count++));
        $$->inner = mergeCode(2, "t", toString(inner_count++));
        // 11个字符串参数
        $$->code = mergeCode(11,
            $1->code ? $1->code : "",
            "#", temp_addr, " = ", $1->inner, " + 0\n",
            "#", $$->inner, " = load ", temp_addr, "\n");
        line_count += 2;
    }
;

/* 参数列表（函数调用时使用）*/
argument_list
    : operate_expression
    {
        $$ = $1;
        // 检查是否为逗号表达式（code 已经包含 arg 指令）
        if($1->num == 3) {
            // 逗号表达式，code 已经包含参数代码，直接使用
        } else {
            // 单个表达式，生成 arg 指令
            $$->code = mergeCode(4, $1->code ? $1->code : "", "#arg ", $1->inner ? $1->inner : "0", "\n");
            line_count++;
        }
    }
    | argument_list ',' operate_expression
    {
        $$ = createTree("ARG_LIST", 2, $1, $3);
        $$->line = yylineno;
        // 检查第三个操作数是否为逗号表达式
        if($3->num == 3) {
            // 逗号表达式，直接合并
            $$->code = mergeCode(2, $1->code ? $1->code : "", $3->code ? $3->code : "");
        } else {
            // 单个表达式，生成 arg 指令
            $$->code = mergeCode(5, $1->code ? $1->code : "", $3->code ? $3->code : "", "#arg ", $3->inner ? $3->inner : "0", "\n");
            line_count++;
        }
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
            // 生成解引用的中间代码: result = load ptr
            $$->inner = mergeCode(2, "t", toString(inner_count++));
            $$->code = mergeCode(6, $2->code ? $2->code : "",
                "#", $$->inner, " = load ", $2->inner, "\n");
            line_count++;
        }else if(!strcmp($1->content, "&")){
            // & 是取地址运算符
            $$ = createTree("ADDR_OF", 1, $2);
            $$->line = yylineno;
            // 生成取地址的中间代码: result = addr var
            $$->inner = mergeCode(2, "t", toString(inner_count++));
            $$->code = mergeCode(6, $2->code ? $2->code : "",
                "#", $$->inner, " = addr ", $2->inner, "\n");
            line_count++;
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
        $$->inner = $3->inner;  // 设置 inner 为最后一个表达式的值
        // 生成参数代码（用于函数调用）
        // 只为非逗号表达式的 $1 生成 arg（避免重复）
        if($1->num != 3) {
            // $1 不是逗号表达式，生成 arg（7个字符串参数）
            $$->code = mergeCode(7, 
                $1->code ? $1->code : "", "#arg ", $1->inner ? $1->inner : "0", "\n",
                "#arg ", $3->inner ? $3->inner : "0", "\n");
        } else {
            // $1 已经是逗号表达式，直接追加新的 arg
            $$->code = mergeCode(4, 
                $1->code ? $1->code : "", 
                "#arg ", $3->inner ? $3->inner : "0", "\n");
        }
        line_count += ($1->num != 3) ? 2 : 1;
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
        // 生成数组声明的中间代码: arr = alloc size*4
        int arr_size = atoi($4->content);
        $$->code = mergeCode(5, "#", $2->content, " = alloc ", toString(arr_size * 4), "\n");
        $$->inner = $2->content;
        line_count++;
    }
    | type ID '[' INT10 ']' '[' INT10 ']'
    {
        $$ = createTree("ARRAY_2D_DECL", 4, $1, $2, $4, $7);
        $$->line = yylineno;
        putTree(hashMap, $2);
        preType = type = 0;
        // 生成二维数组声明的中间代码: arr = alloc rows*cols*4
        int rows = atoi($4->content);
        int cols = atoi($7->content);
        $$->code = mergeCode(5, "#", $2->content, " = alloc ", toString(rows * cols * 4), "\n");
        $$->inner = $2->content;
        line_count++;
    }
    | type ID '[' INT10 ']' '=' '{' assignment_expression_list '}'
    {
        $$ = createTree("ARRAY_DECL_INIT", 4, $1, $2, $4, $8);
        $$->line = yylineno;
        putTree(hashMap, $2);
        preType = type = 0;
        // 生成带初始化的数组声明
        int arr_size = atoi($4->content);
        $$->code = mergeCode(6, "#", $2->content, " = alloc ", toString(arr_size * 4), "\n",
            $8->code ? $8->code : "");
        $$->inner = $2->content;
        line_count++;
    }
    | type '*' ID
    {
        $$ = createTree("POINTER_DECL", 2, $1, $3);
        $$->line = yylineno;
        putTree(hashMap, $3);
        preType = type = 0;
        // 指针声明，初始化为0（3个参数）
        $$->code = mergeCode(3, "#", $3->content, " = 0\n");
        $$->inner = $3->content;
        line_count++;
    }
    | type '*' ID '=' operate_expression
    {
        $$ = createTree("POINTER_DECL_INIT", 3, $1, $3, $5);
        $$->line = yylineno;
        putTree(hashMap, $3);
        preType = type = 0;
        // 指针初始化
        $$->code = mergeCode(6, $5->code ? $5->code : "",
            "#", $3->content, " = ", $5->inner, "\n");
        $$->inner = $3->content;
        line_count++;
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
        // 生成参数的中间代码: param var_name（3个参数）
        $$->code = mergeCode(3, "#param ", $2->content, "\n");
        $$->inner = $2->content;
        line_count++;
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
        $$->code = $1->code;
    }
    | parameter_list ',' parameter_declaration
    {
        $$ = createTree("PARAM_LIST", 2, $1, $3);
        $$->line = yylineno;
        // 合并参数列表的中间代码
        $$->code = mergeCode(2, $1->code ? $1->code : "", $3->code ? $3->code : "");
    }
;

/* 函数定义 */
function_definition
    : type ID '(' parameter_list ')' '{' sentence '}'
    {
        // 修复：包含函数体 $7 (sentence)
        $$ = createTree("FUNC_DEF", 4, $1, $2, $4, $7);
        $$->line = yylineno;
        // 生成函数定义的中间代码
        $$->code = mergeCode(8,
            "#FUNC_BEGIN ", $2->content, "\n",
            $4->code ? $4->code : "",
            $7->code ? $7->code : "",
            "#FUNC_END ", $2->content, "\n");
    }
    | type ID '(' ')' '{' sentence '}'
    {
        $$ = createTree("FUNC_DEF", 3, $1, $2, $6);
        $$->line = yylineno;
        // 生成无参数函数定义的中间代码
        $$->code = mergeCode(7,
            "#FUNC_BEGIN ", $2->content, "\n",
            $6->code ? $6->code : "",
            "#FUNC_END ", $2->content, "\n");
    }
    | VOID ID '(' parameter_list ')' '{' sentence '}'
    {
        // 修复：包含函数体 $7 (sentence)
        $$ = createTree("FUNC_DEF", 4, $1, $2, $4, $7);
        $$->line = yylineno;
        // 生成 void 函数定义的中间代码
        $$->code = mergeCode(8,
            "#FUNC_BEGIN ", $2->content, "\n",
            $4->code ? $4->code : "",
            $7->code ? $7->code : "",
            "#FUNC_END ", $2->content, "\n");
    }
    | VOID ID '(' ')' '{' sentence '}'
    {
        $$ = createTree("FUNC_DEF", 3, $1, $2, $6);
        $$->line = yylineno;
        // 生成无参数 void 函数定义的中间代码
        $$->code = mergeCode(7,
            "#FUNC_BEGIN ", $2->content, "\n",
            $6->code ? $6->code : "",
            "#FUNC_END ", $2->content, "\n");
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
    fprintf(yyout, "%-15s\t%-15s\t%s\n", "单词", "词素", "属性");
	if(!yyparse()){
        //正常解读文件
        printf("✅ 编译成功\n");
        printTree(root);
    }
	else{
        printf("❌ 编译出错\n");
    }
    fclose(out);
    fclose(outInner);
	fclose(yyin);
    fclose(yyout);
    destoryHashMap(hashMap);
	return 0;
}


/**
 * Mini-C 编译器 - 语义分析器
 *
 * 文件: semantic_analyzer.h
 * 描述: 语义分析器的核心定义和接口
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 *
 * 功能:
 * - 遍历AST进行类型检查
 * - 构建符号表
 * - 检测语义错误（类型不匹配、未定义变量等）
 * - 插入隐式类型转换节点
 * - 管理作用域
 */

#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "type_system.h"
#include "symbol_table.h"

// 前向声明（引用现有的Tree结构）
struct Tree;

/* ==================== 语义分析器定义 ==================== */

/**
 * 语义分析器结构体
 *
 * 维护语义分析过程中的状态信息
 */
typedef struct SemanticAnalyzer
{
    SymbolTable *symbol_table;          // 符号表
    int error_count;                    // 错误计数
    int warning_count;                  // 警告计数
    const char *current_file;           // 当前分析的文件名
    bool has_main;                      // 是否有main函数
    Type *current_function_return_type; // 当前函数返回类型（2.0版本）
} SemanticAnalyzer;

/* ==================== 语义分析器创建和销毁 ==================== */

/**
 * 创建语义分析器
 *
 * @param filename 当前分析的文件名
 * @return 语义分析器指针
 */
SemanticAnalyzer *semantic_analyzer_create(const char *filename);

/**
 * 销毁语义分析器
 *
 * @param sa 语义分析器指针
 */
void semantic_analyzer_destroy(SemanticAnalyzer *sa);

/* ==================== 主分析函数 ==================== */

/**
 * 分析整个程序
 *
 * 遍历AST，执行类型检查和符号表构建
 *
 * @param sa 语义分析器指针
 * @param ast AST根节点
 * @return 成功返回true，有错误返回false
 */
bool analyze_program(SemanticAnalyzer *sa, struct Tree *ast);

/**
 * 分析语句
 *
 * @param sa 语义分析器指针
 * @param stmt 语句节点
 */
void analyze_statement(SemanticAnalyzer *sa, struct Tree *stmt);

/**
 * 分析表达式（返回表达式的类型）
 *
 * @param sa 语义分析器指针
 * @param expr 表达式节点
 * @return 表达式的类型
 */
Type *analyze_expression(SemanticAnalyzer *sa, struct Tree *expr);

/**
 * 分析变量声明
 *
 * @param sa 语义分析器指针
 * @param decl 声明节点
 */
void analyze_declaration(SemanticAnalyzer *sa, struct Tree *decl);

/* ==================== 辅助分析函数 ==================== */

/**
 * 分析二元运算表达式
 *
 * @param sa 语义分析器指针
 * @param op 运算符（如 "+"）
 * @param left 左操作数
 * @param right 右操作数
 * @return 运算结果的类型
 */
Type *analyze_binary_op(SemanticAnalyzer *sa, const char *op,
                        struct Tree *left, struct Tree *right);

/**
 * 分析赋值表达式
 *
 * @param sa 语义分析器指针
 * @param lhs 左值
 * @param rhs 右值
 * @return 赋值结果的类型
 */
Type *analyze_assignment(SemanticAnalyzer *sa, struct Tree *lhs, struct Tree *rhs);

/**
 * 分析条件表达式
 *
 * @param sa 语义分析器指针
 * @param cond 条件表达式
 * @return 条件的类型
 */
Type *analyze_condition(SemanticAnalyzer *sa, struct Tree *cond);

/**
 * 分析if语句
 *
 * @param sa 语义分析器指针
 * @param if_node if语句节点
 */
void analyze_if_statement(SemanticAnalyzer *sa, struct Tree *if_node);

/**
 * 分析while语句
 *
 * @param sa 语义分析器指针
 * @param while_node while语句节点
 */
void analyze_while_statement(SemanticAnalyzer *sa, struct Tree *while_node);

/**
 * 分析for语句
 *
 * @param sa 语义分析器指针
 * @param for_node for语句节点
 */
void analyze_for_statement(SemanticAnalyzer *sa, struct Tree *for_node);

/* ==================== 函数分析函数（2.0版本）==================== */

/**
 * 分析函数定义
 *
 * @param sa 语义分析器指针
 * @param func_def 函数定义节点
 */
void analyze_function_definition(SemanticAnalyzer *sa, struct Tree *func_def);

/**
 * 分析return语句
 *
 * @param sa 语义分析器指针
 * @param return_stmt return语句节点
 */
void analyze_return_statement(SemanticAnalyzer *sa, struct Tree *return_stmt);

/**
 * 分析函数调用
 *
 * @param sa 语义分析器指针
 * @param call_node 函数调用节点
 * @return 函数返回类型
 */
Type *analyze_function_call(SemanticAnalyzer *sa, struct Tree *call_node);

/* ==================== 数组分析函数（2.0版本）==================== */

/**
 * 分析数组声明
 *
 * @param sa 语义分析器指针
 * @param decl 数组声明节点
 */
void analyze_array_declaration(SemanticAnalyzer *sa, struct Tree *decl);

/**
 * 分析数组访问
 *
 * @param sa 语义分析器指针
 * @param access 数组访问节点
 * @return 数组元素类型
 */
Type *analyze_array_access(SemanticAnalyzer *sa, struct Tree *access);

/* ==================== 指针分析函数（2.0版本）==================== */

/**
 * 分析指针声明
 *
 * @param sa 语义分析器指针
 * @param decl 指针声明节点
 */
void analyze_pointer_declaration(SemanticAnalyzer *sa, struct Tree *decl);

/**
 * 分析取地址表达式
 *
 * @param sa 语义分析器指针
 * @param addr_of 取地址节点
 * @return 指针类型
 */
Type *analyze_addr_of(SemanticAnalyzer *sa, struct Tree *addr_of);

/**
 * 分析解引用表达式
 *
 * @param sa 语义分析器指针
 * @param deref 解引用节点
 * @return 指针指向的类型
 */
Type *analyze_deref(SemanticAnalyzer *sa, struct Tree *deref);

/* ==================== 结构体分析函数（2.0版本）==================== */

/**
 * 分析结构体定义
 *
 * @param sa 语义分析器指针
 * @param struct_def 结构体定义节点
 */
void analyze_struct_definition(SemanticAnalyzer *sa, struct Tree *struct_def);

/**
 * 分析结构体成员访问
 *
 * @param sa 语义分析器指针
 * @param access 成员访问节点
 * @return 成员类型
 */
Type *analyze_struct_member_access(SemanticAnalyzer *sa, struct Tree *access);

/* ==================== 类型检查辅助函数 ==================== */

/**
 * 检查表达式是否为左值（可赋值）
 *
 * @param expr 表达式节点
 * @return 是左值返回true，否则返回false
 */
bool is_lvalue(struct Tree *expr);

/**
 * 获取节点的类型（如果已标注）
 *
 * 注意：此版本通过节点名称推断类型，完整的类型标注在2.0实现
 *
 * @param sa 语义分析器指针
 * @param node AST节点
 * @return 节点的类型，无法推断返回NULL
 */
Type *get_node_type(SemanticAnalyzer *sa, struct Tree *node);

/**
 * 从变量名查找类型
 *
 * @param sa 语义分析器指针
 * @param var_name 变量名
 * @param line 行号（用于错误报告）
 * @return 变量的类型，未定义返回NULL
 */
Type *lookup_variable_type(SemanticAnalyzer *sa, const char *var_name, int line);

/* ==================== 错误报告 ==================== */

/**
 * 报告语义错误
 *
 * @param sa 语义分析器指针
 * @param line 错误所在行号
 * @param format 错误信息格式字符串
 * @param ... 可变参数
 */
void semantic_error(SemanticAnalyzer *sa, int line, const char *format, ...);

/**
 * 报告语义警告
 *
 * @param sa 语义分析器指针
 * @param line 警告所在行号
 * @param format 警告信息格式字符串
 * @param ... 可变参数
 */
void semantic_warning(SemanticAnalyzer *sa, int line, const char *format, ...);

/**
 * 检查是否有错误
 *
 * @param sa 语义分析器指针
 * @return 有错误返回true，否则返回false
 */
bool has_errors(SemanticAnalyzer *sa);

/* ==================== 辅助工具函数 ==================== */

/**
 * 根据节点名称判断是否为声明语句
 *
 * @param node AST节点
 * @return 是声明返回true，否则返回false
 */
bool is_declaration_node(struct Tree *node);

/**
 * 根据节点名称判断是否为表达式
 *
 * @param node AST节点
 * @return 是表达式返回true，否则返回false
 */
bool is_expression_node(struct Tree *node);

/**
 * 获取运算符字符串
 *
 * @param node 运算符节点
 * @return 运算符字符串（如 "+"），失败返回NULL
 */
const char *get_operator(struct Tree *node);

#endif /* SEMANTIC_ANALYZER_H */

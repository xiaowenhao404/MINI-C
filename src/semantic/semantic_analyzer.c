/**
 * Mini-C 编译器 - 语义分析器实现
 * 
 * 文件: semantic_analyzer.c
 * 描述: 语义分析器的具体实现
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 */

#include "semantic_analyzer.h"
#include "../../c-complier-master/tree.h"  // 引用现有的Tree结构
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* ==================== 前向声明 ==================== */

static void analyze_variable_list(SemanticAnalyzer *sa, struct Tree *vars, Type *var_type);

/* ==================== 语义分析器创建和销毁 ==================== */

/**
 * 创建语义分析器
 */
SemanticAnalyzer* semantic_analyzer_create(const char *filename) {
    SemanticAnalyzer *sa = (SemanticAnalyzer*)malloc(sizeof(SemanticAnalyzer));
    if (!sa) {
        fprintf(stderr, "错误: 语义分析器内存分配失败\n");
        return NULL;
    }
    
    // 初始化类型系统
    init_type_system();
    
    // 创建符号表（使用素数大小127以减少哈希冲突）
    sa->symbol_table = symbol_table_create(127);
    if (!sa->symbol_table) {
        free(sa);
        return NULL;
    }
    
    sa->error_count = 0;
    sa->warning_count = 0;
    sa->current_file = filename;
    sa->has_main = false;
    
    return sa;
}

/**
 * 销毁语义分析器
 */
void semantic_analyzer_destroy(SemanticAnalyzer *sa) {
    if (!sa) {
        return;
    }
    
    // 销毁符号表
    if (sa->symbol_table) {
        symbol_table_destroy(sa->symbol_table);
    }
    
    // 清理类型系统
    cleanup_type_system();
    
    free(sa);
}

/* ==================== 错误报告 ==================== */

/**
 * 报告语义错误
 */
void semantic_error(SemanticAnalyzer *sa, int line, const char *format, ...) {
    if (!sa) {
        return;
    }
    
    // 打印错误位置
    fprintf(stderr, "%s:%d: 错误: ", sa->current_file ? sa->current_file : "unknown", line);
    
    // 打印错误信息
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
    
    sa->error_count++;
}

/**
 * 报告语义警告
 */
void semantic_warning(SemanticAnalyzer *sa, int line, const char *format, ...) {
    if (!sa) {
        return;
    }
    
    // 打印警告位置
    fprintf(stderr, "%s:%d: 警告: ", sa->current_file ? sa->current_file : "unknown", line);
    
    // 打印警告信息
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
    
    sa->warning_count++;
}

/**
 * 检查是否有错误
 */
bool has_errors(SemanticAnalyzer *sa) {
    return sa && sa->error_count > 0;
}

/* ==================== 辅助工具函数 ==================== */

/**
 * 判断节点是否为声明语句
 */
bool is_declaration_node(struct Tree *node) {
    if (!node || !node->name) {
        return false;
    }
    return strcmp(node->name, "declare_expression") == 0;
}

/**
 * 判断节点是否为表达式
 */
bool is_expression_node(struct Tree *node) {
    if (!node || !node->name) {
        return false;
    }
    
    // 各种表达式节点名称
    const char *expr_names[] = {
        "additive_expression",
        "multiplicative_expression",
        "assignment_expression",
        "relational_expression",
        "equality_expression",
        "logical_and_expression",
        "logical_or_expression",
        "primary_expression",
        "postfix_expression",
        "unary_expression",
        NULL
    };
    
    for (int i = 0; expr_names[i] != NULL; i++) {
        if (strcmp(node->name, expr_names[i]) == 0) {
            return true;
        }
    }
    
    return false;
}

/**
 * 获取运算符字符串
 */
const char* get_operator(struct Tree *node) {
    if (!node) {
        return NULL;
    }
    
    // 从子节点中查找运算符节点
    for (int i = 0; i < node->num; i++) {
        if (node->leaves && node->leaves[i]) {
            Tree *child = node->leaves[i];
            
            // 运算符节点通常有特定名称
            if (child->name && 
                (strcmp(child->name, "ADD") == 0 || 
                 strcmp(child->name, "SUB") == 0 ||
                 strcmp(child->name, "MUL") == 0 ||
                 strcmp(child->name, "DIV") == 0 ||
                 strcmp(child->name, "MOD") == 0 ||
                 strcmp(child->name, "LT") == 0 ||
                 strcmp(child->name, "GT") == 0 ||
                 strcmp(child->name, "LE") == 0 ||
                 strcmp(child->name, "GE") == 0 ||
                 strcmp(child->name, "EQ") == 0 ||
                 strcmp(child->name, "NE") == 0 ||
                 strcmp(child->name, "ASSIGN") == 0)) {
                return child->content ? child->content : child->name;
            }
        }
    }
    
    return NULL;
}

/**
 * 检查是否为左值
 */
bool is_lvalue(struct Tree *expr) {
    if (!expr || !expr->name) {
        return false;
    }
    
    // ID节点是左值
    if (strcmp(expr->name, "ID") == 0) {
        return true;
    }
    
    // 数组访问是左值（2.0版本）
    if (strcmp(expr->name, "Array") == 0) {
        return true;
    }
    
    // 指针解引用是左值（2.0版本）
    if (strcmp(expr->name, "Pointer") == 0) {
        return true;
    }
    
    return false;
}

/* ==================== 类型查询函数 ==================== */

/**
 * 从变量名查找类型
 */
Type* lookup_variable_type(SemanticAnalyzer *sa, const char *var_name, int line) {
    if (!sa || !var_name) {
        return NULL;
    }
    
    Symbol *sym = symbol_lookup(sa->symbol_table, var_name);
    if (!sym) {
        semantic_error(sa, line, "变量 '%s' 未定义", var_name);
        return NULL;
    }
    
    return sym->type;
}

/**
 * 获取节点的类型
 * 
 * 根据节点类型推断或查找类型
 */
Type* get_node_type(SemanticAnalyzer *sa, struct Tree *node) {
    if (!node || !node->name) {
        return NULL;
    }
    
    // INT10, INT8, INT16 -> int类型
    if (strcmp(node->name, "INT10") == 0 || 
        strcmp(node->name, "INT8") == 0 ||
        strcmp(node->name, "INT16") == 0) {
        return new_int_type();
    }
    
    // ID -> 从符号表查找
    if (strcmp(node->name, "ID") == 0) {
        if (node->content) {
            return lookup_variable_type(sa, node->content, node->line);
        }
        return NULL;
    }
    
    // 其他类型（float literal等）在扩展版本实现
    
    return NULL;
}

/* ==================== 表达式分析 ==================== */

/**
 * 分析表达式
 */
Type* analyze_expression(SemanticAnalyzer *sa, struct Tree *expr) {
    if (!expr) {
        return NULL;
    }
    
    // 常量表达式
    Type *type = get_node_type(sa, expr);
    if (type) {
        return type;
    }
    
    // 二元运算表达式
    if (is_expression_node(expr) && expr->num >= 3) {
        const char *op = get_operator(expr);
        if (op) {
            // 假设二元运算：leaves[0] op leaves[2]
            if (expr->leaves[0] && expr->leaves[2]) {
                return analyze_binary_op(sa, op, expr->leaves[0], expr->leaves[2]);
            }
        }
    }
    
    // 递归分析子节点
    if (expr->num > 0 && expr->leaves) {
        for (int i = 0; i < expr->num; i++) {
            Type *child_type = analyze_expression(sa, expr->leaves[i]);
            if (child_type) {
                return child_type;
            }
        }
    }
    
    return NULL;
}

/**
 * 分析二元运算
 */
Type* analyze_binary_op(SemanticAnalyzer *sa, const char *op, 
                        struct Tree *left, struct Tree *right) {
    if (!sa || !op || !left || !right) {
        return NULL;
    }
    
    // 获取左右操作数的类型
    Type *left_type = analyze_expression(sa, left);
    Type *right_type = analyze_expression(sa, right);
    
    if (!left_type || !right_type) {
        return NULL;
    }
    
    // 算术运算和关系运算
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 ||
        strcmp(op, "*") == 0 || strcmp(op, "/") == 0 ||
        strcmp(op, "%") == 0 ||
        strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
        strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0 ||
        strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
        
        // 检查类型兼容性
        if (!type_compatible(left_type, right_type)) {
            semantic_error(sa, left->line, 
                         "类型不兼容: 不能对 '%s' 和 '%s' 进行 '%s' 运算",
                         type_to_string(left_type),
                         type_to_string(right_type),
                         op);
            return NULL;
        }
        
        // 返回提升后的类型
        Type *result_type = promote_type(left_type, right_type);
        
        // 关系运算返回int类型（表示bool）
        if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
            strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0 ||
            strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
            return new_int_type();
        }
        
        return result_type;
    }
    
    return NULL;
}

/**
 * 分析赋值表达式
 */
Type* analyze_assignment(SemanticAnalyzer *sa, struct Tree *lhs, struct Tree *rhs) {
    if (!sa || !lhs || !rhs) {
        return NULL;
    }
    
    // 检查左值
    if (!is_lvalue(lhs)) {
        semantic_error(sa, lhs->line, "赋值运算的左侧必须是左值");
        return NULL;
    }
    
    // 获取左右类型
    Type *lhs_type = analyze_expression(sa, lhs);
    Type *rhs_type = analyze_expression(sa, rhs);
    
    if (!lhs_type || !rhs_type) {
        return NULL;
    }
    
    // 检查类型兼容性
    if (!type_compatible(lhs_type, rhs_type)) {
        semantic_error(sa, lhs->line,
                     "类型不匹配: 不能将 '%s' 赋值给 '%s'",
                     type_to_string(rhs_type),
                     type_to_string(lhs_type));
        return NULL;
    }
    
    return lhs_type;
}

/**
 * 分析条件表达式
 */
Type* analyze_condition(SemanticAnalyzer *sa, struct Tree *cond) {
    if (!sa || !cond) {
        return NULL;
    }
    
    Type *cond_type = analyze_expression(sa, cond);
    
    if (!cond_type) {
        return NULL;
    }
    
    // 条件可以是任意数值类型（非零为真）
    if (!is_numeric_type(cond_type)) {
        semantic_warning(sa, cond->line,
                       "条件表达式类型为 '%s'，应为数值类型",
                       type_to_string(cond_type));
    }
    
    return cond_type;
}

/* ==================== 语句分析 ==================== */

/**
 * 分析变量声明
 */
void analyze_declaration(SemanticAnalyzer *sa, struct Tree *decl) {
    if (!sa || !decl) {
        return;
    }
    
    // declare_expression 节点结构: type operate_expression
    // 例如: int a=10, b=20;
    // leaves[0]: type节点（INT）
    // leaves[1]: operate_expression（包含多个赋值表达式）
    
    if (decl->num < 2) {
        return;
    }
    
    Tree *type_node = decl->leaves[0];
    Tree *vars_node = decl->leaves[1];
    
    // 确定声明的类型
    Type *var_type = NULL;
    if (type_node && type_node->name) {
        if (strcmp(type_node->name, "INT") == 0) {
            var_type = new_int_type();
        } else if (strcmp(type_node->name, "FLOAT") == 0) {
            var_type = new_float_type();
        } else if (strcmp(type_node->name, "CHAR") == 0) {
            var_type = new_char_type();
        }
    }
    
    if (!var_type) {
        semantic_error(sa, decl->line, "未知的类型");
        return;
    }
    
    // 处理变量列表（可能是多个变量）
    analyze_variable_list(sa, vars_node, var_type);
}

/**
 * 分析变量列表（辅助函数）
 * 
 * 处理如 int a=10, b=20, c; 这样的多变量声明
 */
static void analyze_variable_list(SemanticAnalyzer *sa, struct Tree *vars, Type *var_type) {
    if (!vars) {
        return;
    }
    
    // 如果是赋值表达式
    if (vars->name && strcmp(vars->name, "assignment_expression") == 0) {
        // leaves[0]: ID, leaves[1]: =, leaves[2]: 表达式
        if (vars->num >= 3 && vars->leaves[0]) {
            Tree *id_node = vars->leaves[0];
            
            if (id_node->name && strcmp(id_node->name, "ID") == 0 && id_node->content) {
                // 插入符号到符号表
                Symbol *sym = symbol_insert(sa->symbol_table, id_node->content, 
                                          var_type, id_node->line);
                
                if (!sym) {
                    semantic_error(sa, id_node->line, 
                                 "变量 '%s' 重定义", id_node->content);
                    return;
                }
                
                sym->is_initialized = true;
                
                // 检查初始化表达式的类型
                if (vars->leaves[2]) {
                    Type *init_type = analyze_expression(sa, vars->leaves[2]);
                    if (init_type && !type_compatible(var_type, init_type)) {
                        semantic_error(sa, id_node->line,
                                     "初始化类型不匹配: 不能将 '%s' 赋值给 '%s'",
                                     type_to_string(init_type),
                                     type_to_string(var_type));
                    }
                }
            }
        }
    }
    // 如果是ID节点（无初始化）
    else if (vars->name && strcmp(vars->name, "ID") == 0 && vars->content) {
        Symbol *sym = symbol_insert(sa->symbol_table, vars->content, 
                                  var_type, vars->line);
        if (!sym) {
            semantic_error(sa, vars->line, "变量 '%s' 重定义", vars->content);
        }
    }
    
    // 递归处理其他子节点（如逗号分隔的多个变量）
    if (vars->num > 0 && vars->leaves) {
        for (int i = 0; i < vars->num; i++) {
            if (vars->leaves[i]) {
                analyze_variable_list(sa, vars->leaves[i], var_type);
            }
        }
    }
}

/**
 * 分析if语句
 */
void analyze_if_statement(SemanticAnalyzer *sa, struct Tree *if_node) {
    if (!sa || !if_node) {
        return;
    }
    
    // if_expression 或 if_else_expression
    // 结构可能不同，需要根据实际AST调整
    
    // 通常: leaves[0]是条件, leaves[1]是then分支, leaves[2]是else分支
    if (if_node->num > 0 && if_node->leaves[0]) {
        analyze_condition(sa, if_node->leaves[0]);
    }
    
    // 分析then分支（可能需要进入新作用域）
    if (if_node->num > 1 && if_node->leaves[1]) {
        analyze_statement(sa, if_node->leaves[1]);
    }
    
    // 分析else分支
    if (if_node->num > 2 && if_node->leaves[2]) {
        analyze_statement(sa, if_node->leaves[2]);
    }
}

/**
 * 分析while语句
 */
void analyze_while_statement(SemanticAnalyzer *sa, struct Tree *while_node) {
    if (!sa || !while_node) {
        return;
    }
    
    // while_expression结构: 条件 + 循环体
    if (while_node->num > 0 && while_node->leaves[0]) {
        analyze_condition(sa, while_node->leaves[0]);
    }
    
    if (while_node->num > 1 && while_node->leaves[1]) {
        analyze_statement(sa, while_node->leaves[1]);
    }
}

/**
 * 分析for语句
 */
void analyze_for_statement(SemanticAnalyzer *sa, struct Tree *for_node) {
    if (!sa || !for_node) {
        return;
    }
    
    // for_expression结构: 初始化, 条件, 增量, 循环体
    // 处理方式类似while，具体根据AST结构调整
    
    // 遍历所有子节点
    if (for_node->leaves) {
        for (int i = 0; i < for_node->num; i++) {
            if (for_node->leaves[i]) {
                if (is_declaration_node(for_node->leaves[i])) {
                    analyze_declaration(sa, for_node->leaves[i]);
                } else if (is_expression_node(for_node->leaves[i])) {
                    analyze_expression(sa, for_node->leaves[i]);
                } else {
                    analyze_statement(sa, for_node->leaves[i]);
                }
            }
        }
    }
}

/**
 * 分析语句
 */
void analyze_statement(SemanticAnalyzer *sa, struct Tree *stmt) {
    if (!sa || !stmt) {
        return;
    }
    
    // 根据节点名称分派到不同的处理函数
    if (stmt->name) {
        // 声明语句
        if (is_declaration_node(stmt)) {
            analyze_declaration(sa, stmt);
        }
        // 表达式语句（包括赋值）
        else if (is_expression_node(stmt)) {
            analyze_expression(sa, stmt);
        }
        // if语句
        else if (strcmp(stmt->name, "if_expression") == 0 ||
                 strcmp(stmt->name, "if_else_expression") == 0) {
            analyze_if_statement(sa, stmt);
        }
        // while语句
        else if (strcmp(stmt->name, "while_expression") == 0) {
            analyze_while_statement(sa, stmt);
        }
        // for语句
        else if (strcmp(stmt->name, "for_expression") == 0) {
            analyze_for_statement(sa, stmt);
        }
        // 复合语句（块语句）
        else if (strcmp(stmt->name, "statement") == 0 ||
                 strcmp(stmt->name, "sentence") == 0) {
            // 可能需要进入新作用域（如果是块语句）
            bool is_block = false;
            
            // 检查是否有大括号（块语句的标志）
            for (int i = 0; i < stmt->num; i++) {
                if (stmt->leaves[i] && stmt->leaves[i]->name) {
                    if (strcmp(stmt->leaves[i]->name, "LCB") == 0) {
                        is_block = true;
                        break;
                    }
                }
            }
            
            if (is_block) {
                enter_scope(sa->symbol_table);
            }
            
            // 递归分析所有子语句
            for (int i = 0; i < stmt->num; i++) {
                if (stmt->leaves[i]) {
                    analyze_statement(sa, stmt->leaves[i]);
                }
            }
            
            if (is_block) {
                exit_scope(sa->symbol_table);
            }
        }
        // 其他语句类型
        else {
            // 递归分析子节点
            for (int i = 0; i < stmt->num; i++) {
                if (stmt->leaves[i]) {
                    analyze_statement(sa, stmt->leaves[i]);
                }
            }
        }
    }
}

/**
 * 分析整个程序
 */
bool analyze_program(SemanticAnalyzer *sa, struct Tree *ast) {
    if (!sa || !ast) {
        return false;
    }
    
    printf("开始语义分析...\n");
    
    // 遍历AST
    analyze_statement(sa, ast);
    
    // 输出分析结果
    printf("\n");
    printf("语义分析完成:\n");
    printf("  错误数: %d\n", sa->error_count);
    printf("  警告数: %d\n", sa->warning_count);
    printf("  符号总数: %d\n", symbol_table_count(sa->symbol_table));
    
    if (sa->error_count > 0) {
        printf("\n语义分析失败！\n");
        return false;
    }
    
    printf("\n语义分析成功！\n");
    return true;
}


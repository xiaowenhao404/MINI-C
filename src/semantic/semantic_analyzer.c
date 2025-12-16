/**
 * Mini-C 编译器 - 语义分析器实现
 *
 * 文件: semantic_analyzer.c
 * 描述: 语义分析器的具体实现
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 */

#include "semantic_analyzer.h"
#include "../../c-complier-master/tree.h" // 引用现有的Tree结构
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* ==================== 前向声明 ==================== */

static void analyze_variable_list(SemanticAnalyzer *sa, struct Tree *vars, Type *var_type);
static StructMember *extract_struct_members(SemanticAnalyzer *sa, Tree *member_list);
static void extract_members_recursive(SemanticAnalyzer *sa, Tree *node,
                                      StructMember **head, StructMember **tail);

/* ==================== 语义分析器创建和销毁 ==================== */

/**
 * 创建语义分析器
 */
SemanticAnalyzer *semantic_analyzer_create(const char *filename)
{
    SemanticAnalyzer *sa = (SemanticAnalyzer *)malloc(sizeof(SemanticAnalyzer));
    if (!sa)
    {
        fprintf(stderr, "错误: 语义分析器内存分配失败\n");
        return NULL;
    }

    // 初始化类型系统
    init_type_system();

    // 创建符号表（使用素数大小127以减少哈希冲突）
    sa->symbol_table = symbol_table_create(127);
    if (!sa->symbol_table)
    {
        free(sa);
        return NULL;
    }

    sa->error_count = 0;
    sa->warning_count = 0;
    sa->current_file = filename;
    sa->has_main = false;
    sa->current_function_return_type = NULL; // 初始不在函数内部

    return sa;
}

/**
 * 销毁语义分析器
 */
void semantic_analyzer_destroy(SemanticAnalyzer *sa)
{
    if (!sa)
    {
        return;
    }

    // 销毁符号表
    if (sa->symbol_table)
    {
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
void semantic_error(SemanticAnalyzer *sa, int line, const char *format, ...)
{
    if (!sa)
    {
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
void semantic_warning(SemanticAnalyzer *sa, int line, const char *format, ...)
{
    if (!sa)
    {
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
bool has_errors(SemanticAnalyzer *sa)
{
    return sa && sa->error_count > 0;
}

/* ==================== 辅助工具函数 ==================== */

/**
 * 判断节点是否为声明语句
 */
bool is_declaration_node(struct Tree *node)
{
    if (!node || !node->name)
    {
        return false;
    }
    return strcmp(node->name, "declare_expression") == 0;
}

/**
 * 判断节点是否为表达式
 */
bool is_expression_node(struct Tree *node)
{
    if (!node || !node->name)
    {
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
        NULL};

    for (int i = 0; expr_names[i] != NULL; i++)
    {
        if (strcmp(node->name, expr_names[i]) == 0)
        {
            return true;
        }
    }

    return false;
}

/**
 * 获取运算符字符串
 */
const char *get_operator(struct Tree *node)
{
    if (!node)
    {
        return NULL;
    }

    // 从子节点中查找运算符节点
    for (int i = 0; i < node->num; i++)
    {
        if (node->leaves && node->leaves[i])
        {
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
                 strcmp(child->name, "ASSIGN") == 0))
            {
                return child->content ? child->content : child->name;
            }
        }
    }

    return NULL;
}

/**
 * 检查是否为左值
 */
bool is_lvalue(struct Tree *expr)
{
    if (!expr || !expr->name)
    {
        return false;
    }

    // ID节点是左值
    if (strcmp(expr->name, "ID") == 0)
    {
        return true;
    }

    // 数组访问是左值（2.0版本）
    if (strcmp(expr->name, "Array") == 0)
    {
        return true;
    }

    // 指针解引用是左值（2.0版本）
    if (strcmp(expr->name, "Pointer") == 0)
    {
        return true;
    }

    return false;
}

/* ==================== 类型查询函数 ==================== */

/**
 * 从变量名查找类型
 */
Type *lookup_variable_type(SemanticAnalyzer *sa, const char *var_name, int line)
{
    if (!sa || !var_name)
    {
        return NULL;
    }

    Symbol *sym = symbol_lookup(sa->symbol_table, var_name);
    if (!sym)
    {
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
Type *get_node_type(SemanticAnalyzer *sa, struct Tree *node)
{
    if (!node || !node->name)
    {
        return NULL;
    }

    // INT10, INT8, INT16 -> int类型
    if (strcmp(node->name, "INT10") == 0 ||
        strcmp(node->name, "INT8") == 0 ||
        strcmp(node->name, "INT16") == 0)
    {
        return new_int_type();
    }

    // ID -> 从符号表查找
    if (strcmp(node->name, "ID") == 0)
    {
        if (node->content)
        {
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
Type *analyze_expression(SemanticAnalyzer *sa, struct Tree *expr)
{
    if (!expr)
    {
        return NULL;
    }

    // 函数调用表达式（2.0版本）
    if (expr->name && strcmp(expr->name, "FUNC_CALL") == 0)
    {
        return analyze_function_call(sa, expr);
    }

    // 取地址表达式（2.0版本）
    if (expr->name && strcmp(expr->name, "ADDR_OF") == 0)
    {
        return analyze_addr_of(sa, expr);
    }

    // 解引用表达式（2.0版本）
    if (expr->name && strcmp(expr->name, "DEREF") == 0)
    {
        return analyze_deref(sa, expr);
    }

    // 结构体成员访问表达式（2.0版本）
    if (expr->name && (strcmp(expr->name, "MEMBER_ACCESS") == 0 ||
                       strcmp(expr->name, "PTR_MEMBER_ACCESS") == 0))
    {
        return analyze_struct_member_access(sa, expr);
    }

    // 数组访问表达式（2.0版本）
    // 注意：数组访问的识别可能需要根据实际的AST结构进行调整
    // 这里暂时跳过，因为数组访问可能通过其他方式识别

    // 常量表达式
    Type *type = get_node_type(sa, expr);
    if (type)
    {
        return type;
    }

    // 二元运算表达式
    if (is_expression_node(expr) && expr->num >= 3)
    {
        const char *op = get_operator(expr);
        if (op)
        {
            // 假设二元运算：leaves[0] op leaves[2]
            if (expr->leaves[0] && expr->leaves[2])
            {
                return analyze_binary_op(sa, op, expr->leaves[0], expr->leaves[2]);
            }
        }
    }

    // 递归分析子节点
    if (expr->num > 0 && expr->leaves)
    {
        for (int i = 0; i < expr->num; i++)
        {
            Type *child_type = analyze_expression(sa, expr->leaves[i]);
            if (child_type)
            {
                return child_type;
            }
        }
    }

    return NULL;
}

/**
 * 分析二元运算
 */
Type *analyze_binary_op(SemanticAnalyzer *sa, const char *op,
                        struct Tree *left, struct Tree *right)
{
    if (!sa || !op || !left || !right)
    {
        return NULL;
    }

    // 获取左右操作数的类型
    Type *left_type = analyze_expression(sa, left);
    Type *right_type = analyze_expression(sa, right);

    if (!left_type || !right_type)
    {
        return NULL;
    }

    // ==================== 指针算术运算（2.0版本）====================

    // 情况1: 指针 + 整数
    if (strcmp(op, "+") == 0 &&
        left_type->kind == TYPE_POINTER && is_integer_type(right_type))
    {
        return left_type; // 结果类型是指针
    }

    // 情况2: 整数 + 指针
    if (strcmp(op, "+") == 0 &&
        is_integer_type(left_type) && right_type->kind == TYPE_POINTER)
    {
        return right_type; // 结果类型是指针
    }

    // 情况3: 指针 - 整数
    if (strcmp(op, "-") == 0 &&
        left_type->kind == TYPE_POINTER && is_integer_type(right_type))
    {
        return left_type; // 结果类型是指针
    }

    // 情况4: 指针 - 指针
    if (strcmp(op, "-") == 0 &&
        left_type->kind == TYPE_POINTER && right_type->kind == TYPE_POINTER)
    {
        // 检查基类型是否相同
        if (!type_equal(left_type->base, right_type->base))
        {
            semantic_error(sa, left->line,
                           "指针相减必须指向相同类型：%s* 和 %s*",
                           type_to_string(left_type->base),
                           type_to_string(right_type->base));
            return NULL;
        }
        return new_int_type(); // 结果是整数（元素个数差）
    }

    // 情况5: 指针比较（==, !=, <, >, <=, >=）
    if ((strcmp(op, "==") == 0 || strcmp(op, "!=") == 0 ||
         strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
         strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) &&
        left_type->kind == TYPE_POINTER && right_type->kind == TYPE_POINTER)
    {
        // 允许比较，返回int类型（表示bool）
        return new_int_type();
    }

    // ==================== 普通算术运算和关系运算 ====================

    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 ||
        strcmp(op, "*") == 0 || strcmp(op, "/") == 0 ||
        strcmp(op, "%") == 0 ||
        strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
        strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0 ||
        strcmp(op, "==") == 0 || strcmp(op, "!=") == 0)
    {

        // 检查类型兼容性
        if (!type_compatible(left_type, right_type))
        {
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
            strcmp(op, "==") == 0 || strcmp(op, "!=") == 0)
        {
            return new_int_type();
        }

        return result_type;
    }

    return NULL;
}

/**
 * 分析赋值表达式
 */
Type *analyze_assignment(SemanticAnalyzer *sa, struct Tree *lhs, struct Tree *rhs)
{
    if (!sa || !lhs || !rhs)
    {
        return NULL;
    }

    // 检查左值
    if (!is_lvalue(lhs))
    {
        semantic_error(sa, lhs->line, "赋值运算的左侧必须是左值");
        return NULL;
    }

    // 获取左右类型
    Type *lhs_type = analyze_expression(sa, lhs);
    Type *rhs_type = analyze_expression(sa, rhs);

    if (!lhs_type || !rhs_type)
    {
        return NULL;
    }

    // 检查类型兼容性
    if (!type_compatible(lhs_type, rhs_type))
    {
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
Type *analyze_condition(SemanticAnalyzer *sa, struct Tree *cond)
{
    if (!sa || !cond)
    {
        return NULL;
    }

    Type *cond_type = analyze_expression(sa, cond);

    if (!cond_type)
    {
        return NULL;
    }

    // 条件可以是任意数值类型（非零为真）
    if (!is_numeric_type(cond_type))
    {
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
void analyze_declaration(SemanticAnalyzer *sa, struct Tree *decl)
{
    if (!sa || !decl)
    {
        return;
    }

    // declare_expression 节点结构: type operate_expression
    // 例如: int a=10, b=20;
    // leaves[0]: type节点（INT）
    // leaves[1]: operate_expression（包含多个赋值表达式）

    if (decl->num < 2)
    {
        return;
    }

    Tree *type_node = decl->leaves[0];
    Tree *vars_node = decl->leaves[1];

    // 确定声明的类型
    Type *var_type = NULL;
    if (type_node && type_node->name)
    {
        if (strcmp(type_node->name, "INT") == 0)
        {
            var_type = new_int_type();
        }
        else if (strcmp(type_node->name, "FLOAT") == 0)
        {
            var_type = new_float_type();
        }
        else if (strcmp(type_node->name, "CHAR") == 0)
        {
            var_type = new_char_type();
        }
    }

    if (!var_type)
    {
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
static void analyze_variable_list(SemanticAnalyzer *sa, struct Tree *vars, Type *var_type)
{
    if (!vars)
    {
        return;
    }

    // 如果是赋值表达式
    if (vars->name && strcmp(vars->name, "assignment_expression") == 0)
    {
        // leaves[0]: ID, leaves[1]: =, leaves[2]: 表达式
        if (vars->num >= 3 && vars->leaves[0])
        {
            Tree *id_node = vars->leaves[0];

            if (id_node->name && strcmp(id_node->name, "ID") == 0 && id_node->content)
            {
                // 插入符号到符号表
                Symbol *sym = symbol_insert(sa->symbol_table, id_node->content,
                                            var_type, id_node->line);

                if (!sym)
                {
                    semantic_error(sa, id_node->line,
                                   "变量 '%s' 重定义", id_node->content);
                    return;
                }

                sym->is_initialized = true;

                // 检查初始化表达式的类型
                if (vars->leaves[2])
                {
                    Type *init_type = analyze_expression(sa, vars->leaves[2]);
                    if (init_type && !type_compatible(var_type, init_type))
                    {
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
    else if (vars->name && strcmp(vars->name, "ID") == 0 && vars->content)
    {
        Symbol *sym = symbol_insert(sa->symbol_table, vars->content,
                                    var_type, vars->line);
        if (!sym)
        {
            semantic_error(sa, vars->line, "变量 '%s' 重定义", vars->content);
        }
    }

    // 递归处理其他子节点（如逗号分隔的多个变量）
    if (vars->num > 0 && vars->leaves)
    {
        for (int i = 0; i < vars->num; i++)
        {
            if (vars->leaves[i])
            {
                analyze_variable_list(sa, vars->leaves[i], var_type);
            }
        }
    }
}

/**
 * 分析if语句
 */
void analyze_if_statement(SemanticAnalyzer *sa, struct Tree *if_node)
{
    if (!sa || !if_node)
    {
        return;
    }

    // if_expression 或 if_else_expression
    // 结构可能不同，需要根据实际AST调整

    // 通常: leaves[0]是条件, leaves[1]是then分支, leaves[2]是else分支
    if (if_node->num > 0 && if_node->leaves[0])
    {
        analyze_condition(sa, if_node->leaves[0]);
    }

    // 分析then分支（可能需要进入新作用域）
    if (if_node->num > 1 && if_node->leaves[1])
    {
        analyze_statement(sa, if_node->leaves[1]);
    }

    // 分析else分支
    if (if_node->num > 2 && if_node->leaves[2])
    {
        analyze_statement(sa, if_node->leaves[2]);
    }
}

/**
 * 分析while语句
 */
void analyze_while_statement(SemanticAnalyzer *sa, struct Tree *while_node)
{
    if (!sa || !while_node)
    {
        return;
    }

    // while_expression结构: 条件 + 循环体
    if (while_node->num > 0 && while_node->leaves[0])
    {
        analyze_condition(sa, while_node->leaves[0]);
    }

    if (while_node->num > 1 && while_node->leaves[1])
    {
        analyze_statement(sa, while_node->leaves[1]);
    }
}

/**
 * 分析for语句
 */
void analyze_for_statement(SemanticAnalyzer *sa, struct Tree *for_node)
{
    if (!sa || !for_node)
    {
        return;
    }

    // for_expression结构: 初始化, 条件, 增量, 循环体
    // 处理方式类似while，具体根据AST结构调整

    // 遍历所有子节点
    if (for_node->leaves)
    {
        for (int i = 0; i < for_node->num; i++)
        {
            if (for_node->leaves[i])
            {
                if (is_declaration_node(for_node->leaves[i]))
                {
                    analyze_declaration(sa, for_node->leaves[i]);
                }
                else if (is_expression_node(for_node->leaves[i]))
                {
                    analyze_expression(sa, for_node->leaves[i]);
                }
                else
                {
                    analyze_statement(sa, for_node->leaves[i]);
                }
            }
        }
    }
}

/**
 * 分析语句
 */
void analyze_statement(SemanticAnalyzer *sa, struct Tree *stmt)
{
    if (!sa || !stmt)
    {
        return;
    }

    // 根据节点名称分派到不同的处理函数
    if (stmt->name)
    {
        // 函数定义（2.0版本）
        if (strcmp(stmt->name, "FUNC_DEF") == 0)
        {
            analyze_function_definition(sa, stmt);
        }
        // 结构体定义（2.0版本）
        else if (strcmp(stmt->name, "STRUCT_DEF") == 0)
        {
            analyze_struct_definition(sa, stmt);
        }
        // return语句（2.0版本）
        else if (strcmp(stmt->name, "return_expression") == 0)
        {
            analyze_return_statement(sa, stmt);
        }
        // 数组声明（2.0版本）
        else if (strcmp(stmt->name, "ARRAY_DECL") == 0 ||
                 strcmp(stmt->name, "ARRAY_DECL_INIT") == 0 ||
                 strcmp(stmt->name, "ARRAY_2D_DECL") == 0)
        {
            analyze_array_declaration(sa, stmt);
        }
        // 指针声明（2.0版本）
        else if (strcmp(stmt->name, "POINTER_DECL") == 0 ||
                 strcmp(stmt->name, "POINTER_DECL_INIT") == 0)
        {
            analyze_pointer_declaration(sa, stmt);
        }
        // 声明语句
        else if (is_declaration_node(stmt))
        {
            analyze_declaration(sa, stmt);
        }
        // 表达式语句（包括赋值）
        else if (is_expression_node(stmt))
        {
            analyze_expression(sa, stmt);
        }
        // if语句
        else if (strcmp(stmt->name, "if_expression") == 0 ||
                 strcmp(stmt->name, "if_else_expression") == 0)
        {
            analyze_if_statement(sa, stmt);
        }
        // while语句
        else if (strcmp(stmt->name, "while_expression") == 0)
        {
            analyze_while_statement(sa, stmt);
        }
        // for语句
        else if (strcmp(stmt->name, "for_expression") == 0)
        {
            analyze_for_statement(sa, stmt);
        }
        // 复合语句（块语句）
        else if (strcmp(stmt->name, "statement") == 0 ||
                 strcmp(stmt->name, "sentence") == 0)
        {
            // 可能需要进入新作用域（如果是块语句）
            bool is_block = false;

            // 检查是否有大括号（块语句的标志）
            for (int i = 0; i < stmt->num; i++)
            {
                if (stmt->leaves[i] && stmt->leaves[i]->name)
                {
                    if (strcmp(stmt->leaves[i]->name, "LCB") == 0)
                    {
                        is_block = true;
                        break;
                    }
                }
            }

            if (is_block)
            {
                enter_scope(sa->symbol_table);
            }

            // 递归分析所有子语句
            for (int i = 0; i < stmt->num; i++)
            {
                if (stmt->leaves[i])
                {
                    analyze_statement(sa, stmt->leaves[i]);
                }
            }

            if (is_block)
            {
                exit_scope(sa->symbol_table);
            }
        }
        // 其他语句类型
        else
        {
            // 递归分析子节点
            for (int i = 0; i < stmt->num; i++)
            {
                if (stmt->leaves[i])
                {
                    analyze_statement(sa, stmt->leaves[i]);
                }
            }
        }
    }
}

/**
 * 分析整个程序
 */
bool analyze_program(SemanticAnalyzer *sa, struct Tree *ast)
{
    if (!sa || !ast)
    {
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

    if (sa->error_count > 0)
    {
        printf("\n语义分析失败！\n");
        return false;
    }

    printf("\n语义分析成功！\n");
    return true;
}

/* ==================== 函数分析函数（2.0版本）==================== */

/**
 * 从类型说明符节点获取类型
 */
static Type *get_type_from_specifier(Tree *type_node)
{
    if (!type_node || !type_node->content)
    {
        return NULL;
    }

    if (strcmp(type_node->content, "INT") == 0)
    {
        return new_int_type();
    }
    else if (strcmp(type_node->content, "FLOAT") == 0)
    {
        return new_float_type();
    }
    else if (strcmp(type_node->content, "CHAR") == 0)
    {
        return new_char_type();
    }
    else if (strcmp(type_node->content, "VOID") == 0)
    {
        return new_void_type();
    }

    return NULL;
}

/**
 * 递归计数参数
 */
static int count_parameters(Tree *param_list)
{
    if (!param_list)
    {
        return 0;
    }

    if (strcmp(param_list->name, "PARAM") == 0 ||
        strcmp(param_list->name, "PARAM_PTR") == 0 ||
        strcmp(param_list->name, "PARAM_ARRAY") == 0)
    {
        return 1;
    }
    else if (strcmp(param_list->name, "PARAM_LIST") == 0)
    {
        return count_parameters(param_list->leaves[0]) +
               count_parameters(param_list->leaves[1]);
    }

    return 0;
}

/**
 * 递归提取参数信息
 */
static void extract_parameters_recursive(Tree *param_list, Type **types,
                                         char **names, int *index)
{
    if (!param_list)
    {
        return;
    }

    if (strcmp(param_list->name, "PARAM") == 0)
    {
        // 单个参数: type name
        Type *param_type = get_type_from_specifier(param_list->leaves[0]);
        char *param_name = strdup(param_list->leaves[1]->content);

        types[*index] = param_type;
        names[*index] = param_name;
        (*index)++;
    }
    else if (strcmp(param_list->name, "PARAM_PTR") == 0)
    {
        // 指针参数: type * name
        Type *base_type = get_type_from_specifier(param_list->leaves[0]);
        Type *param_type = new_pointer_type(base_type);
        char *param_name = strdup(param_list->leaves[1]->content);

        types[*index] = param_type;
        names[*index] = param_name;
        (*index)++;
    }
    else if (strcmp(param_list->name, "PARAM_ARRAY") == 0)
    {
        // 数组参数: type name[]
        // 数组参数退化为指针（C语言规则）
        Type *base_type = get_type_from_specifier(param_list->leaves[0]);
        Type *param_type = new_pointer_type(base_type); // 数组退化为指针
        char *param_name = strdup(param_list->leaves[1]->content);

        types[*index] = param_type;
        names[*index] = param_name;
        (*index)++;
    }
    else if (strcmp(param_list->name, "PARAM_LIST") == 0)
    {
        // 参数列表：递归处理
        extract_parameters_recursive(param_list->leaves[0], types, names, index);
        extract_parameters_recursive(param_list->leaves[1], types, names, index);
    }
}

/**
 * 提取参数列表信息
 */
static void extract_parameters(Tree *param_list, Type ***param_types,
                               char ***param_names, int *param_count)
{
    if (!param_list)
    {
        *param_types = NULL;
        *param_names = NULL;
        *param_count = 0;
        return;
    }

    // 递归计数参数
    int count = count_parameters(param_list);
    *param_count = count;

    if (count == 0)
    {
        *param_types = NULL;
        *param_names = NULL;
        return;
    }

    // 分配数组
    *param_types = (Type **)malloc(sizeof(Type *) * count);
    *param_names = (char **)malloc(sizeof(char *) * count);

    // 递归提取参数
    int index = 0;
    extract_parameters_recursive(param_list, *param_types, *param_names, &index);
}

/**
 * 分析函数定义
 */
void analyze_function_definition(SemanticAnalyzer *sa, Tree *func_def)
{
    if (!sa || !func_def)
    {
        return;
    }

    // 1. 提取函数信息
    Tree *return_type_node = func_def->leaves[0]; // 返回类型
    Tree *name_node = func_def->leaves[1];        // 函数名
    Tree *param_list = NULL;
    Tree *body = NULL;

    // 判断是否有参数列表
    if (func_def->num == 5)
    {
        // 有参数: type name (params) { body }
        param_list = func_def->leaves[2];
        body = func_def->leaves[4]; // 跳过 '{' 和 '}'
    }
    else if (func_def->num == 4)
    {
        // 无参数: type name () { body }
        param_list = NULL;
        body = func_def->leaves[3];
    }

    // 2. 构造函数类型
    Type *return_type = get_type_from_specifier(return_type_node);
    if (!return_type)
    {
        semantic_error(sa, func_def->line, "无效的返回类型");
        return;
    }

    // 提取参数类型
    Type **param_types = NULL;
    char **param_names = NULL;
    int param_count = 0;

    if (param_list)
    {
        extract_parameters(param_list, &param_types, &param_names, &param_count);
    }

    Type *func_type = new_function_type(return_type, param_types, param_count);

    // 3. 插入函数符号到全局作用域
    Symbol *func_sym = symbol_insert(sa->symbol_table, name_node->content,
                                     func_type, func_def->line);
    if (!func_sym)
    {
        semantic_error(sa, func_def->line,
                       "函数 '%s' 重定义", name_node->content);
        // 清理并返回
        if (param_types)
            free(param_types);
        if (param_names)
        {
            for (int i = 0; i < param_count; i++)
            {
                if (param_names[i])
                    free(param_names[i]);
            }
            free(param_names);
        }
        return;
    }
    func_sym->kind = SYM_FUNCTION;

    // 4. 记录当前函数（用于 return 语句检查）
    Type *prev_func_ret_type = sa->current_function_return_type;
    sa->current_function_return_type = return_type;

    // 5. 进入函数作用域
    enter_scope(sa->symbol_table);

    // 6. 插入参数符号到函数作用域
    for (int i = 0; i < param_count; i++)
    {
        Symbol *param_sym = symbol_insert(sa->symbol_table, param_names[i],
                                          param_types[i], func_def->line);
        if (!param_sym)
        {
            semantic_error(sa, func_def->line,
                           "参数 '%s' 重复定义", param_names[i]);
        }
        else
        {
            param_sym->kind = SYM_VARIABLE;
            param_sym->is_initialized = true; // 参数视为已初始化
        }
    }

    // 7. 分析函数体
    if (body)
    {
        analyze_statement(sa, body);
    }

    // 8. 退出函数作用域
    exit_scope(sa->symbol_table);

    // 9. 恢复当前函数返回类型
    sa->current_function_return_type = prev_func_ret_type;

    // 10. 清理临时数组
    if (param_types)
        free(param_types);
    if (param_names)
    {
        for (int i = 0; i < param_count; i++)
        {
            if (param_names[i])
                free(param_names[i]);
        }
        free(param_names);
    }
}

/**
 * 分析 return 语句
 */
void analyze_return_statement(SemanticAnalyzer *sa, Tree *return_stmt)
{
    if (!sa || !return_stmt)
    {
        return;
    }

    // 检查是否在函数内部
    if (!sa->current_function_return_type)
    {
        semantic_error(sa, return_stmt->line,
                       "return 语句只能在函数内部使用");
        return;
    }

    // 检查是否有返回值
    if (return_stmt->num > 0 && return_stmt->leaves[0])
    {
        // 有返回值
        Tree *return_expr = return_stmt->leaves[0];
        Type *return_type = analyze_expression(sa, return_expr);

        if (!return_type)
        {
            semantic_error(sa, return_stmt->line, "无法确定返回表达式的类型");
            return;
        }

        // 检查返回值类型是否匹配
        if (!type_compatible(return_type, sa->current_function_return_type))
        {
            semantic_error(sa, return_stmt->line,
                           "返回类型不匹配：期望 %s，实际 %s",
                           type_to_string(sa->current_function_return_type),
                           type_to_string(return_type));
        }
    }
    else
    {
        // 无返回值（return;）
        if (sa->current_function_return_type->kind != TYPE_VOID)
        {
            semantic_error(sa, return_stmt->line,
                           "函数应返回 %s 类型的值",
                           type_to_string(sa->current_function_return_type));
        }
    }
}

/**
 * 计数实参
 */
static int count_arguments(Tree *arg_list)
{
    if (!arg_list)
    {
        return 0;
    }

    if (strcmp(arg_list->name, "ARG_LIST") == 0)
    {
        return count_arguments(arg_list->leaves[0]) + 1;
    }

    return 1; // 单个参数
}

/**
 * 提取实参类型
 */
static void extract_argument_types(SemanticAnalyzer *sa, Tree *arg_list,
                                   Type **types, int *index)
{
    if (!arg_list)
    {
        return;
    }

    if (strcmp(arg_list->name, "ARG_LIST") == 0)
    {
        // 递归处理参数列表
        extract_argument_types(sa, arg_list->leaves[0], types, index);

        // 分析当前参数
        int current_index = *index;
        Type *arg_type = analyze_expression(sa, arg_list->leaves[1]);

        // 数组参数退化为指针（2.0版本 TASK206）
        if (arg_type && arg_type->kind == TYPE_ARRAY)
        {
            arg_type = new_pointer_type(arg_type->base);
        }

        types[current_index] = arg_type;
        (*index)++;
    }
    else
    {
        // 单个参数
        Type *arg_type = analyze_expression(sa, arg_list);

        // 数组参数退化为指针（2.0版本 TASK206）
        if (arg_type && arg_type->kind == TYPE_ARRAY)
        {
            arg_type = new_pointer_type(arg_type->base);
        }

        types[*index] = arg_type;
        (*index)++;
    }
}

/**
 * 分析函数调用
 */
Type *analyze_function_call(SemanticAnalyzer *sa, Tree *call_node)
{
    if (!sa || !call_node)
    {
        return NULL;
    }

    // call_node 结构: FUNC_CALL(ID, ARG_LIST) 或 FUNC_CALL(ID)
    Tree *func_name_node = call_node->leaves[0];
    Tree *arg_list = (call_node->num > 1) ? call_node->leaves[1] : NULL;

    // 1. 查找函数符号
    Symbol *func_sym = symbol_lookup(sa->symbol_table, func_name_node->content);
    if (!func_sym)
    {
        semantic_error(sa, call_node->line,
                       "未定义的函数 '%s'", func_name_node->content);
        return NULL;
    }

    if (func_sym->kind != SYM_FUNCTION)
    {
        semantic_error(sa, call_node->line,
                       "'%s' 不是函数", func_name_node->content);
        return NULL;
    }

    Type *func_type = func_sym->type;

    // 2. 检查参数数量
    int arg_count = count_arguments(arg_list);
    if (arg_count != func_type->param_count)
    {
        semantic_error(sa, call_node->line,
                       "函数 '%s' 需要 %d 个参数，但提供了 %d 个",
                       func_name_node->content,
                       func_type->param_count,
                       arg_count);
        return func_type->return_type; // 返回期望的类型，继续分析
    }

    // 3. 检查参数类型
    if (arg_list && arg_count > 0)
    {
        Type **arg_types = (Type **)malloc(sizeof(Type *) * arg_count);
        int index = 0;
        extract_argument_types(sa, arg_list, arg_types, &index);

        for (int i = 0; i < arg_count; i++)
        {
            Type *param_type = func_type->param_types[i];
            Type *arg_type = arg_types[i];

            if (arg_type && param_type && !type_compatible(arg_type, param_type))
            {
                semantic_error(sa, call_node->line,
                               "参数 %d 类型不匹配：期望 %s，实际 %s",
                               i + 1,
                               type_to_string(param_type),
                               type_to_string(arg_type));
            }
        }

        free(arg_types);
    }

    // 4. 返回函数返回类型
    return func_type->return_type;
}

/* ==================== 数组分析函数（2.0版本）==================== */

/**
 * 分析数组声明
 *
 * @param sa 语义分析器
 * @param decl 数组声明节点（ARRAY_DECL 或 ARRAY_DECL_INIT）
 */
void analyze_array_declaration(SemanticAnalyzer *sa, Tree *decl)
{
    if (!sa || !decl)
    {
        return;
    }

    // 判断是一维还是二维数组
    bool is_2d = (strcmp(decl->name, "ARRAY_2D_DECL") == 0);

    // ARRAY_DECL 结构: type ID [size] 或 type ID [size] = {init_list}
    // ARRAY_2D_DECL 结构: type ID [rows] [cols]
    // leaves[0]: type 节点
    // leaves[1]: ID 节点
    // leaves[2]: size/rows (INT10 节点)
    // leaves[3]: cols (INT10 节点，仅二维数组) 或 init_list

    Tree *type_node = decl->leaves[0];
    Tree *name_node = decl->leaves[1];
    Tree *size1_node = decl->leaves[2];
    Tree *size2_or_init = (decl->num > 3) ? decl->leaves[3] : NULL;

    // 1. 获取基类型
    Type *base_type = NULL;
    if (type_node && type_node->content)
    {
        if (strcmp(type_node->content, "INT") == 0)
        {
            base_type = new_int_type();
        }
        else if (strcmp(type_node->content, "FLOAT") == 0)
        {
            base_type = new_float_type();
        }
        else if (strcmp(type_node->content, "CHAR") == 0)
        {
            base_type = new_char_type();
        }
    }

    if (!base_type)
    {
        semantic_error(sa, decl->line, "无效的数组基类型");
        return;
    }

    Type *array_type = NULL;
    Tree *init_list = NULL;

    if (is_2d)
    {
        // 二维数组
        int rows = 0, cols = 0;

        if (size1_node && size1_node->content)
        {
            rows = atoi(size1_node->content);
        }
        if (size2_or_init && size2_or_init->content)
        {
            cols = atoi(size2_or_init->content);
        }

        // 验证维度
        if (rows <= 0 || cols <= 0)
        {
            semantic_error(sa, decl->line,
                           "二维数组维度必须为正整数：[%d][%d]", rows, cols);
            return;
        }

        // 创建二维数组类型：先创建列数组，再创建行数组
        Type *col_array = new_array_type(base_type, cols);
        array_type = new_array_type(col_array, rows);
    }
    else
    {
        // 一维数组
        int length = 0;
        if (size1_node && size1_node->content)
        {
            length = atoi(size1_node->content);
        }

        // 验证数组大小
        if (length <= 0)
        {
            semantic_error(sa, decl->line, "数组长度必须为正整数，当前为 %d", length);
            return;
        }

        // 创建数组类型
        array_type = new_array_type(base_type, length);

        // 检查是否有初始化列表
        init_list = size2_or_init;
    }

    if (!array_type)
    {
        semantic_error(sa, decl->line, "创建数组类型失败");
        return;
    }

    // 插入符号表
    char *array_name = name_node->content;
    Symbol *sym = symbol_insert(sa->symbol_table, array_name, array_type, decl->line);
    if (!sym)
    {
        semantic_error(sa, decl->line, "数组 '%s' 重定义", array_name);
        return;
    }

    sym->is_initialized = (init_list != NULL);

    // 如果有初始化列表，检查初始化值
    if (init_list)
    {
        int init_count = count_initializers(init_list);
        int expected_count = is_2d ? 0 : array_type->array_len; // 二维数组初始化更复杂

        if (!is_2d && init_count > expected_count)
        {
            semantic_warning(sa, decl->line,
                             "初始化列表元素过多：数组长度 %d，提供 %d 个初始值",
                             expected_count, init_count);
        }

        // 检查每个初始值的类型
        check_initializer_types(sa, init_list, base_type);
    }
}

/**
 * 计数初始化列表中的元素数量
 */
static int count_initializers(Tree *init_list)
{
    if (!init_list)
    {
        return 0;
    }

    if (strcmp(init_list->name, "INIT_LIST") == 0)
    {
        return count_initializers(init_list->leaves[0]) + 1;
    }

    return 1; // 单个初始值
}

/**
 * 检查初始化列表中值的类型
 */
static void check_initializer_types(SemanticAnalyzer *sa, Tree *init_list, Type *expected_type)
{
    if (!init_list)
    {
        return;
    }

    if (strcmp(init_list->name, "INIT_LIST") == 0)
    {
        // 递归检查列表
        check_initializer_types(sa, init_list->leaves[0], expected_type);
        check_initializer_types(sa, init_list->leaves[1], expected_type);
    }
    else
    {
        // 单个初始值
        Type *init_type = analyze_expression(sa, init_list);
        if (init_type && !type_compatible(expected_type, init_type))
        {
            semantic_warning(sa, init_list->line,
                             "初始化值类型不匹配：期望 %s，实际 %s",
                             type_to_string(expected_type),
                             type_to_string(init_type));
        }
    }
}

/**
 * 分析数组访问表达式
 *
 * @param sa 语义分析器
 * @param access 数组访问节点
 * @return 数组元素的类型
 */
Type *analyze_array_access(SemanticAnalyzer *sa, Tree *access)
{
    if (!sa || !access)
    {
        return NULL;
    }

    // 数组访问由 postfix_expression '[' expr ']' 生成
    // 通过 addDeclator 创建，节点可能名为 "Array" 或包含 declator 字段

    // 提取数组名和下标（具体结构需要查看 addDeclator 的实现）
    // 假设结构: leaves[0] 是数组名, leaves[1] 是下标

    Tree *array_node = access->leaves[0];
    Tree *index_node = access->leaves[1];

    // 1. 检查数组类型
    char *array_name = NULL;
    if (array_node->name && strcmp(array_node->name, "ID") == 0)
    {
        array_name = array_node->content;
    }
    else
    {
        // 可能是复杂表达式
        return NULL;
    }

    Symbol *array_sym = symbol_lookup(sa->symbol_table, array_name);
    if (!array_sym)
    {
        semantic_error(sa, access->line, "未定义的标识符 '%s'", array_name);
        return NULL;
    }

    if (array_sym->type->kind != TYPE_ARRAY)
    {
        semantic_error(sa, access->line, "'%s' 不是数组", array_name);
        return NULL;
    }

    // 2. 检查下标类型
    Type *index_type = analyze_expression(sa, index_node);
    if (index_type && !is_integer_type(index_type))
    {
        semantic_error(sa, access->line, "数组下标必须是整数类型");
        return NULL;
    }

    // 3. 编译期越界检查（如果下标是常量）
    if (index_node && index_node->name &&
        (strcmp(index_node->name, "INT10") == 0 ||
         strcmp(index_node->name, "INT8") == 0 ||
         strcmp(index_node->name, "INT16") == 0))
    {
        int index = atoi(index_node->content);
        if (index < 0 || index >= array_sym->type->array_len)
        {
            semantic_error(sa, access->line,
                           "数组下标越界：索引 %d 超出范围 [0, %d)",
                           index, array_sym->type->array_len);
        }
    }

    // 4. 返回元素类型
    return array_sym->type->base;
}

/* ==================== 指针分析函数（2.0版本）==================== */

/**
 * 分析指针声明
 *
 * @param sa 语义分析器
 * @param decl 指针声明节点（POINTER_DECL 或 POINTER_DECL_INIT）
 */
void analyze_pointer_declaration(SemanticAnalyzer *sa, Tree *decl)
{
    if (!sa || !decl)
    {
        return;
    }

    // POINTER_DECL 结构: type * ID 或 type * ID = expr
    // leaves[0]: type 节点
    // leaves[1]: ID 节点（POINTER_DECL）或 leaves[1]: ID, leaves[2]: init_expr（POINTER_DECL_INIT）

    Tree *type_node = decl->leaves[0];
    Tree *name_node = decl->leaves[1];
    Tree *init_expr = (decl->num > 2) ? decl->leaves[2] : NULL;

    // 1. 获取基类型
    Type *base_type = NULL;
    if (type_node && type_node->content)
    {
        if (strcmp(type_node->content, "INT") == 0)
        {
            base_type = new_int_type();
        }
        else if (strcmp(type_node->content, "FLOAT") == 0)
        {
            base_type = new_float_type();
        }
        else if (strcmp(type_node->content, "CHAR") == 0)
        {
            base_type = new_char_type();
        }
        else if (strcmp(type_node->content, "VOID") == 0)
        {
            base_type = new_void_type();
        }
    }

    if (!base_type)
    {
        semantic_error(sa, decl->line, "无效的指针基类型");
        return;
    }

    // 2. 创建指针类型
    Type *pointer_type = new_pointer_type(base_type);
    if (!pointer_type)
    {
        semantic_error(sa, decl->line, "创建指针类型失败");
        return;
    }

    // 3. 插入符号表
    char *ptr_name = name_node->content;
    Symbol *sym = symbol_insert(sa->symbol_table, ptr_name, pointer_type, decl->line);
    if (!sym)
    {
        semantic_error(sa, decl->line, "指针变量 '%s' 重定义", ptr_name);
        return;
    }

    sym->is_initialized = (init_expr != NULL);

    // 4. 如果有初始化表达式，检查类型
    if (init_expr)
    {
        Type *init_type = analyze_expression(sa, init_expr);
        if (init_type)
        {
            // 允许指针类型或整数类型（整数可以转换为指针）
            if (init_type->kind != TYPE_POINTER && !is_integer_type(init_type))
            {
                semantic_error(sa, decl->line,
                               "指针初始化类型不匹配：期望指针或整数，实际 %s",
                               type_to_string(init_type));
            }
            else if (init_type->kind == TYPE_POINTER)
            {
                // 如果都是指针，检查基类型是否兼容
                if (!type_compatible(init_type->base, base_type))
                {
                    semantic_warning(sa, decl->line,
                                     "指针基类型不匹配：期望 %s*，实际 %s*",
                                     type_to_string(base_type),
                                     type_to_string(init_type->base));
                }
            }
        }
    }
}

/**
 * 分析取地址表达式
 *
 * @param sa 语义分析器
 * @param addr_of 取地址节点
 * @return 指针类型
 */
Type *analyze_addr_of(SemanticAnalyzer *sa, Tree *addr_of)
{
    if (!sa || !addr_of)
    {
        return NULL;
    }

    // ADDR_OF 结构: & expr
    // leaves[0]: 操作数表达式

    Tree *operand = addr_of->leaves[0];
    if (!operand)
    {
        semantic_error(sa, addr_of->line, "取地址运算符缺少操作数");
        return NULL;
    }

    // 1. 检查是否为左值（必须是变量、数组元素、结构体成员等）
    if (!is_lvalue(operand))
    {
        semantic_error(sa, addr_of->line,
                       "取地址运算符的操作数必须是左值（变量、数组元素等）");
        return NULL;
    }

    // 2. 获取操作数类型
    Type *operand_type = analyze_expression(sa, operand);
    if (!operand_type)
    {
        return NULL;
    }

    // 3. 返回指向操作数类型的指针类型
    return new_pointer_type(operand_type);
}

/**
 * 分析解引用表达式
 *
 * @param sa 语义分析器
 * @param deref 解引用节点
 * @return 指针指向的类型
 */
Type *analyze_deref(SemanticAnalyzer *sa, Tree *deref)
{
    if (!sa || !deref)
    {
        return NULL;
    }

    // DEREF 结构: * expr
    // leaves[0]: 操作数表达式（应该是指针）

    Tree *operand = deref->leaves[0];
    if (!operand)
    {
        semantic_error(sa, deref->line, "解引用运算符缺少操作数");
        return NULL;
    }

    // 1. 分析操作数类型
    Type *operand_type = analyze_expression(sa, operand);
    if (!operand_type)
    {
        return NULL;
    }

    // 2. 检查是否为指针类型
    if (operand_type->kind != TYPE_POINTER)
    {
        semantic_error(sa, deref->line,
                       "解引用运算符的操作数必须是指针类型，实际为 %s",
                       type_to_string(operand_type));
        return NULL;
    }

    // 3. 返回指针指向的基类型
    return operand_type->base;
}

/**
 * 检查表达式是否为左值
 *
 * @param expr 表达式节点
 * @return true 如果是左值，false 否则
 */
static bool is_lvalue(Tree *expr)
{
    if (!expr)
    {
        return false;
    }

    // 变量标识符是左值
    if (expr->name && strcmp(expr->name, "ID") == 0)
    {
        return true;
    }

    // 数组访问是左值（通过节点名称判断）
    // 注意：数组访问的识别可能需要根据实际的AST结构进行调整
    // 这里假设数组访问节点有特定的名称模式

    // 解引用表达式是左值：*ptr
    if (expr->name && strcmp(expr->name, "DEREF") == 0)
    {
        return true;
    }

    // 其他表达式（常量、运算结果等）不是左值
    return false;
}

/* ==================== 结构体分析函数（2.0版本）==================== */

/**
 * 分析结构体定义
 *
 * @param sa 语义分析器
 * @param struct_def 结构体定义节点
 */
void analyze_struct_definition(SemanticAnalyzer *sa, Tree *struct_def)
{
    if (!sa || !struct_def)
    {
        return;
    }

    // STRUCT_DEF 结构: struct ID { member_list }
    // leaves[0]: ID 节点（结构体名）
    // leaves[1]: member_list 节点

    Tree *name_node = struct_def->leaves[0];
    Tree *member_list = struct_def->leaves[1];

    if (!name_node || !name_node->content)
    {
        semantic_error(sa, struct_def->line, "结构体名称无效");
        return;
    }

    char *struct_name = name_node->content;

    // 1. 提取成员列表
    StructMember *members = extract_struct_members(sa, member_list);
    if (!members)
    {
        semantic_error(sa, struct_def->line, "结构体 '%s' 没有成员", struct_name);
        return;
    }

    // 2. 创建结构体类型
    Type *struct_type = new_struct_type(struct_name, members);
    if (!struct_type)
    {
        semantic_error(sa, struct_def->line, "创建结构体类型失败");
        return;
    }

    // 3. 将结构体类型插入符号表（作为类型定义）
    // 注意：这里可能需要一个专门的类型定义符号表，暂时使用变量符号表
    Symbol *type_sym = symbol_insert(sa->symbol_table, struct_name, struct_type, struct_def->line);
    if (!type_sym)
    {
        semantic_error(sa, struct_def->line, "结构体 '%s' 重定义", struct_name);
        return;
    }
    type_sym->kind = SYM_TYPEDEF; // 标记为类型定义
}

/**
 * 提取结构体成员列表
 *
 * @param sa 语义分析器
 * @param member_list 成员列表节点
 * @return 结构体成员链表
 */
static StructMember *extract_struct_members(SemanticAnalyzer *sa, Tree *member_list)
{
    if (!member_list)
    {
        return NULL;
    }

    StructMember *head = NULL;
    StructMember *tail = NULL;

    // 递归处理成员列表
    extract_members_recursive(sa, member_list, &head, &tail);

    return head;
}

/**
 * 递归提取结构体成员
 */
static void extract_members_recursive(SemanticAnalyzer *sa, Tree *node,
                                      StructMember **head, StructMember **tail)
{
    if (!node)
    {
        return;
    }

    if (strcmp(node->name, "MEMBER") == 0)
    {
        // 单个成员: type ID
        Tree *type_node = node->leaves[0];
        Tree *name_node = node->leaves[1];

        Type *member_type = get_type_from_specifier(type_node);
        if (!member_type)
        {
            semantic_error(sa, node->line, "无效的成员类型");
            return;
        }

        // 创建成员
        StructMember *member = (StructMember *)malloc(sizeof(StructMember));
        member->name = strdup(name_node->content);
        member->type = member_type;
        member->offset = 0; // 稍后由 new_struct_type 计算
        member->next = NULL;

        // 添加到链表
        if (!*head)
        {
            *head = member;
            *tail = member;
        }
        else
        {
            (*tail)->next = member;
            *tail = member;
        }
    }
    else if (strcmp(node->name, "MEMBER_LIST") == 0)
    {
        // 成员列表：递归处理
        extract_members_recursive(sa, node->leaves[0], head, tail);
        extract_members_recursive(sa, node->leaves[1], head, tail);
    }
}

/**
 * 分析结构体成员访问
 *
 * @param sa 语义分析器
 * @param access 成员访问节点
 * @return 成员类型
 */
Type *analyze_struct_member_access(SemanticAnalyzer *sa, Tree *access)
{
    if (!sa || !access)
    {
        return NULL;
    }

    // MEMBER_ACCESS 结构: expr . ID
    // PTR_MEMBER_ACCESS 结构: expr -> ID
    // leaves[0]: 结构体表达式
    // leaves[1]: 成员名 ID 节点

    Tree *struct_expr = access->leaves[0];
    Tree *member_name_node = access->leaves[1];

    if (!struct_expr || !member_name_node)
    {
        return NULL;
    }

    char *member_name = member_name_node->content;

    // 1. 分析结构体表达式的类型
    Type *struct_type = analyze_expression(sa, struct_expr);
    if (!struct_type)
    {
        return NULL;
    }

    // 2. 检查是否为指针访问（->）
    bool is_pointer_access = (strcmp(access->name, "PTR_MEMBER_ACCESS") == 0);

    if (is_pointer_access)
    {
        // 指针访问：先解引用
        if (struct_type->kind != TYPE_POINTER)
        {
            semantic_error(sa, access->line,
                           "指针成员访问运算符 '->' 的操作数必须是指针类型");
            return NULL;
        }
        struct_type = struct_type->base;
    }

    // 3. 检查是否为结构体类型
    if (struct_type->kind != TYPE_STRUCT)
    {
        semantic_error(sa, access->line,
                       "成员访问运算符的操作数必须是结构体类型，实际为 %s",
                       type_to_string(struct_type));
        return NULL;
    }

    // 4. 查找成员
    StructMember *member = struct_find_member(struct_type, member_name);
    if (!member)
    {
        semantic_error(sa, access->line,
                       "结构体 '%s' 没有成员 '%s'",
                       struct_type->struct_name, member_name);
        return NULL;
    }

    // 5. 返回成员类型
    return member->type;
}

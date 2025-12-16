/**
 * Mini-C 编译器 - IR 构建器实现
 * 
 * 文件: ir_builder.c
 * 描述: 从AST生成中间代码的实现
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 */

#include "ir_builder.h"
#include "../../c-complier-master/tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 1024

/* ==================== IR 构建器创建和销毁 ==================== */

/**
 * 创建IR构建器
 */
IRBuilder* ir_builder_create(SymbolTable *symbol_table) {
    IRBuilder *builder = (IRBuilder*)malloc(sizeof(IRBuilder));
    if (!builder) {
        fprintf(stderr, "错误: IR构建器内存分配失败\n");
        return NULL;
    }
    
    builder->instructions = (IRInstruction**)malloc(sizeof(IRInstruction*) * INITIAL_CAPACITY);
    if (!builder->instructions) {
        fprintf(stderr, "错误: IR指令数组内存分配失败\n");
        free(builder);
        return NULL;
    }
    
    builder->count = 0;
    builder->capacity = INITIAL_CAPACITY;
    builder->temp_count = 0;
    builder->label_count = 0;
    builder->symbol_table = symbol_table;
    
    return builder;
}

/**
 * 销毁IR构建器
 */
void ir_builder_destroy(IRBuilder *builder) {
    if (!builder) {
        return;
    }
    
    // 释放所有IR指令
    for (int i = 0; i < builder->count; i++) {
        if (builder->instructions[i]) {
            ir_instruction_free(builder->instructions[i]);
        }
    }
    
    free(builder->instructions);
    free(builder);
}

/* ==================== 临时变量和标签管理 ==================== */

/**
 * 生成新的临时变量名
 */
char* new_temp(IRBuilder *builder) {
    if (!builder) {
        return NULL;
    }
    
    char *temp = (char*)malloc(20);
    sprintf(temp, "t%d", builder->temp_count++);
    return temp;
}

/**
 * 生成新的标签名
 */
char* new_label(IRBuilder *builder) {
    if (!builder) {
        return NULL;
    }
    
    char *label = (char*)malloc(20);
    sprintf(label, "L%d", builder->label_count++);
    return label;
}

/* ==================== IR 指令发射 ==================== */

/**
 * 添加IR指令
 */
void emit(IRBuilder *builder, IRInstruction *inst) {
    if (!builder || !inst) {
        return;
    }
    
    // 检查容量，必要时扩展
    if (builder->count >= builder->capacity) {
        builder->capacity *= 2;
        builder->instructions = (IRInstruction**)realloc(
            builder->instructions,
            sizeof(IRInstruction*) * builder->capacity
        );
        if (!builder->instructions) {
            fprintf(stderr, "错误: IR指令数组扩展失败\n");
            return;
        }
    }
    
    builder->instructions[builder->count++] = inst;
}

/**
 * 发射二元运算指令
 */
void emit_binary_op(IRBuilder *builder, IROpcode op, const char *arg1, 
                   const char *arg2, const char *result) {
    IRInstruction *inst = ir_instruction_create(op, arg1, arg2, result);
    emit(builder, inst);
}

/**
 * 发射赋值指令
 */
void emit_assign(IRBuilder *builder, const char *source, const char *dest) {
    IRInstruction *inst = ir_instruction_create(IR_ASSIGN, source, NULL, dest);
    emit(builder, inst);
}

/**
 * 发射标签指令
 */
void emit_label(IRBuilder *builder, const char *label) {
    IRInstruction *inst = ir_instruction_create(IR_LABEL, NULL, NULL, label);
    emit(builder, inst);
}

/**
 * 发射跳转指令
 */
void emit_goto(IRBuilder *builder, const char *label) {
    IRInstruction *inst = ir_instruction_create(IR_GOTO, NULL, NULL, label);
    emit(builder, inst);
}

/**
 * 发射条件跳转指令
 */
void emit_if_false(IRBuilder *builder, const char *cond, const char *label) {
    IRInstruction *inst = ir_instruction_create(IR_IF_FALSE, cond, NULL, label);
    emit(builder, inst);
}

/* ==================== AST 到 IR 翻译 ==================== */

/**
 * 翻译表达式
 */
char* translate_expression(IRBuilder *builder, struct Tree *expr, Type *expr_type) {
    if (!builder || !expr) {
        return NULL;
    }
    
    // 函数调用表达式（2.0版本）
    if (expr->name && strcmp(expr->name, "FUNC_CALL") == 0) {
        return translate_function_call(builder, expr, expr_type);
    }
    
    // 数组访问表达式（2.0版本）
    if (expr->declator && strcmp(expr->declator->name, "Array") == 0) {
        return translate_array_access(builder, expr);
    }
    
    // 常量表达式：INT10, INT8, INT16
    if (expr->name && 
        (strcmp(expr->name, "INT10") == 0 ||
         strcmp(expr->name, "INT8") == 0 ||
         strcmp(expr->name, "INT16") == 0)) {
        return expr->content ? strdup(expr->content) : strdup("0");
    }
    
    // 标识符：ID
    if (expr->name && strcmp(expr->name, "ID") == 0) {
        return expr->content ? strdup(expr->content) : NULL;
    }
    
    // 二元运算表达式
    if (expr->name && expr->num >= 3 && expr->leaves) {
        // 判断是否为二元运算
        if (strcmp(expr->name, "additive_expression") == 0 ||
            strcmp(expr->name, "multiplicative_expression") == 0 ||
            strcmp(expr->name, "relational_expression") == 0 ||
            strcmp(expr->name, "equality_expression") == 0) {
            
            // leaves[0]: 左操作数, leaves[1]: 运算符, leaves[2]: 右操作数
            char *left = translate_expression(builder, expr->leaves[0], NULL);
            char *right = translate_expression(builder, expr->leaves[2], NULL);
            
            if (!left || !right) {
                if (left) free(left);
                if (right) free(right);
                return NULL;
            }
            
            // 生成临时变量存储结果
            char *temp = new_temp(builder);
            
            // 确定操作码
            IROpcode op = IR_ADD;  // 默认
            Tree *op_node = expr->leaves[1];
            if (op_node && op_node->content) {
                const char *op_str = op_node->content;
                
                // 算术运算
                if (strcmp(op_str, "+") == 0) op = IR_ADD;
                else if (strcmp(op_str, "-") == 0) op = IR_SUB;
                else if (strcmp(op_str, "*") == 0) op = IR_MUL;
                else if (strcmp(op_str, "/") == 0) op = IR_DIV;
                else if (strcmp(op_str, "%") == 0) op = IR_MOD;
                // 关系运算
                else if (strcmp(op_str, "<") == 0) op = IR_LT;
                else if (strcmp(op_str, ">") == 0) op = IR_GT;
                else if (strcmp(op_str, "<=") == 0) op = IR_LE;
                else if (strcmp(op_str, ">=") == 0) op = IR_GE;
                else if (strcmp(op_str, "==") == 0) op = IR_EQ;
                else if (strcmp(op_str, "!=") == 0) op = IR_NE;
                // 逻辑运算
                else if (strcmp(op_str, "&&") == 0) op = IR_AND;
                else if (strcmp(op_str, "||") == 0) op = IR_OR;
            }
            
            // 根据类型判断是否使用浮点指令
            if (expr_type && expr_type->kind == TYPE_FLOAT) {
                if (op == IR_ADD) op = IR_FADD;
                else if (op == IR_SUB) op = IR_FSUB;
                else if (op == IR_MUL) op = IR_FMUL;
                else if (op == IR_DIV) op = IR_FDIV;
            }
            
            // 发射指令
            emit_binary_op(builder, op, left, right, temp);
            
            free(left);
            free(right);
            
            return temp;
        }
        
        // 赋值表达式
        if (strcmp(expr->name, "assignment_expression") == 0) {
            // leaves[0]: 左值, leaves[1]: =, leaves[2]: 右值
            char *rhs = translate_expression(builder, expr->leaves[2], NULL);
            char *lhs = translate_expression(builder, expr->leaves[0], NULL);
            
            if (rhs && lhs) {
                emit_assign(builder, rhs, lhs);
                free(rhs);
                return lhs;
            }
            
            if (rhs) free(rhs);
            if (lhs) free(lhs);
            return NULL;
        }
    }
    
    // 递归处理子节点
    if (expr->num > 0 && expr->leaves) {
        return translate_expression(builder, expr->leaves[0], expr_type);
    }
    
    return NULL;
}

/**
 * 翻译if语句
 */
static void translate_if(IRBuilder *builder, struct Tree *if_node) {
    if (!builder || !if_node) {
        return;
    }
    
    // if_node通常包含: 条件, then分支, [else分支]
    Tree *cond = if_node->num > 0 ? if_node->leaves[0] : NULL;
    Tree *then_stmt = if_node->num > 1 ? if_node->leaves[1] : NULL;
    Tree *else_stmt = if_node->num > 2 ? if_node->leaves[2] : NULL;
    
    if (!cond) return;
    
    // 翻译条件表达式
    char *cond_var = translate_expression(builder, cond, NULL);
    
    if (else_stmt) {
        // if-else 结构
        char *else_label = new_label(builder);
        char *end_label = new_label(builder);
        
        // if !cond goto else_label
        emit_if_false(builder, cond_var, else_label);
        
        // then 分支
        if (then_stmt) {
            translate_statement(builder, then_stmt);
        }
        emit_goto(builder, end_label);
        
        // else 分支
        emit_label(builder, else_label);
        translate_statement(builder, else_stmt);
        
        // end
        emit_label(builder, end_label);
        
        free(else_label);
        free(end_label);
    } else {
        // if 结构（无else）
        char *end_label = new_label(builder);
        
        // if !cond goto end_label
        emit_if_false(builder, cond_var, end_label);
        
        // then 分支
        if (then_stmt) {
            translate_statement(builder, then_stmt);
        }
        
        // end
        emit_label(builder, end_label);
        
        free(end_label);
    }
    
    if (cond_var) free(cond_var);
}

/**
 * 翻译while语句
 */
static void translate_while(IRBuilder *builder, struct Tree *while_node) {
    if (!builder || !while_node) {
        return;
    }
    
    // while_node包含: 条件, 循环体
    Tree *cond = while_node->num > 0 ? while_node->leaves[0] : NULL;
    Tree *body = while_node->num > 1 ? while_node->leaves[1] : NULL;
    
    if (!cond) return;
    
    char *loop_label = new_label(builder);
    char *end_label = new_label(builder);
    
    // loop:
    emit_label(builder, loop_label);
    
    // 翻译条件
    char *cond_var = translate_expression(builder, cond, NULL);
    
    // if !cond goto end
    emit_if_false(builder, cond_var, end_label);
    
    // 循环体
    if (body) {
        translate_statement(builder, body);
    }
    
    // goto loop
    emit_goto(builder, loop_label);
    
    // end:
    emit_label(builder, end_label);
    
    if (cond_var) free(cond_var);
    free(loop_label);
    free(end_label);
}

/**
 * 翻译语句
 */
void translate_statement(IRBuilder *builder, struct Tree *stmt) {
    if (!builder || !stmt || !stmt->name) {
        return;
    }
    
    // 函数定义（2.0版本）
    if (strcmp(stmt->name, "FUNC_DEF") == 0) {
        translate_function_definition(builder, stmt);
        return;
    }
    
    // return 语句（2.0版本）
    if (strcmp(stmt->name, "return_expression") == 0) {
        translate_return_statement(builder, stmt);
        return;
    }
    
    // 声明语句
    if (strcmp(stmt->name, "declare_expression") == 0) {
        // 如果有初始化，翻译赋值表达式
        if (stmt->num >= 2 && stmt->leaves[1]) {
            translate_expression(builder, stmt->leaves[1], NULL);
        }
        return;
    }
    
    // 赋值表达式
    if (strcmp(stmt->name, "assignment_expression") == 0) {
        translate_expression(builder, stmt, NULL);
        return;
    }
    
    // if语句
    if (strcmp(stmt->name, "if_expression") == 0 ||
        strcmp(stmt->name, "if_else_expression") == 0) {
        translate_if(builder, stmt);
        return;
    }
    
    // while语句
    if (strcmp(stmt->name, "while_expression") == 0) {
        translate_while(builder, stmt);
        return;
    }
    
    // 函数调用（output_int, input_int）
    if (strcmp(stmt->name, "output") == 0 || strcmp(stmt->name, "input") == 0) {
        if (stmt->num >= 2 && stmt->leaves[1]) {
            char *arg = translate_expression(builder, stmt->leaves[1], NULL);
            if (arg) {
                // 发射参数指令
                IRInstruction *param = ir_instruction_create(IR_PARAM, NULL, NULL, arg);
                emit(builder, param);
                
                // 发射调用指令
                const char *func_name = strcmp(stmt->name, "output") == 0 ? "output" : "input";
                IRInstruction *call = ir_instruction_create(IR_CALL, NULL, NULL, (char*)func_name);
                emit(builder, call);
                
                free(arg);
            }
        }
        return;
    }
    
    // 复合语句或句子列表：递归处理所有子节点
    if (strcmp(stmt->name, "sentence") == 0 || strcmp(stmt->name, "statement") == 0) {
        for (int i = 0; i < stmt->num; i++) {
            if (stmt->leaves[i]) {
                translate_statement(builder, stmt->leaves[i]);
            }
        }
        return;
    }
    
    // 其他表达式类型
    if (strstr(stmt->name, "expression") != NULL) {
        translate_expression(builder, stmt, NULL);
        return;
    }
    
    // 递归处理子节点
    for (int i = 0; i < stmt->num; i++) {
        if (stmt->leaves[i]) {
            translate_statement(builder, stmt->leaves[i]);
        }
    }
}

/**
 * 翻译整个程序
 */
void translate_program(IRBuilder *builder, struct Tree *ast) {
    if (!builder || !ast) {
        return;
    }
    
    printf("开始生成中间代码...\n");
    
    // 遍历AST生成IR
    translate_statement(builder, ast);
    
    printf("中间代码生成完成: %d 条指令\n", builder->count);
}

/* ==================== 输出 IR ==================== */

/**
 * 打印IR到标准输出
 */
void ir_builder_print(IRBuilder *builder) {
    if (!builder) {
        return;
    }
    
    printf("\n========== 中间代码（IR）==========\n");
    for (int i = 0; i < builder->count; i++) {
        printf("%3d: ", i + 1);
        ir_instruction_print(builder->instructions[i]);
    }
    printf("==================================\n\n");
}

/**
 * 将IR输出到文件
 */
void ir_builder_output_to_file(IRBuilder *builder, const char *filename) {
    if (!builder || !filename) {
        return;
    }
    
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "错误: 无法打开文件 %s\n", filename);
        return;
    }
    
    // 输出每条指令
    for (int i = 0; i < builder->count; i++) {
        IRInstruction *inst = builder->instructions[i];
        char buffer[256];
        ir_instruction_to_string(inst, buffer, sizeof(buffer));
        fprintf(file, "%d %s\n", i + 1, buffer);
    }
    
    fclose(file);
    printf("IR已输出到文件: %s\n", filename);
}

/**
 * 获取IR指令数量
 */
int ir_builder_count(IRBuilder *builder) {
    return builder ? builder->count : 0;
}

/* ==================== 函数翻译（2.0版本）==================== */

/**
 * 翻译函数定义
 * 
 * @param builder IR构建器
 * @param func_def 函数定义 AST 节点
 */
void translate_function_definition(IRBuilder *builder, Tree *func_def) {
    if (!builder || !func_def) {
        return;
    }
    
    // 提取函数名（func_def->leaves[1] 是函数名节点）
    Tree *name_node = func_def->leaves[1];
    char *func_name = name_node->content;
    
    Tree *body = NULL;
    
    // 根据子节点数量判断是否有参数列表
    if (func_def->num == 5) {
        // 有参数: type name (params) { body }
        body = func_def->leaves[3];  // 跳过 {, 取 sentence
    } else if (func_def->num == 4) {
        // 无参数: type name () { body }
        body = func_def->leaves[2];  // 跳过 {, 取 sentence
    }
    
    // 1. 生成函数开始标记
    IRInstruction *begin = ir_instruction_create(IR_FUNC_BEGIN, func_name, NULL, NULL);
    emit(builder, begin);
    
    // 2. 翻译函数体
    if (body) {
        translate_statement(builder, body);
    }
    
    // 3. 生成函数结束标记
    IRInstruction *end = ir_instruction_create(IR_FUNC_END, func_name, NULL, NULL);
    emit(builder, end);
}

/**
 * 递归计数参数
 */
static int count_call_arguments(Tree *arg_list) {
    if (!arg_list) {
        return 0;
    }
    
    if (strcmp(arg_list->name, "ARG_LIST") == 0) {
        return count_call_arguments(arg_list->leaves[0]) + 1;
    }
    
    return 1;  // 单个参数
}

/**
 * 递归翻译参数列表，生成 PARAM 指令
 * 
 * @param builder IR构建器
 * @param arg_list 参数列表节点
 * @return 参数数量
 */
static int translate_call_arguments(IRBuilder *builder, Tree *arg_list) {
    if (!arg_list) {
        return 0;
    }
    
    int count = 0;
    
    if (strcmp(arg_list->name, "ARG_LIST") == 0) {
        // 递归处理参数列表
        count += translate_call_arguments(builder, arg_list->leaves[0]);
        
        // 处理当前参数（右边的参数）
        char *arg_result = translate_expression(builder, arg_list->leaves[1], NULL);
        if (arg_result) {
            IRInstruction *param = ir_instruction_create(IR_PARAM, arg_result, NULL, NULL);
            emit(builder, param);
            free(arg_result);
            count++;
        }
    } else {
        // 单个参数
        char *arg_result = translate_expression(builder, arg_list, NULL);
        if (arg_result) {
            IRInstruction *param = ir_instruction_create(IR_PARAM, arg_result, NULL, NULL);
            emit(builder, param);
            free(arg_result);
            count = 1;
        }
    }
    
    return count;
}

/**
 * 翻译函数调用表达式
 * 
 * @param builder IR构建器
 * @param call_node 函数调用 AST 节点
 * @param expr_type 表达式类型（可选）
 * @return 存储结果的临时变量名
 */
char* translate_function_call(IRBuilder *builder, Tree *call_node, Type *expr_type) {
    if (!builder || !call_node) {
        return NULL;
    }
    
    // 提取函数名和参数列表
    // call_node->leaves[0] 是函数名 ID 节点
    Tree *func_name_node = call_node->leaves[0];
    char *func_name = func_name_node->content;
    
    Tree *arg_list = (call_node->num > 1) ? call_node->leaves[1] : NULL;
    
    // 1. 翻译参数表达式，为每个参数生成 PARAM 指令
    int arg_count = 0;
    if (arg_list) {
        arg_count = translate_call_arguments(builder, arg_list);
    }
    
    // 2. 生成 CALL 指令
    char *result = new_temp(builder);
    char arg_count_str[32];
    snprintf(arg_count_str, sizeof(arg_count_str), "%d", arg_count);
    
    IRInstruction *call = ir_instruction_create(IR_CALL, func_name, arg_count_str, result);
    emit(builder, call);
    
    return result;
}

/**
 * 翻译 return 语句
 * 
 * @param builder IR构建器
 * @param return_stmt return 语句 AST 节点
 */
void translate_return_statement(IRBuilder *builder, Tree *return_stmt) {
    if (!builder || !return_stmt) {
        return;
    }
    
    // 检查是否有返回值
    // return_expression 可能有子节点（返回值表达式）
    if (return_stmt->num > 0 && return_stmt->leaves[0]) {
        // 有返回值: return expr;
        Tree *return_expr = return_stmt->leaves[0];
        char *result = translate_expression(builder, return_expr, NULL);
        
        if (result) {
            IRInstruction *ret = ir_instruction_create(IR_RETURN, result, NULL, NULL);
            emit(builder, ret);
            free(result);
        }
    } else {
        // 无返回值: return;
        IRInstruction *ret = ir_instruction_create(IR_RETURN, NULL, NULL, NULL);
        emit(builder, ret);
    }
}

/* ==================== 数组翻译（2.0版本）==================== */

/**
 * 翻译数组访问表达式
 * 
 * @param builder IR构建器
 * @param access 数组访问节点
 * @return 存储元素值的临时变量名
 */
char* translate_array_access(IRBuilder *builder, Tree *access) {
    if (!builder || !access) {
        return NULL;
    }
    
    // 数组访问节点结构: leaves[0] 是数组名, leaves[1] 是下标
    Tree *array_node = access->leaves[0];
    Tree *index_node = access->leaves[1];
    
    if (!array_node || !index_node) {
        return NULL;
    }
    
    char *array_name = array_node->content;
    
    // 1. 翻译下标表达式
    char *index_result = translate_expression(builder, index_node, NULL);
    if (!index_result) {
        return NULL;
    }
    
    // 2. 获取数组符号信息（需要知道元素大小）
    Symbol *array_sym = symbol_lookup(builder->symbol_table, array_name);
    if (!array_sym || array_sym->type->kind != TYPE_ARRAY) {
        free(index_result);
        return NULL;
    }
    
    int element_size = array_sym->type->base->size;
    
    // 3. 计算偏移量: offset = index * element_size
    char *offset_temp = new_temp(builder);
    char size_str[32];
    snprintf(size_str, sizeof(size_str), "%d", element_size);
    
    IRInstruction *mul_inst = ir_instruction_create(IR_MUL, index_result, size_str, offset_temp);
    emit(builder, mul_inst);
    
    // 4. 计算元素地址: addr = array_base + offset
    char *addr_temp = new_temp(builder);
    IRInstruction *addr_inst = ir_instruction_create(IR_ARRAY_ADDR, array_name, offset_temp, addr_temp);
    emit(builder, addr_inst);
    
    // 5. 加载元素值: value = *addr
    char *value_temp = new_temp(builder);
    IRInstruction *load_inst = ir_instruction_create(IR_LOAD, addr_temp, NULL, value_temp);
    emit(builder, load_inst);
    
    free(index_result);
    free(offset_temp);
    free(addr_temp);
    
    return value_temp;
}


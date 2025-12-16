/**
 * Mini-C 编译器 - 中间代码（IR）实现
 * 
 * 文件: ir.c
 * 描述: IR指令的具体实现
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 */

#include "ir.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ==================== IR 指令创建 ==================== */

/**
 * 创建IR指令
 */
IRInstruction* ir_instruction_create(IROpcode op, const char *arg1, 
                                    const char *arg2, const char *result) {
    IRInstruction *inst = (IRInstruction*)malloc(sizeof(IRInstruction));
    if (!inst) {
        fprintf(stderr, "错误: IR指令内存分配失败\n");
        return NULL;
    }
    
    inst->op = op;
    inst->line = 0;
    
    // 复制字符串参数（如果不为NULL）
    inst->arg1 = arg1 ? strdup(arg1) : NULL;
    inst->arg2 = arg2 ? strdup(arg2) : NULL;
    inst->result = result ? strdup(result) : NULL;
    
    return inst;
}

/**
 * 释放IR指令
 */
void ir_instruction_free(IRInstruction *inst) {
    if (!inst) {
        return;
    }
    
    if (inst->arg1) free(inst->arg1);
    if (inst->arg2) free(inst->arg2);
    if (inst->result) free(inst->result);
    
    free(inst);
}

/* ==================== IR 指令输出 ==================== */

/**
 * 将操作码转换为字符串
 */
const char* opcode_to_string(IROpcode op) {
    switch (op) {
        case IR_ADD: return "+";
        case IR_SUB: return "-";
        case IR_MUL: return "*";
        case IR_DIV: return "/";
        case IR_MOD: return "%";
        case IR_FADD: return "f+";
        case IR_FSUB: return "f-";
        case IR_FMUL: return "f*";
        case IR_FDIV: return "f/";
        case IR_I2F: return "i2f";
        case IR_F2I: return "f2i";
        case IR_C2I: return "c2i";
        case IR_LT: return "<";
        case IR_GT: return ">";
        case IR_LE: return "<=";
        case IR_GE: return ">=";
        case IR_EQ: return "==";
        case IR_NE: return "!=";
        case IR_AND: return "&&";
        case IR_OR: return "||";
        case IR_NOT: return "!";
        case IR_ASSIGN: return "=";
        case IR_LABEL: return "LABEL";
        case IR_GOTO: return "goto";
        case IR_IF_FALSE: return "if";
        case IR_IF_TRUE: return "if_true";
        case IR_FUNC_BEGIN: return "FUNC_BEGIN";
        case IR_FUNC_END: return "FUNC_END";
        case IR_PARAM: return "arg";
        case IR_CALL: return "call";
        case IR_RETURN: return "return";
        case IR_LOAD: return "load";
        case IR_STORE: return "store";
        case IR_ADDR: return "&";
        default: return "unknown";
    }
}

/**
 * 打印IR指令
 */
void ir_instruction_print(IRInstruction *inst) {
    if (!inst) {
        return;
    }
    
    const char *op_str = opcode_to_string(inst->op);
    
    printf("%s\t", op_str);
    if (inst->arg1) printf("%s\t", inst->arg1);
    else printf("_\t");
    if (inst->arg2) printf("%s\t", inst->arg2);
    else printf("_\t");
    if (inst->result) printf("%s", inst->result);
    else printf("_");
    printf("\n");
}

/**
 * 将IR指令输出为字符串
 */
void ir_instruction_to_string(IRInstruction *inst, char *buffer, int size) {
    if (!inst || !buffer || size <= 0) {
        return;
    }
    
    const char *op_str = opcode_to_string(inst->op);
    
    // 根据指令类型生成不同格式
    switch (inst->op) {
        case IR_LABEL:
            // 标签: "L1:"
            snprintf(buffer, size, "%s:", inst->result ? inst->result : "?");
            break;
            
        case IR_GOTO:
            // 无条件跳转: "goto L1"
            snprintf(buffer, size, "goto %s", inst->result ? inst->result : "?");
            break;
            
        case IR_IF_FALSE:
            // 条件跳转: "if t0 == 0 goto L1"
            snprintf(buffer, size, "if %s == 0 goto %s", 
                    inst->arg1 ? inst->arg1 : "?",
                    inst->result ? inst->result : "?");
            break;
            
        case IR_FUNC_BEGIN:
            // 函数开始: "FUNC_BEGIN func_name"
            snprintf(buffer, size, "FUNC_BEGIN %s", inst->arg1 ? inst->arg1 : "?");
            break;
            
        case IR_FUNC_END:
            // 函数结束: "FUNC_END func_name"
            snprintf(buffer, size, "FUNC_END %s", inst->arg1 ? inst->arg1 : "?");
            break;
            
        case IR_RETURN:
            // 返回: "return t0" 或 "return"
            if (inst->arg1) {
                snprintf(buffer, size, "return %s", inst->arg1);
            } else {
                snprintf(buffer, size, "return");
            }
            break;
            
        case IR_CALL:
            // 函数调用: "t0 = call func"
            snprintf(buffer, size, "%s = call %s", 
                    inst->result ? inst->result : "?",
                    inst->arg1 ? inst->arg1 : "?");
            break;
            
        case IR_PARAM:
            // 参数: "arg t0"
            snprintf(buffer, size, "arg %s", inst->arg1 ? inst->arg1 : "?");
            break;
            
        case IR_NOT:
        case IR_I2F:
        case IR_F2I:
        case IR_C2I:
            // 一元运算: "t0 = ! t1" 或 "t0 = i2f t1"
            snprintf(buffer, size, "%s = %s %s",
                    inst->result ? inst->result : "?",
                    op_str,
                    inst->arg1 ? inst->arg1 : "?");
            break;
            
        default:
            // 二元运算或赋值: "t0 = t1 + t2"
            if (inst->arg2) {
                snprintf(buffer, size, "%s = %s %s %s",
                        inst->result ? inst->result : "?",
                        inst->arg1 ? inst->arg1 : "?",
                        op_str,
                        inst->arg2);
            } else {
                // 赋值: "t0 = t1"
                snprintf(buffer, size, "%s = %s",
                        inst->result ? inst->result : "?",
                        inst->arg1 ? inst->arg1 : "?");
            }
            break;
    }
}

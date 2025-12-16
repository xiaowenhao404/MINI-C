/**
 * Mini-C 编译器 - 中间代码（IR）定义
 * 
 * 文件: ir.h
 * 描述: 四元式中间代码的核心定义
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 */

#ifndef IR_H
#define IR_H

/* ==================== IR 指令操作码 ==================== */

typedef enum {
    /* 整数算术运算 */
    IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_MOD,
    /* 浮点算术运算 */
    IR_FADD, IR_FSUB, IR_FMUL, IR_FDIV,
    /* 类型转换 */
    IR_I2F, IR_F2I, IR_C2I,
    /* 关系运算 */
    IR_LT, IR_GT, IR_LE, IR_GE, IR_EQ, IR_NE,
    /* 逻辑运算 */
    IR_AND, IR_OR, IR_NOT,
    /* 赋值 */
    IR_ASSIGN,
    /* 控制流 */
    IR_LABEL, IR_GOTO, IR_IF_FALSE, IR_IF_TRUE,
    /* 函数调用（2.0版本扩展）*/
    IR_FUNC_BEGIN, IR_FUNC_END,  // 函数边界标记
    IR_PARAM, IR_CALL, IR_RETURN,
    /* 数组和指针（2.0版本扩展）*/
    IR_ARRAY_ADDR,  // 数组元素地址计算: addr = base + index * size
    IR_LOAD,        // 从地址加载: result = *addr
    IR_STORE,       // 存储到地址: *addr = value
    IR_ADDR         // 取地址: result = &var
} IROpcode;

/**
 * 四元式指令结构
 */
typedef struct IRInstruction {
    IROpcode op;
    char *arg1;
    char *arg2;
    char *result;
    int line;
} IRInstruction;

/* 函数声明 */
IRInstruction* ir_instruction_create(IROpcode op, const char *arg1, 
                                    const char *arg2, const char *result);
void ir_instruction_free(IRInstruction *inst);
const char* opcode_to_string(IROpcode op);
void ir_instruction_print(IRInstruction *inst);
void ir_instruction_to_string(IRInstruction *inst, char *buffer, int size);

#endif /* IR_H */

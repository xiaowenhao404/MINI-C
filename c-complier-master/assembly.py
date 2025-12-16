
four = []
file = open('Innercode', 'r')
stmts = file.readlines()
for i in range(len(stmts)):
    stmts[i] = stmts[i].replace('\n', '')

for stmt in stmts:
    ops = stmt.split()
    for i in range(len(ops)):
        try:
            ops[i] = int(ops[i])
        except:
            pass
    if ops[1] == "if":
        four.append(("j==", ops[2], 1, ops[4]))
    elif ops[1] == "goto":
        four.append(('j', '_', '_', ops[2]))
    elif ops[1] == "arg":
        four.append(('arg', '_', '_', ops[2]))
    elif ops[1] == "call":
        four.append(('call', '_', '_', ops[2]))
    elif ops[1] == "return":
        val = ops[2] if len(ops) > 2 else '_'
        four.append(('ret', '_', '_', val))
    elif len(ops)>3 and ops[3] == '!':
        four.append((ops[3], ops[4], '_', ops[1]))
    else:
        if (ops[1][0] == 't'):
            four.append((ops[4], ops[3], ops[5], ops[1]))
        else:
            four.append((ops[2], ops[3], '_', ops[1]))


output_file = open('assembly.asm', 'w')
result = []
t = []
data = [-1] * 100

rule = {
    "j==": ["判断", "JE"],
    "%": ["运算", ""],
    "^": ["运算", ""],
    "=": ["赋值", "mov"],
    "+": ["运算", "add"],
    "-": ["运算", "sub"],
    "*": ["运算", "mul"],
    "/": ["运算", "div"],
    ">=": ["运算", "GE"],
    "<=": ["运算", "LE"],
    ">": ["运算", "GT"],
    "<": ["运算", "LT"],
    "==": ["运算", ""],
    "!=": ["运算", "ne"],
    "||": ["运算", "and"],
    "&&": ["运算", "or"],
    "!": ["运算", "not"],
    "j": ["转移", "jmp"],
    "ret": ["返回", "RET"],
    "arg": ["函数", "arg"],
    "call": ["函数", "call"],
}
# print("error : 翻译错误")

n = 0
for line in four:
    if rule[line[0]][0] == "判断":
        t.append(n)
        t.append(line[3] - 1)
    elif rule[line[0]][0] == "转移":
        t.append(n)
        t.append(line[3] - 1)
    n = n + 1
t = list(set(t))
# t.sort()
four_in = {}
m = 1
for i in t:
    four_in[i] = "CODE" + str(m)
    m += 1
# print(four_in)

tab = "        "
result += [("extern printf, scanf")]
result += [("global main")]
result += [("main:")]
result += [tab + "enter 120, 0"]
inx = "[rbp-"

for i in range(len(four)):
    if i in four_in:
        result += [four_in[i] + ":"]
    if rule[four[i][0]][0] == "判断":
        # result += [tab + "mov eax," + (four[i][1]) + ""]
        if four[i - 1][0] == '>' or four[i - 1][0] == '<' or four[i - 1][0] == '>=' \
        or four[i - 1][0] == '<=' or four[i - 1][0] == '=='or four[i - 1][0] == '!=':
            if isinstance(four[i - 1][1], int):
                result += [tab + "mov eax," + str(four[i - 1][1])]
            else:
                result += [tab + "mov eax," + inx + str(4 * data.index(four[i - 1][1]) + 4) + "]"]
            if isinstance(four[i - 1][2], int):
                result += [tab + "mov ebx," + str(four[i - 1][2])]
            else:
                result += [tab + "mov ebx," + inx + str(4 * data.index(four[i - 1][2]) + 4) + "]"]
            result += [tab + "cmp eax,ebx"]
            if four[i - 1][0] == '>':
                result += [tab + "" + "ja" + " " + four_in[four[i][3] - 1]]
            elif four[i - 1][0] == '<':
                result += [tab + "" + "jb" + " " + four_in[four[i][3] - 1]]
            elif four[i - 1][0] == '>=':
                result += [tab + "" + "jae" + " " + four_in[four[i][3] - 1]]
            elif four[i - 1][0] == '<=':
                result += [tab + "" + "jbe" + " " + four_in[four[i][3] - 1]]
            elif four[i - 1][0] == '==':
                result += [tab + "" + "je" + " " + four_in[four[i][3] - 1]]
            elif four[i - 1][0] == '!=':
                result += [tab + "" + "jne" + " " + four_in[four[i][3] - 1]]
        else:
            if isinstance(four[i][2], int):
                result += [tab + "mov eax," + str(four[i][2])]
            else:
                result += [tab + "mov eax," + inx + str(4 * data.index(four[i][2]) + 4) + "]"]
            result += [tab + "cmp eax,1"]
            result += [tab + "" + "je" + " " + four_in[four[i][3] - 1]]
        result += [""]
    elif rule[four[i][0]][0] == "赋值":
        if isinstance(four[i][1], int) or isinstance(four[i][1], float):
            data[data.index(-1)] = four[i][3]
            result += [tab + "mov eax," + str(four[i][1])]
            result += [tab + "mov " + inx + str(4 * data.index(four[i][3]) + 4) + "],eax"]
        else:
            data[data.index(-1)] = four[i][3]             
            result += [tab + "mov eax," + inx + str(4 * data.index(four[i][1]) + 4) + "]"]
            result += [tab + "mov " + inx + str(4 * data.index(four[i][3]) + 4) + "],eax"]
    elif rule[four[i][0]][0] == "运算":
        if isinstance(four[i][1], int) or isinstance(four[i][1], float):
            result += [tab + "mov eax," + str(four[i][1])]
        elif four[i][1] != '_':
            result += [tab + "mov eax," + inx + str(4 * data.index(four[i][1]) + 4) + "]"]
        if isinstance(four[i][2], int) or isinstance(four[i][2], float):
            result += [tab + "mov ebx," + str(four[i][2])]
        elif four[i][2] != '_':
            result += [tab + "mov ebx," + inx + str(4 * data.index(four[i][2]) + 4) + "]"]
        if four[i][0] == "+" or four[i][0] == "-":
            result += [tab + rule[four[i][0]][1] + " eax,ebx"]
        elif four[i][0] == "*":
            result += [tab + "mul ebx"]
        elif four[i][0] == "/":
            result += [tab + "div ebx"]
        elif four[i][0] == "%":
            result += [tab + "div ebx"]
            result += [tab + "mov eax,edx"]
        elif four[i][0] == "^":
            result += [tab + "times " + str(four[i][2]) + " mul eax"]
        elif four[i][0] == ">" or four[i][0] == "<" or four[i][0] == ">=" or \
            four[i][0] == "<=" or four[i][0] == "==" or four[i][0] == "!=":
            pass
        elif four[i][0] == "&&" or four[i][0] == "||":
            result += [tab + rule[four[i][0]][1] + " eax,ebx"]
        elif four[i][0] == "!":
            result += [tab + rule[four[i][0]][1] + " eax"]
        else:
            result += ["----运算符未定义---"]
        data[data.index(-1)] = four[i][3]
        # result += [tab + "mov eax," + (four[i][3]) + ""]
        result += [tab + "mov " + inx + str(4 * data.index(four[i][3]) + 4) + "],eax"]

    elif rule[four[i][0]][0] == "转移":
        result += [tab + "jmp " + str(four_in[four[i][3] - 1])]
        result += [""]

    elif rule[four[i][0]][0] == "返回":
        result += [tab + "leave"]
        result += [tab + "ret"]

    elif rule[four[i][0]][0] == "函数":
        if four[i][0] == "arg":
            result += [tab + "sub rsp, 8"]
            result += [tab + "mov rsi," + inx + str(4 * data.index(four[i][3]) + 4) + "]"]
        if four[i][0] == "call" and four[i][3] == "output":
            result += [tab + "mov rdi, out_format"]
            result += [tab + "xor rax, rax"]
            result += [tab + "call printf"]
            result += [tab + "xor rax, rax"]
            result += [tab + "add rsp, 8"]
        elif four[i][0] == "call" and four[i][3] == "input":
            result += [tab + 'mov rsi, number']
            result += [tab + 'mov rdi, in_format']
            result += [tab + "xor rax, rax"]
            result += [tab + 'call scanf']
            result += [tab + 'mov rbx, [number]']
            result += [tab + 'add rsp, 8']

# result += ["CODE" + str(m-1) + ":"]
# result += [tab + "leave"]
# result += [tab + "ret"]
result += [""]
result += ["section .data"]
result += [tab + 'out_format: db "%#d", 10, 0']
result += [tab + 'in_format: db "%d", 0']

result += ["section .bss"]
result += [tab + 'number resb 4']
for line in result:
    output_file.write(f'{line}\n')

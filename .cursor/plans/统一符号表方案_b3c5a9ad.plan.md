---
name: 统一符号表方案
overview: 将 yacc.y 的 hashMap 符号信息导入到语义分析器的 SymbolTable，消除两套符号表不同步导致的假错误。
todos:
  - id: add-import-func
    content: 在 semantic_analyzer.c 中添加 import_symbols_from_hashmap 函数
    status: completed
  - id: type-mapping
    content: 实现 hashMap type 到 Type* 的映射逻辑
    status: completed
    dependencies:
      - add-import-func
  - id: handle-declator
    content: 处理数组和指针的 Declator 转换
    status: completed
    dependencies:
      - type-mapping
  - id: modify-yacc
    content: 修改 yacc.y 调用符号导入函数
    status: completed
    dependencies:
      - add-import-func
  - id: test-verify
    content: 测试验证假错误被消除
    status: completed
    dependencies:
      - modify-yacc
---

# 统一符号表方案

## 问题根源

当前存在两套独立的符号表：

1. **hashMap** ([src/utils/hashMap.c](src/utils/hashMap.c)) - 语法分析阶段使用，通过 `putTree()` 添加变量
2. **SymbolTable** ([src/semantic/symbol_table.c](src/semantic/symbol_table.c)) - 语义分析阶段使用，独立维护

两套符号表不同步，导致语义分析器报告"未定义变量"等假错误。

## 解决方案

在语义分析开始前，从 hashMap 导入所有符号到 SymbolTable。

```mermaid
flowchart LR
    A[语法分析] --> B[hashMap填充]
    B --> C[导入符号到SymbolTable]
    C --> D[语义分析]
    D --> E[错误检查基于统一符号表]
```



## 实现步骤

### 1. 添加符号导入函数

在 [src/semantic/semantic_analyzer.c](src/semantic/semantic_analyzer.c) 中添加：

```c
// 从 hashMap 导入符号到语义分析器的符号表
void import_symbols_from_hashmap(SemanticAnalyzer *sa, HashMap *hashMap);
```



### 2. 类型映射

hashMap 中的 `type` 是整数，需要映射到 Type* 结构：

- `1` (INT 从 yacc.y) -> `new_int_type()`
- `2` (FLOAT) -> `new_float_type()`
- `3` (CHAR) -> `new_char_type()`

### 3. 修改 yacc.y

在 `analyze_program()` 调用前，传递 hashMap：

```c
import_symbols_from_hashmap(analyzer, hashMap);
bool semantic_success = analyze_program(analyzer, root);
```



### 4. 处理数组和指针

hashMap 中的 `Declator` 链表记录了数组/指针信息，需要转换为 Type 系统的数组/指针类型。

## 涉及文件

| 文件 | 修改内容 |

|------|---------|

| [src/semantic/semantic_analyzer.h](src/semantic/semantic_analyzer.h) | 添加 `import_symbols_from_hashmap` 声明 |

| [src/semantic/semantic_analyzer.c](src/semantic/semantic_analyzer.c) | 实现符号导入函数 |

| [src/frontend/yacc.y](src/frontend/yacc.y) | 调用符号导入函数 |

## 预期效果

- 语义分析器能识别所有在语法分析阶段声明的变量
- 消除"变量未定义"等假错误
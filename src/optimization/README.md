# 活性分析优化模块

## 概述

本模块实现了 Mini-C 编译器 2.0 版本的核心优化功能：基于活性分析的栈槽复用优化。

## 功能特性

### TASK209: 控制流图(CFG)构建

- **基本块划分**: 自动识别基本块入口和出口
- **CFG构建**: 建立基本块之间的前驱后继关系
- **标签映射**: 支持跳转目标的快速查找

### TASK210: 活性分析算法

- **USE/DEF集合计算**: 自动分析每个基本块使用的变量和定义的变量
- **数据流方程求解**: 迭代求解活性分析方程
- **LIVE_IN/LIVE_OUT计算**: 计算每个基本块入口和出口的活跃变量集合

### TASK211: 栈槽复用优化

- **干涉图构建**: 根据活跃变量集合构建变量干涉图
- **图着色算法**: 使用贪心着色算法分配栈槽
- **栈空间优化**: 减少栈空间使用，提高代码效率

## 使用方法

### 1. 构建控制流图

```c
#include "optimization/cfg.h"

// 假设已有 IR 指令数组
IRInstruction **instructions = ...;
int count = ...;

// 构建 CFG
CFG *cfg = build_cfg(instructions, count);

// 打印 CFG（调试用）
cfg_print(cfg);
```

### 2. 执行活性分析

```c
#include "optimization/liveness.h"

// 执行活性分析
bool success = perform_liveness_analysis(cfg);

if (success) {
    // 打印活性分析结果
    print_liveness_analysis(cfg);
}
```

### 3. 栈槽分配

```c
#include "optimization/stack_alloc.h"

// 构建干涉图
InterferenceGraph *ig = build_interference_graph(cfg);

// 分配栈槽（假设最多使用 16 个栈槽）
int slots_used = allocate_stack_slots(ig, 16);

// 打印分配结果
print_stack_allocation(ig);

// 获取变量的栈槽编号
int slot = get_variable_slot(ig, "var_name");
```

## 数据结构

### BasicBlock（基本块）

- `id`: 基本块编号
- `instructions`: 指令列表
- `predecessors`: 前驱基本块
- `successors`: 后继基本块
- `live_in`: 入口活跃变量集合
- `live_out`: 出口活跃变量集合
- `use`: 使用的变量集合
- `def`: 定义的变量集合

### CFG（控制流图）

- `blocks`: 基本块数组
- `entry`: 入口基本块
- `exit`: 出口基本块

### InterferenceGraph（干涉图）

- `nodes`: 变量节点数组
- 每个节点包含邻居信息（干涉关系）

## 算法说明

### 活性分析数据流方程

```
LIVE_OUT[B] = ∪ LIVE_IN[S]  (S是B的后继)
LIVE_IN[B] = USE[B] ∪ (LIVE_OUT[B] - DEF[B])
```

### 栈槽分配规则

- 如果两个变量在某个基本块的 `LIVE_OUT` 中同时存在，则它们**干涉**
- 干涉的变量不能共享同一个栈槽
- 使用贪心着色算法为变量分配栈槽

## 优化效果

通过活性分析和栈槽复用，可以显著减少栈空间使用：

- **示例1**: 如果变量 `a` 和 `b` 的活跃范围不重叠，它们可以共享同一个栈槽
- **示例2**: 循环中的临时变量可以在每次迭代后复用栈槽

## 测试

运行测试程序：

```bash
cd tests/optimization
# 编译测试程序（需要先编译编译器）
./compiler test_liveness.c
```

## 注意事项

1. **溢出处理**: 如果变量数量超过可用栈槽数，部分变量会被标记为"溢出"，需要特殊处理
2. **迭代收敛**: 活性分析使用迭代算法，最多迭代 100 次
3. **性能**: CFG 构建和活性分析的复杂度为 O(n²)，对于大型程序可能需要优化

## 相关文件

- `cfg.h/c`: 控制流图构建
- `liveness.h/c`: 活性分析算法
- `stack_alloc.h/c`: 栈槽分配优化

---

**版本**: 2.0  
**作者**: Mini-C 开发团队


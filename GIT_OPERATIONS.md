# Git 操作说明

## 当前待提交的文件

由于 PowerShell 中文路径编码问题，请在 **Git Bash 终端**中执行以下命令：

### 方法 1：在 Git Bash 中执行

```bash
cd ~/Desktop/资料/绩点/大三上资料/编译原理课设/Mini-C

# 添加计划文件
git add .cursor/plans/mini-c_2.0_详细开发计划_f4ccd52d.plan.md

# 查看状态
git status --short

# 提交
git commit -m "docs: add detailed 2.0 development plan file

- Add .cursor/plans/mini-c_2.0_详细开发计划_f4ccd52d.plan.md
- Contains detailed task breakdown for v2.0 features
- Includes AI assistant prompts for each subtask"
```

### 方法 2：使用项目提供的脚本

或者使用项目根目录下的 scripts/git_commit.ps1：

```powershell
cd C:\Users\36774\Desktop\资料\绩点\大三上资料\编译原理课设\Mini-C
git add .cursor/plans/mini-c_2.0_详细开发计划_f4ccd52d.plan.md
git add GIT_OPERATIONS.md
.\scripts\git_commit.ps1 -Title "docs: add 2.0 plan file and git operations guide"
```

## 已完成的提交

✅ 提交 1: `docs: add DEV_PLAN_2.0.md for Mini-C version 2.0` (commit: 263b7af)
✅ 提交 2: `feat: implement function type support for TASK205.1` (commit: d04019a)

## 需要提交的文件

- `.cursor/plans/mini-c_2.0_详细开发计划_f4ccd52d.plan.md` - 详细开发计划（plan 格式）
- `GIT_OPERATIONS.md` - 本说明文件

---

**说明**: 由于 Windows PowerShell 对包含中文的长路径支持不佳，建议使用 Git Bash 或 WSL 进行 Git 操作。


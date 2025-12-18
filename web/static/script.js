/**
 * Mini-C 编译器 Web 界面 - 前端交互逻辑
 * 作者：Mini-C 开发团队
 * 版本：3.0
 */

// ==================== 全局状态 ====================

let editor;  // CodeMirror 编辑器实例
let currentResults = {};  // 存储当前编译结果

// ==================== 初始化 ====================

document.addEventListener('DOMContentLoaded', () => {
    console.log('Mini-C IDE 初始化中...');
    
    // 1. 初始化 CodeMirror 编辑器
    initCodeMirror();
    
    // 2. 绑定编译按钮事件
    const compileBtn = document.getElementById('compile-btn');
    if (compileBtn) {
        compileBtn.addEventListener('click', runCompile);
    }
    
    // 3. 绑定快捷键 (Ctrl+Enter 或 Cmd+Enter 编译)
    document.addEventListener('keydown', (e) => {
        if ((e.ctrlKey || e.metaKey) && e.key === 'Enter') {
            e.preventDefault();
            runCompile();
        }
    });
    
    console.log('✓ Mini-C IDE 初始化完成');
});

/**
 * 初始化 CodeMirror 编辑器
 */
function initCodeMirror() {
    const editorWrapper = document.getElementById('editor-wrapper');
    
    if (!editorWrapper) {
        console.error('错误: 找不到编辑器容器 #editor-wrapper');
        return;
    }
    
    // 创建 CodeMirror 实例
    editor = CodeMirror(editorWrapper, {
        mode: 'text/x-csrc',  // C 语言模式
        theme: 'nord',         // Nord 主题（深色舒适）
        lineNumbers: true,     // 显示行号
        indentUnit: 4,         // 缩进单位：4个空格
        tabSize: 4,            // Tab 大小
        smartIndent: true,     // 智能缩进
        lineWrapping: false,   // 不自动换行
        autofocus: true,       // 自动聚焦
        matchBrackets: true,   // 括号匹配
        
        // 默认代码（示例）
        value: `// Mini-C 编译器示例程序
// 
// 功能：演示基础功能、浮点运算和优化
// 提示：点击右上角 "编译与运行" 按钮

void main() {
    // 测试1：基本变量和算术运算
    int a = 10;
    int b = 20;
    int c = a + b;
    output_int(c);  // 输出: 30
    
    // 测试2：常量折叠优化
    int result = 3 + 4 * 5;  // 编译期优化为 23
    output_int(result);  // 输出: 23
    
    // 测试3：浮点运算和类型转换
    float x = 3.14;
    float y = 2.0;
    float sum = x + y;
    output_float(sum);  // 输出: 5.14
    
    // 测试4：控制流和死代码消除
    if (1) {
        output_int(100);  // 保留
    } else {
        output_int(0);    // 死代码，会被优化删除
    }
    
    // 测试5：循环
    int i = 0;
    while (i < 3) {
        output_int(i);
        i = i + 1;
    }
}`
    });
    
    console.log('✓ CodeMirror 编辑器已初始化');
}

// ==================== 编译功能 ====================

/**
 * 运行编译
 */
async function runCompile() {
    const btn = document.getElementById('compile-btn');
    const consoleDiv = document.getElementById('console-output');
    const loadingOverlay = document.getElementById('loading-overlay');
    
    // 显示加载状态
    btn.disabled = true;
    btn.innerHTML = '<span class="icon">⌛</span> 编译中...';
    consoleDiv.textContent = "编译启动...\n";
    if (loadingOverlay) {
        loadingOverlay.style.display = 'flex';
    }
    
    try {
        // 获取编辑器代码
        const code = editor.getValue();
        
        if (!code.trim()) {
            throw new Error('代码为空，请输入C代码后再编译。');
        }
        
        // 发送编译请求
        const response = await fetch('/compile', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ code: code })
        });
        
        if (!response.ok) {
            throw new Error(`HTTP错误: ${response.status} ${response.statusText}`);
        }
        
        const data = await response.json();
        
        // 更新全局结果数据
        if (data.results) {
            currentResults = data.results;
        }
        
        // 更新控制台输出
        if (data.console) {
            consoleDiv.textContent = data.console;
        } else {
            consoleDiv.textContent = "编译完成，但无输出信息。";
        }
        
        // 自动切换到第一个有内容的标签页
        autoSwitchToFirstAvailableTab();
        
        // 刷新当前显示的内容
        const activeTab = document.querySelector('.tab-btn.active');
        if (activeTab) {
            const key = activeTab.getAttribute('onclick').match(/'([^']+)'/)[1];
            updateDisplay(key);
        }
        
        // 显示成功提示
        if (data.success) {
            console.log('✓ 编译成功');
        }
        
    } catch (error) {
        console.error('编译出错:', error);
        consoleDiv.textContent += `\n\n前端错误: ${error.message}\n\n`;
        consoleDiv.textContent += '请检查：\n';
        consoleDiv.textContent += '1. 编译器是否已编译（在项目根目录执行 make）\n';
        consoleDiv.textContent += '2. Flask 服务器是否正常运行\n';
        consoleDiv.textContent += '3. 网络连接是否正常\n';
    } finally {
        // 恢复按钮状态
        btn.disabled = false;
        btn.innerHTML = '<span class="icon">▶</span> 编译与运行';
        
        // 隐藏加载动画
        if (loadingOverlay) {
            loadingOverlay.style.display = 'none';
        }
    }
}

/**
 * 自动切换到第一个有内容的标签页
 */
function autoSwitchToFirstAvailableTab() {
    const tabKeys = ['tokens', 'ast', 'ir', 'asm'];
    
    for (const key of tabKeys) {
        if (currentResults[key] && 
            !currentResults[key].includes('[系统提示]') &&
            !currentResults[key].includes('[错误]')) {
            // 找到第一个有效内容的标签
            switchTabProgrammatically(key);
            break;
        }
    }
}

// ==================== 标签页切换 ====================

/**
 * 切换标签页（由按钮点击触发）
 * 
 * @param {string} key - 标签页键名（tokens/ast/ir/asm）
 */
function switchTab(key) {
    // 1. 更新按钮样式
    document.querySelectorAll('.tab-btn').forEach(btn => {
        btn.classList.remove('active');
    });
    
    // 找到被点击的按钮并标记为active
    event.target.closest('.tab-btn').classList.add('active');
    
    // 2. 更新内容显示
    updateDisplay(key);
}

/**
 * 程序化切换标签页（不依赖event）
 * 
 * @param {string} key - 标签页键名
 */
function switchTabProgrammatically(key) {
    // 1. 更新按钮样式
    document.querySelectorAll('.tab-btn').forEach(btn => {
        btn.classList.remove('active');
        
        // 检查按钮的onclick属性
        const onclick = btn.getAttribute('onclick');
        if (onclick && onclick.includes(`'${key}'`)) {
            btn.classList.add('active');
        }
    });
    
    // 2. 更新内容显示
    updateDisplay(key);
}

/**
 * 更新内容显示区域
 * 
 * @param {string} key - 要显示的内容键名
 */
function updateDisplay(key) {
    const display = document.getElementById('content-display');
    
    if (!display) {
        console.error('错误: 找不到内容显示区域 #content-display');
        return;
    }
    
    const content = currentResults[key];
    
    if (content) {
        // 显示内容
        display.textContent = content;
        
        // 添加标题提示
        const titles = {
            'tokens': '词法分析结果 (Token 流)',
            'ast': '语法分析结果 (抽象语法树)',
            'ir': '中间代码 (四元式IR)',
            'asm': '汇编代码 (x86-64 NASM)'
        };
        
        if (!content.includes('[系统提示]') && !content.includes('[错误]')) {
            display.textContent = `// ${titles[key]}\n// 文件: ${FILES_MAP_DISPLAY[key]}\n\n${content}`;
        }
    } else {
        // 无内容
        display.textContent = `// 暂无数据\n// \n// 请先点击 "编译与运行" 按钮生成 ${key.toUpperCase()} 结果。`;
    }
}

// 文件名映射（用于显示）
const FILES_MAP_DISPLAY = {
    'tokens': 'Lexical',
    'ast': 'Grammatical',
    'ir': 'Innercode',
    'asm': 'assembly.asm'
};

// ==================== 工具函数 ====================

/**
 * 格式化代码行数
 */
function countLines(text) {
    return text ? text.split('\n').length : 0;
}

/**
 * 显示提示消息
 */
function showToast(message, type = 'info') {
    console.log(`[${type.toUpperCase()}] ${message}`);
    // TODO: 可以添加 toast 通知组件
}

// ==================== 调试信息 ====================

console.log('Mini-C IDE 前端脚本已加载');
console.log('快捷键：Ctrl+Enter 或 Cmd+Enter 快速编译');


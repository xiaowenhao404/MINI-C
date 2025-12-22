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
    
    // 4. 初始化可拖动分隔条
    initResizeHandles();
    
    // 5. 初始化独立缩放功能
    initIndependentZoom();
    
    console.log('✓ Mini-C IDE 初始化完成');
});

// ==================== 可拖动分隔条功能 ====================

/**
 * 初始化可拖动分隔条
 */
function initResizeHandles() {
    // 水平分隔条（调整左右面板宽度）
    const hHandle = document.getElementById('resize-handle-h');
    const editorPane = document.getElementById('editor-pane');
    const resultPane = document.getElementById('result-pane');
    
    if (hHandle && editorPane && resultPane) {
        let isResizingH = false;
        
        hHandle.addEventListener('mousedown', (e) => {
            isResizingH = true;
            hHandle.classList.add('active');
            document.body.style.cursor = 'col-resize';
            document.body.style.userSelect = 'none';
            e.preventDefault();
        });
        
        document.addEventListener('mousemove', (e) => {
            if (!isResizingH) return;
            
            const container = editorPane.parentElement;
            const containerRect = container.getBoundingClientRect();
            const newEditorWidth = e.clientX - containerRect.left;
            const minWidth = 200;
            const maxWidth = containerRect.width - minWidth - 6;
            
            if (newEditorWidth >= minWidth && newEditorWidth <= maxWidth) {
                editorPane.style.flex = 'none';
                editorPane.style.width = newEditorWidth + 'px';
                resultPane.style.flex = '1';
                
                // 刷新 CodeMirror 编辑器
                if (editor) editor.refresh();
            }
        });
        
        document.addEventListener('mouseup', () => {
            if (isResizingH) {
                isResizingH = false;
                hHandle.classList.remove('active');
                document.body.style.cursor = '';
                document.body.style.userSelect = '';
            }
        });
    }
    
    // 垂直分隔条（调整控制台高度）
    const vHandle = document.getElementById('resize-handle-v');
    const consolePane = document.getElementById('console-pane');
    const mainSplit = document.querySelector('.main-split');
    
    if (vHandle && consolePane && mainSplit) {
        let isResizingV = false;
        
        vHandle.addEventListener('mousedown', (e) => {
            isResizingV = true;
            vHandle.classList.add('active');
            document.body.style.cursor = 'row-resize';
            document.body.style.userSelect = 'none';
            e.preventDefault();
        });
        
        document.addEventListener('mousemove', (e) => {
            if (!isResizingV) return;
            
            const appContainer = document.querySelector('.app-container');
            const containerRect = appContainer.getBoundingClientRect();
            const headerHeight = 50; // header height
            const newConsoleHeight = containerRect.bottom - e.clientY;
            const minHeight = 100;
            const maxHeight = containerRect.height * 0.5;
            
            if (newConsoleHeight >= minHeight && newConsoleHeight <= maxHeight) {
                consolePane.style.height = newConsoleHeight + 'px';
                
                // 刷新 CodeMirror 编辑器
                if (editor) editor.refresh();
            }
        });
        
        document.addEventListener('mouseup', () => {
            if (isResizingV) {
                isResizingV = false;
                vHandle.classList.remove('active');
                document.body.style.cursor = '';
                document.body.style.userSelect = '';
            }
        });
    }
    
    console.log('✓ 可拖动分隔条已初始化');
}

// ==================== 独立缩放功能 ====================

/**
 * 初始化独立缩放功能
 * Ctrl + 鼠标滚轮可以单独调整各子窗口的字体大小
 */
function initIndependentZoom() {
    // 定义可缩放的区域
    const zoomableAreas = [
        { selector: '#editor-wrapper', minSize: 10, maxSize: 24, currentSize: 14 },
        { selector: '#content-display', minSize: 10, maxSize: 24, currentSize: 13 },
        { selector: '#console-output', minSize: 10, maxSize: 20, currentSize: 12 }
    ];
    
    zoomableAreas.forEach(area => {
        const element = document.querySelector(area.selector);
        if (!element) return;
        
        element.addEventListener('wheel', (e) => {
            // 只有按住 Ctrl 键时才触发缩放
            if (!e.ctrlKey) return;
            
            e.preventDefault();
            e.stopPropagation();
            
            // 根据滚轮方向调整字体大小
            if (e.deltaY < 0) {
                // 向上滚动，放大
                area.currentSize = Math.min(area.currentSize + 1, area.maxSize);
            } else {
                // 向下滚动，缩小
                area.currentSize = Math.max(area.currentSize - 1, area.minSize);
            }
            
            // 应用新的字体大小
            if (area.selector === '#editor-wrapper') {
                // CodeMirror 编辑器需要特殊处理
                const cmElement = element.querySelector('.CodeMirror');
                if (cmElement) {
                    cmElement.style.fontSize = area.currentSize + 'px';
                    if (editor) editor.refresh();
                }
            } else {
                element.style.fontSize = area.currentSize + 'px';
            }
            
            console.log(`${area.selector} 字体大小: ${area.currentSize}px`);
        }, { passive: false });
    });
    
    console.log('✓ 独立缩放功能已初始化 (Ctrl + 滚轮)');
}

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
        value: `// ============================================
// Mini-C 编译器 v3.0 综合功能测试
// ============================================

// 自定义函数：加法
int add(int a, int b) {
    return a + b;
}

void main() {
    // ===== 1. 整数运算 =====
    int a = 10;
    int b = 20;
    int c = a + b;
    output_int(c);  // 30
    
    // ===== 2. 实型数据（浮点运算）=====
    float x = 3.14;
    float y = 2.0;
    float sum = x + y;
    output_float(sum);  // 5.14
    
    // ===== 3. 常量折叠优化 =====
    int result = 3 + 4 * 5;
    output_int(result);  // 23
    
    // ===== 4. 函数调用 =====
    int sum2 = add(15, 25);
    output_int(sum2);  // 40
    
    // ===== 5. if-else 控制流 =====
    if (1) {
        output_int(100);  // 100
    } else {
        output_int(0);
    }
    
    // ===== 6. while 循环 =====
    int i = 0;
    while (i < 3) {
        output_int(i);  // 0, 1, 2
        i = i + 1;
    }
    
    // ===== 7. 浮点乘法 =====
    float pi = 3.14;
    float doubled = pi * 2.0;
    output_float(doubled);  // 6.28
    
    // ===== 8. 数组和指针声明 =====
    int arr[5];
    int* ptr;
}

// 预期输出：30, 5.14, 23, 40, 100, 0, 1, 2, 6.28
// 
// 编译器支持的语法特性：
// ✓ 实型数据 float
// ✓ 函数定义与调用
// ✓ 一维/二维数组声明
// ✓ 指针声明
// ✓ 结构体定义
// ✓ if-else 控制流
// ✓ while 循环
// ✓ 常量折叠优化`
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
        
        // 自动滚动到底部（优先展示运行结果）
        consoleDiv.scrollTop = consoleDiv.scrollHeight;
        
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


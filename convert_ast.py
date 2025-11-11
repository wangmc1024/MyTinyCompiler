def parse_ast(ast_str):
    """解析AST字符串为嵌套的字典结构，支持(节点名) ((子节点1)) ((子节点2)) ...格式"""
    # 移除所有空格和换行符，便于处理嵌套结构
    ast_str = ast_str.replace(' ', '').replace('\n', '').replace('\r', '')
    
    if not ast_str or ast_str[0] != '(' or ast_str[-1] != ')':
        return None
    
    # 提取括号内的内容
    content = ast_str[1:-1]
    
    # 处理没有子节点的情况
    if '(' not in content:
        # 检查是否是空白节点（如ε），如果是则跳过
        if content == 'ε':
            return None
        return {'name': content, 'children': []}
    
    # 查找第一个'('之前的内容作为节点名
    first_paren = content.find('(')
    node_name = content[:first_paren]
    
    # 如果节点名称为空，可能是格式问题，尝试重新解析
    if node_name == '':
        # 尝试找到第一个非空字符作为节点名
        for i, char in enumerate(content):
            if char != '(':
                # 找到第一个非'('的字符，从这个位置开始找节点名
                next_paren = content.find('(', i)
                if next_paren != -1:
                    node_name = content[i:next_paren]
                    first_paren = next_paren
                    break
        
        # 如果还是空，返回None
        if node_name == '':
            return None
    
    # 跳过空白节点
    if node_name == 'ε':
        return None
    
    # 剩余部分应该是子节点
    children_str = content[first_paren:]
    
    # 解析子节点
    children = []
    balance = 0
    start = 0
    
    for i, char in enumerate(children_str):
        if char == '(':
            balance += 1
        elif char == ')':
            balance -= 1
        
        # 当平衡回到0时，我们找到了一个完整的子节点
        if balance == 0 and i > start:
            child_str = children_str[start:i+1]
            # 检查子节点是否为空节点
            if child_str == '()':
                # 空节点，跳过
                pass
            else:
                child = parse_ast(child_str)
                if child:  # 只添加非空节点
                    children.append(child)
            start = i + 1
    
    # 如果节点名称以')'结尾，去掉这个多余的右括号
    if node_name.endswith(')'):
        node_name = node_name.rstrip(')')
    
    return {'name': node_name, 'children': children}

def ast_to_mermaid(ast, parent_id=0, node_id=1):
    """将AST结构转换为mermaid语法"""
    mermaid_lines = []
    current_id = node_id
    
    # 添加当前节点与父节点的连接
    if parent_id != 0:
        mermaid_lines.append(f'    {parent_id} --> {current_id}')
    
    # 添加当前节点（确保节点名称正确显示）
    node_name = ast["name"] if ast["name"] else "[]"
    # 转义特殊字符并用引号包围
    node_name = node_name.replace('"', '&quot;').replace('\\', '\\\\').replace(')', '&#41;').replace('(', '&#40;')
    mermaid_lines.append(f'    {current_id}["{node_name}"]')
    
    next_id = current_id + 1
    
    # 递归处理子节点
    for child in ast['children']:
        child_lines, next_id = ast_to_mermaid(child, current_id, next_id)
        mermaid_lines.extend(child_lines)
    
    return mermaid_lines, next_id

def generate_html(mermaid_code, output_file='ast_tree.html'):
    """生成包含mermaid图表的HTML文件，支持放大缩小功能"""
    html_content = f'''<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>抽象语法树可视化</title>
    <script src="https://cdn.jsdelivr.net/npm/mermaid@11.4.1/dist/mermaid.min.js"></script>
    <script>
        let currentScale = 1.0;
        const minScale = 0.3;
        const maxScale = 3.0;
        const scaleStep = 0.1;
        
        function zoomIn() {{
            if (currentScale < maxScale) {{
                currentScale += scaleStep;
                updateZoom();
            }}
        }}
        
        function zoomOut() {{
            if (currentScale > minScale) {{
                currentScale -= scaleStep;
                updateZoom();
            }}
        }}
        
        function resetZoom() {{
            currentScale = 1.0;
            updateZoom();
        }}
        
        function updateZoom() {{
            const mermaidContainer = document.getElementById('mermaid-container');
            if (mermaidContainer) {{
                mermaidContainer.style.transform = `scale(${{currentScale}})`;
                mermaidContainer.style.transformOrigin = 'top left';
                
                // 更新缩放显示
                const zoomDisplay = document.getElementById('zoom-display');
                if (zoomDisplay) {{
                    zoomDisplay.textContent = `${{Math.round(currentScale * 100)}}%`;
                }}
            }}
        }}
        
        // 添加键盘快捷键支持
        document.addEventListener('keydown', function(event) {{
            if (event.ctrlKey) {{
                switch(event.key) {{
                    case '=':
                    case '+':
                        event.preventDefault();
                        zoomIn();
                        break;
                    case '-':
                        event.preventDefault();
                        zoomOut();
                        break;
                    case '0':
                        event.preventDefault();
                        resetZoom();
                        break;
                }}
            }}
        }});
        
        mermaid.initialize({{ 
            startOnLoad: true,
            theme: 'default',
            flowchart: {{ 
                rankDir: 'TB',
                nodeSpacing: 10,
                rankSpacing: 30
            }}
        }});
    </script>
    <style>
        body {{ 
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #f5f5f5;
        }}
        .header {{
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 20px;
            padding: 15px;
            background: white;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }}
        .zoom-controls {{
            display: flex;
            align-items: center;
            gap: 10px;
        }}
        .zoom-btn {{
            padding: 8px 16px;
            border: 1px solid #ddd;
            background: white;
            border-radius: 4px;
            cursor: pointer;
            font-size: 16px;
            transition: all 0.2s;
        }}
        .zoom-btn:hover {{
            background: #f0f0f0;
            border-color: #999;
        }}
        .zoom-display {{
            font-weight: bold;
            min-width: 50px;
            text-align: center;
        }}
        .mermaid-container {{
            width: 100%;
            overflow: auto;
            background: white;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            padding: 20px;
            transition: transform 0.2s ease;
        }}
        .mermaid {{
            min-width: fit-content;
            min-height: fit-content;
        }}
        .shortcut-hint {{
            font-size: 12px;
            color: #666;
            margin-top: 5px;
        }}
    </style>
</head>
<body>
    <div class="header">
        <h1>AST Tree Visualization</h1>
        <div class="zoom-controls">
            <button class="zoom-btn" onclick="zoomOut()" title="缩小 (Ctrl+-)">-</button>
            <span class="zoom-display" id="zoom-display">100%</span>
            <button class="zoom-btn" onclick="zoomIn()" title="放大 (Ctrl++)">+</button>
            <button class="zoom-btn" onclick="resetZoom()" title="重置缩放 (Ctrl+0)">重置</button>
        </div>
    </div>
    <div class="shortcut-hint">提示: 使用 Ctrl++ 放大, Ctrl+- 缩小, Ctrl+0 重置</div>
    <div id="mermaid-container" class="mermaid-container">
        <div class="mermaid">
            graph TD
{mermaid_code}
        </div>
    </div>
</body>
</html>'''
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(html_content)

def main():
    try:
        # 读取AST文件内容
        with open('outputAST.txt', 'r', encoding='utf-8') as f:
            ast_content = f.read()
        
        # 解析AST
        ast = parse_ast(ast_content)
        if not ast:
            print("无法解析AST内容，请检查格式是否正确")
            return
        
        # 转换为mermaid语法
        mermaid_lines, _ = ast_to_mermaid(ast)
        mermaid_code = '\n'.join(mermaid_lines)
        
        # 生成HTML文件
        generate_html(mermaid_code)
        print("成功生成AST可视化文件: ast_tree.html")
    
    except FileNotFoundError:
        print("错误: 找不到outputAST.txt文件")
    except Exception as e:
        print(f"处理过程中发生错误: {str(e)}")

if __name__ == "__main__":
    main()
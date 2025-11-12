def ast_to_mermaid(ast, parent_id=0, node_id=1):
    mermaid_lines = []
    current_id = node_id
    
    if parent_id != 0:
        mermaid_lines.append(f'    {parent_id} --> {current_id}')

    node_name = ast.root if ast.root else "[]"
    
    if node_name == '""':
        node_name = "空字符串"
    
    mermaid_lines.append(f'    {current_id}[{node_name}]')
    
    next_id = current_id + 1
    
    for child in ast.children:
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
        const minScale = 0.1;  // 减小最小缩放比例
        const maxScale = 5.0;   // 增大最大缩放比例
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
                rankSpacing: 30,
                diagramPadding: 10,
                useMaxWidth: false,  // 不限制最大宽度
                htmlLabels: true
            }},
            securityLevel: 'loose',  // 松散安全级别以支持更多特性
            maxTextSize: 100000,     // 增加最大文本大小限制
            maxEdges: 10000          // 增加最大边数限制
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
        <h1>AST可视化</h1>
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

def main(ast):
    try:
        mermaid_lines, _ = ast_to_mermaid(ast)
        mermaid_code = '\n'.join(mermaid_lines)
        
        generate_html(mermaid_code)
        print("成功生成AST可视化文件: ast_tree.html")
    
    except FileNotFoundError:
        print("错误: 找不到outputAST.txt文件")
    except Exception as e:
        print(f"处理过程中发生错误: {str(e)}")

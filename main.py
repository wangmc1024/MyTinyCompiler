from PySide6.QtWidgets import QApplication, QDialog
from main_ui import Ui_compiler
import sys
import lexical
import syntactic
import TAC
import AST


class MyDialog(QDialog):
    def __init__(self):
        super().__init__()
        
        self.ui = Ui_compiler()
        self.ui.setupUi(self)

        self.ui.lexical.clicked.connect(self.lexical_analysis)
        self.ui.syntactic.clicked.connect(self.syntactic_analysis)
        self.ui.AST.clicked.connect(self.AST_analysis)
        self.ui.TAC.clicked.connect(self.TAC_analysis)

    def lexical_analysis(self):
        self.ui.output.clear()
        self.ui.debug.clear()
        try:
            source_code = self.ui.input.toPlainText()
            if not source_code.strip():
                self.ui.output.addItem("错误：请输入源代码")
                return
                
            lexer = lexical.Lexer(source_code)
            currentToken = lexer.nextInput()
            self.ui.output.addItem(f"{currentToken.type} {currentToken.value}")
            while currentToken.value != "#" and currentToken.type != "error":
                currentToken = lexer.nextInput()
                self.ui.output.addItem(f"{currentToken.type} {currentToken.value}")
            
            self.ui.output.addItem("词法分析结束")
            debugInfo = lexer.getDebugInfo()
            for info in debugInfo:
                self.ui.debug.addItem(info)

        except Exception as e:
            self.ui.output.addItem(f"词法分析错误: {str(e)}")
            
    def syntactic_analysis(self):
        self.ui.output.clear()
        self.ui.debug.clear()
        try:
            source_code = self.ui.input.toPlainText()
            if not source_code.strip():
                self.ui.output.addItem("错误：请输入源代码")
                return
                
            lexer = lexical.Lexer(source_code)
            parser = syntactic.Parser(lexer)
            parser.parseProgram()
            parseResult = parser.getParseResult()
            for line in parseResult:
                self.ui.output.addItem(line)
            debugInfo = lexer.getDebugInfo()
            for info in debugInfo:
                self.ui.debug.addItem(info)
        except Exception as e:
            self.ui.output.addItem(f"语法分析错误: {str(e)}")
         
    def AST_analysis(self):
        self.ui.output.clear()
        self.ui.debug.clear()
        try:
            source_code = self.ui.input.toPlainText()
            if not source_code.strip():
                self.ui.output.addItem("错误：请输入源代码")
                return
                
            lexer = lexical.Lexer(source_code)
            astAnalyzer = AST.ASTSemanticAnalyzer(lexer)
            astAnalyzer.analyze()
            ast = astAnalyzer.getAST()
            if ast:
                astInfo = ast.toString()
                self.ui.output.addItem(astInfo)
            
                with open("outputAST.txt", "w", encoding="utf-8") as f:
                    f.write(astInfo)
            else:
                self.ui.output.addItem("AST生成失败")
            
            outputInfo = astAnalyzer.getOutputInfo()
            if outputInfo:
                for info in outputInfo:
                    self.ui.output.addItem(info)
                
            debugInfo = astAnalyzer.getDebugInfo()
            for info in debugInfo:
                self.ui.debug.addItem(info)
        
            
        except Exception as e:
            self.ui.output.addItem(f"AST分析错误: {str(e)}")
            
    def TAC_analysis(self):         
        self.ui.output.clear()
        self.ui.debug.clear()
        try:
            source_code = self.ui.input.toPlainText()
            if not source_code.strip():
                self.ui.output.addItem("错误：请输入源代码")
                return
                
            lexer = lexical.Lexer(source_code)
            tacGenerator = TAC.TACGenerator(lexer)
            tacGenerator.generate()
            
            # 获取TAC表并显示
            tacTable = tacGenerator.getTACTable()
            midCode = tacTable.dump()
            for line in midCode:
                self.ui.output.addItem(line)
                
            debugInfo = tacGenerator.getDebugInfo()
            for info in debugInfo:
                self.ui.debug.addItem(info)
                
            with open("outputTAC.txt", "w", encoding="utf-8") as f:
                f.write("\n".join(midCode))
        except Exception as e:
            self.ui.output.addItem(f"TAC分析错误: {str(e)}")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    dialog = MyDialog()
    dialog.show()
    sys.exit(app.exec())
from PySide6.QtWidgets import QApplication,QDialog
from main_ui import Ui_compiler
import sys
import lexical


class MyDialog(QDialog):
    def __init__(self):
        super().__init__()  # 初始化父类QDialog
        
        # 初始化UI
        self.ui = Ui_compiler()
        self.ui.setupUi(self)

        self.ui.lexical.clicked.connect(self.lexical_analysis)
        self.ui.syntactic.clicked.connect(self.syntactic_analysis)
        self.ui.AST.clicked.connect(self.AST_analysis)
        self.ui.TAC.clicked.connect(self.TAC_analysis)

    def lexical_analysis(self):
        self.ui.output.clear()
        self.ui.debug.clear()
        source_code = self.ui.input.toPlainText()
        lexer = lexical.Lexer(source_code)
        currentToken = lexer.nextInput()
        self.ui.output.addItem(f"{currentToken.type} {currentToken.value}")
        while currentToken.value != "#" and currentToken.type != "error":
            currentToken = lexer.nextInput()
            self.ui.output.addItem(f"{currentToken.type} {currentToken.value}")
        
        self.ui.output.addItem(f"词法分析结束")
        debugInfo = lexer.getDebugInfo()
        for info in debugInfo:
            self.ui.debug.addItem(info)
    def syntactic_analysis(self):
        self.ui.output.clear()
        self.ui.debug.clear()
        print("语法分析开始")
         
    def AST_analysis(self):
        print("AST Analysis")
    def TAC_analysis(self):
        print("TAC Analysis")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    dialog = MyDialog()
    dialog.show()
    sys.exit(app.exec())

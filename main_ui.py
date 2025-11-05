# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'main_ui.ui'
##
## Created by: Qt User Interface Compiler version 6.10.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QDialog, QHBoxLayout, QListWidget,
    QListWidgetItem, QPushButton, QSizePolicy, QTextEdit,
    QWidget)

class Ui_compiler(object):
    def setupUi(self, compiler):
        if not compiler.objectName():
            compiler.setObjectName(u"compiler")
        compiler.resize(731, 604)
        self.horizontalLayoutWidget = QWidget(compiler)
        self.horizontalLayoutWidget.setObjectName(u"horizontalLayoutWidget")
        self.horizontalLayoutWidget.setGeometry(QRect(9, 10, 711, 80))
        self.horizontalLayout = QHBoxLayout(self.horizontalLayoutWidget)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.input = QTextEdit(self.horizontalLayoutWidget)
        self.input.setObjectName(u"input")
        self.input.setTabChangesFocus(False)

        self.horizontalLayout.addWidget(self.input)

        self.horizontalLayoutWidget_2 = QWidget(compiler)
        self.horizontalLayoutWidget_2.setObjectName(u"horizontalLayoutWidget_2")
        self.horizontalLayoutWidget_2.setGeometry(QRect(9, 100, 711, 80))
        self.horizontalLayout_2 = QHBoxLayout(self.horizontalLayoutWidget_2)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.lexical = QPushButton(self.horizontalLayoutWidget_2)
        self.lexical.setObjectName(u"lexical")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.lexical.sizePolicy().hasHeightForWidth())
        self.lexical.setSizePolicy(sizePolicy)

        self.horizontalLayout_2.addWidget(self.lexical)

        self.syntactic = QPushButton(self.horizontalLayoutWidget_2)
        self.syntactic.setObjectName(u"syntactic")
        sizePolicy.setHeightForWidth(self.syntactic.sizePolicy().hasHeightForWidth())
        self.syntactic.setSizePolicy(sizePolicy)

        self.horizontalLayout_2.addWidget(self.syntactic)

        self.TAC = QPushButton(self.horizontalLayoutWidget_2)
        self.TAC.setObjectName(u"TAC")
        self.TAC.setEnabled(True)
        sizePolicy.setHeightForWidth(self.TAC.sizePolicy().hasHeightForWidth())
        self.TAC.setSizePolicy(sizePolicy)
        self.TAC.setIconSize(QSize(16, 50))

        self.horizontalLayout_2.addWidget(self.TAC)

        self.AST = QPushButton(self.horizontalLayoutWidget_2)
        self.AST.setObjectName(u"AST")
        sizePolicy.setHeightForWidth(self.AST.sizePolicy().hasHeightForWidth())
        self.AST.setSizePolicy(sizePolicy)
        self.AST.setIconSize(QSize(16, 50))

        self.horizontalLayout_2.addWidget(self.AST)

        self.horizontalLayoutWidget_3 = QWidget(compiler)
        self.horizontalLayoutWidget_3.setObjectName(u"horizontalLayoutWidget_3")
        self.horizontalLayoutWidget_3.setGeometry(QRect(10, 190, 711, 401))
        self.horizontalLayout_3 = QHBoxLayout(self.horizontalLayoutWidget_3)
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalLayout_3.setContentsMargins(0, 0, 0, 0)
        self.output = QListWidget(self.horizontalLayoutWidget_3)
        QListWidgetItem(self.output)
        self.output.setObjectName(u"output")

        self.horizontalLayout_3.addWidget(self.output)

        self.debug = QListWidget(self.horizontalLayoutWidget_3)
        QListWidgetItem(self.debug)
        self.debug.setObjectName(u"debug")

        self.horizontalLayout_3.addWidget(self.debug)


        self.retranslateUi(compiler)

        QMetaObject.connectSlotsByName(compiler)
    # setupUi

    def retranslateUi(self, compiler):
        compiler.setWindowTitle(QCoreApplication.translate("compiler", u"Dialog", None))
        self.lexical.setText(QCoreApplication.translate("compiler", u"\u8bcd\u6cd5", None))
        self.syntactic.setText(QCoreApplication.translate("compiler", u"\u8bed\u6cd5", None))
        self.TAC.setText(QCoreApplication.translate("compiler", u"\u8bed\u4e49TAC", None))
        self.AST.setText(QCoreApplication.translate("compiler", u"\u8bed\u4e49AST", None))

        __sortingEnabled = self.output.isSortingEnabled()
        self.output.setSortingEnabled(False)
        ___qlistwidgetitem = self.output.item(0)
        ___qlistwidgetitem.setText(QCoreApplication.translate("compiler", u"output", None));
        self.output.setSortingEnabled(__sortingEnabled)


        __sortingEnabled1 = self.debug.isSortingEnabled()
        self.debug.setSortingEnabled(False)
        ___qlistwidgetitem1 = self.debug.item(0)
        ___qlistwidgetitem1.setText(QCoreApplication.translate("compiler", u"debug", None));
        self.debug.setSortingEnabled(__sortingEnabled1)

    # retranslateUi


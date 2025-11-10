#include "syntactic.hpp"

Parser::Parser(Lexer& lexer_) : lexer(lexer_), currentToken(lexer_.nextInput()) {
}



const std::vector<std::string>& Parser::getParseResult() const {
    return parseResult;
}

bool Parser::match(const std::string& expectedType) {
    if (currentToken.type != expectedType) {
        print("匹配失败:expect " + expectedType + ", got " + currentToken.type);
        return false;
    }
    
    print("匹配成功:expect " + expectedType);
    currentToken = lexer.nextInput();
    return true;
}

void Parser::print(const std::string& msg) {
    parseResult.push_back(msg);
}

void Parser::parseProgram() {
    print("推导:<程序> -> <变量说明部分><语句部分>");
    parseDeclareSection();
    parseStatementSection();
    print("语法分析结束");
}

void Parser::parseDeclareSection() {
    print("推导:<变量说明部分>-><变量说明语句>分号A");
    parseDeclareStatement();
    if (!match("分号")) return;
    parseA();
}

void Parser::parseA() {
    print("选择产生式:A-><变量说明语句>分号A|ε");
    
    if (currentToken.type == "变量说明") {
        print("推导:A-><变量说明语句>分号A");
        parseDeclareStatement();
        if (!match("分号")) return;
        parseA();
    }
    else if (currentToken.type == "标识符" || currentToken.type == "if" || currentToken.type == "while") {
        print("推导:A->ε");
    }
}

void Parser::parseB() {
    print("选择产生式:B->逗号 标识符 B |ε");
    
    if (currentToken.type == "逗号") {
        print("推导:B->逗号 标识符 B");
        if (!match("逗号")) return;
        if (!match("标识符")) return;
        parseB();
    }
    else if (currentToken.type == "分号") {
        print("推导:B->ε");
    }
}

void Parser::parseStatement() {
    print("选择产生式:<语句>-><赋值语句>|<条件语句>|<循环语句>");
    
    if (currentToken.type == "标识符") {
        print("推导:<语句>-><赋值语句>");
        parseAssignmentStatement();
    }
    else if (currentToken.type == "if") {
        print("推导:<语句>-><条件语句>");
        parseConditionalStatement();
    }
    else if (currentToken.type == "while") {
        print("推导:<语句>-><循环语句>");
        parseLoopStatement();
    }
}

void Parser::parseNestedStatement() {
    print("选择产生式:<嵌套语句>-><语句> 分号 | <复合语句>");
    
    if (currentToken.type == "标识符" || currentToken.type == "if" || currentToken.type == "while") {
        print("推导:<嵌套语句>-><语句> 分号");
        parseStatement();
        if (!match("分号")) return;
    }
    else if (currentToken.type == "begin") {
        print("推导:<嵌套语句>-><复合语句>");
        parseCompoundStatement();
    }
}
void Parser::parseDeclareStatement() {
    print("推导:<变量说明语句>->变量说明<标识符列表>");
    if (!match("变量说明")) return;
    parseVarList();
}

void Parser::parseVarList() {
    print("推导:<标识符列表>->标识符B");
    if (!match("标识符")) return;
    parseB();
}


void Parser::parseStatementSection() {
    print("推导:<语句部分>-><语句>C");
    parseStatement();
    parseC();
}

void Parser::parseC() {
    print("选择产生式:C->分号<语句>C|ε");
    
    if (currentToken.type == "分号") {
        print("推导:C->分号<语句>C");
        if (!match("分号")) return;
        parseStatement();
        parseC();
    }
    else if (currentToken.type == "#" || currentToken.type == "end") {
        print("推导:C->ε");
    }
}



void Parser::parseAssignmentStatement() {
    print("推导:<赋值语句>->标识符 赋值符 <表达式>");
    if (!match("标识符")) return;
    if (!match("赋值号")) return;
    parseExpression();
}

void Parser::parseExpression() {
    print("推导:<表达式>-><conjunction> D");
    parseConjunction();
    parseD();
}

void Parser::parseD() {
    print("选择产生式:D->or <conjunction> D | ε");
    
    if (currentToken.type == "or") {
        print("推导:D->or <conjunction> D");
        if (!match("or")) return;
        parseConjunction();
        parseD();
    }
    else if (currentToken.type == "分号" || currentToken.type == "#" || 
             currentToken.type == "end" || currentToken.type == "右括号") {
        print("推导:D->ε");
    }
}

void Parser::parseConjunction() {
    print("推导:<conjunction>-><inversion> G");
    parseInversion();
    parseG();
}

void Parser::parseG() {
    print("选择产生式:G->and <inversion> G | ε");
    
    if (currentToken.type == "and") {
        print("推导:G->and <inversion> G");
        if (!match("and")) return;
        parseInversion();
        parseG();
    }
    else if (currentToken.type == "分号" || 
             currentToken.type == "#" || currentToken.type == "end" || 
             currentToken.type == "右括号"||currentToken.type == "or") {
        print("推导:G->ε");
    }
}

void Parser::parseInversion() {
    print("选择产生式:<inversion>-> not <inversion> | <关系表达式>");
    
    if (currentToken.type == "not") {
        print("推导:<inversion>-> not <inversion>");
        if (!match("not")) return;
        parseInversion();
    }
    else if (currentToken.type == "标识符" || currentToken.type == "true" || 
             currentToken.type == "false" || currentToken.type == "字符串" || 
             currentToken.type == "整数" || currentToken.type == "左括号") {
        print("推导:<inversion>-> <关系表达式>");
        parseRelationExpression();
    }
}

void Parser::parseRelationExpression() {
    print("推导:<关系表达式>-> <算术表达式> E");
    parseMathExpression();
    parseE();
}

void Parser::parseE() {
    print("选择产生式:E-> 关系 <算术表达式> E | ε ");
    
    if (currentToken.type == "关系") {
        print("推导:E-> 关系 <算术表达式> E");
        if (!match("关系")) return;
        parseMathExpression();
        parseE();
    }
    else if (currentToken.type == "and" || currentToken.type == "or" || 
             currentToken.type == "右括号" || currentToken.type == "分号" || 
             currentToken.type == "#" || currentToken.type == "end") {
        print("推导:E->ε");
    }
}

void Parser::parseMathExpression() {
    print("推导:<算术表达式> -> <term> H ");
    parseTerm();
    parseH();
}

void Parser::parseTerm() {
    print("推导:<term>-><factor> I");
    parseFactor();
    parseI();
}

void Parser::parseH() {
    print("选择产生式:H-> 加法 <term> H | ε ");
    
    if (currentToken.type == "加法") {
        print("推导:H-> " + currentToken.type + " <term> H");
        if (!match("加法")) return;
        parseTerm();
        parseH();
    }
    else if (currentToken.type == "关系" || currentToken.type == "右括号" || 
             currentToken.type == "分号" || currentToken.type == "#" || 
             currentToken.type == "end" || currentToken.type == "and" || 
             currentToken.type == "or") {
        print("推导:H->ε");
    }
}

void Parser::parseI() {
    print("选择产生式:I-> 乘法 <factor> I | ε ");
    
    if (currentToken.type == "乘法") {
        print("推导:I-> 乘法 <factor> I");
        if (!match("乘法")) return;
        parseFactor();
        parseI();
    }
    else if (currentToken.type == "加法" || 
             currentToken.type == "关系" || currentToken.type == "右括号" || 
             currentToken.type == "分号" || currentToken.type == "#" || 
             currentToken.type == "end" || currentToken.type == "and" || 
             currentToken.type == "or") {
        print("推导:I->ε");
    }
}

void Parser::parseFactor() {
    print("选择产生式:<factor>-> 标识符|true|false| 字符串 |整数|左括号 <表达式> 右括号 ");
    
    if (currentToken.type == "标识符") {
        print("推导:<factor>-> 标识符");
        match("标识符");
    }
    else if (currentToken.type == "true") {
        print("推导:<factor>-> true");
        match("true");
    }
    else if (currentToken.type == "false") {
        print("推导:<factor>-> false");
        match("false");
    }
    else if (currentToken.type == "字符串") {
        print("推导:<factor>-> 字符串");
        if (!match("字符串")) return;
    }
    else if (currentToken.type == "整数") {
        print("推导:<factor>-> 整数");
        match("整数");
    }
    else if (currentToken.type == "左括号") {
        print("推导:<factor>-> 左括号<表达式>右括号");
        if (!match("左括号")) return;
        parseExpression();
        if (!match("右括号")) return;
    }
}


void Parser::parseConditionalStatement() {
    print("推导:<条件语句>-> if 左括号 <表达式> 右括号 <嵌套语句> else <嵌套语句>");
    if (!match("if")) return;
    if (!match("左括号")) return;
    parseExpression();
    if (!match("右括号")) return;
    parseNestedStatement();
    if (!match("else")) return;
    parseNestedStatement();
}

void Parser::parseLoopStatement() {
    print("推导:<循环语句>->while 左括号 <表达式> 右括号 冒号 <嵌套语句>");
    if (!match("while")) return;
    if (!match("左括号")) return;
    parseExpression();
    if (!match("右括号")) return;
    if (!match("冒号")) return;
    parseNestedStatement();
}

void Parser::parseCompoundStatement() {
    print("推导:<复合语句>-> begin <语句部分> end");
    if (!match("begin")) return;
    parseStatementSection();
    if (!match("end")) return;
}
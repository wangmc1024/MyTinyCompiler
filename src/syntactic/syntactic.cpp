#include "syntactic.hpp"

Parser::Parser(Lexer& lexer_) : lexer(lexer_), currentToken(lexer_.nextInput()) {
}

void Parser::parseProgram() {
    print("推导:<程序> -> <变量说明部分><语句部分>");
    parseDeclareSection();
    parseStatementSection();
    print("语法分析结束");
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

void Parser::parseDeclareSection() {
    print("推导:<变量说明部分>-><变量声明语句>分号A");
    parseDeclareStatement();
    match("分号");
    parseA();
}

void Parser::parseA() {
    print("选择产生式:A-><变量说明语句>分号A|ε");
    
    if (currentToken.type == "变量说明") {
        print("推导:A-><变量说明语句>分号A");
        parseDeclareStatement();
        match("分号");
        parseA();
    }
    else if (currentToken.type == "标识符" || currentToken.type == "if") {
        print("推导:A->ε");
    }
}

void Parser::parseDeclareStatement() {
    print("推导:<变量说明语句>->变量说明<标识符列表>");
    match("变量说明");
    parseVarList();
}

void Parser::parseVarList() {
    print("推导:<标识符列表>->标识符B");
    match("标识符");
    parseB();
}

void Parser::parseB() {
    print("选择产生式:B->逗号 标识符 B |ε");
    
    if (currentToken.type == "逗号") {
        print("推导:B->逗号 标识符 B");
        match("逗号");
        match("标识符");
        parseB();
    }
    else if (currentToken.type == "分号") {
        print("推导:B->ε");
    }
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
        match("分号");
        parseStatement();
        parseC();
    }
    else if (currentToken.type == "#" || currentToken.type == "end") {
        print("推导:C->ε");
    }
}

void Parser::parseStatement() {
    print("选择产生式:<语句>-><赋值语句>|<条件语句>");
    
    if (currentToken.type == "标识符") {
        print("推导:<语句>-><赋值语句>");
        parseAssignmentStatement();
    }
    else if (currentToken.type == "if") {
        parseConditionalStatement();
    }
}

void Parser::parseAssignmentStatement() {
    print("推导:<赋值语句>->标识符 赋值号 <表达式>");
    match("标识符");
    match("赋值号");
    parseExpression();
}

void Parser::parseExpression() {
    print("推导:<表达式>-> <inversion> D");
    parseInversion();
    parseD();
}

void Parser::parseD() {
    print("选择产生式:D-> or <inversion> D| ε");
    
    if (currentToken.type == "or") {
        print("推导:D-> or <inversion> D");
        match("or");
        parseInversion();
        parseD();
    }
    else if (currentToken.type == "分号" || currentToken.type == "#" || 
             currentToken.type == "end" || currentToken.type == "右括号") {
        print("推导:D->ε");
    }
}

void Parser::parseInversion() {
    print("选择产生式:<inversion>-> not <inversion> | <关系表达式>");
    
    if (currentToken.type == "not") {
        print("推导:<inversion>-> not <inversion>");
        match("not");
        parseInversion();
    }
    else if (currentToken.type == "标识符" || currentToken.type == "true" ||
             currentToken.type == "false" || currentToken.type == "整数" ||
             currentToken.type == "左括号") {
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
    print("选择产生式:E-> 关系 <算术表达式> | ε ");
    
    if (currentToken.type == "关系") {
        print("推导:E-> 关系 <算术表达式>");
        match("关系");
        parseMathExpression();
    }
    else if (currentToken.type == "分号" || currentToken.type == "#" ||
             currentToken.type == "end" || currentToken.type == "右括号" ||
             currentToken.type == "or") {
        print("推导:E->ε");
    }
}

void Parser::parseMathExpression() {
    print("推导:<算术表达式> -> <factor> F ");
    parseFactor();
    parseF();
}

void Parser::parseF() {
    print("选择产生式:F-> 乘法 <factor> F | ε ");
    
    if (currentToken.type == "乘法") {
        print("推导:F-> 乘法 <factor> F");
        match("乘法");
        parseFactor();
        parseF();
    }
    else if (currentToken.type == "分号" || currentToken.type == "#" ||
             currentToken.type == "end" || currentToken.type == "右括号" ||
             currentToken.type == "关系" || currentToken.type == "or") {
        print("推导:F->ε");
    }
}

void Parser::parseFactor() {
    print("选择产生式:<factor>-> 标识符|true|false| 整数 |左括号 <表达式> 右括号 ");
    
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
    else if (currentToken.type == "整数") {
        print("推导:<factor>-> 整数");
        match("整数");
    }
    else if (currentToken.type == "左括号") {
        print("推导:<factor>-> 左括号<表达式>右括号");
        match("左括号");
        parseExpression();
        match("右括号");
    }
}

void Parser::parseConditionalStatement() {
    print("推导:<条件语句>-> if 左括号 <表达式> 右括号 <复合语句> else <复合语句>");
    match("if");
    match("左括号");
    parseExpression();
    match("右括号");
    parseCompoundStatement();
    match("else");
    parseCompoundStatement();
}

void Parser::parseCompoundStatement() {
    print("推导:<复合语句>-> begin <语句部分> end");
    match("begin");
    parseStatementSection();
    match("end");
}
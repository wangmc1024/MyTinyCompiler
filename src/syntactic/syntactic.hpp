#pragma once
#include <vector>
#include <string>
#include "../lexical/lexical.hpp"

class Parser {
private:
    Lexer& lexer;
    Token currentToken;
    std::vector<std::string> parseResult;

public:
    Parser(Lexer& lexer_);
    void parseProgram();
    const std::vector<std::string>& getParseResult() const;

    Lexer& getLexer() const { return lexer; }

private:
    bool match(const std::string& expectedType);
    void print(const std::string& msg);
    
    void parseDeclareSection();
    void parseA();
    void parseDeclareStatement();
    void parseVarList();
    void parseB();
    void parseStatementSection();
    void parseC();
    void parseStatement();
    void parseAssignmentStatement();
    void parseExpression();
    void parseD();
    void parseConjunction();  // 新增：对应<conjunction>
    void parseG();  // 新增：对应G产生式
    void parseInversion();
    void parseRelationExpression();
    void parseE();
    void parseMathExpression();
    void parseTerm();  // 新增：对应<term>
    void parseH();  // 新增：对应H产生式
    void parseI();  // 新增：对应I产生式
    //void parseF();
    void parseFactor();
    void parseString();  // 新增：对应<字符串>
    void parseConditionalStatement();
    void parseLoopStatement();  // 新增：对应<循环语句>
    void parseNestedStatement();  // 新增：对应<嵌套语句>
    void parseCompoundStatement();
};
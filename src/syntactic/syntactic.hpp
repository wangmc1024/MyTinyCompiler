#pragma once
#include <vector>
#include <string>
#include "../lexical/lexical.hpp"

class Lexer;
class Token;

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
    void parseInversion();
    void parseRelationExpression();
    void parseE();
    void parseMathExpression();
    void parseF();
    void parseFactor();
    void parseConditionalStatement();
    void parseCompoundStatement();
};

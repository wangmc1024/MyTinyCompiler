#pragma once

#include <string>
#include <vector>

class Parser;

class Token {
public:
    std::string type;  
    std::string value; 

    Token(std::string type_, std::string value_) 
        : type(std::move(type_)), value(std::move(value_)) {}
    
    std::string toString() const;
};

class Lexer {
    friend class Parser;
private:
    std::string sourceProgram;  
    int pointer;                
    std::vector<std::string> debugInfo;  

public:
    Lexer(std::string source);
  
    Token nextInput();

    const std::vector<std::string>& getDebugInfo() const;
    
    void clearDebugInfo();
private:
    // 向调试信息列表添加内容
    void debug(const std::string& msg);
};

#pragma once

#include <string>
#include <vector>

class Parser;

class Token {
public:
    std::string type;  // 词法单元类型
    std::string value; // 词法单元的值

    Token(std::string type_, std::string value_) 
        : type(std::move(type_)), value(std::move(value_)) {}
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

    // 获取调试信息列表
    const std::vector<std::string>& getDebugInfo() const;

    // 清空调试信息
    void clearDebugInfo();
private:
    // 向调试信息列表添加内容
    void debug(const std::string& msg);
};

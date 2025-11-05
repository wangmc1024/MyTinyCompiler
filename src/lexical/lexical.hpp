#pragma once

#include <string>
#include <vector>

// Token类：存储词法单元的类型和值
class Token {
public:
    std::string type;  // 词法单元类型
    std::string value; // 词法单元的值

    Token(std::string type_, std::string value_) 
        : type(std::move(type_)), value(std::move(value_)) {}
};

class Lexer {
private:
    std::string sourceProgram;  
    int pointer;                
    std::vector<std::string> debugInfo;  

public:
    // 构造函数：接收源程序字符串，初始化解析指针
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

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "../lexical/lexical.hpp"

class triple
{
public:
    std::string name;
    std::string type;
    std::string value;

    triple(const std::string& name)
        : name(name), type(""), value("") {}

    int getValueAsINT32()
    {
        if (type != "int" || value.empty())
            return -1;
        return std::stoi(value);
    }

    bool getValueAsBOOL()
    {
        if (type != "bool" || value.empty())
            return false;
        return value == "true";
    }

    std::string toString() const
    {
        return " " + name + "|  " + type + "|  " + value;
    }
};

class identifierTable
{
private:
    std::vector<triple> table;

public:
    identifierTable()
    {
        table.clear();
    }

    
    triple getIdentifierByName(const std::string& name)
    {
        for (auto& t : table)
        {
            if (t.name == name)
                return t;
        }
        // 返回一个空的triple对象表示未找到
        return triple("");
    }

    
    bool identifierExists(const std::string& name)
    {
        for (const auto& t : table)
        {
            if (t.name == name)
                return true;
        }
        return false;
    }

    // 添加：获取标识符类型的方法
    std::string getTypeByName(const std::string& name)
    {
        for (const auto& t : table)
        {
            if (t.name == name)
                return t.type;
        }
        return ""; // 返回空字符串表示未找到
    }

    // 添加：获取标识符值的方法
    std::string getValueByName(const std::string& name)
    {
        for (const auto& t : table)
        {
            if (t.name == name)
                return t.value;
        }
        return ""; // 返回空字符串表示未找到
    }

    // 添加：检查标识符是否有有效的类型
    bool identifierHasValidType(const std::string& name)
    {
        for (const auto& t : table)
        {
            if (t.name == name && !t.type.empty())
                return true;
        }
        return false;
    }

    bool Add(const std::string& name)
    {
        if (identifierExists(name))
            return false;
        table.emplace_back(name);
        return true;
    }

    bool UpdateTypeByName(const std::string& name, const std::string& type)
    {
        // 修改：直接遍历查找并更新
        for (auto& t : table)
        {
            if (t.name == name)
            {
                if (!t.type.empty())
                    return false;
                t.type = type;
                return true;
            }
        }
        return false;
    }

    bool UpdateValueByName(const std::string& name, const std::string& value)
    {
        // 修改：直接遍历查找并更新
        for (auto& t : table)
        {
            if (t.name == name)
            {
                t.value = value;
                return true;
            }
        }
        return false;
    }
};



class node
{
private:
    std::vector<node> children;

public:
    std::string root;
    node(const std::string& root)
        : root(root)
    {
        children.clear();
    }

    void addChild(const node& child)
    {
        children.push_back(child);
    }

    void addNodeAsFirstChild(const node& child)
    {
        children.insert(children.begin(), child);
    }

    // 修改：返回bool值表示是否找到，通过引用参数返回结果
    bool getFirstLeftNonLeafChild(node& result)
    {
        if (children.empty())
            return false;
        
        node* pointer = &children[0];
        node* lastPointer = nullptr;
        
        while (!pointer->children.empty())
        {
            lastPointer = pointer;
            pointer = &pointer->children[0];
        }
        
        if (lastPointer != nullptr)
        {
            result = *lastPointer;
            return true;
        }
        return false;
    }

    std::string toString() const
    {
        std::string str = "(" + root + ")";
        if (children.empty())
            return str;
        
        str += "(";
        for (size_t i = 0; i < children.size(); ++i)
        {
            str += children[i].toString();
            if (i != children.size() - 1)
                str += ",";
        }
        str += ")";
        
        return str;
    }

    std::vector<node> getChildren(){
        return children;
    }

};

// AST语义分析类
class ASTSemanticAnalyzer
{
private:
    Lexer& lexer;
    Token currentToken;
    identifierTable idenTable;
    node ast;  // 修改：直接使用node对象而不是shared_ptr
    bool isDebug;
    std::vector<std::string> debugInfo;
    std::vector<std::string> outputInfo;

    void debug(const std::string& str)
    {
        if (isDebug)
            debugInfo.push_back(str);
    }

    void print(const std::string& str)
    {
        if (!isDebug)
            outputInfo.push_back(str);
    }

    bool match(const std::string& expectedType)
    {
        if (currentToken.type != expectedType)
        {
            if(isDebug)
                debug("匹配失败：expect{" + expectedType + "},got{" + currentToken.type + "}");
            else
                print("匹配失败：expect{" + expectedType + "},got{" + currentToken.type + "}");
            return false;
        }
        else
        {
            if(isDebug)
                debug("匹配成功：expect{" + expectedType + "}");
            else
                print("匹配成功：expect{" + expectedType + "}");
            
            currentToken = lexer.nextInput();
            debug(currentToken.toString());
            return true;
        }
    }

    node translateASTProgram();
    node translateASTDeclareSection();
    node translateASTA();
    node translateASTDeclareStatement();
    node translateASTVarList(const std::string& type);
    node translateASTB(const std::string& type);
    node translateASTStatementSection();
    node translateASTC();
    node translateASTStatement();
    node translateASTAssignmentStatement();
    node translateASTConditionalStatement();
    node translateASTLoopStatement();
    node translateASTNestedStatement();
    node translateASTCompoundStatement();
    
    // 表达式相关函数
    node translateASTExpression();
    node translateASTD();
    node translateASTConjunction();
    node translateASTG();
    node translateASTInversion();
    node translateASTRelationExpression();
    node translateASTE();
    node translateASTMathExpression();
    node translateASTF();
    node translateASTTerm();
    node translateASTH();
    node translateASTI();
    node translateASTFactor();
    node translateASTString();

public:
    ASTSemanticAnalyzer(Lexer& lexer_)
        :lexer(lexer_), ast("程序"), isDebug(true),currentToken(lexer.nextInput()){}

    ~ASTSemanticAnalyzer()
    {
        
    }

    void analyze(bool debugMode = true)
    {
        isDebug = debugMode;
        
        idenTable = identifierTable();

        debug(currentToken.toString());
        
        ast = translateASTProgram();
    }

    node getAST() const  // 修改：返回node对象而不是shared_ptr
    {
        return ast;
    }


    const std::vector<std::string>& getDebugInfo() const
    {
        return debugInfo;
    }

    const std::vector<std::string>& getOutputInfo() const
    {
        return outputInfo;
    }
};
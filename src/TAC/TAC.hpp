#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "../lexical/lexical.hpp"

// TAC部分依赖的类定义

class triple
{
public:
    std::string name;
    std::string type;
    std::string value;

    triple(const std::string& name)
        : name(name), type(""), value("") {}

    // 添加接受三个参数的构造函数
    triple(const std::string& name, const std::string& type, const std::string& value)
        : name(name), type(type), value(value) {}

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

    triple* getIdentifierByName(const std::string& name)
    {
        for (auto& t : table)
        {
            if (t.name == name)
                return &t;
        }
        return nullptr;
    }

    // 添加缺失的方法
    bool identifierExists(const std::string& name)
    {
        return getIdentifierByName(name) != nullptr;
    }

    // 添加缺失的方法
    std::string getTypeByName(const std::string& name)
    {
        triple* temp = getIdentifierByName(name);
        if (temp != nullptr)
            return temp->type;
        return "";
    }

    bool Add(const std::string& name)
    {
        if (getIdentifierByName(name) != nullptr)
            return false;
        table.emplace_back(name);
        return true;
    }

    bool UpdateTypeByName(const std::string& name, const std::string& type)
    {
        triple* temp = getIdentifierByName(name);
        if (temp == nullptr)
            return false;
        if (!temp->type.empty())
            return false;
        temp->type = type;
        return true;
    }

    bool UpdateValueByName(const std::string& name, const std::string& value)
    {
        triple* temp = getIdentifierByName(name);
        if (temp == nullptr)
            return false;
        temp->value = value;
        return true;
    }

    void dump(std::ostream& os) const
    {
        os << "| \t name| \t type| \t value|" << std::endl;
        for (const auto& t : table)
        {
            os << "|" << t.toString() << "|" << std::endl;
        }
    }
};

class tempVariableTable
{
private:
    std::vector<triple> table;

public:
    tempVariableTable()
    {
        table.clear();
    }

    triple createNewVariable()
    {
        triple temp("T" + std::to_string(table.size()));
        table.push_back(temp);
        return temp;
    }

    const std::vector<triple>& getTable() const
    {
        return table;
    }
};



// TAC类
class TAC
{
public:
    std::string op;
    std::string oprand1;
    std::string oprand2;
    std::string result;

    TAC(const std::string& op, const std::string& oprand1, 
        const std::string& oprand2, const std::string& result)
        : op(op), oprand1(oprand1), oprand2(oprand2), result(result) {}

    std::string toString() const
    {
        return "(" + op + ", " + oprand1 + ", " + oprand2 + ", " + result + ")";
    }
};

// 中间代码表类
class midCodeTable
{
private:
    std::vector<TAC> table;

public:
    midCodeTable()
    {
        table.clear();
    }

    int NXQ() const
    {
        return table.size();
    }

    void generate(const std::string& op, const std::string& oprand1, 
                 const std::string& oprand2, const std::string& result)
    {
        table.emplace_back(op, oprand1, oprand2, result);
    }

    void backpatch(int index, const std::string& result)
    {
        if (index >= 0 && static_cast<size_t>(index) < table.size())
        {
            table[index].result = result;
        }
    }

    std::vector<std::string> dump() const
    {
        std::vector<std::string> result;
        for (size_t i = 0; i < table.size(); ++i)
        {
            result.push_back("(" + std::to_string(i) + ") " + table[i].toString());
        }
        return result;

    }


    const std::vector<TAC>& getTable() const
    {
        return table;
    }
};


class TACGenerator{
private:
    Lexer& lexer;
    Token currentToken;
    identifierTable idenTable;
    tempVariableTable tempVarTable;
    midCodeTable tacTable;
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

    void translateTACProgram();
    void translateTACDeclareSection();
    void translateTACA();
    void translateTACDeclareStatement();
    void translateTACVarList(const std::string& type);
    void translateTACB(const std::string& type);
    void translateTACStatementSection();
    void translateTACC();
    void translateTACStatement();
    void translateTACAssignmentStatement();
    void translateTACConditionalStatement();
    void translateTACLoopStatement();  // 添加循环语句处理函数声明
    void translateTACNestedStatement();  // 添加嵌套语句处理函数声明
    void translateTACCompoundStatement();
    
    // 表达式相关函数
    triple translateTACExpression();
    triple translateTACD(const triple& E1);
    triple translateTACConjunction();
    triple translateTACG(const triple& E1);
    triple translateTACInversion();
    triple translateTACRelationExpression();
    triple translateTACE(const triple& E1);
    triple translateTACMathExpression();
    triple translateTACF(const triple& E1);
    triple translateTACTerm();
    triple translateTACH(const triple& E1);
    triple translateTACI(const triple& E1);
    triple translateTACFactor();
    triple translateTACString();

public:
    TACGenerator(Lexer& lexer_)
        :lexer(lexer_), isDebug(true), currentToken(lexer.nextInput()){}

    ~TACGenerator()
    {
        
    }

    void generate(bool debugMode = true)
    {
        isDebug = debugMode;
        
        idenTable = identifierTable();
        tempVarTable = tempVariableTable();
        tacTable = midCodeTable();

        debug(currentToken.toString());
        
        translateTACProgram();
    }

    midCodeTable& getTACTable()
    {
        return tacTable;
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
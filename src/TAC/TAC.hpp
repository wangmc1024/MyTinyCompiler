#pragma once
#include "../lexical/lexical.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <map>

// 三元组类
// 在triple类中添加类型转换辅助函数
class triple
{
public:
    std::string name;
    std::string type;
    std::string value;

    triple()
        : name(""), type(""), value("") {}

    triple(const std::string& name)
        : name(name), type(""), value("") {}
    
    triple(const std::string& name, const std::string& type, const std::string& value)
        : name(name), type(type), value(value) {}

    int getValueAsINT32() const
    {
        if (type == "int")
        {
            try
            {
                return std::stoi(value);
            }
            catch (const std::exception&)
            {
                return 0;
            }
        }
        else if (type == "bool")
        {
            return (value == "true") ? 1 : 0;
        }
        return 0;
    }

    bool getValueAsBOOL() const
    {
        if (type == "bool")
        {
            if (value == "true")
                return true;
            else if (value == "false")
                return false;
        }
        else if (type == "int")
        {
            try
            {
                return std::stoi(value) != 0;
            }
            catch (const std::exception&)
            {
                return false;
            }
        }
        return false;
    }

    std::string toString() const
    {
        return " " + name + "| " + type + "| " + value + " ";
    }
    
    // 类型转换函数
    static std::string convertToBool(const triple& t)
    {
        if (t.type == "bool")
            return t.value;
        else if (t.type == "int")
        {
            try
            {
                return (std::stoi(t.value) != 0) ? "true" : "false";
            }
            catch (const std::exception&)
            {
                return "false";
            }
        }
        else if (t.type == "string")
        {
            return (t.value.empty()) ? "false" : "true";
        }
        return "false";
    }
    
    static std::string convertToInt(const triple& t)
    {
        if (t.type == "int")
            return t.value;
        else if (t.type == "bool")
        {
            return (t.value == "true") ? "1" : "0";
        }
        else if (t.type == "string")
        {
            try
            {
                return std::to_string(std::stoi(t.value));
            }
            catch (const std::exception&)
            {
                return "0";
            }
        }
        return "0";
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

    bool identifierExists(const std::string& name) const
    {
        for (const auto& t : table)
        {
            if (t.name == name)
                return true;
        }
        return false;
    }

    std::string getTypeByName(const std::string& name) const
    {
        for (const auto& t : table)
        {
            if (t.name == name)
                return t.type;
        }
        return "";
    }

    bool Add(const std::string& name)
    {
        if (identifierExists(name))
            return false;
        table.emplace_back(name, "", "");
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
    const std::vector<triple>& getTable() const
    {
        return table;
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

    // 程序
    void translateTACProgram();
    
    // 变量声明部分
    void translateTACDeclareSection();
    void translateTACA();
    void translateTACDeclareStatement();
    void translateTACVarList(const std::string& type);
    void translateTACB(const std::string& type);
    
    // 语句部分
    void translateTACStatementSection();
    void translateTACC();
    void translateTACStatement();
    void translateTACAssignmentStatement();
    void translateTACConditionalStatement();
    void translateTACLoopStatement();
    void translateTACNestedStatement();
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
    triple translateTACH(const triple& E1);
    triple translateTACTerm();
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
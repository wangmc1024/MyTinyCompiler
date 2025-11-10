#include "TAC.hpp"
#include <cctype>
#include <stdexcept>

void TACGenerator::translateTACProgram()
{
    debug("推导：<程序> -> <变量说明部分><语句部分>");
    translateTACDeclareSection();
    translateTACStatementSection();
    debug("语法分析结束");
}

void TACGenerator::translateTACDeclareSection()
{
    debug("推导：<变量说明部分>-><变量声明语句>分号A");
    translateTACDeclareStatement();
    match("分号");
    translateTACA();
}

void TACGenerator::translateTACA()
{
    debug("选择产生式：A-><变量说明语句>分号A|ε");
    if (currentToken.type == "变量说明")
    {
        debug("推导：A-><变量说明语句>分号A");
        translateTACDeclareStatement();
        match("分号");
        translateTACA();
    }
    else if (currentToken.type == "标识符" || currentToken.type == "if" || currentToken.type == "while")
    {
        debug("推导：A->ε");
    }
}

void TACGenerator::translateTACDeclareStatement()
{
    debug("推导：<变量说明语句>->变量说明<标识符列表>");
    std::string type = currentToken.value;
    match("变量说明");
    translateTACVarList(type);
}

void TACGenerator::translateTACVarList(const std::string& type)
{
    debug("推导：<标识符列表>->标识符B");
    std::string name = currentToken.value;
    match("标识符");
    
    // 语义检查和添加到标识符表
    if (!idenTable.Add(name))
    {
        debug("语义错误：标识符 " + name + " 已声明");
        throw std::runtime_error("Identifier already declared: " + name);
    }
    
    if (!idenTable.UpdateTypeByName(name, type))
    {
        debug("语义错误：无法更新标识符 " + name + " 的类型");
        throw std::runtime_error("Failed to update type for identifier: " + name);
    }
    
    // 生成声明的TAC指令
    tacTable.generate("declare", type, "null", name + ".type");
    
    translateTACB(type);
}

void TACGenerator::translateTACB(const std::string& type)
{
    debug("选择产生式：B->逗号 标识符 B |ε");
    if (currentToken.type == "逗号")
    {
        debug("推导：B->逗号 标识符 B");
match("逗号");
        
        std::string name = currentToken.value;
        match("标识符");
        
        // 语义检查和添加到标识符表
        if (!idenTable.Add(name))
        {
            debug("语义错误：标识符 " + name + " 已声明");
            throw std::runtime_error("Identifier already declared: " + name);
        }
        
        if (!idenTable.UpdateTypeByName(name, type))
        {
            debug("语义错误：无法更新标识符 " + name + " 的类型");
            throw std::runtime_error("Failed to update type for identifier: " + name);
        }
        
        // 生成声明的TAC指令
        tacTable.generate("declare", type, "null", name + ".type");
        
        translateTACB(type);
    }
    else if (currentToken.type == "分号")
    {
        debug("推导：B->ε");
    }
}

void TACGenerator::translateTACStatementSection()
{
    debug("推导：<语句部分>-><语句>C");
    translateTACStatement();
    translateTACC();
}

void TACGenerator::translateTACC()
{
    debug("选择产生式：C->分号<语句>C|ε");
    if (currentToken.type == "分号")
    {
        debug("推导：C->分号<语句>C");
        match("分号");
        translateTACStatement();
        translateTACC();
    }
    else if (currentToken.type == "#" || currentToken.type == "end")
    {
        debug("推导：C->ε");
    }
}

void TACGenerator::translateTACStatement()
{
    debug("选择产生式：<语句>-><赋值语句>|<条件语句>|<循环语句>");
    if (currentToken.type == "标识符")
    {
        debug("推导：<语句>-><赋值语句>");
        translateTACAssignmentStatement();
    }
    else if (currentToken.type == "if")
    {
        debug("推导：<语句>-><条件语句>");
        translateTACConditionalStatement();
    }
    else if (currentToken.type == "while")
    {
        debug("推导：<语句>-><循环语句>");
        translateTACLoopStatement();  // 添加循环语句处理
    }
}

void TACGenerator::translateTACAssignmentStatement()
{
    debug("推导：<赋值语句>->标识符 赋值号 <表达式>");
    std::string name = currentToken.value;
    match("标识符");
    match("赋值号");
    
    triple Exp = translateTACExpression();
    
    // 更新标识符类型
    idenTable.UpdateTypeByName(name, Exp.type);
    
    // 生成赋值TAC指令
    tacTable.generate("assign", Exp.name, "null", name);
}

triple TACGenerator::translateTACExpression()
{
    debug("推导：<表达式>-> <conjunction> D");
    triple E1 = translateTACConjunction();
    triple E = translateTACD(E1);
    return E;
}

triple TACGenerator::translateTACD(const triple& E1)
{
    debug("选择产生式：D-> or <conjunction> D| ε");
    if (currentToken.type == "or")
    {
        debug("推导：D-> or <conjunction> D");
        match("or");
        
        triple E2 = translateTACConjunction();
        
        // 创建临时变量
        triple T = tempVarTable.createNewVariable();
        T.type = "bool";
        
        // 简单的语义分析：假设都是bool类型
        if (E1.type != "bool" || E2.type != "bool")
        {
            debug("语义错误：or运算需要bool类型操作数");
            throw std::runtime_error("OR operation requires boolean operands");
        }
        
        // 生成or运算的TAC指令
        tacTable.generate("or", E1.name, E2.name, T.name);
        
        return translateTACD(T);
    }
    else if (currentToken.type == "分号" || currentToken.type == "#" || 
             currentToken.type == "end" || currentToken.type == "右括号")
    {
        debug("推导：D->ε");
        return E1;
    }
    
    return E1;
}

triple TACGenerator::translateTACConjunction()
{
    debug("推导：<conjunction>-><inversion> G");
    triple E1 = translateTACInversion();
    triple E = translateTACG(E1);
    return E;
}

triple TACGenerator::translateTACG(const triple& E1)
{
    debug("选择产生式：G-> and <inversion> G | ε");
    if (currentToken.type == "and")
    {
        debug("推导：G-> and <inversion> G");
        match("and");
        
        triple E2 = translateTACInversion();
        
        // 创建临时变量
        triple T = tempVarTable.createNewVariable();
        T.type = "bool";
        
        // 语义分析：检查操作数类型
        if (E1.type != "bool" || E2.type != "bool")
        {
            debug("语义错误：and运算需要bool类型操作数");
            throw std::runtime_error("AND operation requires boolean operands");
        }
        
        // 生成and运算的TAC指令
        tacTable.generate("and", E1.name, E2.name, T.name);
        
        return translateTACG(T);
    }
    else if (currentToken.type == "分号" || currentToken.type == "#" || 
             currentToken.type == "end" || currentToken.type == "右括号")
    {
        debug("推导：G->ε");
        return E1;
    }
    
    return E1;
}

triple TACGenerator::translateTACInversion()
{
    debug("选择产生式：<inversion>-> not <inversion> | <关系表达式>");
    if (currentToken.type == "not")
    {
        debug("推导：<inversion>-> not <inversion>");
        match("not");
        
        triple E1 = translateTACInversion();
        
        // 创建临时变量
        triple T = tempVarTable.createNewVariable();
        T.type = "bool";
        
        // 语义分析：检查操作数类型
        if (E1.type != "bool")
        {
            debug("语义错误：not运算需要bool类型操作数");
            throw std::runtime_error("NOT operation requires boolean operand");
        }
        
        // 生成not运算的TAC指令
        tacTable.generate("not", E1.name, "null", T.name);
        
        return T;
    }
    else if (currentToken.type == "标识符")
    {
        debug("推导：<inversion>-> <关系表达式>");
        return translateTACRelationExpression();
    }
    
    return triple("error");
}

triple TACGenerator::translateTACRelationExpression()
{
    debug("推导：<关系表达式>-> <算术表达式> E");
    triple E1 = translateTACMathExpression();
    triple E = translateTACE(E1);
    return E;
}

triple TACGenerator::translateTACE(const triple& E1)
{
    debug("选择产生式：E-> 关系 <算术表达式> | ε ");
    if (currentToken.type == "关系")
    {
        debug("推导：E-> 关系 <算术表达式>");
        std::string op = currentToken.value;
        match("关系");
        
        triple E2 = translateTACMathExpression();
        
        // 创建临时变量
        triple T = tempVarTable.createNewVariable();
        T.type = "bool";
        
        // 语义分析：检查操作数类型是否一致
        if (E1.type != E2.type)
        {
            debug("语义错误：关系运算操作数类型不匹配");
            throw std::runtime_error("Relational operation operands type mismatch");
        }
        
        // 转换关系运算符为TAC指令
        std::string tac_op;
        if (op == "<") tac_op = "lt";
        else if (op == ">") tac_op = "gt";
        else if (op == "<>") tac_op = "noteq";
        else if (op == "==") tac_op = "eq";
        else if (op == "<=") tac_op = "le";
        else if (op == ">=") tac_op = "ge";
        else
        {
            debug("语义错误：未知的关系运算符 " + op);
            throw std::runtime_error("Unknown relational operator: " + op);
        }
        
        // 生成关系运算的TAC指令
        tacTable.generate(tac_op, E1.name, E2.name, T.name);
        
        return T;
    }
    else if ( currentToken.type == "and" )
    {
        debug("推导：E->ε");
        return E1;
    }
    
    return E1;
}

triple TACGenerator::translateTACMathExpression()
{
    debug("推导：<算术表达式> -> <term> H ");
    triple E1 = translateTACTerm();
    triple E = translateTACH(E1);
    return E;
}

triple TACGenerator::translateTACH(const triple& E1)
{
    debug("选择产生式：H-> 加法 <term> H | 减法 <term> H | ε ");
    if (currentToken.type == "加法" || currentToken.type == "减法")
    {
        debug("推导：H-> " + currentToken.type + " <term> H");
        std::string op = currentToken.value;
        std::string opType = currentToken.type;
        match(opType);
        
        triple E2 = translateTACTerm();
        
        // 创建临时变量
        triple T = tempVarTable.createNewVariable();
        T.type = "int";
        
        // 语义分析：检查操作数类型
        if (E1.type != "int" || E2.type != "int")
        {
            debug("语义错误：算术运算需要int类型操作数，当前操作数类型为: " + E1.type + " 和 " + E2.type);
            throw std::runtime_error("Arithmetic operation requires integer operands. Got: " + E1.type + " and " + E2.type);
        }
        
        // 转换运算符为TAC指令
        std::string tac_op;
        if (op == "+") tac_op = "add";
        else if (op == "-") tac_op = "sub";
        else
        {
            debug("语义错误：未知的算术运算符 " + op);
            throw std::runtime_error("Unknown arithmetic operator: " + op);
        }
        
        // 生成算术运算的TAC指令
        tacTable.generate(tac_op, E1.name, E2.name, T.name);
        
        return translateTACH(T);
    }
    else if (currentToken.type == "关系" || currentToken.type == "右括号" || 
             currentToken.type == "分号" || currentToken.type == "#" || 
             currentToken.type == "end" || currentToken.type == "and" || 
             currentToken.type == "or")
    {
        debug("推导：H->ε");
        return E1;
    }
    
    return E1;
}

triple TACGenerator::translateTACTerm()
{
    debug("推导：<term>-><factor> I");
    triple E1 = translateTACFactor();
    triple E = translateTACI(E1);
    return E;
}

triple TACGenerator::translateTACI(const triple& E1)
{
    debug("选择产生式：I-> 乘法 <factor> I | ε ");
    if (currentToken.type == "乘法")
    {
        debug("推导：I-> 乘法 <factor> I");
        std::string op = currentToken.value;
        match("乘法");
        
        triple E2 = translateTACFactor();
        
        // 创建临时变量
        triple T = tempVarTable.createNewVariable();
        T.type = "int";
        
        // 语义分析：检查操作数类型
        if (E1.type != "int" || E2.type != "int")
        {
            debug("语义错误：乘法运算需要int类型操作数");
            throw std::runtime_error("Multiplication operation requires integer operands");
        }
        
        // 转换运算符为TAC指令
        std::string tac_op;
        if (op == "*") tac_op = "mul";
        else if (op == "/") tac_op = "div";
        else if (op == "%") tac_op = "mod";
        else
        {
            debug("语义错误：未知的乘法运算符 " + op);
            throw std::runtime_error("Unknown multiplication operator: " + op);
        }
        
        // 生成乘法运算的TAC指令
        tacTable.generate(tac_op, E1.name, E2.name, T.name);
        
        return translateTACI(T);
    }
    else if (currentToken.type == "加法" || currentToken.type == "关系")
    {
        debug("推导：I->ε");
        return E1;
    }
    
    return E1;
}

triple TACGenerator::translateTACFactor()
{
    debug("选择产生式：<factor>-> 标识符|true|false| 整数 | 字符串 |左括号 <表达式> 右括号 ");
    
    if (currentToken.type == "标识符")
    {
        debug("推导：<factor>-> 标识符");
        std::string name = currentToken.value;
        match("标识符");
        
        // 语义检查：检查标识符是否已声明
        if (!idenTable.identifierExists(name))
        {
            debug("语义错误：标识符 " + name + " 未声明");
            throw std::runtime_error("Identifier not declared: " + name);
        }
        
        // 获取标识符类型
        std::string type = idenTable.getTypeByName(name);
        return triple(name, type, name);  // 使用正确的构造函数
    }
    else if (currentToken.type == "true" || currentToken.type == "false")
    {
        debug("推导：<factor>-> " + currentToken.type);
        std::string value = currentToken.value;
        std::string type = "bool";
        match(currentToken.type);
        return triple(value, type, value);  // 使用正确的构造函数
    }
    else if (currentToken.type == "整数")
    {
        debug("推导：<factor>-> 整数");
        std::string value = currentToken.value;
        std::string type = "int";
        match("整数");
        return triple(value, type, value);
    }
    else if (currentToken.type == "字符串")
    {
        debug("推导：<factor>-> 字符串");
        std::string value = currentToken.value;
        std::string type = "string";
        match("字符串");
        return triple(value, type, value);
    }
    else if (currentToken.type == "左括号")
    {
        debug("推导：<factor>-> 左括号 <表达式> 右括号");
        match("左括号");
        
        triple expression = translateTACExpression();
        
        match("右括号");
        
        return expression;
    }
    else {
        debug("错误：无法识别的factor类型: " + currentToken.type);
        throw std::runtime_error("Unexpected token in factor: " + currentToken.type);
    }
}

triple TACGenerator::translateTACString()
{
    debug("推导：<字符串>-> 引号 <字母序列> 引号");
    match("字符串");
    triple t = tempVarTable.createNewVariable();
    t.type = "string";
    t.value = currentToken.value;
    return t;
}

void TACGenerator::translateTACConditionalStatement()
{
    debug("推导：<条件语句>-> if 左括号 <表达式> 右括号 <复合语句> else <复合语句>");
    match("if");
    match("左括号");
    
    triple T = translateTACExpression();
    
    // 语义分析：检查条件表达式是否为bool类型
    if (T.type != "bool")
    {
        debug("语义错误：条件表达式必须是bool类型");
        throw std::runtime_error("Condition expression must be boolean type");
    }
    
    // 生成条件跳转TAC指令
    tacTable.generate("jnz", T.name, "null", std::to_string(tacTable.NXQ() + 2));
    int falseIndex = tacTable.NXQ();
    tacTable.generate("jump", "null", "null", "0");
    
    match("右括号");
    
    translateTACCompoundStatement();
    
    int exitIndex = tacTable.NXQ();
    tacTable.generate("jump", "null", "null", "0");
    
    // 回填false分支的目标地址
    tacTable.backpatch(falseIndex, std::to_string(tacTable.NXQ() + 1));
    
    match("else");
    
    translateTACCompoundStatement();
    
    // 回填exit跳转的目标地址
    tacTable.backpatch(exitIndex, std::to_string(tacTable.NXQ() + 1));
}

void TACGenerator::translateTACCompoundStatement()
{
    debug("推导：<复合语句>-> begin <语句部分> end");
    match("begin");
    translateTACStatementSection();
    match("end");
}

// 添加循环语句处理函数
void TACGenerator::translateTACLoopStatement()
{
    debug("推导：<循环语句>->while 左括号 <表达式> 右括号 冒号 <嵌套语句>");
    
    match("while");
    match("左括号");
    
    // 记录循环开始位置
    int loopStart = tacTable.NXQ() + 1;
    
    triple condition = translateTACExpression();
    
    // 语义分析：检查条件表达式是否为bool类型
    if (condition.type != "bool")
    {
        debug("语义错误：循环条件表达式必须是bool类型");
        throw std::runtime_error("Loop condition expression must be boolean type");
    }
    
    // 生成条件跳转TAC指令
    tacTable.generate("jnz", condition.name, "null", std::to_string(tacTable.NXQ() + 2));
    int falseIndex = tacTable.NXQ();
    tacTable.generate("jump", "null", "null", "0");
    
    match("右括号");
    match("冒号");
    
    // 处理循环体
    translateTACNestedStatement();
    
    // 生成跳转回循环开始的指令
    tacTable.generate("jump", "null", "null", std::to_string(loopStart));
    
    // 回填false分支的目标地址（跳出循环）
    tacTable.backpatch(falseIndex, std::to_string(tacTable.NXQ() + 1));
}

// 添加嵌套语句处理函数
void TACGenerator::translateTACNestedStatement()
{
    debug("选择产生式：<嵌套语句>-><语句> 分号 | <复合语句>");
    
    if (currentToken.type == "begin") {
        debug("推导：<嵌套语句>-><复合语句>");
        translateTACCompoundStatement();
    } else {
        debug("推导：<嵌套语句>-><语句> 分号");
        translateTACStatement();
        match("分号");
    }
}
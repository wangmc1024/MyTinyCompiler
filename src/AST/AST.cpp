#include "AST.hpp"
#include <cctype>
#include <stdexcept>

node ASTSemanticAnalyzer::translateASTProgram()
{
    debug("推导：<程序> -> <变量说明部分><语句部分>");
    node programNode("topLevel");
    
    node declareSection = translateASTDeclareSection();
    node statementSection = translateASTStatementSection();
    
    programNode.addChild(declareSection);
    programNode.addChild(statementSection);
    
    debug("语法分析结束");
    return programNode;
}

node ASTSemanticAnalyzer::translateASTDeclareSection()
{
    debug("推导：<变量说明部分>-><变量声明语句>分号A");
    node declareSectionNode("declareList");
    
    node declareStatement = translateASTDeclareStatement();
    match("分号");
    node A = translateASTA();
    
    declareSectionNode.addChild(declareStatement);
    declareSectionNode.addChild(node("分号"));
    declareSectionNode.addChild(A);
    
    return declareSectionNode;
}

node ASTSemanticAnalyzer::translateASTA()
{
    debug("选择产生式：A-><变量说明语句>分号A|ε");
    node ANode("linkDeclareList");
    
    if (currentToken.type == "变量说明")
    {
        debug("推导：A-><变量说明语句>分号A");
        node declareStatement = translateASTDeclareStatement();
        match("分号");
        node AChild = translateASTA();
        
        ANode.addChild(declareStatement);
        ANode.addChild(AChild);
    }
    else if (currentToken.type == "标识符" || currentToken.type == "if" || currentToken.type == "while")
    {
        debug("推导：A->ε");
        ANode = node("emptyDeclareList");
    }
    
    return ANode;
}

node ASTSemanticAnalyzer::translateASTDeclareStatement()
{
    debug("推导：<变量说明语句>->变量说明<标识符列表>");
    node declareStatementNode("declare");
    
    std::string type = currentToken.value;
    node typeNode(type);
    match("变量说明");
    
    node varList = translateASTVarList(type);
    
    declareStatementNode.addChild(typeNode);
    declareStatementNode.addChild(varList);
    
    return declareStatementNode;
}

node ASTSemanticAnalyzer::translateASTVarList(const std::string& type)
{
    debug("推导：<标识符列表>->标识符B");
    node varListNode("varList");
    
    std::string name = currentToken.value;
    node idNode(name);
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
    
    node B = translateASTB(type);
    
    varListNode.addChild(idNode);
    varListNode.addChild(B);
    
    return varListNode;
}

node ASTSemanticAnalyzer::translateASTB(const std::string& type)
{
    debug("选择产生式：B->逗号 标识符 B |ε");
    node BNode("linkVarList");
    
    if (currentToken.type == "逗号")
    {
        debug("推导：B->逗号 标识符 B");
 
        match("逗号");
        
        std::string name = currentToken.value;
        node idNode(currentToken.value);
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
        
        node BChild = translateASTB(type);
        
        BNode.addChild(idNode);
        BNode.addChild(BChild);
    }
    else if (currentToken.type == "分号")
    {
        debug("推导：B->ε");
        BNode = node("emptyVarList");
    }
    
    return BNode;
}

node ASTSemanticAnalyzer::translateASTStatementSection()
{
    debug("推导：<语句部分>-><语句>C");
    node statementSectionNode("seqList");
    
    node statement = translateASTStatement();
    node C = translateASTC();
    
    statementSectionNode.addChild(statement);
    statementSectionNode.addChild(C);
    
    return statementSectionNode;
}

node ASTSemanticAnalyzer::translateASTC()
{
    debug("选择产生式：C->分号<语句>C|ε");
    node CNode("linkSeqList");
    
    if (currentToken.type == "分号")
    {
        debug("推导：C->分号<语句>C");
        match("分号");
        
        node statement = translateASTStatement();
        node CChild = translateASTC();
        
        CNode.addChild(statement);
        CNode.addChild(CChild);
    }
    else if (currentToken.type == "#" || currentToken.type == "end")
    {
        debug("推导：C->ε");
        CNode = node("emptySeqList");
    }
    
    return CNode;
}

node ASTSemanticAnalyzer::translateASTStatement()
{
    debug("选择产生式：<语句>-><赋值语句>|<条件语句>|<循环语句>");
    node statementNode("Statement");
    
    if (currentToken.type == "标识符")
    {
        debug("推导：<语句>-><赋值语句>");
        node assignmentStmt = translateASTAssignmentStatement();
        statementNode.addChild(assignmentStmt);
    }
    else if (currentToken.type == "if")
    {
        debug("推导：<语句>-><条件语句>");
        node conditionalStmt = translateASTConditionalStatement();
        statementNode.addChild(conditionalStmt);
    }
    else if (currentToken.type == "while")
    {
        debug("推导：<语句>-><循环语句>");
        node loopStmt = translateASTLoopStatement();
        statementNode.addChild(loopStmt);
    }
    
    return statementNode;
}

node ASTSemanticAnalyzer::translateASTAssignmentStatement()
{
    debug("推导：<赋值语句>->标识符 赋值号 <表达式>");
    node assignmentNode("assign");
    
    std::string name = currentToken.value;
    node idNode(currentToken.value);
    match("标识符");
    

    match("赋值号");
    
    node expression = translateASTExpression();
    
    if (!idenTable.identifierExists(name))
    {
        debug("语义错误：标识符 " + name + " 未声明");
        throw std::runtime_error("Identifier not declared: " + name);
    }
    
    assignmentNode.addChild(idNode);
    assignmentNode.addChild(expression);
    
    return assignmentNode;
}

#pragma region 表达式相关函数
//----------------------------------------------------------------------

node ASTSemanticAnalyzer::translateASTExpression()
{
    debug("推导：<表达式>-> <conjunction> D");
    node expressionNode("expression");
    
    node conjunction = translateASTConjunction();
    node D = translateASTD(conjunction);
    
    expressionNode.addChild(conjunction);
    expressionNode.addChild(D);
    
    return expressionNode;
}

node ASTSemanticAnalyzer::translateASTD(const node& E1)
{
    debug("选择产生式：D-> or <conjunction> D| ε");
    node DNode("D");
    
    if (currentToken.type == "or")
    {
        debug("推导：D-> or <conjunction> D");
        node orNode(currentToken.value);
        match("or");
        
        node conjunction = translateASTConjunction();
        node DChild = translateASTD(conjunction);
        
        DNode.addChild(orNode);
        DNode.addChild(conjunction);
        DNode.addChild(DChild);
    }
    else if (currentToken.type == "分号" || currentToken.type == "#" || 
             currentToken.type == "end" || currentToken.type == "右括号"
            )
    {
        debug("推导：D->ε");
        DNode = node("ε");
    }
    
    return DNode;
}

node ASTSemanticAnalyzer::translateASTConjunction()
{
    debug("推导：<conjunction>-><inversion> G");
    node conjunctionNode("conjunction");
    
    node inversion = translateASTInversion();
    node G = translateASTG(inversion);
    
    conjunctionNode.addChild(inversion);
    conjunctionNode.addChild(G);
    
    return conjunctionNode;
}

node ASTSemanticAnalyzer::translateASTG(const node& E1)
{
    debug("选择产生式：G-> and <inversion> G | ε");
    node GNode("G");
    
    if (currentToken.type == "and")
    {
        debug("推导：G-> and <inversion> G");
        node andNode(currentToken.value);
        match("and");
        
        node inversion = translateASTInversion();
        node GChild = translateASTG(inversion);
        
        GNode.addChild(andNode);
        GNode.addChild(inversion);
        GNode.addChild(GChild);
    }
    else if (currentToken.type == "分号" || 
             currentToken.type == "or" ||
             currentToken.type == "#" || 
             currentToken.type == "end" || 
             currentToken.type == "右括号"
             )
    {
        debug("推导：G->ε");
        GNode = node("ε");
    }
    
    return GNode;
}

node ASTSemanticAnalyzer::translateASTInversion()
{
    debug("选择产生式：<inversion>-> not <inversion> | <关系表达式>");
    node inversionNode("inversion");
    
    if (currentToken.type == "not")
    {
        debug("推导：<inversion>-> not <inversion>");
        node notNode(currentToken.value);
        match("not");
        
        node inversionChild = translateASTInversion();
        
        inversionNode.addChild(notNode);
        inversionNode.addChild(inversionChild);
    }
    else
    {
        debug("推导：<inversion>-> <关系表达式>");
        node relationExpr = translateASTRelationExpression();
        inversionNode.addChild(relationExpr);
    }
    
    return inversionNode;
}

node ASTSemanticAnalyzer::translateASTRelationExpression()
{
    debug("推导：<关系表达式>-> <算术表达式> E");
    node relationExprNode("关系表达式");
    
    node mathExpr = translateASTMathExpression();
    node E = translateASTE(mathExpr);
    
    relationExprNode.addChild(mathExpr);
    relationExprNode.addChild(E);
    
    return relationExprNode;
}

node ASTSemanticAnalyzer::translateASTE(const node& E1)
{
    debug("选择产生式：E-> 关系 <算术表达式> E | ε ");
    node ENode("E");
    
    if (currentToken.type == "关系")
    {
        debug("推导：E-> 关系 <算术表达式> E");
        node relNode(currentToken.value);
        match("关系");
        
        node mathExpr = translateASTMathExpression();
        node EChild = translateASTE(mathExpr);
        
        ENode.addChild(relNode);
        ENode.addChild(mathExpr);
        ENode.addChild(EChild);
    }
    else if (currentToken.type == "and" || currentToken.type == "or" ||
             currentToken.type == "分号" || currentToken.type == "#" || 
             currentToken.type == "end" || currentToken.type == "右括号")
    {
        debug("推导：E->ε");
        ENode = node("ε");
    }
    
    return ENode;
}

node ASTSemanticAnalyzer::translateASTMathExpression()
{
    debug("推导：<算术表达式> -> <term> H ");
    node mathExprNode("算术表达式");
    
    node term = translateASTTerm();
    node H = translateASTH(term);
    
    mathExprNode.addChild(term);
    mathExprNode.addChild(H);
    
    return mathExprNode;
}

node ASTSemanticAnalyzer::translateASTTerm()
{
    debug("推导：<term>-><factor> I");
    node termNode("term");
    
    node factor = translateASTFactor();
    node I = translateASTI(factor);
    
    termNode.addChild(factor);
    termNode.addChild(I);
    
    return termNode;
}

node ASTSemanticAnalyzer::translateASTH(const node& E1)
{
    debug("选择产生式：H-> 加法 <term> H | ε ");
    node HNode("H");
    
    if (currentToken.type == "加法" )
    {
        debug("推导：H-> " + currentToken.type + " <term> H");
        node opNode(currentToken.value);
        std::string opType = currentToken.type;
        match(opType);
        
        node term = translateASTTerm();
        node HChild = translateASTH(term);
        
        HNode.addChild(opNode);
        HNode.addChild(term);
        HNode.addChild(HChild);
    }
    else if (currentToken.type == "关系" || currentToken.type == "and" || 
             currentToken.type == "or" || currentToken.type == "分号" ||
             currentToken.type == "#" || currentToken.type == "end" || 
             currentToken.type == "右括号")
    {
        debug("推导：H->ε");
        HNode = node("ε");
    }
    
    return HNode;
}

node ASTSemanticAnalyzer::translateASTI(const node& E1)
{
    debug("选择产生式：I-> 乘法 <factor> I | ε ");
    node INode("I");
    
    if (currentToken.type == "乘法")
    {
        debug("推导：I-> 乘法 <factor> I");
        node mulNode(currentToken.value);
        match("乘法");
        
        node factor = translateASTFactor();
        node IChild = translateASTI(factor);
        
        INode.addChild(mulNode);
        INode.addChild(factor);
        INode.addChild(IChild);
    }
    else if (currentToken.type == "加法" ||
             currentToken.type == "关系" || currentToken.type == "and" || 
             currentToken.type == "or" || currentToken.type == "分号" ||
             currentToken.type == "#" || currentToken.type == "end" || 
             currentToken.type == "右括号")
    {
        debug("推导：I->ε");
        INode = node("ε");
    }
    
    return INode;
}

node ASTSemanticAnalyzer::translateASTFactor()
{
    debug("选择产生式：<factor>-> 标识符|true|false| 整数 | 字符串 |左括号 <表达式> 右括号 ");
    node factorNode("factor");
    
    if (currentToken.type == "标识符")
    {
        debug("推导：<factor>-> 标识符");
        node idNode(currentToken.value);
        std::string name = currentToken.value;
        match("标识符");
        
        // 语义检查：检查标识符是否已声明
        if (!idenTable.identifierExists(name))
        {
            debug("语义错误：标识符 " + name + " 未声明");
            throw std::runtime_error("Identifier not declared: " + name);
        }
        
        factorNode.addChild(idNode);
    }
    else if (currentToken.type == "true" || currentToken.type == "false")
    {
        debug("推导：<factor>-> " + currentToken.type);
        node boolNode(currentToken.value);
        match(currentToken.type);
        factorNode.addChild(boolNode);
    }
    else if (currentToken.type == "整数")
    {
        debug("推导：<factor>-> 整数");
        node intNode(currentToken.value);
        match("整数");
        factorNode.addChild(intNode);
    }
    else if (currentToken.type == "字符串")
    {
        debug("推导：<factor>-> 字符串");
        node stringNode(currentToken.value);
        match("字符串");
        factorNode.addChild(stringNode);
    }
    else if (currentToken.type == "左括号")
    {
        debug("推导：<factor>-> 左括号 <表达式> 右括号");
        node leftParen("左括号");
        match("左括号");
        
        node expression = translateASTExpression();
        
        node rightParen("右括号");
        match("右括号");
        
        factorNode.addChild(leftParen);
        factorNode.addChild(expression);
        factorNode.addChild(rightParen);
    }
    else {
        debug("错误：无法识别的factor类型: " + currentToken.type);
        throw std::runtime_error("Unexpected token in factor: " + currentToken.type);
    }
    
    return factorNode;
}

#pragma endregion 表达式相关函数

//-------------------------------------------------------------
node ASTSemanticAnalyzer::translateASTConditionalStatement()
{
    debug("推导：<条件语句>-> if 左括号 <表达式> 右括号 <嵌套语句> else <嵌套语句>");
    node condStmtNode("ifThenElse");
    
 
    match("if");
    
    match("左括号");
    
    node expression = translateASTExpression();

    match("右括号");
    
    node nestedStmt1 = translateASTNestedStatement();
    
    match("else");
    
    node nestedStmt2 = translateASTNestedStatement();

    condStmtNode.addChild(expression);
    condStmtNode.addChild(nestedStmt1);
    condStmtNode.addChild(nestedStmt2);
    
    return condStmtNode;
}

node ASTSemanticAnalyzer::translateASTLoopStatement()
{
    debug("推导：<循环语句>->while 左括号 <表达式> 右括号 冒号 <嵌套语句>");
    node loopStmtNode("while");
    
    match("while");

    match("左括号");
    
    node expression = translateASTExpression();

    match("右括号");
    

    match("冒号");
    
    node nestedStmt = translateASTNestedStatement();
    loopStmtNode.addChild(expression);
    loopStmtNode.addChild(nestedStmt);
    
    return loopStmtNode;
}

node ASTSemanticAnalyzer::translateASTNestedStatement()
{
    debug("选择产生式：<嵌套语句>-><语句> 分号 | <复合语句>");
    node nestedStmtNode("嵌套语句");
    
    if (currentToken.type == "标识符" || currentToken.type == "if" || currentToken.type == "while")
    {
        debug("推导：<嵌套语句>-><语句> 分号");
        node statement = translateASTStatement();
 
        match("分号");
        
        nestedStmtNode.addChild(statement);

    }
    else if (currentToken.type == "begin")
    {
        debug("推导：<嵌套语句>-><复合语句>");
    }
    
    return nestedStmtNode;
}

node ASTSemanticAnalyzer::translateASTCompoundStatement()
{
    debug("推导：<复合语句>-> begin <语句部分> end");
    node compoundStmtNode("复合语句");
    

    match("begin");
    
    node statementSection = translateASTStatementSection();

    match("end");
    compoundStmtNode.addChild(statementSection);
    return compoundStmtNode;
}
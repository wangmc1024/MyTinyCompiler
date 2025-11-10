#include "lexical.hpp"
#include <cctype>

std::string Token::toString() const {
    return "(" + type + ", " + value + ")";
}


Lexer::Lexer(std::string source) 
    : sourceProgram(std::move(source) + "#"), 
      pointer(0) {}


Token Lexer::nextInput() {
    int state = 0;         
    std::string tmpWord;   

    while (pointer < sourceProgram.size()) {
        char symbol = sourceProgram[pointer];

        // 跳过空白字符
        if (isspace(symbol)) {
            pointer++;
            continue;
        }

        switch (state) {
            case 0: 
                if (symbol == '$') {
                    state = 100;
                    tmpWord += symbol;
                    pointer++;
                } else if (isdigit(symbol)) {
                    // 整数以数字开头，进入整数识别状态
                    state = 200;
                    tmpWord += symbol;
                    pointer++;
                }else if(symbol == ';'){
                    pointer++;
                    return Token("分号", tmpWord + symbol);
                }
                 else if (symbol == 'i') {
                    // 可能是"if"或"in"(变量说明)，进入i开头关键字状态
                    state = 400;
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == 'b') {
                    // 可能是"bool"(变量说明)，进入b开头关键字状态
                    state = 500;
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == ',') {
                    pointer++;
                    return Token("逗号", tmpWord + symbol);
                } else if (symbol == ':') {
                    // 可能是赋值号":="，进入赋值号识别状态
                    state = 700;
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == 'o') {
                    // 可能是"or"，进入o开头关键字状态
                    state = 800;
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == 'n') {
                    // 可能是"not"，进入n开头关键字状态
                    state = 900;
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == 't') {
                    // 可能是"true"，进入t开头关键字状态
                    state = 1000;
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == 'f') {
                    // 可能是"false"，进入f开头关键字状态
                    state = 1100;
                    tmpWord += symbol;
                    pointer++;
                }else if(symbol == '('){
                    pointer++;
                    return Token("左括号", tmpWord + symbol);
                } else if(symbol == ')'){
                    pointer++;
                    return Token("右括号", tmpWord + symbol);
                }
                 else if (symbol == '<') {
                    // 关系运算符(<或<=)，进入<识别状态
                    state = 1400;
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == '>') {
                    state = 1410;
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == '=') {
                    // 可能是==，进入=识别状态
                    state = 1420;
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == '*' || symbol == '/' || symbol == '%') {
                    // 乘法运算符(*或/)，直接返回
                    pointer++;
                    return Token("乘法", tmpWord + symbol);
                } else if (symbol == 'e') {
                    // 可能是"else"，进入e开头关键字状态
                    state = 1700;
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == '{') {
                    // begin标记({)，直接返回
                    pointer++;
                    return Token("begin", tmpWord + symbol);
                } else if (symbol == '}') {
                    pointer++;
                    return Token("end", tmpWord + symbol);
                }else if(symbol == '+' || symbol == '-'){
                    pointer++;
                    return Token("加法", tmpWord + symbol);
                }else if(symbol == 'a'){
                    state = 3000;
                    tmpWord += symbol;
                    pointer++;
                }else if(symbol == 's'){
                    state = 4000;
                    tmpWord += symbol;
                    pointer++;
                }else if(symbol == 'w'){
                    state = 5000;
                    tmpWord += symbol;
                    pointer++;
                }
                else if(symbol == '"'){
                    state = 6000;
                    tmpWord += symbol;
                    pointer++;
                }
                else if(symbol == '#'){
                    // 直接识别为结束符号#
                    pointer++;
                    return Token("#", "#");
                }
                else {
                    // 无法识别的符号，返回错误
                    pointer++;
                    return Token("error", "状态0无法识别的符号:" + std::string(1, symbol));
                }
                break;

            case 100:  // 标识符状态(已读取$，等待后续字符)
                if (islower(symbol)) {
                    // 标识符中间为小写字母，进入下一级状态
                    state = 101;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    // 标识符格式错误($后不是小写字母)
                    pointer++;
                    return Token("error", "标识符格式错误($后需跟小写字母)");
                }
                break;

            case 101:  // 标识符中间状态(可包含字母或数字)
                if (islower(symbol) || isdigit(symbol)) {
                    // 继续添加字符
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == '$') {
                    // 标识符以$结尾，识别完成
                    tmpWord += symbol;
                    pointer++;
                    return Token("标识符", tmpWord);
                } else {
                    // 标识符未正确结束(未以$结尾)
                    return Token("error", "标识符未以$结尾");
                }
                break;

            case 200:  // 整数状态(读取数字序列)
                if (isdigit(symbol)) {
                    // 继续添加数字
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("整数", tmpWord);
                }
                break;

            case 400:  // i开头关键字(可能是if或in)
                if (symbol == 'n') {
                    // 可能是"in"(变量说明)，进入下一级状态
                    state = 401;
                    tmpWord += symbol;
                    pointer++;
                } else if (symbol == 'f') {
                    // 识别为"if"
                    tmpWord += symbol;
                    pointer++;
                    return Token("if", tmpWord);
                } else {
                    return Token("error", "i开头关键字格式错误");
                }
                break;

            case 401:  // in状态(验证是否为"int")
                if (symbol == 't') {
                    // 识别为"int"(变量说明)
                    tmpWord += symbol;
                    pointer++;
                    return Token("变量说明", tmpWord);
                } else {
                    return Token("error", "in后需跟t(应为int)");
                }
                break;

            case 500:  // b开头关键字(可能是bool)
                if (symbol == 'o') {
                    state = 501;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "b开头关键字格式错误(应为bool)");
                }
                break;

            case 501:  // bo状态(继续验证bool)
                if (symbol == 'o') {
                    state = 502;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "bo后需跟o(应为bool)");
                }
                break;

            case 502:  // boo状态(继续验证bool)
                if (symbol == 'l') {
                    // 识别为"bool"(变量说明)
                    tmpWord += symbol;
                    pointer++;
                    return Token("变量说明", tmpWord);
                } else {
                    return Token("error", "boo后需跟l(应为bool)");      
                }
                break;

            case 700:  // :状态(验证是否为:=)
                if (symbol == '=') {
                    // 识别为赋值号":="
                    tmpWord += symbol;
                    pointer++;
                    return Token("赋值号", tmpWord);
                } else {
                    return Token("冒号", tmpWord);
                }
                break;

            case 800:  // o开头关键字(验证是否为or)
                if (symbol == 'r') {
// 识别为"or"
                    tmpWord += symbol;
                    pointer++;
                    return Token("or", tmpWord);
                } else {
                    return Token("error", "o后需跟r(应为or)");
                }
                break;

            case 900:  // n开头关键字(验证是否为not)
                if (symbol == 'o') {
                    state = 901;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "n开头关键字格式错误(应为not)");
                }
                break;

            case 901:  // no状态(继续验证not)
                if (symbol == 't') {
                    // 识别为"not"
                    tmpWord += symbol;
                    pointer++;
                    return Token("not", tmpWord);
                } else {
                    return Token("error", "no后需跟t(应为not)");
                }
                break;

            case 1000:  // t开头关键字(验证是否为true)
                if (symbol == 'r') {
                    state = 1001;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "t开头关键字格式错误(应为true)");
                }
                break;

            case 1001:  // tr状态(继续验证true)
                if (symbol == 'u') {
                    state = 1002;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "tr后需跟u(应为true)");
                }
                break;

            case 1002:  // tru状态(继续验证true)
                if (symbol == 'e') {
                    // 识别为"true"
                    tmpWord += symbol;
                    pointer++;
                    return Token("true", tmpWord);
                } else {
                    return Token("error", "tru后需跟e(应为true)");
                }
                break;

            case 1100:  // f开头关键字(验证是否为false)
                if (symbol == 'a') {
                    state = 1101;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "f开头关键字格式错误(应为false)");
                }
                break;

            case 1101:  // fa状态(继续验证false)
                if (symbol == 'l') {
                    state = 1102;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "fa后需跟l(应为false)");
                }
                break;

            case 1102:  // fal状态(继续验证false)
                if (symbol == 's') {
                    state = 1103;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "fal后需跟s(应为false)");
                }
                break;

            case 1103:  // fals状态(继续验证false)
                if (symbol == 'e') {
                    // 识别为"false"
                    tmpWord += symbol;
                    pointer++;
                    return Token("false", tmpWord);
                } else {
                    return Token("error", "fals后需跟e(应为false)");
                }
                break;

            case 1400:  // <状态(验证是否为<或<=)
                if (symbol == '=') {
                    // 识别为"<="
                    tmpWord += symbol;
                    pointer++;
                    return Token("关系", tmpWord);
                }
                else if (symbol == '>'){
                    tmpWord += symbol;
                    pointer++;
                    return Token("关系", tmpWord);
                }
                else {
                    // 识别为"<"
                    return Token("关系", tmpWord);
                }
                break;
            
            case 1410:  // >状态(验证是否为>或>=)
                if (symbol == '=') {
                    // 识别为">="
                    tmpWord += symbol;
                    pointer++;
                    return Token("关系", tmpWord);
                }
                else {
                    // 识别为">"
                    return Token("关系", tmpWord);
                }
                break;

            case 1420:  // =状态(验证是否为==)
                if (symbol == '=') {
                    // 识别为"=="
                    tmpWord += symbol;
                    pointer++;
                    return Token("关系", tmpWord);
                } else {
                    return Token("error", "=后需跟=(应为==)");
                }
                break;

            case 1700:  // e开头关键字(验证是否为else)
                if (symbol == 'l') {
                    state = 1701;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "e开头关键字格式错误(应为else)");
                }
                break;

            case 1701:  // el状态(继续验证else)
                if (symbol == 's') {
                    state = 1702;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "el后需跟s(应为else)");
                }
                break;

            case 1702:  // els状态(继续验证else)
                if (symbol == 'e') {
                    // 识别为"else"
                    tmpWord += symbol;
                    pointer++;
                    return Token("else", tmpWord);
                } else {
                    return Token("error", "els后需跟e(应为else)");
                }
                break;
            case 3000:
                if(symbol == 'n'){
                    state = 3001;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "a后需跟n(应为and)");
                }
                break;
            case 3001:
                if(symbol == 'd'){
                    tmpWord += symbol;
                    pointer++;
                    return Token("and", tmpWord);
                } else {
                    return Token("error", "and后需跟d(应为and)");
                }
                break;
            //string
            case 4000:
                if(symbol == 't'){
                    state = 4001;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "s后需跟(应为string)");
                }
                break;
                
            case 4001:
                if(symbol == 'r'){
                    state = 4002;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "st后需跟r(应为string)");
                }
                break;
            case 4002:
                if(symbol == 'i'){
                    state = 4003;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "str后需跟t(应为string)");
                }
                break;
            case 4003:
                if(symbol == 'n'){
                    state = 4004;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "stri后需跟i(应为string)");
                }
                break;
            case 4004:
                if(symbol == 'g'){
                    tmpWord += symbol;
                    pointer++;
                    return Token("变量说明", tmpWord);
                } else {
                    return Token("error", "string后需跟g(应为string)");
                }
                break;
            
            //while
            case 5000:
                if(symbol == 'h'){
                    state = 5001;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "w后需跟h(应为while)");
                }
                break;
            case 5001:
                if(symbol == 'i'){
                    state = 5002;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "wh后需跟i(应为while)");
                }
                break;
            case 5002:
                if(symbol == 'l'){
                    state = 5003;
                    tmpWord += symbol;
                    pointer++;

                } else {
                    return Token("error", "whi后需跟l(应为while)");
                }
                break;
            case 5003:
                if(symbol == 'e'){
                    tmpWord += symbol;
                    pointer++;
                    return Token("while", tmpWord);
                } else {
                    return Token("error", "whil后需跟e(应为while)");
                }
                break;
            case 6000:
                if(symbol == '"'){
                    // 空字符串情况
                    tmpWord += symbol;
                    pointer++;
                    return Token("字符串", tmpWord);
                } else if(isdigit(symbol) || islower(symbol) || isupper(symbol)){
                    state = 6001;
                    tmpWord += symbol;
                    pointer++;
                } else {
                    return Token("error", "字符串格式错误: 引号后应为字母或数字");
                }
                break;
            case 6001:
                if(isdigit(symbol) || islower(symbol) || isupper(symbol)){
                    // 继续读取字符串内容
                    tmpWord += symbol;
                    pointer++;
                } else if(symbol == '"'){
                    // 字符串结束
                    tmpWord += symbol;
                    pointer++;
                    return Token("字符串", tmpWord);
                } else {
                    return Token("error", "字符串格式错误: 缺少结束引号");
                }
                break;

            default:
                pointer++;
                debug("error: 无法识别的字符: " + std::string(1, symbol));
                if (pointer >= sourceProgram.size()) {
                    return Token("error", "未知状态且已到达程序末尾");
                }
                break;
        }
    }
    if (pointer < sourceProgram.size()) {
        pointer++;
        return Token("#", "#");
    }
    
    return Token("#", "#");
}

// 调试信息添加
void Lexer::debug(const std::string& msg) {
    debugInfo.push_back(msg);
}


const std::vector<std::string>& Lexer::getDebugInfo() const {
    return debugInfo;
}


void Lexer::clearDebugInfo() {
    debugInfo.clear();
}
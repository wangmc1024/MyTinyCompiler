### 给定文法

```text
<程序> → <变量说明部分> <语句部分> 
<变量说明部分> → <变量说明部分> <变量说明语句> ; | <变量说明语句> ; 
<变量说明语句> → <变量说明> <标识符列表> 
<变量说明> → int | string | bool 
<标识符列表> → <标识符列表> , <标识符> | <标识符> 
<标识符> → $ <字母> <字母序列> $ 
<语句部分> → <语句部分> ; <语句> | <语句> 
<语句> → <赋值语句>|<条件语句>|<循环语句> 
<赋值语句> → <标识符> := <表达式> 
<表达式> → <disjunction>                    
<disjunction> → < conjunction > | <disjunction> or < conjunction > 
< conjunction > → < inversion > | < conjunction > and < inversion > 
< inversion > → not < inversion > | < 关系表达式 > 
< 关系表达式 >  → <算术表达式> <关系运算符> <算术表达式> | <算术表达式> 
<算术表达式> → <term> | <算术表达式> <加法运算符> <term> 
<term> → <factor>| <term> <乘法运算符> <factor> 
<factor>  → <标识符> | true | false | <字符串> | <整数数列> | ( <表达式> )  
<关系运算符> → < | > | <> | >= | <= | == 
<加法运算符> → + | - 
<乘法运算符> → * | / | % 
<字符串> → “<字母序列>” 
<字母序列> → <字母序列><数字>|<字母序列><字母>|ε 
<整数数列> → <整数数列><数字>|<数字> 
<条件语句> → if （<表达式>）<嵌套语句> else <嵌套语句> 
<循环语句> → while （<表达式>）: <嵌套语句> 
<嵌套语句> → <语句>; |<复合语句> 
<复合语句> → { <语句部分> } 
<字母> → a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z 
<数字> → 0|1|2|3|4|5|6|7|8|9
```

### 消除左递归和回溯

```text
 <程序>→<变量说明部分><语句部分>
 <变量说明部分>→<变量说明语句> 分号 A
 A→<变量说明语句> 分号 A | ε
 <变量说明语句>→变量说明<标识符列表>
 <标识符列表>→标识符 B
 B→逗号 标识符 B | ε
 <语句部分>→<语句> C
 C→分号 <语句> C | ε
 <语句> → <赋值语句> | <条件语句> | <循环语句>
 <赋值语句>→标识符 赋值符 <表达式>
 <表达式>→<conjunction> D
 D→or <conjunction>D | ε
 <conjunction>→<inversion> G
 G→and <inversion>G | ε
 <inversion>→not <inversion>|<关系表达式>
 <关系表达式>→ <算术表达式> E
 E→关系<算术表达式> E | ε
 <算术表达式> → <term>H
 H→ 加法 <term> H | ε
 <term>→<factor>I
 I→ 乘法 <factor> I | ε
 <factor>→标识符 | true | false |<字符串> | 整数 | 左括号 <表达式> 右括号
 <字符串>→左引号 字母序列 右引号
 <条件>→if  左括号 <表达式> 右括号 <嵌套语句> else <嵌套语句>
 <循环>→while  左括号 <表达式> 右括号  冒号 <嵌套语句>
 <嵌套语句>→<语句> 分号 | <复合语句>
 <复合语句>→begin <语句部分> end
```

### 程序界面

![image-20251118171913905](C:\Users\Sanziyue\AppData\Roaming\Typora\typora-user-images\image-20251118171913905.png)

### 运行结果

![image-20251118172200517](C:\Users\Sanziyue\AppData\Roaming\Typora\typora-user-images\image-20251118172200517.png)

![image-20251118172222087](C:\Users\Sanziyue\AppData\Roaming\Typora\typora-user-images\image-20251118172222087.png)

![image-20251118172238002](C:\Users\Sanziyue\AppData\Roaming\Typora\typora-user-images\image-20251118172238002.png)

![image-20251118172254413](C:\Users\Sanziyue\AppData\Roaming\Typora\typora-user-images\image-20251118172254413.png)

### AST可视化

![image-20251118172103917](C:\Users\Sanziyue\AppData\Roaming\Typora\typora-user-images\image-20251118172103917.png)
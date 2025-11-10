using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }



        private string sourceProgram;
        private int pointer;
        private token currentToken;
        private identifierTable idenTable;
        private tempVariableTable tempVarTable;
        private midCodeTable  tacTable;
        private node ast;

        private bool isDebug = false;

        private void print(string str) { listBox1.Items.Add(str); }
        private void debug(string str) { listBox2.Items.Add(str); }


        private class token {
            public string type { set; get; }
            public string value { set; get; }

            public token(string type,string value) {
                this.type = type;
                this.value = value;
            }

            public override string ToString()
            {
                return $"({this.type},{this.value})";
            }

        }

        private class triple
        {
            public string name { set; get; }
            public string type { set; get; }
            public string value { set; get; }

            public triple(string name)
            {
                this.name = name;
                this.type = "";
                this.value = "";
            }

            public int getValueAsINT32()
            {
                if (this.type != "int") return -1;
                if (this.value == "") return -1;

                return Convert.ToInt32(this.value);
            }

            public bool getValueAsBOOL()
            {
                if (this.type != "bool") return false;
                if (this.value == "") return false;
                if (this.value == "true") return true;

                return false;
            }


            public override string ToString()
            {
                return $" {this.name}|  {this.type}|  {this.value}";
            }

        }

        private class identifierTable
        {
            public List<triple> table = new List<triple> ();

            public identifierTable()
            {
                this.table.Clear();
            }

            public triple getIdentifierByName(string name)
            {
                foreach(triple t in table)
                {
                    if (t.name == name) return t;
                }
                return null;
            }

            public bool Add(string name)
            {
                if (getIdentifierByName (name )!=null)
                {
                    return false;
                }

                this.table.Add(new triple(name));
                return true;
            }

            public bool UpdateTypeByName(string name,string type)
            {
                triple temp = getIdentifierByName(name);
                if (temp == null) return false;
                if (temp.type != "") { return false; }
                temp.type = type;
                return true;
            }

            public bool UpdateValueByName(string name, string value)
            {
                triple temp = getIdentifierByName(name);
                if (temp == null) return false;
                temp.value  = value ;
                return true;
            }

            public void dump(ListBox lb)
            {
                lb.Items.Add($"| \t name| \t type| \t value|");
                foreach (triple t in table)
                {
                    lb.Items.Add($"|{t.ToString()}|");
                }
            }

        }

        private class tempVariableTable
        {
            public List<triple> table = new List<triple>();

            public tempVariableTable()
            {
                this.table.Clear();
            }

            public triple createNewVariable()
            {

                triple temp =new triple($"T{table.Count}");
                table.Add(temp);
                return temp;
            }

        }

        private class TAC
        {
            public string op { set; get; }
            public string oprand1 { set; get; }
            public string oprand2 { set; get; }
            public string result { set; get; }

            public TAC(string op, string oprand1, string oprand2, string result)
            {
                this.op = op;
                this.oprand1 = oprand1;
                this.oprand2 = oprand2;
                this.result = result;
            }

            public override string ToString()
            {
                return $"({this.op}, {this.oprand1}, {this.oprand2}, {this.result})";
            }

        }

        private class midCodeTable
        {
            public List<TAC> table = new List<TAC>();

            public int NXQ { get { return table.Count; } }
            public midCodeTable()
            {
                this.table.Clear();
            }

            public void generate(string op, string oprand1, string oprand2, string result)
            {
                this.table.Add(new TAC(op, oprand1, oprand2, result));
            }

            public void backpatch(int index, string result)
            {
                if (index >= table.Count) return;
                this.table[index].result = result;
            }

            public void dump(ListBox lb)
            {
                for(int i =0;i<table.Count; i++)
                {
                    lb.Items.Add($"({i} {table[i].ToString()}");
                }
            }

            public void saveToTXT(string path)
            {
                StreamWriter sw = new StreamWriter(path);

                for (int i = 0; i < table.Count; i++)
                {
                    sw.WriteLine ($"({i}) {table[i].ToString ()}");
                }

                sw.Close();
            }

        }

        private class node
        {
            public string root;
            public List<node> children = new List<node>();

            public node(string root)
            {
                this.children.Clear();
                this.root = root;
            }

            public void addChild(node child)
            {
                this.children.Add(child);
            }

            public void addNodeAsFirstChild(node child)
            {
                children.Insert(0, child);
            }

            public node getFirstLeftNonLeafChild()
            {
                if (children.Count == 0) return null;

                node pointer = children[0];
                node lastPointer = null;
                while(pointer .children .Count != 0)
                {
                    lastPointer = pointer;
                    pointer = pointer.children[0];
                }

                return lastPointer;
            }

            public override string ToString()
            {
                //根-左-右
                string str = $"({this.root})";

                if (children.Count == 0) return str;

                str += "(";
                foreach (node child in children)
                {

                    if (child == null) { str += "出错"; }
                    else { str += $"{child.ToString()}"; }
                }

                str += ")";

                return str;


            }


            public void saveToTXT(string path)
            {
                StreamWriter sw = new StreamWriter(path);

                
                    sw.WriteLine(this .ToString ());
                

                sw.Close();
            }

        }

        #region 词法分析
        private token nextInput(){

            string tmpWord = "";
            int state = 0;



            while (sourceProgram[pointer] != '#')
            {

                char symbol = sourceProgram[pointer];

                if (symbol == ' ') { pointer++; continue; }

                if (state == 0)
                {

                    if (symbol == '$')
                    {
                        state = 100;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol >= '0' && symbol <= '9')
                    {
                        state = 200;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol == ';')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("分号", tmpWord);
                    }
                    else if (symbol == 'i')
                    {
                        state = 400;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol == 'b')
                    {
                        state = 500;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol == ',')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("逗号", tmpWord);
                    }
                    else if (symbol == ':')
                    {
                        state = 700;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol == 'o')
                    {
                        state = 800;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol == 'n')
                    {
                        state = 900;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol == 't')
                    {
                        state = 1000;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol == 'f')
                    {
                        state = 1100;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol == '(')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("左括号", tmpWord);
                    }
                    else if (symbol == ')')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("右括号", tmpWord);
                    }
                    else if (symbol == '<')
                    {
                        state = 1400;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol == '>')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("关系", tmpWord);
                    }
                    else if (symbol == '=')
                    {
                        state = 1420;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol == '*' || symbol == '/')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("乘法", tmpWord);
                    }
                    else if (symbol == 'e')
                    {
                        state = 1700;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else if (symbol == '{')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("begin", tmpWord);
                    }
                    else if (symbol == '}')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("end", tmpWord);
                    }
                    else
                    {
                        pointer++;
                        return new token("error", $"状态{state}无法识别的符号");
                    }



                }

                if (state == 100)
                {

                    if (symbol >= 'a' && symbol <= 'z')
                    {
                        state = 101;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }

                }

                if (state == 101)
                    {

                        if ((symbol >= 'a' && symbol <= 'z') || (symbol >= '0' && symbol <= '9'))
                        {
                            state = 101;
                            tmpWord += symbol;
                            pointer++;
                            continue;
                        }
                        else if (symbol == '$')
                        {
                            tmpWord += symbol;
                            pointer++;
                            return new token("标识符", tmpWord);
                        }

                    }

                if (state == 200)
                {
                    if (symbol >= '0' && symbol <= '9')
                    {
                        state = 200;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                    else
                    {
                        return new token("整数", tmpWord);
                    }
                }

                if (state == 400)
                {
                    if (symbol == 'n')
                    {
                        state = 401;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }else if(symbol == 'f')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("if", tmpWord);
                    }
                }

                if (state == 401)
                {
                    if (symbol == 't')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("变量说明", tmpWord);
                    }
                }

                if (state == 500)
                {
                    if(symbol == 'o')
                    {
                        state = 501;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                }

                if (state == 501)
                {
                    if (symbol == 'o')
                    {
                        state = 502;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                }

                if (state == 502)
                {
                    if (symbol == 'l')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token ("变量说明",tmpWord );
                    }
                }

                if(state == 700)
                {
                    if(symbol == '=')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("赋值号", tmpWord);
                    }
                }
                if (state == 800)
                {
                    if (symbol == 'r')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("or", tmpWord);
                    }
                }
                if (state == 900)
                {
                    if (symbol == 'o')
                    {
                        state = 901;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                }
                if (state == 901)
                {
                    if (symbol == 't')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token ("not",tmpWord );
                    }
                }

                if (state == 1000)
                {
                    if (symbol == 'r')
                    {
                        state = 1001;
                        tmpWord += symbol;
                        pointer++;
                        continue ;
                    }
                }

                if (state == 1001)
                {
                    if (symbol == 'u')
                    {
                        state = 1002;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                }

                if (state == 1002)
                {
                    if (symbol == 'e')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token ("true",tmpWord );
                    }
                }

                if (state == 1100)
                {
                    if (symbol == 'a')
                    {
                        state = 1101;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                }
                if (state == 1101)
                {
                    if (symbol == 'l')
                    {
                        state = 1102;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                }
                if (state == 1102)
                {
                    if (symbol == 's')
                    {
                        state = 1103;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                }
                if (state == 1103)
                {
                    if (symbol == 'e')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token ("false",tmpWord );
                    }
                }
                if (state == 1400)
                {
                    if (symbol == '>')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token ("关系",tmpWord );
                    }
                    else
                    {
                        return new token("关系", tmpWord);
                    }
                }

                if (state == 1420)
                {
                    if (symbol == '=')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token("关系", tmpWord);
                    }

                }

                if (state == 1700)
                {
                    if (symbol == 'l')
                    {
                        state = 1701;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                }

                if (state == 1701)
                {
                    if (symbol == 's')
                    {
                        state = 1702;
                        tmpWord += symbol;
                        pointer++;
                        continue;
                    }
                }
                if (state == 1702)
                {
                    if (symbol == 'e')
                    {
                        tmpWord += symbol;
                        pointer++;
                        return new token ("else",tmpWord );
                    }
                }


                pointer++;
                debug("skipping"+symbol );


            }

            pointer++;

            return new token("#", $"#");
       }  

        private void button1_Click(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
            listBox2.Items.Clear();

            sourceProgram = textBox1.Text+"#" ;
            pointer = 0;

            currentToken = nextInput();
            print (currentToken.ToString());

            while(currentToken .value != "#"&& currentToken .type !="error") {
                currentToken = nextInput();
                print(currentToken.ToString());
            }

            print("词法分析结束");
        }
#endregion 

        #region 语法分析
        private void button2_Click(object sender, EventArgs e)
        {
            isDebug = false;
            listBox1.Items.Clear();
            listBox2.Items.Clear();

            sourceProgram = textBox1.Text + "#";
            pointer = 0;

            currentToken = nextInput();
            debug(currentToken.ToString());

            parseProgram();

        }

        private bool match(string expectedTokenType)
        {
            if(currentToken .type!= expectedTokenType)
            {
                //match失败
                if(isDebug )
                    debug($"匹配失败：expect{expectedTokenType},got{currentToken.type}");
                else
                    print($"匹配失败：expect{expectedTokenType},got{currentToken.type}");
                return false;
            }
            if (isDebug) 
                debug ($"匹配成功：expect{expectedTokenType}");
            else 
                print($"匹配成功：expect{expectedTokenType}");
            currentToken = nextInput();
            debug(currentToken.ToString());

            return true;

        }

        private void parseProgram()
        {

            print("推导：<程序> -> <变量说明部分><语句部分>");
            parseDeclareSection();
            parseStatementSection();

            print("语法分析结束");

        }


        private void parseDeclareSection() 
        {
            print("推导：<变量说明部分>-><变量声明语句>分号A");

            parseDeclareStatement();
            match("分号");
            parseA();

        }

        private void parseA() 
        {
            print("选择产生式：A-><变量说明语句>分号A|ε");

            if(currentToken .type == "变量说明")
            {

                print("推导：A-><变量说明语句>分号A");
                parseDeclareStatement();
                match("分号");
                parseA();


            }
            else if(currentToken .type =="标识符"||currentToken .type =="if")
                {
                print("推导：A->ε");
                }

        }

        private void parseDeclareStatement()
        {
            print("推导：<变量说明语句>->变量说明<标识符列表>");

            match("变量说明");
            parseVarList();


        }

       
        private void parseVarList()
        {
            print("推导：<标识符列表>->标识符B");
            match("标识符");
            parseB();
        }


        private void parseB()
        {
            print("选择产生式：B->逗号 标识符 B |ε");

            if (currentToken.type == "逗号")
            {

                print("推导：B->逗号 标识符 B");
                match("逗号");
                match("标识符");
                parseB();


            }
            else if (currentToken.type == "分号")
            {
                print("推导：B->ε");
            }


        }


        private void parseStatementSection()
        { 
            print("推导：<语句部分>-><语句>C");
            parseStatement();
            parseC();
        }


        private void parseC()
        {
            print("选择产生式：C->分号<语句>C|ε");

            if (currentToken.type == "分号")
            {

                print("推导：C->分号<语句>C");
                match("分号");
                parseStatement();
                parseC();


            }
            else if (currentToken.type == "#"||currentToken .type =="end")
            {
                print("推导：C->ε");
            }
        }

        private void parseStatement()
        {
            print("选择产生式：<语句>-><赋值语句>|<条件语句>");

            if (currentToken.type == "标识符")
            {

                print("推导：<语句>-><赋值语句>");
                parseAssignmentStatement();


            }
            else if (currentToken.type == "if")
            {
                parseConditionalStatement();
            }

        }

        private void parseAssignmentStatement()
        {
            print("推导：<赋值语句>->标识符 赋值号 <表达式>");
            match("标识符");
            match("赋值号");
            parseExpression();
        }

        private void parseExpression()
        {
            print("推导：<表达式>-> <inversion> D");
            parseInversion();
            parseD();
        }

        private void parseD()
        {
            print("选择产生式：D-> or <inversion> D| ε");

            if (currentToken.type == "or")
            {

                print("推导：D-> or <inversion> D");
                match ("or");
                parseInversion();
                parseD();


            }
            else if (currentToken.type == "分号"||currentToken .type =="#"
                ||currentToken .type=="end"||currentToken .type =="右括号" )
            {

                print("推导：D->ε");
            }

        }

        private void parseInversion()
        {
            print("选择产生式：<inversion>-> not <inversion> | <关系表达式>");

            if (currentToken.type == "not")
            {

                print("推导：<inversion>-> not <inversion>");
                match("not");
                parseInversion();

            }
            else if (currentToken.type == "标识符" || currentToken.type == "true"
                || currentToken.type == "false" || currentToken.type == "整数"
                || currentToken.type == "左括号" )
            {
                print("推导：<inversion>-> <关系表达式>");
                parseRelationExpression();

            }

        }

        private void parseRelationExpression()
        {
            print("推导：<关系表达式>-> <算术表达式> E");
            parseMathExpression();
            parseE();

        }

        private void parseE()
        {
            print("选择产生式：E-> 关系 <算术表达式> | ε ");

            if (currentToken.type == "关系")
            {

                print("推导：E-> 关系 <算术表达式>");
                match("关系");
                parseMathExpression();

            }
            else if (currentToken.type == "分号" || currentToken.type == "#"
                || currentToken.type == "end" || currentToken.type == "右括号"
                || currentToken.type == "or")
            {
                print("推导：E->ε");
            }


        }

        private void parseMathExpression()
        {
            print("推导：<算术表达式> -> <factor> F ");
            parseFactor();
            parseF();
        }


        private void parseF()
        {
            print("选择产生式：F-> 乘法 <factor> F | ε ");

            if (currentToken.type == "乘法")
            {

                print("推导：F-> 乘法 <factor> F");
                match("乘法");
                parseFactor();
                parseF();

            }
            else if (currentToken.type == "分号" || currentToken.type == "#"
                || currentToken.type == "end" || currentToken.type == "右括号"
                || currentToken.type == "关系" || currentToken.type == "or")
            {
                print("推导：F->ε");
            }
        }

        private void parseFactor()
        {
            print("选择产生式：<factor>-> 标识符|true|false| 整数 |左括号 <表达式> 右括号 ");

            if (currentToken.type == "标识符")
            {

                print("推导：<factor>-> 标识符");
                match("标识符");
                

            }
            else if (currentToken.type == "true")
            {

                print("推导：<factor>-> true");
                match("true");

            }
            else if (currentToken.type == "false")
            {

                print("推导：<factor>-> false");
                match("false");

            }
            else if (currentToken.type == "整数")
            {

                print("推导：<factor>-> 整数");
                match("整数");

            }
            else if (currentToken.type == "左括号")
            {

                print("推导：<factor>-> 左括号 <表达式> 右括号");
                match("左括号");
                parseExpression();
                match("右括号");
            }

        }

        private void parseConditionalStatement()
        {
            print("推导：<条件语句>-> if 左括号 <表达式> 右括号 <复合语句> else <复合语句>");
            match("if");
            match("左括号");
            parseExpression();
            match("右括号");
            parseCompoundStatement();
            match("else");
            parseCompoundStatement();

        }

        private void parseCompoundStatement()
        {
            print("推导：<复合语句>-> begin <语句部分> end");
            match("begin");
            parseStatementSection();
            match("end");

        }

        #endregion

        #region TAC语义分析
        private void button3_Click(object sender, EventArgs e)
        {
            isDebug = true;
            listBox1.Items.Clear();
            listBox2.Items.Clear();

            sourceProgram = textBox1.Text + "#";
            pointer = 0;


            idenTable = new identifierTable();
            tempVarTable = new tempVariableTable();
            tacTable = new midCodeTable();

            

            currentToken = nextInput();
            debug(currentToken.ToString());

            translateTACProgram();

            tacTable.dump(listBox1);

            /*string path = Application.StartupPath + @"\outputTAC.txt";
            tacTable.saveToTXT(path);
            print("save to" + path);*/
        }

        private void translateTACProgram()
        {

            debug("推导：<程序> -> <变量说明部分><语句部分>");
            translateTACDeclareSection();
            translateTACStatementSection();

            debug("语法分析结束");

        }


        private void translateTACDeclareSection()
        {
            debug("推导：<变量说明部分>-><变量声明语句>分号A");

            translateTACDeclareStatement();
            match("分号");
            translateTACA();

        }

        private void translateTACA()
        {
            debug("选择产生式：A-><变量说明语句>分号A|ε");

            if (currentToken.type == "变量说明")
            {

                debug("推导：A-><变量说明语句>分号A");
                translateTACDeclareStatement();
                match("分号");
                translateTACA();


            }
            else if (currentToken.type == "标识符" || currentToken.type == "if")
            {
                debug("推导：A->ε");
            }

        }

        private void translateTACDeclareStatement()
        {
            debug("推导：<变量说明语句>->变量说明<标识符列表>");

            string type = currentToken.value;
            match("变量说明");
            translateTACVarList(type);


        }


        private void translateTACVarList(string type)
        {
            debug("推导：<标识符列表>->标识符B");

            string name = currentToken.value;
            match("标识符");
            //语义检查
            idenTable.Add(name);
            idenTable.UpdateTypeByName(name, type);

            tacTable.generate("declare", type, "null", $"{name}.type");

            translateTACB(type);
        }


        private void translateTACB(string type)
        {
            debug("选择产生式：B->逗号 标识符 B |ε");

            if (currentToken.type == "逗号")
            {

                debug("推导：B->逗号 标识符 B");
                match("逗号");

                string name = currentToken.value;
                match("标识符");
                idenTable.Add(name);
                idenTable.UpdateTypeByName(name, type);

                tacTable.generate("declare", type, "null", $"{name}.type");
                translateTACB(type);


            }
            else if (currentToken.type == "分号")
            {
                debug("推导：B->ε");
            }


        }


        private void translateTACStatementSection()
        {
            debug("推导：<语句部分>-><语句>C");
            translateTACStatement();
            translateTACC();
        }


        private void translateTACC()
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

        private void translateTACStatement()
        {
            debug("选择产生式：<语句>-><赋值语句>|<条件语句>");

            if (currentToken.type == "标识符")
            {

                debug("推导：<语句>-><赋值语句>");
                translateTACAssignmentStatement();


            }
            else if (currentToken.type == "if")
            {
                translateTACConditionalStatement();
            }

        }

        private void translateTACAssignmentStatement()
        {
            debug("推导：<赋值语句>->标识符 赋值号 <表达式>");

            string name = currentToken.value;
            match("标识符");
            match("赋值号");
           triple Exp= translateTACExpression();

            //更新标识符列表，只有动态编译才能获得正确结果
            idenTable.UpdateTypeByName(name, Exp.value);

            tacTable.generate("assign", Exp.name, "null", name);

        }

        private triple  translateTACExpression()
        {
            debug("推导：<表达式>-> <inversion> D");
             triple E1= translateTACInversion();
             triple E=translateTACD(E1);

            return E;
        }

        private triple  translateTACD(triple E1)
        {
            debug("选择产生式：D-> or <inversion> D| ε");

            if (currentToken.type == "or")
            {

                debug("推导：D-> or <inversion> D");

                match("or");
                triple E2= translateTACInversion();

                //创建临时变量
                triple T = tempVarTable.createNewVariable();
                T.type = "bool";
                //根据E1和E2进行or运算，获得T正确的值
                T.value = "true";

                tacTable.generate("or", E1.name, E2.name, T.name);
                return  translateTACD(T);


            }
            else if (currentToken.type == "分号" || currentToken.type == "#"
                || currentToken.type == "end" || currentToken.type == "右括号")
            {

                debug("推导：D->ε");
                return E1;
            }

            return null;
        }

        private triple  translateTACInversion()
        {
            debug("选择产生式：<inversion>-> not <inversion> | <关系表达式>");

            if (currentToken.type == "not")
            {

                debug("推导：<inversion>-> not <inversion>");
                match("not");
                triple E1= translateTACInversion();

                //创建临时变量
                triple T = tempVarTable.createNewVariable();
                T.type = "bool";
                //根据E1进行not运算，获得T正确的值
                if (E1 == null) { return null; }
                T.value = (E1.value == "true") ? "false" : "true"; 
                
                tacTable.generate("not", E1.name, "null", T.name);

                return T;

            }
            else if (currentToken.type == "标识符" || currentToken.type == "true"
                || currentToken.type == "false" || currentToken.type == "整数"
                || currentToken.type == "左括号")
            {
                debug("推导：<inversion>-> <关系表达式>");
                return  translateTACRelationExpression();

            }

            return null;
        }

        private triple  translateTACRelationExpression()
        {
            debug("推导：<关系表达式>-> <算术表达式> E");
            triple E1= translateTACMathExpression();

            triple E= translateTACE(E1);
            return E;

        }

        private triple  translateTACE(triple E1)
        {
            debug("选择产生式：E-> 关系 <算术表达式> | ε ");

            if (currentToken.type == "关系")
            {

                debug("推导：E-> 关系 <算术表达式>");

                string op = currentToken.value;
                match("关系");
                triple E2= translateTACMathExpression();

                //创建临时变量
                triple T = tempVarTable.createNewVariable();
                T.type = "bool";
                //根据E1和E2进行关系运算，获得T正确的值
                T.value = "true";

                if (op == "<") { op = "lt"; }
                else if (op == ">") { op = "gt"; }
                else if (op == "<>") { op = "noteq"; }
                else if (op == "==") { op = "eq"; }
                tacTable.generate(op, E1.name, E2.name, T.name);

                return T;
            }
            else if (currentToken.type == "分号" || currentToken.type == "#"
                || currentToken.type == "end" || currentToken.type == "右括号"
                || currentToken.type == "or")
            {
                debug("推导：E->ε");
                return E1;
            }

            return null;
        }

        private triple  translateTACMathExpression()
        {
            debug("推导：<算术表达式> -> <factor> F ");
            triple E1= translateTACFactor();
            triple E= translateTACF(E1);
            return E;
        }


        private triple  translateTACF(triple E1)
        {
            debug("选择产生式：F-> 乘法 <factor> F | ε ");

            if (currentToken.type == "乘法")
            {

                debug("推导：F-> 乘法 <factor> F");

                string op = currentToken.value;
                match("乘法");
                triple E2= translateTACFactor();


                //创建临时变量
                triple T = tempVarTable.createNewVariable();
                T.type = "int";
                //根据E1和E2进行算数运算，获得T正确的值
                T.value = "1";

                if (op == "*") { op = "mul"; }
                else if (op == "/") { op = "div"; }
                
                tacTable.generate(op, E1.name, E2.name, T.name);


                return  translateTACF(T);

            }
            else if (currentToken.type == "分号" || currentToken.type == "#"
                || currentToken.type == "end" || currentToken.type == "右括号"
                || currentToken.type == "关系" || currentToken.type == "or")
            {
                debug("推导：F->ε");
                return E1;
            }

            return null;
        }

        private triple  translateTACFactor()
        {
            debug("选择产生式：<factor>-> 标识符|true|false| 整数 |左括号 <表达式> 右括号 ");

            if (currentToken.type == "标识符")
            {

                debug("推导：<factor>-> 标识符");
                string name = currentToken.value;
                match("标识符");

                return idenTable.getIdentifierByName(name);

            }
            else if (currentToken.type == "true")
            {

                debug("推导：<factor>-> true");
                match("true");

                triple t = new triple("true");
                t.type = "bool";
                t.value = "true";

                return t;
            }
            else if (currentToken.type == "false")
            {

                debug("推导：<factor>-> false");
                match("false");

                triple t = new triple("false");
                t.type = "bool";
                t.value = "false";

                return t;

            }
            else if (currentToken.type == "整数")
            {

                debug("推导：<factor>-> 整数");

                string value = currentToken.value;
                match("整数");

                triple t = new triple(value);
                t.type = "bool";
                t.value = value;

                return t;
            }
            else if (currentToken.type == "左括号")
            {

                debug("推导：<factor>-> 左括号 <表达式> 右括号");
                match("左括号");
                triple Exp= translateTACExpression();
                match("右括号");

                return Exp;
            }

            return null;
        }

        private void translateTACConditionalStatement()
        {
            debug("推导：<条件语句>-> if 左括号 <表达式> 右括号 <复合语句> else <复合语句>");
            match("if");
            match("左括号");
            triple T= translateTACExpression();

            tacTable.generate("jnz", T.name, "null", (tacTable.NXQ + 2).ToString());
            int falseIndex = tacTable.NXQ;
            tacTable.generate("jump", "null", "null", "0");

            match("右括号");
            translateTACCompoundStatement();

            int exitIndex = tacTable.NXQ;
            tacTable.generate("jump", "null", "null", "0");
            tacTable.backpatch(falseIndex, (tacTable.NXQ + 2).ToString());

            match("else");
            translateTACCompoundStatement();

            tacTable.backpatch(exitIndex, (tacTable.NXQ + 2).ToString());

        }

        private void translateTACCompoundStatement()
        {
            debug("推导：<复合语句>-> begin <语句部分> end");
            match("begin");
            translateTACStatementSection();
            match("end");

        }
        #endregion

        #region AST语义分析
        private void button4_Click(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
            listBox2.Items.Clear();

            sourceProgram = textBox1.Text + "#";
            pointer = 0;

            idenTable = new identifierTable();
            tempVarTable = new tempVariableTable();

            isDebug = true;

            currentToken = nextInput();
            debug(currentToken.ToString());


            //print(ast.getFirstLeftNonLeafChild().root);
            ast= tanslateASTProgram();

            print(ast.ToString());

            string path = Application.StartupPath + @"\outputAST.txt";
            ast.saveToTXT(path);
            print("save to" + path);

        }

        private node tanslateASTProgram()
        {

            debug("推导：<程序> -> <变量说明部分><语句部分>");

            node root = new node("topLevel");

            node c1=tanslateASTDeclareSection();
            node c2=tanslateASTStatementSection();

          

            debug("语法分析结束");
            root.addChild(c1);
            root.addChild(c2);

            return root;
        }


        private node tanslateASTDeclareSection()
        {
            debug("推导：<变量说明部分>-><变量声明语句>分号A");

            node root = new node("declareList");

            node c1= tanslateASTDeclareStatement();
            match("分号");
            node c2=tanslateASTA();

            root.addChild(c1);
            root.addChild(c2);

            return root;

        }

        private node tanslateASTA()
        {
            debug("选择产生式：A-><变量说明语句>分号A|ε");

            if (currentToken.type == "变量说明")
            {

                debug("推导：A-><变量说明语句>分号A");

                node root = new node("linkDeclareList");

                node c1 = tanslateASTDeclareStatement();
                match("分号");
                node c2 = tanslateASTA();

                root.addChild(c1);
                root.addChild(c2);
                return root;

            }
            else if (currentToken.type == "标识符" 
                || currentToken.type == "if")
            {
                debug("推导：A->ε");

                return new node("emptyDeclareList");
            }

            return null;
        }

        private node tanslateASTDeclareStatement()
        {
            debug("推导：<变量说明语句>->变量说明<标识符列表>");

            node root = new node("declare");

            node c1 = new node(currentToken.value);
            match("变量说明");
            node c2= tanslateASTVarList();

            root.addChild(c1);
            root.addChild(c2);

            return root;
        }


        private node  tanslateASTVarList()
        {
            debug("推导：<标识符列表> -> 标识符 B");

            node root = new node("varList");
            node c1 = new node(currentToken.value);
            match("标识符");
            node c2= tanslateASTB();

            root.addChild(c1);
            root.addChild(c2);

            return root;
        }


        private node  tanslateASTB()
        {
            debug("选择产生式：B->逗号 标识符 B |ε");

            if (currentToken.type == "逗号")
            {

                debug("推导：B->逗号 标识符 B");
                node root = new node("linkVarList");

                match("逗号");

                node c1 = new node(currentToken.value);
                match("标识符");
                node c2= tanslateASTB();

                root.addChild(c1);
                root.addChild(c2);

                return root;
            }
            else if (currentToken.type == "分号")
            {
                debug("推导：B->ε");

                return new node("emptyVarList");
            }

            return null;
        }

        //wrong
        private node tanslateASTStatementSection()
        {
            debug("推导：<语句部分>-><语句>C");
            node root = new node("Statementist");
            node c1 = tanslateASTStatement();
            node c2= tanslateASTC();

            root.addChild(c1);
            root.addChild(c2);
            return root;
        }


        private node tanslateASTC()
        {
            debug("选择产生式：C->分号<语句>C|ε");

            if (currentToken.type == "分号")
            {

                debug("推导：C->分号<语句>C");
                node root = new node("linkSeqList");

                match("分号");
                node c1=tanslateASTStatement();
                node c2=tanslateASTC();

                root.addChild(c1);
                root.addChild(c2);
                return root;


            }
            else if (currentToken.type == "#" || currentToken.type == "end")
            {
                debug("推导：C->ε");
                return new node("emptySeqList");
            }
            return null;
        }

        private node  tanslateASTStatement()
        {
            debug("选择产生式：<语句>-><赋值语句>|<条件语句>");

            if (currentToken.type == "标识符")
            {

                debug("推导：<语句>-><赋值语句>");
                return  tanslateASTAssignmentStatement();


            }
            else if (currentToken.type == "if")
            {
                return  tanslateASTConditionalStatement();
            }

            return null;
        }

        private node tanslateASTAssignmentStatement()
        {
            debug("推导：<赋值语句>->标识符 赋值号 <表达式>");

            node root = new node("assign");

            node c1 = new node(currentToken.value);
            match("标识符");

            match("赋值号");
            node c2=tanslateASTExpression();

            root.addChild(c1);
            root.addChild(c2);
            return root;
        }

        private node tanslateASTExpression()
        {
            debug("推导：<表达式>-> <inversion> D");
            node c1 = tanslateASTInversion();
            node c2 = tanslateASTD();

            if (c2 == null) return null;//如果出错则退出

            if (c2.root == "empty")
            {
                return c1;
            }
            else
            {
                node insertPoint = c2.getFirstLeftNonLeafChild();

                if (insertPoint == null)
                {
                    c2.addNodeAsFirstChild(c1);
                }
                else
                {
                    insertPoint.addNodeAsFirstChild(c1);
                }
                return c2;
            }
        }

        private node tanslateASTD()
        {
            debug("选择产生式：D-> or <inversion> D| ε");

            if (currentToken.type == "or")
            {

                debug("推导：D-> or <inversion> D");

                string op = "or";
                match("or");
                node c1= tanslateASTInversion();
                node c2= tanslateASTD();

                if (c2 == null) return null; //如果出错则退出

                if (c2.root == "empty")
                {
                    //构建 操作符-第二factor的子树
                    node root = new node(op);
                    root.addChild(c1);
                    return root;
                }
                else
                {

                    node newFirstChild = new node(op);
                    newFirstChild.addChild(c1);

                    
                    node insertPoint = c2.getFirstLeftNonLeafChild();


                    if (insertPoint == null)
                    {
                        c2.addNodeAsFirstChild(newFirstChild );
                    }
                    else
                    {
                        insertPoint.addNodeAsFirstChild(newFirstChild );
                    }
                    return c2;


                }



            }
            else if (currentToken.type == "分号" || currentToken.type == "#"
                || currentToken.type == "end" || currentToken.type == "右括号")
            {

                debug("推导：D->ε");
                return new node("empty");
            }

            return null;
        }

        private node tanslateASTInversion()
        {
            debug("选择产生式：<inversion>-> not <inversion> | <关系表达式>");

            if (currentToken.type == "not")
            {

                debug("推导：<inversion>-> not <inversion>");

                node root = new node("not");
                match("not");
                node c1= tanslateASTInversion();

                root.addChild(c1);
                return root;

            }
            else if (currentToken.type == "标识符" || currentToken.type == "true"
                || currentToken.type == "false" || currentToken.type == "整数"
                || currentToken.type == "左括号")
            {
                debug("推导：<inversion>-> <关系表达式>");
                return  tanslateASTRelationExpression();

            }

            return null;
        }

        private node tanslateASTRelationExpression()
        {
            debug("推导：<关系表达式>-> <算术表达式> E");
            node c1= tanslateASTMathExpression();
            node c2= tanslateASTE();

            if (c2.root == "empty")
            {
                return c1;
            }
            else
            {
                c2.addNodeAsFirstChild(c1);
                return c2;
            }
        }

        private node tanslateASTE()
        {
            debug("选择产生式：E-> 关系 <算术表达式> | ε ");

            if (currentToken.type == "关系")
            {

                debug("推导：E-> 关系 <算术表达式>");

                string op = currentToken.value;
                if (op == "<") { op = "lt"; }
                else if (op == ">") { op = "gt"; }
                else if (op == "<>") { op = "noteq"; }
                else if (op == "==") { op = "eq"; }

                match("关系");

                node c1= tanslateASTMathExpression();

                node root = new node(op);
                root.addChild(c1);

                return root;

            }
            else if (currentToken.type == "分号" || currentToken.type == "#"
                || currentToken.type == "end" || currentToken.type == "右括号"
                || currentToken.type == "or")
            {
                debug("推导：E->ε");
                return new node("empty");
            }

            return null;
        }

        private node tanslateASTMathExpression()
        {
            debug("推导：<算术表达式> -> <factor> F ");
            node c1 = tanslateASTFactor();
            node c2= tanslateASTF();

            if (c2 == null) return null;//如果出错则退出

            if(c2.root == "empty")
            {
                return c1;
            }
            else
            {
                node insertPoint = c2.getFirstLeftNonLeafChild();

                if (insertPoint == null)
                {
                    c2.addNodeAsFirstChild(c1);
                }
                else
                {
                    insertPoint.addNodeAsFirstChild(c1);
                }
                    return c2;
                
            }
        }


        private node tanslateASTF()
        {
            debug("选择产生式：F-> 乘法 <factor> F | ε ");

            if (currentToken.type == "乘法")
            {

                debug("推导：F-> 乘法 <factor> F");

                string op = currentToken.value;
                if (op == "*") { op = "mul"; }
                else if(op == "/") { op = "div"; }


                match("乘法");
                node c1= tanslateASTFactor();
                node c2= tanslateASTF();

                if (c2 == null) return null; //如果出错则退出

                if(c2.root == "empty")
                {
                    //构建 操作符-第二factor的子树
                    node root = new node(op);
                    root.addChild(c1);
                    return root;
                }
                else
                {
                   
                    node newFirstChild = new node(op);
                    newFirstChild.addChild(c1);

                    node insertPoint = c2.getFirstLeftNonLeafChild();
                   

                    if (insertPoint == null)
                    {
                        c2.addNodeAsFirstChild(newFirstChild );
                    }
                    else
                    {
                        insertPoint.addNodeAsFirstChild(newFirstChild );
                    }
                    return c2;





                }

            }
            else if (currentToken.type == "分号" || currentToken.type == "#"
                || currentToken.type == "end" || currentToken.type == "右括号"
                || currentToken.type == "关系" || currentToken.type == "or")
            {
                debug("推导：F->ε");
                return new node("empty");
            }

            return null;
        }

        private node tanslateASTFactor()
        {
            debug("选择产生式：<factor>-> 标识符|true|false| 整数 |左括号 <表达式> 右括号 ");

            if (currentToken.type == "标识符")
            {

                debug("推导：<factor>-> 标识符");
                node leave = new node(currentToken.value);
                match("标识符");

                return leave;
            }
            else if (currentToken.type == "true")
            {

                debug("推导：<factor>-> true");
                match("true");
                return new node("true");
            }
            else if (currentToken.type == "false")
            {

                debug("推导：<factor>-> false");
                match("false");
                return new node("false");
            }
            else if (currentToken.type == "整数")
            {

                debug("推导：<factor>-> 整数");
                node leave = new node(currentToken.value);
                match("整数");
                return leave;
            }
            else if (currentToken.type == "左括号")
            {

                debug("推导：<factor>-> 左括号 <表达式> 右括号");
                match("左括号");
                node c1=tanslateASTExpression();
                match("右括号");

                return c1;
            }

            return null;
        }

        private node  tanslateASTConditionalStatement()
        {
            debug("推导：<条件语句>-> if 左括号 <表达式> 右括号 <复合语句> else <复合语句>");

            node root = new node("ifThenElse");

            match("if");
            match("左括号");
            node c1 = tanslateASTExpression();
            match("右括号");
            node c2 = tanslateASTCompoundStatement();
            match("else");
            node c3 = tanslateASTCompoundStatement();

            root.addChild(c1);
            root.addChild(c2);
            root.addChild(c3);
            return root ;
        }

        private node tanslateASTCompoundStatement()
        {
            debug("推导：<复合语句>-> begin <语句部分> end");
            match("begin");
            node c1= tanslateASTStatementSection();
            match("end");

            return c1;
        }
    }
}
#endregion 
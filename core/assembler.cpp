#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
#include <json/json.h>

#if defined(WIN32) || defined(_WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

using namespace std;

// 运算符
const char operators[] = {'+', '-', '*', '/', '%', '>', '<', '=', '|', '&', '~', '!', '^'};
// 分隔符
const char delimiters[] = {'(', ')', '{', '}', '[', ']', '#', ';', ',', ':', '.', '"', '\''};
// 关键字
const string keywords[] = {
    "asm", "else", "new", "this", "auto", "enum", "operator", "throw", "bool", "explicit",
    "private", "true",
    "break", "export", "protected", "try", "case", "extern", "public", "typedef", "catch",
    "false", "register", "typerid",
    "char", "float", "reinterpret_cast", "typename", "class", "for", "return", "union", "const",
    "friend", "short", "unsigned",
    "const_cast", "goto", "signed", "using", "continue", "if", "sizeof", "virtual", "default",
    "inline", "static", "void",
    "delete", "int", "static_cast", "volatile", "do", "long", "struct", "wchar_t", "double",
    "mutable", "switch", "while",
    "dynamic_cast", "namespace", "template", "main", "cout", "cin", "using", "namespace", "std"};

// 字符是否数字
bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

// 字符是否是可以用于命名的字符
bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '$';
}

// 字符是否运算符
bool isOperator(char c)
{
    for (char op : operators)
    {
        if (op == c)
            return true;
    }
    return false;
}

// 字符是否分隔符
bool isDelimiter(char c)
{
    for (char del : delimiters)
    {
        if (c == del)
            return true;
    }
    return false;
}

// Token是否关键字
bool isKeyword(string token)
{
    for (string keyword : keywords)
    {
        if (keyword == token)
            return true;
    }
    return false;
}

// 字符是否十六进制(大于等于10的部分)
bool isHex(char c)
{
    return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

// Token类型
enum TokenType
{
    KEYWORD,    // 关键字
    IDENTIFIER, // 可命名的字符串
    NUMBER,     // 数字（二进制或十六进制）
    OPERATOR,   // 运算符
    DELIMITER,  // 分隔符
    STRING,     // 串
    COMMENT,    // 注释
    ERROR,      // 解析错误
};

// Token
struct Token
{
    enum TokenType type;
    string value;

    void setToken(TokenType type, string value)
    {
        this->type = type;
        this->value = value;
    }
};

// 转化为数字Token
Token toNumberToken(char start, fstream &fs)
{
    Token token;
    string value;
    value += start;
    char cur;
    while (fs.get(cur))
    {
        if (isDigit(cur))
        {
            value += cur;
            continue;
        }
        // 小数点后
        if (cur == '.')
        {
            value += cur;
            while (fs.get(cur))
            {
                if (isDigit(cur))
                {
                    value += cur;
                    continue;
                }
                // 科学计数
                if (cur == 'e' || cur == 'E')
                {
                    value += cur;
                    fs.get(cur); // 探测是否是 + 或 -
                    if (cur == '+' || cur == '-' || isDigit(cur))
                    {
                        value += cur;
                    }
                    while (fs.get(cur))
                    {
                        if (isDigit(cur))
                        {
                            value += cur;
                            continue;
                        }
                        break;
                    }
                }
                break;
            }
            break;
        }
        // 科学计数
        if (cur == 'e' || cur == 'E')
        {
            value += cur;
            fs.get(cur); // 探测是否是 + 或 -
            if (cur == '+' || cur == '-' || isDigit(cur))
            {
                value += cur;
            }
            while (fs.get(cur))
            {
                if (isDigit(cur))
                {
                    value += cur;
                    continue;
                }
                break;
            }
            break;
        }
        // 十六进制
        if (cur == 'x' || cur == 'X')
        {
            value += cur;
            while (fs.get(cur))
            {
                if (isDigit(cur) || isHex(cur))
                {
                    value += cur;
                    continue;
                }
                break;
            }
        }
        break;
    }
    fs.unget(); // 退回一个未识别的字符
    token.setToken(TokenType::NUMBER, value);
    return token;
}

// 转化为AlphaToken，可能包含Keyword和Identifier
Token toAlphaToken(char start, fstream &fs)
{
    Token token;
    string value;
    value += start;
    char cur;
    while (fs.get(cur))
    {
        if (isAlpha(cur) || isDigit(cur))
        {
            value += cur;
            continue;
        }
        break;
    }
    if (isKeyword(value))
    {
        token.setToken(TokenType::KEYWORD, value);
    }
    else
    {
        token.setToken(TokenType::IDENTIFIER, value);
    }
    fs.unget(); // 回退一个未识别的字符
    return token;
}

// 转化为操作符Token
Token toOperatorToken(char start, fstream &fs)
{
    Token token;
    string value;
    value += start;
    char next = fs.peek(); // 探测一个字符
    if (next == '=')
    {
        value += next;
        fs.ignore(); // 忽略探测到的字符
    }
    else if ((next == '<' || next == '>') && next == start)
    {
        // << >>
        value += next;
        fs.ignore();
        next = fs.peek(); // 可能是 <<= 或 >>=
        if (next == '=')
        {
            value += next;
            fs.ignore();
        }
    }
    else if ((next == '+' || next == '-' || next == '|' || next == '&') && next == start)
    {
        // ++ -- && ||
        value += next;
        fs.ignore();
    }
    else if (next == '>' && start == '-')
    {
        // ->
        value += next;
        fs.ignore();
    }
    token.setToken(TokenType::OPERATOR, value);
    return token;
}

// 转化为定界符Token
Token toDelimiterToken(char start, fstream &_)
{
    Token token;
    string value;
    value += start;
    token.setToken(TokenType::DELIMITER, value);
    return token;
}

// 转化为串Token
Token toStringToken(char start, fstream &fs)
{
    Token token;
    string value;
    value += start;
    char cur;
    bool transferFlag = false; // 当前为转义字符
    bool closed = false;       // 是否正常关闭
    while (fs.get(cur))
    {
        value += cur;
        if (cur == '\\' && !transferFlag)
        {
            transferFlag = true;
            continue;
        }
        // 结束串
        if (cur == start && !transferFlag)
        {
            closed = true;
            break;
        }
        transferFlag = false;
    }
    if (!closed)
    {
        token.setToken(TokenType::ERROR, "[ERROR]未闭合的字符串: " + value);
    }
    else
    {
        token.setToken(TokenType::STRING, value);
    }
    return token;
}

// 转化为注释Token
Token toCommentToken(char start, fstream &fs)
{
    Token token;
    string value;
    value += start;
    char cur;
    // 注释开头：// 或 /*
    fs.get(cur);
    value += cur;
    bool multiline = cur == '*'; // 是否多行注释
    while (fs.get(cur))
    {
        if (cur == '\n' && !multiline)
        {
            // 单行注释结束
            break;
        }
        value += cur;
        if (cur == '*' && multiline)
        {
            // 探测多行注释是否结束
            fs.get(cur);
            value += cur;
            if (cur == '/')
            {
                // 关闭注释
                break;
            }
        }
    }
    token.setToken(TokenType::COMMENT, value);
    return token;
}

// 错误解析
Token toErrorToken(char start, fstream &_)
{
    return Token{TokenType::ERROR, "无法解析的Token"};
}

// 根据第一个字母获取处理器
Token (*getHandlerByFirstLetter(char start, fstream &fs))(char, fstream &)
{
    if (isDigit(start))
        return toNumberToken;
    if (isAlpha(start))
        return toAlphaToken;
    if (start == '"' || start == '\'')
        return toStringToken;
    if (start == '/' && (fs.peek() == '*' || fs.peek() == '/'))
        return toCommentToken;
    if (isOperator(start))
        return toOperatorToken;
    if (isDelimiter(start))
        return toDelimiterToken;
    return toErrorToken;
}

// 根据类型获取中文名称
string getTokenTypeName(TokenType type)
{
    switch (type)
    {
    case TokenType::COMMENT:
        return "注释";
    case TokenType::STRING:
        return "字符串";
    case TokenType::DELIMITER:
        return "特殊符号";
    case TokenType::OPERATOR:
        return "操作符";
    case TokenType::IDENTIFIER:
        return "标识符";
    case TokenType::KEYWORD:
        return "关键字";
    case TokenType::NUMBER:
        return "数字";
    case TokenType::ERROR:
        return "解析错误";
    }
}

// 处理一段文本，输出所有Token
extern "C" EXPORT bool assemble(string input, string output)
{
    fstream fs(input, ios::in);
    Json::FastWriter writer;
    Json::Value value;

    char cur;
    while (fs.get(cur))
    {
        // 制表符、空格、回车、换行跳过
        if (cur == 9 || cur == 32 || cur == 13 || cur == 10)
            continue;
        // 获取处理器，生成Token
        Token token = getHandlerByFirstLetter(cur, fs)(cur, fs);
        Json::Value cur;
        cur["type"] = getTokenTypeName(token.type);
        cur["value"] = token.value;
        cur["typeId"] = token.type;
        value.append(cur);
    }
    fs.close();
    // JSON序列化为字符串
    string result = writer.write(value);
    // 将结果写入本地文件
    fstream file(output, ios::out);
    file.write(result.c_str(), result.size());
    if (file.bad()) {
        file.close();
        return false;
    }
    file.close();
    return true;
}
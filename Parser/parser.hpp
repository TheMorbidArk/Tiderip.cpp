//
// Created by MorbidArk on 2023/2/25.
//

#pragma once

#include "common.hpp"
#include "utils.hpp"

enum class TokenType : int
{
    TOKEN_UNKNOWN = 0,
    // 数据类型
    TOKEN_NUM,           //数字
    TOKEN_STRING,           //字符串
    TOKEN_ID,               //变量名
    TOKEN_INTERPOLATION,     //内嵌表达式
    
    // 关键字(系统保留字)
    TOKEN_TIDE,           //'Tide'
    TOKEN_FUN,           //'fun'
    TOKEN_IF,           //'if'
    TOKEN_ELSE,               //'else'
    TOKEN_TRUE,               //'true'
    TOKEN_FALSE,               //'false'
    TOKEN_WHILE,               //'while'
    TOKEN_FOR,               //'for'
    TOKEN_BREAK,               //'break'
    TOKEN_CONTINUE,         //'continue'
    TOKEN_RETURN,           //'return'
    TOKEN_NULL,               //'null'
    
    //以下是关于类和模块导入的token
    TOKEN_CLASS,               //'class'
    TOKEN_THIS,               //'this'
    TOKEN_STATIC,           //'static'
    TOKEN_IS,           // 'is'
    TOKEN_SUPER,               //'super'
    TOKEN_IMPORT,           //'import'
    
    //分隔符
    TOKEN_COMMA,           //','
    TOKEN_COLON,           //':'
    TOKEN_LEFT_PAREN,       //'('
    TOKEN_RIGHT_PAREN,       //')'
    TOKEN_LEFT_BRACKET,       //'['
    TOKEN_RIGHT_BRACKET,       //']'
    TOKEN_LEFT_BRACE,       //'{'
    TOKEN_RIGHT_BRACE,       //'}'
    TOKEN_DOT,           //'.'
    TOKEN_DOT_DOT,       //'..'
    
    //简单双目运算符
    TOKEN_ADD,           //'+'
    TOKEN_SUB,           //'-'
    TOKEN_MUL,           //'*'
    TOKEN_DIV,           //'/'
    TOKEN_MOD,           //'%'
    
    //赋值运算符
    TOKEN_ASSIGN,       //'='
    
    // 位运算符
    TOKEN_BIT_AND,       //'&'
    TOKEN_BIT_OR,       //'|'
    TOKEN_BIT_NOT,       //'~'
    TOKEN_BIT_SHIFT_RIGHT,  //'>>'
    TOKEN_BIT_SHIFT_LEFT,   //'<<'
    
    // 逻辑运算符
    TOKEN_LOGIC_AND,       //'&&'
    TOKEN_LOGIC_OR,       //'||'
    TOKEN_LOGIC_NOT,       //'!'
    
    //关系操作符
    TOKEN_EQUAL,           //'=='
    TOKEN_NOT_EQUAL,       //'!='
    TOKEN_GREATE,       //'>'
    TOKEN_GREATE_EQUAL,       //'>='
    TOKEN_LESS,           //'<'
    TOKEN_LESS_EQUAL,       //'<='
    
    TOKEN_QUESTION,       //'?'
    
    //文件结束标记,仅词法分析时使用
    TOKEN_EOF,           //'EOF'
    /* NOTE 添加新的Key(关键字)需在此之后  */
    TOKEN_ELIF          //'elif'
};

typedef struct
{
    TokenType type;
    const char *start;
    uint32_t length;
    uint32_t lineNo;
    // Value value;
} Token;

class parser
{
  public:
    const char *file;
    const char *sourceCode;
    const char *nextCharPtr;
    char curChar;
    Token curToken;
    Token preToken;
    int interpolationExpectRightParenNum;       // 期望的右括号数量,用于跟踪小括号对儿的嵌套
    parser *parent;  //指向父parser
    VM *vm;
    
    static char lookAheadChar(Parser *parser);
    static void getNextChar(Parser *parser);
    static void getNextToken(Parser *parser);
    static bool matchToken(Parser *parser, TokenType expected);
    static void consumeCurToken(Parser *parser, TokenType expected, const char *errMsg);
    static void consumeNextToken(Parser *parser, TokenType expected, const char *errMsg);
    static void initParser(VM *vm, Parser *parser, const char *file, const char *sourceCode);
    
};

namespace parserUtil
{
    static TokenType idOrkeyword(const char *start, uint32_t length);
    static bool matchNextChar(Parser *parser, char expectedChar);
    static void skipBlanks(Parser *parser);
    static void parseId(Parser *parser, TokenType type);
    static void parseHexNum(Parser *parser);
    static void parseDecNum(Parser *parser);
    static void parseOctNum(Parser *parser);
    static void parseNum(Parser *parser);
    static void parseUnicodeCodePoint(Parser *parser, Buffer<Byte> *buf);
    static void parseString(Parser *parser);
    static void skipAline(Parser *parser);
    static void skipComment(Parser *parser);
}

uint32_t getByteNumOfEncodeUtf8(int value);

uint8_t encodeUtf8(uint8_t *buf, int value);

#define PEEK_TOKEN(parserPtr) parserPtr->curToken.type

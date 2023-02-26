//
// Created by MorbidArk on 2023/2/25.
//

#include <cstring>
#include <cctype>
#include "parser.hpp"

using namespace parserUtil;

typedef struct keywordToken
{
    const char *keyword;
    uint8_t length;
    TokenType token;
} KeywordToken;  //关键字(保留字)结构

//关键字查找表
KeywordToken keywordsToken[] = {
    /* NOTE 添加新的Key(关键字)需在此添加形如 {"elif",     4, TOKEN_ELIF} 语句  */
    { "Tide", 4, TokenType::TOKEN_TIDE },
    { "fun", 3, TokenType::TOKEN_FUN },
    { "if", 2, TokenType::TOKEN_IF },
    { "elif", 4, TokenType::TOKEN_ELIF },
    { "else", 4, TokenType::TOKEN_ELSE },
    { "true", 4, TokenType::TOKEN_TRUE },
    { "false", 5, TokenType::TOKEN_FALSE },
    { "while", 5, TokenType::TOKEN_WHILE },
    { "for", 3, TokenType::TOKEN_FOR },
    { "break", 5, TokenType::TOKEN_BREAK },
    { "continue", 8, TokenType::TOKEN_CONTINUE },
    { "return", 6, TokenType::TOKEN_RETURN },
    { "null", 4, TokenType::TOKEN_NULL },
    { "class", 5, TokenType::TOKEN_CLASS },
    { "is", 2, TokenType::TOKEN_IS },
    { "static", 6, TokenType::TOKEN_STATIC },
    { "this", 4, TokenType::TOKEN_THIS },
    { "super", 5, TokenType::TOKEN_SUPER },
    { "import", 6, TokenType::TOKEN_IMPORT },
    { nullptr, 0, TokenType::TOKEN_UNKNOWN }
};

// 判断start是否为关键字并返回相应的token
static TokenType parserUtil::idOrkeyword(const char *start, uint32_t length)
{
    uint32_t idx = 0;
    while (keywordsToken[idx].keyword != nullptr)
    {
        if (keywordsToken[idx].length == length &&
            memcmp(keywordsToken[idx].keyword, start, length) == 0)
        {
            return keywordsToken[idx].token;
        }
        idx++;
    }
    return TokenType::TOKEN_ID;
}

char parser::lookAheadChar(Parser *parser)
{
    return *parser->nextCharPtr;
}

void parser::getNextChar(Parser *parser)
{
    parser->curChar = *parser->nextCharPtr++;
}

bool parserUtil::matchNextChar(Parser *parser, char expectedChar)
{
    if (parser::lookAheadChar(parser) == expectedChar)
    {
        parser::getNextChar(parser);
        return true;
    }
    return false;
}

static void parserUtil::skipBlanks(Parser *parser)
{
    while (isspace(parser->curChar))
    {
        if (parser->curChar == '\n')
        {
            parser->curToken
                  .lineNo++;
        }
        parser::getNextChar(parser);
    }
}

static void parserUtil::parseId(Parser *parser, TokenType type)
{
    while (isalnum(parser->curChar) || parser->curChar == '_')
    {
        parser::getNextChar(parser);
    }
    
    //nextCharPtr会指向第1个不合法字符的下一个字符,因此-1
    auto length = (uint32_t)(parser->nextCharPtr - parser->curToken
                                                         .start - 1);
    if (type != TokenType::TOKEN_UNKNOWN)
    {
        parser->curToken
              .type = type;
    }
    else
    {
        parser->curToken
              .type = idOrkeyword(parser->curToken
                                        .start, length);
    }
    parser->curToken
          .length = length;
}

static void parserUtil::parseHexNum(Parser *parser)
{
    while (isxdigit(parser->curChar))
    {
        parser::getNextChar(parser);
    }
}

static void parserUtil::parseDecNum(Parser *parser)
{
    while (isdigit(parser->curChar))
    {
        parser::getNextChar(parser);
    }
    
    //若有小数点
    if (parser->curChar == '.' && isdigit(parser::lookAheadChar(parser)))
    {
        parser::getNextChar(parser);
        while (isdigit(parser->curChar))
        { //解析小数点之后的数字
            parser::getNextChar(parser);
        }
    }
}

static void parserUtil::parseOctNum(Parser *parser)
{
    while (parser->curChar >= '0' && parser->curChar < '8')
    {
        parser::getNextChar(parser);
    }
}

static void parserUtil::parseNum(Parser *parser)
{
    //十六进制的0x前缀
    if (parser->curChar == '0' && matchNextChar(parser, 'x'))
    {
        parser::getNextChar(parser);  //跳过'x'
        parseHexNum(parser);   //解析十六进制数字
    }
    else if (parser->curChar == '0'
        && isdigit(parser::lookAheadChar(parser)))
    {  // 八进制
        parseOctNum(parser);
    }
    else
    {      //解析十进制
        parseDecNum(parser);
    }
    //nextCharPtr会指向第1个不合法字符的下一个字符,因此-1
    parser->curToken
          .length =
        (uint32_t)(parser->nextCharPtr - parser->curToken
                                               .start - 1);
    parser->curToken
          .type = TokenType::TOKEN_NUM;
}

static void parserUtil::parseUnicodeCodePoint(Parser *parser, Buffer<Byte> *buf)
{
    uint32_t idx = 0;
    int value = 0;
    uint8_t digit = 0;

//获取数值,u后面跟着4位十六进制数字
    while (idx++ < 4)
    {
        parser::getNextChar(parser);
        if (parser->curChar == '\0')
        {
            LEX_ERROR(parser, "unterminated unicode!");
        }
        if (parser->curChar >= '0' && parser->curChar <= '9')
        {
            digit = parser->curChar - '0';
        }
        else if (parser->curChar >= 'a' && parser->curChar <= 'f')
        {
            digit = parser->curChar - 'a' + 10;
        }
        else if (parser->curChar >= 'A' && parser->curChar <= 'F')
        {
            digit = parser->curChar - 'A' + 10;
        }
        else
        {
            LEX_ERROR(parser, "invalid unicode!");
        }
        value = value * 16 | digit;
    }
    
    uint32_t byteNum = getByteNumOfEncodeUtf8(value);
    ASSERT(byteNum != 0, "utf8 encode bytes should be between 1 and 4!");
    
    //为代码通用, 下面会直接写buf->datas,在此先写入byteNum个0,以保证事先有byteNum个空间
    Buffer<Byte>::BufferFillWrite(parser->vm, buf, 0, byteNum);
    
    //把value编码为utf8后写入缓冲区buf
    encodeUtf8(buf->datas + buf->count - byteNum, value);
}

static void parserUtil::parseString(Parser *parser)
{
    Buffer<Byte> str;
    Buffer<Byte>::BufferInit(&str);
    while (true)
    {
        parser::getNextChar(parser);
        
        if (parser->curChar == '\0')
        {
            LEX_ERROR(parser, "unterminated string!");
        }
        
        if (parser->curChar == '"')
        {
            parser->curToken
                  .type = TokenType::TOKEN_STRING;
            break;
        }
        
        if (parser->curChar == '%')
        {
            if (!matchNextChar(parser, '('))
            {
                LEX_ERROR(parser, "'%' should followed by '('!");
            }
            if (parser->interpolationExpectRightParenNum > 0)
            {
                COMPILE_ERROR(parser, "sorry, I don`t support nest interpolate expression!");
            }
            parser->interpolationExpectRightParenNum = 1;
            parser->curToken
                  .type = TokenType::TOKEN_INTERPOLATION;
            break;
        }
        
        if (parser->curChar == '\\')
        {   //处理转义字符
            parser::getNextChar(parser);
            switch (parser->curChar)
            {
            case '0':
                Buffer<Byte>::BufferAdd(parser->vm, &str, '\0');
                break;
            case 'a':
                Buffer<Byte>::BufferAdd(parser->vm, &str, '\a');
                break;
            case 'b':
                Buffer<Byte>::BufferAdd(parser->vm, &str, '\b');
                break;
            case 'f':
                Buffer<Byte>::BufferAdd(parser->vm, &str, '\f');
                break;
            case 'n':
                Buffer<Byte>::BufferAdd(parser->vm, &str, '\n');
                break;
            case 'r':
                Buffer<Byte>::BufferAdd(parser->vm, &str, '\r');
                break;
            case 't':
                Buffer<Byte>::BufferAdd(parser->vm, &str, '\t');
                break;
            case 'u':
                parseUnicodeCodePoint(parser, &str);
                break;
            case '"':
                Buffer<Byte>::BufferAdd(parser->vm, &str, '"');
                break;
            case '\\':
                Buffer<Byte>::BufferAdd(parser->vm, &str, '\\');
                break;
            default:
                LEX_ERROR(parser, "unsupport escape \\%c", parser->curChar);
                break;
            }
        }
        else
        {   //普通字符
            Buffer<Byte>::BufferAdd(parser->vm, &str, parser->curChar);
        }
    }
    
    Buffer<Byte>::BufferClear(parser->vm, &str);
}

static void parserUtil::skipAline(Parser *parser)
{
    parser::getNextChar(parser);
    while (parser->curChar != '\0')
    {
        if (parser->curChar == '\n')
        {
            parser->curToken
                  .lineNo++;
            parser::getNextChar(parser);
            break;
        }
        parser::getNextChar(parser);
    }
}

static void parserUtil::skipComment(Parser *parser)
{
    char nextChar = parser::lookAheadChar(parser);
    if (parser->curChar == '/')
    {  // 行注释
        skipAline(parser);
    }
    else
    {   // 区块注释
        while (nextChar != '*' && nextChar != '\0')
        {
            parser::getNextChar(parser);
            if (parser->curChar == '\n')
            {
                parser->curToken
                      .lineNo++;
            }
            nextChar = parser::lookAheadChar(parser);
        }
        if (matchNextChar(parser, '*'))
        {
            if (!matchNextChar(parser, '/'))
            {   //匹配*/
                LEX_ERROR(parser, "expect '/' after '*'!");
            }
            parser::getNextChar(parser);
        }
        else
        {
            LEX_ERROR(parser, "expect '*/' before file end!");
        }
    }
    skipBlanks(parser);  //注释之后有可能会有空白字符
}

void parser::getNextToken(Parser *parser)
{
    parser->preToken = parser->curToken;
    skipBlanks(parser);  // 跳过待识别单词之前的空格
    parser->curToken
          .type = TokenType::TOKEN_EOF;
    parser->curToken
          .length = 0;
    parser->curToken
          .start = parser->nextCharPtr - 1;
    while (parser->curChar != '\0')
    {
        switch (parser->curChar)
        {
        case ',':
            parser->curToken
                  .type = TokenType::TOKEN_COMMA;
            break;
        case ':':
            parser->curToken
                  .type = TokenType::TOKEN_COLON;
            break;
        case '(':
            if (parser->interpolationExpectRightParenNum > 0)
            {
                parser->interpolationExpectRightParenNum++;
            }
            parser->curToken
                  .type = TokenType::TOKEN_LEFT_PAREN;
            break;
        case ')':
            if (parser->interpolationExpectRightParenNum > 0)
            {
                parser->interpolationExpectRightParenNum--;
                if (parser->interpolationExpectRightParenNum == 0)
                {
                    parseString(parser);
                    break;
                }
            }
            parser->curToken
                  .type = TokenType::TOKEN_RIGHT_PAREN;
            break;
        case '[':
            parser->curToken
                  .type = TokenType::TOKEN_LEFT_BRACKET;
            break;
        case ']':
            parser->curToken
                  .type = TokenType::TOKEN_RIGHT_BRACKET;
            break;
        case '{':
            parser->curToken
                  .type = TokenType::TOKEN_LEFT_BRACE;
            break;
        case '}':
            parser->curToken
                  .type = TokenType::TOKEN_RIGHT_BRACE;
            break;
        case '.':
            if (matchNextChar(parser, '.'))
            {
                parser->curToken
                      .type = TokenType::TOKEN_DOT_DOT;
            }
            else
            {
                parser->curToken
                      .type = TokenType::TOKEN_DOT;
            }
            break;
        case '=':
            if (matchNextChar(parser, '='))
            {
                parser->curToken
                      .type = TokenType::TOKEN_EQUAL;
            }
            else
            {
                parser->curToken
                      .type = TokenType::TOKEN_ASSIGN;
            }
            break;
        case '+':
            parser->curToken
                  .type = TokenType::TOKEN_ADD;
            break;
        case '-':
            parser->curToken
                  .type = TokenType::TOKEN_SUB;
            break;
        case '*':
            parser->curToken
                  .type = TokenType::TOKEN_MUL;
            break;
        case '/':
            //跳过注释'//'或'/*'
            if (matchNextChar(parser, '/') || matchNextChar(parser, '*'))
            {   //跳过注释'//'或'/*'
                skipComment(parser);
                
                //重置下一个token起始地址
                parser->curToken
                      .start = parser->nextCharPtr - 1;
                continue;
            }
            else
            {         // '/'
                parser->curToken
                      .type = TokenType::TOKEN_DIV;
            }
            break;
        case '%':
            parser->curToken
                  .type = TokenType::TOKEN_MOD;
            break;
        case '&':
            if (matchNextChar(parser, '&'))
            {
                parser->curToken
                      .type = TokenType::TOKEN_LOGIC_AND;
            }
            else
            {
                parser->curToken
                      .type = TokenType::TOKEN_BIT_AND;
            }
            break;
        case '|':
            if (matchNextChar(parser, '|'))
            {
                parser->curToken
                      .type = TokenType::TOKEN_LOGIC_OR;
            }
            else
            {
                parser->curToken
                      .type = TokenType::TOKEN_BIT_OR;
            }
            break;
        case '~':
            parser->curToken
                  .type = TokenType::TOKEN_BIT_NOT;
            break;
        case '?':
            parser->curToken
                  .type = TokenType::TOKEN_QUESTION;
            break;
        case '>':
            if (matchNextChar(parser, '='))
            {
                parser->curToken
                      .type = TokenType::TOKEN_GREATE_EQUAL;
            }
            else if (matchNextChar(parser, '>'))
            {
                parser->curToken
                      .type = TokenType::TOKEN_BIT_SHIFT_RIGHT;
            }
            else
            {
                parser->curToken
                      .type = TokenType::TOKEN_GREATE;
            }
            break;
        case '<':
            if (matchNextChar(parser, '='))
            {
                parser->curToken
                      .type = TokenType::TOKEN_LESS_EQUAL;
            }
            else if (matchNextChar(parser, '<'))
            {
                parser->curToken
                      .type = TokenType::TOKEN_BIT_SHIFT_LEFT;
            }
            else
            {
                parser->curToken
                      .type = TokenType::TOKEN_LESS;
            }
            break;
        case '!':
            if (matchNextChar(parser, '='))
            {
                parser->curToken
                      .type = TokenType::TOKEN_NOT_EQUAL;
            }
            else
            {
                parser->curToken
                      .type = TokenType::TOKEN_LOGIC_NOT;
            }
            break;
        
        case '"':
            parseString(parser);
            break;
        
        default:
            //处理变量名及数字
            //进入此分支的字符肯定是数字或变量名的首字符
            //后面会调用相应函数把其余字符一并解析
            
            //首字符是字母或'_'则是变量名
            if (isalpha(parser->curChar) || parser->curChar == '_')
            {
                parseId(parser, TokenType::TOKEN_UNKNOWN);  //解析变量名其余的部分
            }
            else if (isdigit(parser->curChar))
            { //数字
                parseNum(parser);
            }
            else
            {
                if (parser->curChar == '#' && matchNextChar(parser, '!'))
                {
                    skipAline(parser);
                    parser->curToken
                          .start = parser->nextCharPtr - 1;  //重置下一个token起始地址
                    continue;
                }
                LEX_ERROR(parser, "unsupport char: \'%c\', quit.", parser->curChar);
            }
            return;
        }
        //大部分case的出口
        parser->curToken
              .length = (uint32_t)(parser->nextCharPtr - parser->curToken
                                                               .start);
        getNextChar(parser);
        return;
    }
}

bool parser::matchToken(Parser *parser, TokenType expected)
{
    if (parser->curToken
              .type == expected)
    {
        getNextToken(parser);
        return true;
    }
    return false;
}

//断言当前token为expected并读入下一token,否则报错errMsg
void parser::consumeCurToken(Parser *parser, TokenType expected, const char *errMsg)
{
    if (parser->curToken
              .type != expected)
    {
        COMPILE_ERROR(parser, errMsg);
    }
    getNextToken(parser);
}

//断言下一个token为expected,否则报错errMsg
void parser::consumeNextToken(Parser *parser, TokenType expected, const char *errMsg)
{
    getNextToken(parser);
    if (parser->curToken
              .type != expected)
    {
        COMPILE_ERROR(parser, errMsg);
    }
}

void parser::initParser(VM *vm, Parser *parser, const char *file, const char *sourceCode)
{
    parser->file = file;
    parser->sourceCode = sourceCode;
    parser->curChar = *parser->sourceCode;
    parser->nextCharPtr = parser->sourceCode + 1;
    parser->curToken.lineNo = 1;
    parser->curToken.type = TokenType::TOKEN_UNKNOWN;
    parser->curToken.start = nullptr;
    parser->curToken.length = 0;
    parser->preToken = parser->curToken;
    parser->interpolationExpectRightParenNum = 0;
    parser->vm = vm;
}

parser::parser(VM *vm, const char *file, const char *sourceCode)
{
    this->file = file;
    this->sourceCode = sourceCode;
    this->curChar = *this->sourceCode;
    this->nextCharPtr = this->sourceCode + 1;
    this->curToken.lineNo = 1;
    this->curToken.type = TokenType::TOKEN_UNKNOWN;
    this->curToken.start = nullptr;
    this->curToken.length = 0;
    this->preToken = this->curToken;
    this->interpolationExpectRightParenNum = 0;
    this->vm = vm;
}


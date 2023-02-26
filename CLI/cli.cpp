//
// Created by MorbidArk on 2023/2/19.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include "cli.hpp"
#include "utils.hpp"

#include "vm.hpp"
#include "parser.hpp"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

namespace cli
{
    const short MAX_LINE_LEN = 1024;
    char *rootDir = nullptr;
}

void Cli::runFile(const char *path)
{
    const char *lastSlash = strrchr(path, '/');
    if (lastSlash != nullptr)
    {
        char *root = (char *)malloc(lastSlash - path + 2);
        memcpy(root, path, lastSlash - path + 1);
        root[lastSlash - path + 1] = '\0';
        cli::rootDir = root;
    }
    
    /* TODO <Cli::runFile> 读入文件,执行语句 executeModule() */
    std::string sourceCode = readFile(path);
    cout << sourceCode.c_str() << endl;
}

void Cli::runFileToken(const char *path)
{
    const char *lastSlash = strrchr(path, '/');
    if (lastSlash != nullptr)
    {
        char *root = (char *)malloc(lastSlash - path + 2);
        memcpy(root, path, lastSlash - path + 1);
        root[lastSlash - path + 1] = '\0';
        cli::rootDir = root;
    }
    
    VM *vm = new class vm();
    std::string sourceCode = readFile(path);
    
    Parser parser(vm, path, sourceCode.c_str());
    cout << "SourceCode is :\n"
         << sourceCode.c_str()
         << "===SourceCode End===\n";
    
    const char *tokenArray[] = {
        "UNKNOWN",
        "NUM",
        "STRING",
        "ID",
        "INTERPOLATION",
        "VAR",
        "FUN",
        "IF",
        "ELSE",
        "TRUE",
        "FALSE",
        "WHILE",
        "FOR",
        "BREAK",
        "CONTINUE",
        "RETURN",
        "NULL",
        "CLASS",
        "THIS",
        "STATIC",
        "IS",
        "SUPER",
        "IMPORT",
        "COMMA",
        "COLON",
        "LEFT_PAREN",
        "RIGHT_PAREN",
        "LEFT_BRACKET",
        "RIGHT_BRACKET",
        "LEFT_BRACE",
        "RIGHT_BRACE",
        "DOT",
        "DOT_DOT",
        "ADD",
        "SUB",
        "MUL",
        "DIV",
        "MOD",
        "ASSIGN",
        "BIT_AND",
        "BIT_OR",
        "BIT_NOT",
        "BIT_SHIFT_RIGHT",
        "BIT_SHIFT_LEFT",
        "LOGIC_AND",
        "LOGIC_OR",
        "LOGIC_NOT",
        "EQUAL",
        "NOT_EQUAL",
        "GREATE",
        "GREATE_EQUAL",
        "LESS",
        "LESS_EQUAL",
        "QUESTION",
        "EOF"
    };
    
    while (parser.curToken.type != TokenType::TOKEN_EOF)
    {
        parser::getNextToken(&parser);
        printf("%dL: %s [", parser.curToken.lineNo, tokenArray[int(parser.curToken.type)]);
        uint32_t idx = 0;
        while (idx < parser.curToken.length)
        {
            printf("%c", *(parser.curToken.start + idx++));
        }
        printf("]\n");
    }
}

char *Cli::getFilePath(const char *moduleName)
{
    uint32_t rootDirLength = cli::rootDir == nullptr ? 0 : strlen(cli::rootDir);
    uint32_t nameLength = strlen(moduleName);
    uint32_t pathLength = rootDirLength + nameLength + strlen(".vt");
    char *path = (char *)malloc(pathLength + 1);
    
    if (cli::rootDir != nullptr)
    {
        memmove(path, cli::rootDir, rootDirLength);
    }
    
    memmove(path + rootDirLength, moduleName, nameLength);
    memmove(path + rootDirLength + nameLength, ".vt", 3);
    path[pathLength] = '\0';
    
    return path;
}

std::string Cli::readFile(const char *path)
{
    /* TODO 文件编解码输入标准ASCII流 */
    std::ifstream infile(path);
    if (!infile.is_open())
    {
        IO_ERROR("Could`t open file \"%s\".\n", path);
    }
    std::stringstream stringBuffer;
    stringBuffer << infile.rdbuf();
    infile.close();
    std::string fileContent(stringBuffer.str());
    
    return fileContent;
}

void Cli::runCli()
{
    
    char *lineStr = new char[cli::MAX_LINE_LEN];
    
    while (true)
    {
        try
        {
            if (!cin.getline(lineStr, (cli::MAX_LINE_LEN + 1))
                    .gcount())
            {
                throw std::runtime_error("Input Eerror <Cli::runCli>");
            }
        }
        catch (std::runtime_error &err)
        {
            cerr << err.what() << endl;
            cin.clear();
            continue;
        }
        if (!strncmp(lineStr, "exit", 4))
        {
            break;
        }
        
        /* TODO <Cli::runCli> 执行语句 executeModule() */
        cout << lineStr << endl;
    }
    delete[] lineStr;
}


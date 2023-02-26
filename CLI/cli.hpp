//
// Created by MorbidArk on 2023/2/19.
//

#pragma once

#include <string>
#include <sstream>

#define VERSION "0.1.1"

class Cli
{
  public:
    static void runFile(const char *path);
    
    static void runFileToken(const char *path);
    
    static char *getFilePath(const char *moduleName);
    
    static std::string readFile(const char *path);
    
    static void runCli();
    
    inline static const char *logoImg = "                         11111111111111111111\r\n"
                                        "                    111111i                1111111\r\n"
                                        "       :1        1111i                          11111\r\n"
                                        "       11     1111                                 :111i\r\n"
                                        "\r\n"
                                        "  111111111111     111,       111,            111   11111,  111,   11111111111,\r\n"
                                        "      1111                    111,  111111111  11 11,               11      11,\r\n"
                                        "      1111         111, ;11111111,  11     11  111,         111,    1111111111,\r\n"
                                        "      1111         111, 111,  111,  111111111  11,          111,    11,\r\n"
                                        "      1111         111, 111,  111,  111        11,          111,    11,\r\n"
                                        "      ;;;;         111,  11111111,  i11111111  11,          111,    11,\r\n"
                                        "\r\n"
                                        "  1, ,111111111111:                                     .:i11111111111i,\r\n"
                                        "1111111             111.                 .;11111111111111111111111111111111111i\r\n"
                                        "                       i111111111111111111111111111111111111111111111\r\n"
                                        "                                 .....                           ,:\r\n";
    inline static const char *versionMsg = ("Tiderip Version:" VERSION "\r\n");
};

//
// Created by MorbidArk on 2023/2/19.
//

#include <iostream>
#include "cli.hpp"
#include "clipp.h"

using std::cout;
using std::endl;
using std::string;

[[maybe_unused]]
unsigned char flagDebug = 0;

enum class mode
{
    runFileMode, runCliMode, helpMode
};

#include "metaObj.hpp"
#include "vm.hpp"

int main()
{
    VM *vm = new class vm();
    ObjModule *objModule = new ObjModule(vm, "1234");
    
    ObjModule *objModule1 = ObjModule::newObjModule(vm, "123");
    
    cout << objModule->name->value.start
         << endl
         << objModule1->name->value.start;
    
    return 0;
}

#if 0
int main(int argc, char *argv[])
{
    
    mode selected = mode::helpMode;
    string input;
    string dict, out;
    
    bool progr = false;
    
    auto out_debug =
        (
            (clipp::option("-o", "--output").set(progr) &
                clipp::value("outfile", out)
            ) % "Output content write to file",
                clipp::option("-d", "--debug")
                    .call([] { flagDebug = 1; })
                    .doc("debug mode on")
        );
    
    auto runFileMode =
        (
            clipp::command("make").set(selected, mode::runFileMode),
                clipp::values("filePath", input),
                out_debug
        );
    
    auto runCliMode =
        (
            clipp::command("cli").set(selected, mode::runCliMode),
                out_debug
        );
    
    auto helpMode =
        (
            clipp::command("help")
                .set(selected, mode::helpMode)
        );
    
    auto cli =
        (
            (runFileMode | runCliMode | helpMode),
                clipp::option("-v", "--version")
                    .call([] {
                      cout << Cli::logoImg
                           << "\t\t\t"
                           << Cli::versionMsg;
                    })
                    .doc("show version")
        );
    
    if (argc == 1)
    {
        cout << clipp::usage_lines(cli, "Tiderip")
             << endl;
        return 0;
    }
    
    if (parse(argc, argv, cli))
    {
        switch (selected)
        {
        case mode::runFileMode:
            Cli::runFile(input.c_str());
            break;
        case mode::runCliMode:
            Cli::runCli();
            break;
        case mode::helpMode:
            cout << make_man_page(cli, "Tiderip");
            break;
        }
    }
    return 0;
}
#endif

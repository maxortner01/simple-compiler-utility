#include "compu/Core/Compiler.h"
#include "compu/Util/Log.h"

#include <vector>

namespace compu
{
    std::string Compiler::executeCommand(const std::string &command)
    {
        system(command.c_str());
        return "";
    }

    void Compiler::changeDirectory(const char* dir)
    {
        COMP_DEBUG("Changing to directory: {0}", dir);

        std::string cmd = "cd ";
        cmd += dir;

        executeCommand(cmd);
    }
}
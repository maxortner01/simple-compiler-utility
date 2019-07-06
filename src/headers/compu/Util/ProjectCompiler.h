#pragma once

#include "compu/Core/Compiler.h"

namespace compu
{
    // If more compilers are added in the Compilers folder,
    // add an enum here and expand the switch statement in getCompiler
    enum class ProjectCompiler
    {
        MinGW
    };

    class CompilerObject
    {
        static ProjectCompiler _comp;

    protected:
        static Compiler* getCompiler();
        static void setCompiler(ProjectCompiler comp);
    };
}
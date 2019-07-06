#include "compu/Util/ProjectCompiler.h"

#include "compu/Core/Compilers/MinGW.h"

namespace compu
{
    ProjectCompiler CompilerObject::_comp = ProjectCompiler::MinGW;

    Compiler* CompilerObject::getCompiler()
    {
        switch (_comp)
        {
        case ProjectCompiler::MinGW:
            return &MinGW::get();
        
        default:
            return nullptr;
        }
    }

    void CompilerObject::setCompiler(ProjectCompiler comp)
    {
        _comp = comp;
    }
}
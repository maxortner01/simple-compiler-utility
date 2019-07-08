#pragma once

#include <vector>
#include <string>
#include "compu/Util/Singleton.h"

namespace compu
{
    class Project;

    enum CompilerFlags
    {
        COMP_OK
    };

    #define COMP_BODY_DEF(bef, aft) \
        bef std::string getStaticExtension()  const aft\
        bef std::string getDynamicExtension() const aft\
        bef int         compileCppToO(const char* cppfile, Project* proj, std::vector<std::string> flags = {}) const aft\
        bef int         linkObjsToFin(std::vector<std::string> files, Project* proj) const aft\
        bef void        removeFile(std::string dir) const aft

    #define COMPILE_DERIVE(cls) : public Compiler, public Util::Singleton<cls> {\
        COMP_BODY_DEF( , override;) }

    class Compiler
    {
    protected:
        static std::string executeCommand(const std::string &command);

    public: 
        void changeDirectory(const char* dir) const;
        void makeFileModTimeCurrent(const char* file) const;
        int  getFileModTime(const char* file) const;

        COMP_BODY_DEF(virtual, = 0;);
    };
}
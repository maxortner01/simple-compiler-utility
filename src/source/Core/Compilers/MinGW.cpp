#include "compu/Core/Compilers/MinGW.h"
#include "compu/Core/Project.h"

namespace compu
{
    std::string MinGW::getStaticExtension() const 
    {
        return ".a";
    }

    std::string MinGW::getDynamicExtension() const 
    {
        return ".so";
    }

    int MinGW::compileCppToO(const char* cppfile, Project* proj, std::vector<std::string> flags) const
    {
        std::string cpp_file;

        int dot;
        int slash = 0;

        for (int i = (unsigned int)strlen(cppfile) - 1; i < (unsigned int)strlen(cppfile); i--)
        {
            /**/ if (cppfile[i] == '.')
            {
                dot = i;
            }
            else if (cppfile[i] == '/')
            {
                slash = i + 1;
                break;
            }
        }

        cpp_file = std::string(cppfile + slash, cppfile + dot);

        std::string command;
        command += "g++ "; 
        
        /**/ if (proj->getProjectType() == StaticLibrary)
            command += " -fPIC --static ";
        else if (proj->getProjectType() == DynamicLibrary)
            command += " -fPIC ";

        std::string ofile = proj->getDirectories().obj + cpp_file + ".o";

        // Get the system modification times for both
        int o_mod = getFileModTime(ofile.c_str());
        int c_mod = getFileModTime(cppfile);

        if (o_mod - c_mod == 0)
            return COMP_OK;

        command += " -O1 ";
        command += " -c -o " + ofile + " " + cppfile + " ";

        std::pair<std::vector<std::string>*, std::string> lists[] = {
            { &proj->getIncludeDirectories(), "-I" },
            { &flags,                         "-D" }
        };

        for (int k = 0; k < 2; k++)
            for (int i = 0; i < lists[k].first->size(); i++)
                command += lists[k].second + lists[k].first->at(i) + " ";

        COMP_TRACE("Compiling CPP file '{0}' into file '{1}'", cppfile, ofile);

        //COMP_DEBUG(command);
        executeCommand(command);

        makeFileModTimeCurrent(cppfile);
        makeFileModTimeCurrent(ofile.c_str());

        return COMP_OK;
    }

    int MinGW::linkObjsToFin(std::vector<std::string> files, Project* proj) const 
    {
        std::string command;
        command += "g++ "; 
        
        if (proj->getProjectType() > Executable)
            command += "-shared";

        command += " -o " + proj->getDirectories().bin + proj->getOutputFile() + " ";

        for (int i = 0; i < files.size(); i++)
            command += files.at(i) + " ";

        std::pair<std::vector<std::string>*, std::string> lists[] = {
            { &proj->getLibraryDirectories(), "-L" },
            { &proj->getLibraryFiles()      , "-l" }
        };

        for (int k = 0; k < 2; k++)
            for (int i = 0; i < lists[k].first->size(); i++)
                command += lists[k].second + lists[k].first->at(i) + " ";

        if (proj->getProjectType() == Executable)
            command += " -static";

        //COMP_DEBUG(command);
        executeCommand(command);

        return COMP_OK;
    }

    void MinGW::removeFile(std::string dir) const
    {
#ifndef WIN32
        executeCommand("rm " + dir);
#else
        executeCommand("del " + dir);
#endif
    }
}
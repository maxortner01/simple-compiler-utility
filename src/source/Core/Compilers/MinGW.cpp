#include "compu/Core/Compilers/MinGW.h"
#include "compu/Core/Project.h"

#ifndef WIN32
#include <unistd.h>
#else
#define stat _stat
#endif

#include <iostream>

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
            if (cppfile[i] == '.')
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
        int o_mod = -1;
        int c_mod = -1;
        struct stat result;
        if (stat(ofile.c_str(), &result) == 0)
        {
#ifndef WIN32
            o_mod = result.st_mtim.tv_sec;
#else
            o_mod = (int)result.st_mtime;
#endif
        }
        if (stat(cppfile, &result) == 0)
        {
#ifndef WIN32
            c_mod = result.st_mtim.tv_sec;
#else
            c_mod = (int)result.st_mtime;
#endif
        }

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

#ifndef WIN32
        executeCommand("touch " + std::string(cppfile));
        executeCommand("touch " + ofile);
#else
        executeCommand("type nul >> " + std::string(cppfile));
        executeCommand("type nul >> " + ofile);
#endif

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

        //COMP_DEBUG(command);
        executeCommand(command);

        return COMP_OK;
    }

    void MinGW::removeFile(std::string dir) const
    {
#ifndef WIN32
        executeCommand("rm " + dir);
#endif
    }
}
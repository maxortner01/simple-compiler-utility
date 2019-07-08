#include "compu/Core/Compiler.h"
#include "compu/Util/Log.h"

#include <vector>

#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#define stat _stat
#endif

namespace compu
{
    std::string Compiler::executeCommand(const std::string &command)
    {
        system(command.c_str());
        return "";
    }

    void Compiler::changeDirectory(const char* dir) const
    {
        COMP_DEBUG("Changing to directory: {0}", dir);

        std::string cmd = "cd ";
        cmd += dir;

        executeCommand(cmd);
    }

    void Compiler::makeFileModTimeCurrent(const char* file) const
    {
#ifndef WIN32
        // Using Ubuntu's simple, elegent way of changint file modification times 
        executeCommand("touch " + std::string(file));
#else   
        // Using windows' overlly verbose API to handle changing the file modification times 
        SYSTEMTIME systemtime;
        GetSystemTime(&systemtime);

        FILETIME filetime;
        SystemTimeToFileTime(&systemtime, &filetime);

        HANDLE cpp_filename = CreateFile(file, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            
        SetFileTime(cpp_filename, (LPFILETIME)NULL, (LPFILETIME)NULL, &filetime);

        CloseHandle(cpp_filename);
#endif
    }
    
    int Compiler::getFileModTime(const char* file) const
    {
        struct stat result;
        if (stat(file, &result) == 0)
        {
#ifndef WIN32
            return result.st_mtim.tv_sec;
#else
            return (int)result.st_mtime;
#endif
        }

        return -1;
    }
}
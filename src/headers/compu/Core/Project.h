#pragma once

#include "compu/Util/ProjectCompiler.h"
#include "compu/Util/Log.h"
#include "Compiler.h"

#include <string>
#include <vector>
#include <map>

namespace compu
{
    template<typename T>
    struct Pair { T a; T b; };

    struct Directories {
        std::string bin;
        std::string obj;
        std::string src;
    };

    enum ProjectType
    {
        Executable,
        DynamicLibrary,
        StaticLibrary
    };

    enum class Routine
    {
        Compile,
        Clean,
        Recompile
    };

    class Project : public CompilerObject
    {
        std::string _projectname, _projectdir;
        std::map<std::string, std::vector<std::string>> data;

        std::vector<std::string> collectSourceFiles() const;
        std::vector<std::string> getFilesInDirectory(const char* dir, bool recursive = false, std::vector<const char*> extensions = {}) const;
        std::vector<std::string> getFilesInDirectory(const char* dir, bool recursive = false, const char* extensions = "*") const;
        void insertDataPoint(std::string key, std::vector<std::string> values);

    public:
        // Uses tinyxml to load in a project file
        Project(const char* filename, Routine routine = Routine::Compile);

        void clean();
        void compile  (std::vector<std::string> flags = {}, bool link = true);
        void recompile(std::vector<std::string> flags = {}, bool link = true);

        std::string getName()        const;
        std::string getExtension()   const;
        std::string getOutputFile()  const;
        int         getProjectType() const;

        Directories getDirectories() const;
        std::vector<std::string> &getLibraryFiles();
        std::vector<std::string> &getLibraryDirectories();
        std::vector<std::string> &getIncludeDirectories();

        // Handles directory searching
        static Project* getProjectFromDirectory(const char* name = "*", const char* dir = "./", Routine routine = Routine::Compile);

        // Uses tinyxml2 to generate a project file
        static unsigned int generateProjectFile(const char* name);
    };
}
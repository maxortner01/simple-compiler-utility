#include <string>
#include <vector>
#include <iostream>

#include "compu/Core/Project.h"

using namespace compu;

// makeproject will generate a project
// -N {File name}
// -C {Directory}

int main(int argc, char** argv)
{
    Util::Log::get().init();

    std::vector<Pair<const char*>> arguments;
    for (int i = 0; i < argc; i++)
    {
        if (sizeof(argv[i]) > 0 && argv[i][0] == '-')
        {
            if (i + 1 >= argc)
            {
                COMP_ERROR("No argument value for flag '{0}'!", argv[i]);
                return 1;
            }

            Pair<const char*> p;
            p.a = argv[i];
            p.b = argv[i + 1];
            arguments.push_back(p);
        }
    }
    arguments.shrink_to_fit();

    // DEFAULT if no arguments are given
    // Compile the project file in the given directory
    if (argc - arguments.size() * 2 == 1 || (argc == 2 && std::string(argv[1]) == "compile"))
    {
        const char* dir = "./";
        const char* name = "*";

        for (int i = 0; i < arguments.size(); i++)
        {
            /**/ if (std::string(arguments[i].a) == "-C")
                dir = arguments[i].b;
            else if (std::string(arguments[i].a) == "-N")
                name = arguments[i].b;
        }

        Project* project = Project::getProjectFromDirectory(name, dir);

        if (project == nullptr)
        {
            COMP_ERROR("No project file found in current directory!");
            return 1;
        }

        project->compile();

        // compile project
        return 0;
    }

    // Used to generate a project file
    if (argc >= 2 && std::string(argv[1]) == "makeproject")
    {
        // If no name is given after make project
        if (argc == 2)
        {
            COMP_ERROR("No project name given!");
            return 1;
        }

        unsigned int code = Project::generateProjectFile(argv[2]);

        // generateProjectFIle will return 0 if good
        if (code > 0)
        {
            COMP_ERROR("Project generation exited with code {0}!", code);
            return code;
        }

        COMP_INFO("Project file [ {0} ] generation success", std::string(argv[2]) + ".proj");
        return 0;
    }

    if (argc >= 2 && std::string(argv[1]) == "clean")
    {
        const char* dir = "./";
        const char* name = "*";

        for (int i = 0; i < arguments.size(); i++)
        {
            /**/ if (std::string(arguments[i].a) == "-C")
                dir = arguments[i].b;
            else if (std::string(arguments[i].a) == "-N")
                name = arguments[i].b;
        }

        Project* project = Project::getProjectFromDirectory(name, dir, compu::Routine::Clean);

        project->clean();

        return 0;
    }

    if (argc >= 2 && std::string(argv[1]) == "recompile")
    {
        const char* dir = "./";
        const char* name = "*";

        for (int i = 0; i < arguments.size(); i++)
        {
            /**/ if (std::string(arguments[i].a) == "-C")
                dir = arguments[i].b;
            else if (std::string(arguments[i].a) == "-N")
                name = arguments[i].b;
        }

        Project* project = Project::getProjectFromDirectory(name, dir, compu::Routine::Clean);
        project->clean();

        project = Project::getProjectFromDirectory(name, dir, compu::Routine::Compile);
        project->compile();

        return 0;
    }

    COMP_ERROR("Command '{0}' not recognized!", argv[1]);
    return 1;
}
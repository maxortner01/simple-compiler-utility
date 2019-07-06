#include "compu/Core/Project.h"

#include <iostream>
#include <dirent.h>
#include <tinyxml/tinyxml2.h>

namespace compu
{
    std::vector<std::string> Project::collectSourceFiles() const
    {
        std::vector<std::string> src_files;

        std::string directory = getDirectories().src;

        src_files.shrink_to_fit();
        return src_files;
    }

    std::vector<std::string> Project::getFilesInDirectory(const char* dir, bool recursive, std::vector<const char*> extensions) const
    {
        std::vector<std::string> r;

        for (int i = 0; i < extensions.size(); i++)
        {
            std::vector<std::string> s = getFilesInDirectory(dir, recursive, extensions[i]);
            r.insert(r.end(), s.begin(), s.end());
        }

        return r;
    }

    std::vector<std::string> Project::getFilesInDirectory(const char* dir, bool recursive, const char* extension) const
    {
        std::vector<std::string> r;

        DIR* d;
        struct dirent* ent;
        if ((d = opendir(dir)) != NULL)
        {
            while ((ent = readdir(d)) != NULL)
            {
                std::string itemname = ent->d_name;
                std::string s_ext;

                bool found_dot = false;
                for (int i = 0; i < itemname.length(); i++)
                    if (itemname[i] == '.')
                    {
                        found_dot = true;
                        s_ext = std::string(itemname.begin() + i, itemname.end());
                        break;
                    }

                if (!found_dot && recursive) 
                {
                    std::string directory = dir;
                    directory += itemname + "/";

                    std::vector<std::string> sub = getFilesInDirectory(directory.c_str(), recursive, extension);
                    r.insert(r.end(), sub.begin(), sub.end());
                }
                else if (found_dot)
                {
                    if ((std::string(extension) != "*" && s_ext == std::string(extension)) || std::string(extension) == "*")
                    {
                        std::string finname = dir;
                        finname += itemname;

                        r.push_back(finname);
                    }
                }
            }
            closedir(d);
        }

        r.shrink_to_fit();
        return r;
    }

    void Project::insertDataPoint(std::string key, std::vector<std::string> values)
    {
        std::pair<std::string, std::vector<std::string>> p(key, values);
        data.insert(p);
    }

    Project::Project(const char* filename, Routine routine)
    {
        using namespace tinyxml2;

        _projectname = filename;
        for (int i = _projectname.length(); i >= 0; i--)
            if (_projectname[i] == '/')
            {
                _projectdir = std::string(_projectname.begin(), _projectname.begin() + i + 1);
                _projectname = std::string(_projectname.begin() + i + 1, _projectname.end());
                break;
            }

        XMLDocument doc;
        XMLError err = doc.LoadFile(filename);
        if (err != XML_SUCCESS) 
        {
            COMP_ERROR("{2}: Unable to load file '{0}'! Error Code: {1}", filename, err, _projectname);
            exit(err);
        }

        // Premtively add these points for library
        insertDataPoint("Library Directories", {});
        insertDataPoint("Library Files", {});

        XMLNode* root = doc.RootElement();
        for (XMLElement* node = root->FirstChildElement(); node; node = node->NextSiblingElement())
        {
            std::string key(node->Name());
            
            std::vector<std::string> values;
            for (XMLElement* point = node->FirstChildElement(); point; point = point->NextSiblingElement())
            {
                std::string point_name(point->Name());

                /**/ if (key == "Directories" || key == "Settings")
                    insertDataPoint(point_name, { point->GetText() });
                else if (key == "Dependancies" && point_name == "Library")
                {
                    for (XMLElement* lib = point->FirstChildElement(); lib; lib = lib->NextSiblingElement())
                    {
                        std::string name(lib->Name());

                        /**/ if (name == "dir")
                        {

                            bool cdir = true;
                            if (lib->Attribute("cdir") != NULL)
                                cdir = (std::string(lib->Attribute("cdir")) != "false");

                            std::string lib_dir;
                            if (cdir) lib_dir += _projectdir;
                            lib_dir += lib->GetText();
                            
                            data.at("Library Directories").push_back(lib_dir);
                        }
                        else if (name == "file")
                            data.at("Library Files").push_back(lib->GetText());
                    }
                }
                else if (key == "Dependancies" && point_name == "proj")
                {
                    // Whether or not to link
                    std::string attr;
                    if (point->Attribute("link") == NULL)
                        attr = "true";
                    else
                        attr = point->Attribute("link");
                    bool link = (attr != "false");

                    // Grab the flags
                    std::vector<std::string> flags;
                    if (point->Attribute("flags") != NULL)
                    {
                        std::string txt = point->Attribute("flags");

                        for (int i = 0; i < txt.length(); i++)
                        {
                            if (txt[i] == ' ' || i == txt.length() - 1)
                            {
                                int offset = 0;
                                if (i == txt.length() - 1)
                                    offset++;

                                flags.push_back(std::string(txt.begin(), txt.begin() + i + offset));
                                txt.erase(txt.begin(), txt.begin() + i + 1);
                                i = 0;
                                continue;
                            }
                        }
                    }

                    std::string directory = _projectdir;
                    directory += point->GetText();

                    // Create and compile the project
                    Project* proj = new Project(directory.c_str(), routine);

                    switch (routine)
                    {
                        case Routine::Compile:
                            proj->compile(flags, link);
                            break;

                        case Routine::Recompile:
                            proj->recompile(flags, link);
                            break;

                        case Routine::Clean:
                            proj->clean();
                            break;
                    };

                    if (link)
                    {
                        std::string file = proj->getOutputFile();
                        file.erase(file.begin(), file.begin() + 3);
                        file.erase(file.end() - 3, file.end());

                        // Link the project to this project
                        data.at("Library Directories").push_back(proj->getDirectories().bin);
                        data.at("Library Files").push_back(file);
                    }
                }
                else if (key == "Includes")
                {
                    bool cdir = true;
                    if (point->Attribute("cdir") != NULL)
                        cdir = (std::string(point->Attribute("cdir")) != "false");

                    std::string include_str;
                    if (cdir) include_str += _projectdir;
                    include_str += point->GetText();

                    values.push_back(include_str);
                }
            }

            if (key == "Includes")
                insertDataPoint(key, values);
        }
    }

    void Project::clean()
    {
        COMP_INFO("Cleaning project '{0}'", _projectname);
        std::vector<std::string> files = getFilesInDirectory(getDirectories().obj.c_str(), false, { ".o" });
        for (int i = 0; i < files.size(); i++)
            getCompiler()->removeFile(files[i]);
    }

    void Project::compile(std::vector<std::string> flags, bool link)
    {
        COMP_INFO("Compiling project '{0}'", _projectname);

        std::vector<std::string> files = getFilesInDirectory(getDirectories().src.c_str(), true, { ".cpp", ".c" });
        COMP_INFO("Collected {0} Source File(s) from {1}", files.size(), getDirectories().src);
        for (int i = 0; i < files.size(); i++)
            getCompiler()->compileCppToO(files.at(i).c_str(), this, flags);

        if (!link) return;

        files.clear();
        files = getFilesInDirectory(getDirectories().obj.c_str(), false, ".o");
        COMP_INFO("Linking object files to executable '{0}'", getDirectories().bin + getOutputFile());
        getCompiler()->linkObjsToFin(files, this);
    }

    void Project::recompile(std::vector<std::string> flags, bool link)
    {
        clean();
        compile(flags, link);
    } 

    std::string Project::getName() const
    {
        return _projectname;
    }

    std::string Project::getExtension() const
    {
        int type = getProjectType();
        switch(type)
        {
            case Executable:
#ifdef WIN32
                return ".exe";
#else
                return "";
#endif
            case StaticLibrary:
                return getCompiler()->getStaticExtension();
            case DynamicLibrary:
                return getCompiler()->getDynamicExtension();
            default:
                return "";
        };
    }

    std::string Project::getOutputFile() const
    {
        std::string pref = "";
        if (getProjectType() > Executable)
            pref += "lib";

        return pref + data.at("OutputFile")[0] + getExtension();
    }

    int Project::getProjectType() const
    {
        try {
            return std::stoi(data.at("FileType")[0]);
        } catch(...) {
            COMP_ERROR("File type must be a numerical value 0-2");
            exit(1);
        }
    }

    Directories Project::getDirectories() const
    {
        Directories d;

        try
        {
            d.src = _projectdir + data.at("src_dir").at(0);
            d.obj = _projectdir + data.at("obj_dir").at(0);
            d.bin = _projectdir + data.at("bin_dir").at(0);

            if (d.src[d.src.length() - 1] != '/') d.src.push_back('/');
            if (d.obj[d.obj.length() - 1] != '/') d.obj.push_back('/');
            if (d.bin[d.bin.length() - 1] != '/') d.bin.push_back('/');
        } catch(...) {
            COMP_ERROR("Error using crucial directories in project '{0}'!", _projectname);
            exit(1);
        }

        return d;
    }

    std::vector<std::string> &Project::getLibraryFiles() 
    {
        return data.at("Library Files");
    }

    std::vector<std::string> &Project::getLibraryDirectories() 
    {
        return data.at("Library Directories");
    }

    std::vector<std::string> &Project::getIncludeDirectories() 
    {
        return data.at("Includes");
    }

    Project* Project::getProjectFromDirectory(const char* name, const char* dir, Routine routine)
    {
        DIR* d;
        struct dirent* ent;

        if ((d = opendir(dir)) != NULL)
        {
            while ((ent = readdir(d)) != NULL)
            {
                std::string filename = std::string(ent->d_name);

                // If the last 5 chars are .proj
                if (filename.length() >= 5 && std::string(filename.end() - 5, filename.end()) == ".proj")
                {
                    // Create project with the name IF
                    //  given name is default '*'
                    //     OR
                    //  given name isn't '*' AND the name before extension is equal to given name
                    //     OR
                    //  given name isn't '*' AND the whole filename is equal to given name

                    std::string given_name(name);
                    std::string name_bef_ext = std::string(filename.begin(), filename.end() - 5);
                    if (given_name == "*" || (given_name != "*" && name_bef_ext == given_name) 
                        || (given_name != "*" && filename == given_name))
                    {
                        std::string fname = std::string(dir) + filename;
                        const char* f_c   = fname.c_str();

                        return new Project(f_c, routine);
                    }
                }
            }

            closedir(d);
        }        

        return nullptr;
    }

    unsigned int Project::generateProjectFile(const char* name)
    {
        using namespace tinyxml2;
        std::string s_name = std::string(name) + ".proj";
        const char* c_name = s_name.c_str();

        XMLDocument document;
        XMLDeclaration* decl = document.NewDeclaration();
        decl->SetValue("xml version='1.0' encoding=''");
        document.InsertEndChild(decl);

        XMLNode* rootNode = document.NewElement("Project");
        document.InsertEndChild(rootNode);

        // Directories
        Pair<const char*> directories[] = {
            { "src_dir", "Put source files folder here... (ex. ./src)" },
            { "bin_dir", "Put output directory here... (ex. ./bin)" },
            { "obj_dir", "Put temp object files folder here... (ex. ./obj)" }
        };

        XMLElement* directoriesElement = document.NewElement("Directories");
        rootNode->InsertEndChild(directoriesElement);

        for (int i = 0; i < sizeof(directories) / sizeof(Pair<const char*>); i++)
        {
            XMLElement* directory = document.NewElement(directories[i].a);
            directory->SetText(directories[i].b);
            directoriesElement->InsertEndChild(directory);
        }

        // Settings
        XMLElement* settingsElement = document.NewElement("Settings");
        rootNode->InsertEndChild(settingsElement);

        XMLElement* outputNameElement = document.NewElement("OutputFile");
        outputNameElement->SetText("Put the name of your project file here (no extension)");
        settingsElement->InsertEndChild(outputNameElement);

        XMLElement* exeTypeElement = document.NewElement("FileType");
        exeTypeElement->SetText("0 for executable, 1 for dynamic library, 2 for static library");
        settingsElement->InsertEndChild(exeTypeElement);

        // Includes
        XMLElement* includesElement = document.NewElement("Includes");
        rootNode->InsertEndChild(includesElement);

        XMLElement* sampleInclude = document.NewElement("dir");
        sampleInclude->SetText("[ SAMPLE ] Put each include directory in a seperate field");
        includesElement->InsertEndChild(sampleInclude);

        // Dependencies
        XMLElement* dependencyElement = document.NewElement("Dependancies");
        rootNode->InsertEndChild(dependencyElement);

        XMLElement* sampleLibrary = document.NewElement("Library");
        dependencyElement->InsertEndChild(sampleLibrary);

        XMLElement* sampleLibraryDir = document.NewElement("dir");
        sampleLibraryDir->SetText("Put the location of this library here");
        sampleLibrary->InsertEndChild(sampleLibraryDir);

        XMLElement* sampleLibraryFile = document.NewElement("file");
        sampleLibraryFile->SetText("Put the individual library files here");
        sampleLibrary->InsertEndChild(sampleLibraryFile);

        XMLElement* sampleLibraryFile2 = document.NewElement("file");
        sampleLibraryFile2->SetText("You can even do multiple!");
        sampleLibrary->InsertEndChild(sampleLibraryFile2);

        XMLElement* sampleProject = document.NewElement("proj");
        sampleProject->SetText("Or put the location of another project file");
        dependencyElement->InsertEndChild(sampleProject);

        XMLError err = document.SaveFile(c_name);

        return err;
    }
}
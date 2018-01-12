#pragma once

#include <iostream>
#include <filesystem>

#include <LibSL/LibSL.h>
#include <imgui.h>

#ifdef WIN32
#include "Windows.h"
#endif

#include "Resources.h"

class Project
{
  const std::string m_NodeFolder = "nodes";

public:

  std::string path;
  std::set<std::string> openFolders;

  void importLua(std::string srcPath) 
  {
    using namespace std::experimental::filesystem;
    std::string nodeDir = nodefolder();
		std::string filename = extractFileName(srcPath);
    nodeDir += extractFileName(srcPath);
    copy(srcPath, nodeDir); // copy_options::overwrite_existing);
  }

  void createNodefolder() 
  {
    std::string NodeDir = nodefolder();
    createDirectory(NodeDir.c_str());
  }

  void listLuaFileInDir(std::vector<std::string>& files) 
  {
    std::string NodeDir = nodefolder();
    listFiles(NodeDir.c_str(), files);
  }

  void listLuaFileInDir(std::vector<std::string>& files, std::string directory)
  {
    using namespace std::experimental::filesystem;

    for (directory_iterator itr(directory); itr != directory_iterator(); ++itr)
    {
      std::experimental::filesystem::path file = itr->path();
      if (!is_directory(file)) {
        files.push_back(file.filename().generic_string());
        std::cout << file.filename().generic_string() << std::endl;
      }
    }
  }

  void exctractPathFromXml(std::string& s) 
  {
    std::string directory;
    const size_t last_slash_idx = s.rfind(Resources::separator());
    if (std::string::npos != last_slash_idx) {
      directory = s.substr(0, last_slash_idx);
      path = directory;
    }
  }

  void listFolderinDir(std::vector<std::string>& files, std::string folder)
  {
    using namespace std::experimental::filesystem;

    for (directory_iterator itr(folder); itr != directory_iterator(); ++itr)
    {
      std::experimental::filesystem::path file = itr->path();
      if (is_directory(file))files.push_back(file.filename().generic_string());
    }
  }

  bool copyFile(const std::string filename, const std::string dest) {
    puts(filename.c_str());
    std::ifstream infile(filename.c_str(), std::ifstream::binary);
    std::ofstream outfile(dest.c_str(), std::ofstream::binary);

    infile.seekg(0, infile.end);

    long size = (long)infile.tellg(); //size of the infile

    if (size == -1) {
      std::cerr << "problem with the copy" << std::endl;
      return false;

    }

    infile.seekg(0);
    char* buffer = new char[size];

    infile.read(buffer, size);
    outfile.write(buffer, size);

    outfile.close();
    infile.close();

    delete[] buffer;

    return true;
  }

  bool copyDir(const std::string source, const std::string destination)
  {
    using namespace std::experimental::filesystem;

    std::experimental::filesystem::path dir(source);
    if (!is_directory(dir)) {
      std::cerr << "cannot open " << source << " as directory." << std::endl;
      return false;
    }
   
    bool isCopied(false);
    for (directory_iterator itr(dir); itr != directory_iterator(); ++itr)
    {
      std::experimental::filesystem::path file = itr->path();
      if (is_directory(file)) {
        isCopied = copyDir(source + file.generic_string(), destination);
      }
      else {
        if (strcmp(file.generic_string().c_str(), ".") != 0 && strcmp(file.generic_string().c_str(), "..") != 0) {
          isCopied = copyFile(source + file.generic_string(), destination + file.generic_string());
        }
        else {
          isCopied = true;
        }
      }
      if (!isCopied) {
        return false;
      }
    }
    return true;
  }

  //---------------------------------------------------

  std::string relativePath(std::string& path) 
  {
    int nfsize = (int)nodefolder().size();
    if (path[nfsize+1] == Resources::separator()) nfsize++;
    std::string name = path.substr(nfsize);
    return name;
  }

  //---------------------------------------------------

  std::string nodefolder()
  {
    std::string NodeDir = Resources::toPath(path, m_NodeFolder);
    return NodeDir;
  }

  //---------------------------------------------------

  void copyEmitNode()
  {
    importLua(std::string(Resources::toPath(Resources::path(), "lua_constant", "emit.lua")));
  }
  
  //---------------------------------------------------
  
  std::string recursiveFileSelecter(std::string current_dir)
  {
    std::vector<std::string> files;
    listLuaFileInDir(files, current_dir);
    std::vector<std::string> directories;
    std::string nameDir = "";
    listFolderinDir(directories, current_dir);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 1.0f, 1.0f));
    ForIndex(i, directories.size()){
      if (ImGui::CollapsingHeader(directories[i].c_str())) {
        nameDir = recursiveFileSelecter(Resources::toPath(current_dir, directories[i]));
        //ImGui::EndMenu();
      }
    }
    ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1., 1., 1.0, 1));
    
    
    ForIndex(i, files.size()) {
      if (ImGui::MenuItem(files[i].c_str())) {
        nameDir = Resources::toPath(current_dir, files[i]);
      }
    }
    ImGui::PopStyleColor();

    return nameDir;
  }

  //---------------------------------------------------

  std::string renderFileSelecter(v2i pos)
  {
    std::string nameDir = "";
    ImGui::Begin("Menu");
    nameDir = recursiveFileSelecter(nodefolder());
    ImGui::SetWindowPos(ImVec2((float)pos[0], (float)pos[1]));
    ImGui::End();
    return nameDir;
  }

  //---------------------------------------------------

};

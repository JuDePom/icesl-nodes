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
public:

  std::string path;
  std::set<std::string> openFolders;

  void importLua(std::string srcPath) 
  {
    using namespace std::experimental::filesystem;
    std::string nodeDir = this->path + "/node/";
    nodeDir += extractFileName(srcPath);
    copy(srcPath, nodeDir); // copy_options::overwrite_existing);
  }

  void createNodefolder() 
  {
    std::string NodeDir = path + "/node";
    createDirectory(NodeDir.c_str());
  }

  void listLuaFileInDir(std::vector<std::string>& files) 
  {
    std::string NodeDir = path + "/node";
    listFiles(NodeDir.c_str(), files);
  }

  void listLuaFileInDir(std::vector<std::string>& files, std::string directory)
  {
    listFiles(directory.c_str(), files);
  }

  void exctractPathFromXml(std::string& s) 
  {
    std::string directory;
    const size_t last_slash_idx = s.rfind('\\');
    if (std::string::npos != last_slash_idx) {
      directory = s.substr(0, last_slash_idx);
      path = directory;
    }
  }

  void listFolderinDir(std::vector<std::string>& files, std::string folder)
  {

  }

  bool copyDir(std::string source, std::string destination)
  {
    return false;
  }

  //---------------------------------------------------

  std::string relativePath(std::string& path) 
  {
    int nfsize = (int)nodefolder().size();
    std::string name = path.substr(nfsize);
    return name;
  }

  //---------------------------------------------------

  std::string nodefolder()
  {
    std::string NodeDir = path + "/node/";
    return NodeDir;
  }

  //---------------------------------------------------

  void copyEmitNode()
  {
    importLua(std::string(Resources::path() + "/lua_constant/emit.lua"));
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
      if (ImGui::BeginMenu(directories[i].c_str())){
        nameDir = recursiveFileSelecter(current_dir + "/" + directories[i]);
        ImGui::EndMenu();
      }
    }
    ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1., 1., 1.0, 1));
    ForIndex(i, files.size()){
      if (ImGui::MenuItem(files[i].c_str())){
        nameDir = current_dir + "/" + files[i].c_str();
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
    nameDir = recursiveFileSelecter(path + "/node/");
    ImGui::SetWindowPos(ImVec2((float)pos[0], (float)pos[1]));
    ImGui::End();
    return nameDir;
  }

  //---------------------------------------------------

};

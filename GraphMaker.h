#include "remote_handler.h"

#include <LibSL/LibSL.h>

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstdio>

#include "project.h"
#include "NodeWindow.h"
#include "GraphSaver.h"
#include "GenericWindow.h"
#include "graphError.h"
#include "GraphSaver.h"


class GraphMaker
{
  ConsoleWindow console; // TODO: naming convetion
  Project       project; // TODO: naming convetion // handle copy and paste in the project directory

  NodeGraph     m_NodeGraph;

  bool          activeProject; // TODO: naming convetion
  bool          m_showRMenu;
  int           m_W;
  int           m_H;
  v2i           m_mousePos;
  NodeSelecter  m_nodeSelecter;
  std::string   m_MasterScriptPath;

  std::string tempFileName()
  {
    return std::string(tmpnam(NULL)) + ".lua";
  }

  GraphMaker()
  {
    activeProject = false;
    m_showRMenu = false;
    m_mousePos = v2i(0, 0);
    // produce temporary file for master scripts
    m_MasterScriptPath = tempFileName();
    {
      // make sure the file exists
      std::ofstream f(m_MasterScriptPath);
      f << "emit(Void)" << std::endl;
      f.close();
    }
  };

public:

  const std::string& masterScriptPath() const { return m_MasterScriptPath; }

  std::vector<NodeWindow*>& getNodeWindows() 
  { 
    return m_NodeGraph.nodeWindows; 
  }

  static GraphMaker& getInstance() 
  {
    static GraphMaker n;
    return n;
  }

  void onChange();
  void makeNewNode(std::string path, v2i pos);

  void deleteNodeWindow(NodeWindow* nw);

  bool hasActiveProject() 
  {
    return activeProject;
  }

  void createProject(std::string path)
  {
    project.path = path;
    activeProject = true;
    project.createNodefolder();
    std::string source(Resources::path() + "/basic_nodes");
    std::string dest(project.nodefolder());
    project.copyDir(source, dest);
    project.copyEmitNode();
  }

  Project& getProject()
  {
    return project;
  }

  void setShowRMenu(bool b)
  {
    m_showRMenu = b;
  }

  bool isRMenuShown() 
  {
    return m_showRMenu;
  }

  void setMousePos(v2i vec)
  {
    m_mousePos = vec;
  }

  void setWindow(int W, int H)
  {
    m_W = W;
    m_H = H;
    console.setPos(v2i(0, 3 * m_H / 4));
    console.setSize(v2i(m_W, m_H / 4));
  }

  void saveGraph(std::string path);
  void loadGraph(std::string path);

  void checkError(Messaging::msg_script_error& err);
  void convert_error(Messaging::msg_script_error& err);
  void nameWindow(NodeWindow* n);
  void onIdle();
  void renderImgui();
  void RenderMenu();

};


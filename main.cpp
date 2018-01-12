// JH 2017
// SL 2018

#include "remote_handler.h"
#include "graphMaker.h"

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>

#include "imgui/imgui.h"

#include "FileDialog.h"
#include "NodeWindow.h"
#include "NodeLua.h"
#include "Resources.h"

#include <thread>

#ifdef USE_GLUT
#include <GL/glut.h>
#endif

// --------------------------------------------------------------

LIBSL_WIN32_FIX

using namespace std;

int g_W = 1024;
int g_H = 1024;

// --------------------------------------------------------------

void drawImgUIStuff()
{
  GraphMaker::getInstance().renderImgui();
}

// --------------------------------------------------------------

void mainRender()
{
  glClearColor(1.0f, 1.0f, 1.0f, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawImgUIStuff();
}

// --------------------------------------------------------------

void mainKeyPressed(uchar k)
{

}

// --------------------------------------------------------------

void mainScanCode(uint sc)
{

}

// --------------------------------------------------------------

void mainScanCodeRelease(uint sc)
{

}

// --------------------------------------------------------------

void mainMousePressed(uint x, uint y, uint button, uint flags)
{
  if (flags == LIBSL_BUTTON_DOWN && button == LIBSL_RIGHT_BUTTON) {
    GraphMaker::getInstance().setShowRMenu(true);
    GraphMaker::getInstance().setMousePos(v2i(x, y));
  }
  if (GraphMaker::getInstance().isRMenuShown() && flags == LIBSL_BUTTON_DOWN && button == LIBSL_LEFT_BUTTON) {
    GraphMaker::getInstance().setShowRMenu(false);
  }

  if (flags == LIBSL_BUTTON_DOWN && button == LIBSL_MIDDLE_BUTTON) {
    GraphMaker::getInstance().setMousePos(v2i(x, y));
    GraphMaker::getInstance().setMoveGraph(true);
  }
  if (flags == LIBSL_BUTTON_UP && button == LIBSL_MIDDLE_BUTTON) {
    GraphMaker::getInstance().setMoveGraph(false);
  }
}

// --------------------------------------------------------------

void mainMouseMotion(uint x, uint y)
{
  if (GraphMaker::getInstance().isGraphMoving()) {
    GraphMaker::getInstance().moveOrigin(v2i(x, y));
  }

}

// --------------------------------------------------------------

void mainMouseWheel(int incr)
{

}

// --------------------------------------------------------------

//TODO WARNING JH: This is here and not in LIBSL because BK can use it without updating LIBSL
//there are some duplicated code

#ifdef USE_GLUT
void mouse(int glut_btn, int glut_state, int x, int y)
{
  if ((glut_btn & GLUT_LEFT_BUTTON)
    && (glut_btn & GLUT_RIGHT_BUTTON)) {
    glut_btn = GLUT_MIDDLE_BUTTON;
  }

  uint button = 0, flags = 0;
  if (glut_btn == GLUT_LEFT_BUTTON)   button = LIBSL_LEFT_BUTTON;
  else if (glut_btn == GLUT_MIDDLE_BUTTON) button = LIBSL_MIDDLE_BUTTON;
  else if (glut_btn == GLUT_RIGHT_BUTTON)  button = LIBSL_RIGHT_BUTTON;

  if (glut_state == GLUT_DOWN) {
    flags |= LIBSL_BUTTON_DOWN;
  } else if (glut_state == GLUT_UP) {
    flags |= LIBSL_BUTTON_UP;
  }

  if ((glut_btn == 3) || (glut_btn == 4)) // It's a wheel event
  {
    // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
    if (glut_state == GLUT_UP) return; // Disregard redundant GLUT_UP events
    if (glut_btn == 3)SimpleUI::onMouseWheel(1);
    if (glut_btn == 4)SimpleUI::onMouseWhefind_package(Qt4 REQUIRED QtCore QtGui)
      INCLUDE(${ QT_USE_FILE })el(-1);
    return;
  }
  SimpleUI::onMouseButtonPressed(x, y, button, flags);

}
#endif

// --------------------------------------------------------------

void clientCallback(Messaging::msg_script_error& err)
{
  GraphMaker::getInstance().checkError(err);
}

// --------------------------------------------------------------

int main(int argc, char **argv)
{
#ifndef WIN32
  int useless = pthread_getconcurrency(); //workaround a bug of ubuntu distrib OLOL
#endif
  try {

#ifdef WIN32
    // attempts to launch IceSL
    vector<string> paths;
    paths.push_back("." + Resources::separator());
    paths.push_back(string(LibSL::System::Application::executablePath()) + Resources::separator());
    paths.push_back(Resources::toPath("C:", "Program Files", "INRIA", "IceSL", "bin"));
    paths.push_back(Resources::toPath("C:", "Program Files (x86)", "INRIA", "IceSL", "bin"));
    ForIndex(i, paths.size()) {
      string exe = Resources::toPath(paths[i], "IceSL-slicer.exe");
      if (exists(exe.c_str())) {
        STARTUPINFO si; 
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
        char cmdl[MAX_PATH];
        strcpy_s<MAX_PATH>(cmdl, ("\"" + exe + "\" --remote " + GraphMaker::getInstance().masterScriptPath() ).c_str());
        BOOL ok = CreateProcessA(exe.c_str(), cmdl, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        if (!ok) {
          throw Fatal("Cannot launch IceSL.");
        } else {
          LibSL::System::Process::sleep(2000); // wait a bit for IceSL to init
        }
        break;
      }
    }
#endif

    // init messaging system
    bool ok = Messaging::getInstance().initClient(clientCallback);
    if (!ok) {
      throw Fatal("Cannot connect to IceSL. Is it running? Was it launched with the --remote flag?");
    }
    Messaging::getInstance().spawn_client();

    // UI
    TrackballUI::onRender = mainRender;
    TrackballUI::onKeyPressed = mainKeyPressed;
    TrackballUI::onMouseButtonPressed = mainMousePressed;
    TrackballUI::init(g_W, g_H);
    GraphMaker::getInstance().setWindow(g_W, g_H);

    // GL
    glEnable(GL_DEPTH_TEST);

    // imgui
    SimpleUI::bindImGui();
    TrackballUI::trackball().set(v3f(0), v3f(0), quatf(v3f(1, 0, 0), -1.0f)*quatf(v3f(0, 0, 1), 2.5f));
    SimpleUI::initImGui();
    SimpleUI::onReshape(g_W, g_H);

    // main loop
    TrackballUI::loop();

    // clean up
    // Messaging::getInstance().end_client(); // TODO: proper termination
    SimpleUI::terminateImGui();
    
    // shutdown UI
    TrackballUI::shutdown();

  } catch (Fatal& e) {
    cerr << Console::red << e.message() << Console::gray << endl;
    return (-1);
  }

  return 0;
}

// --------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(icesl-nodes)
Set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

Include(UseCXX11)

add_definitions(-DSRC_PATH="\"${CMAKE_SOURCE_DIR}/\"")
add_definitions(-DASIO_STANDALONE)

IF(WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")
  SET(CMAKE_EXE_LINKER_FLAGS "/LARGEADDRESSAWARE")
ELSE()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_NO_CXX11_SCOPED_ENUMS")
  find_package(Qt4 REQUIRED QtCore QtGui)
  INCLUDE(${QT_USE_FILE})
ENDIF()

# if(UNIX)
# find_package(Boost COMPONENTS system filesystem REQUIRED)
# include_directories(. ${BOOST_INCLUDE_DIR})
# ENDIF()

# SET(CMAKE_BUILD_TYPE Debug)

SET(SOURCES
  NodeWindow.h
  NodeWindow.cpp
  NodeLua.cpp
  NodeLua.h
  main.cpp
  GraphMaker.cpp
  GraphMaker.h
  GraphError.h
  FileDialog.cpp
  FileDialog.h
  project.h
  tweaks.h
  tweaks.cpp
  GenericWindow.h
  GraphSaver.h
  GraphSaver.cpp
  Resources.h
  Resources.cpp
)

INCLUDE_DIRECTORIES( ${LIBSL_INCLUDE_DIR} )

IF(NOT APPLE)
ADD_DEFINITIONS(-DUSE_GLUX)
ELSE()
ADD_DEFINITIONS(-DAPPLE)
ENDIF(NOT APPLE)

if(WIN32)
set(USER $ENV{USERNAME})
else(WIN32)
set(USER $ENV{USER})
endif(WIN32)

ADD_EXECUTABLE(icesl-nodes ${SOURCES} ${HEADERS})
IF(WIN32)
TARGET_LINK_LIBRARIES(icesl-nodes libremote_public LibSL LibSL_gl )
ELSE(WIN32)
TARGET_LINK_LIBRARIES(icesl-nodes libremote_public LibSL LibSL_gl pthread
 ${Boost_FILESYSTEM_LIBRARY}
 ${Boost_SYSTEM_LIBRARY}
 ${QT_LIBRARIES}
)
ENDIF(WIN32)

SET_TARGET_PROPERTIES(icesl-nodes PROPERTIES DEBUG_POSTFIX "-d")
set_target_properties(icesl-nodes PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../icesl-next/bin")
set_target_properties(icesl-nodes PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../icesl-next/bin")
set_target_properties(icesl-nodes PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../icesl-next/bin")
set_target_properties(icesl-nodes PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/../icesl-next/bin")

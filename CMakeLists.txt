cmake_minimum_required(VERSION 3.28...3.30)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" ON)
option(ENABLE_QT "Use Qt functionality" ON)

include(compilerconfig)
include(defaults)
include(helpers)

add_library(
  ${CMAKE_PROJECT_NAME}
  MODULE
  src/DrawDock.cpp
  src/DrawDock.hpp
  src/SettingsDialog.cpp
  src/SettingsDialog.hpp
  src/draw.c
  src/draw.h
  src/shared_memory_wrapper.cpp
  src/shared_memory_wrapper.h
  src/feature_flags.cpp
  src/feature_flags.h
  src/plugin-path.h
)

find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

# The Python backend runs as a separate process (launched via QProcess), so the
# plugin must NOT embed libpython. Embedding it clashes with OBS' own scripting
# (obs-scripting) Python symbols on macOS and breaks numpy/torch in-process.

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

if(ENABLE_FRONTEND_API)
  find_package(obs-frontend-api REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api)
endif()

if(ENABLE_QT)
  find_package(Qt6 COMPONENTS Widgets Core)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)

  if(APPLE AND TARGET WrapOpenGL::WrapOpenGL)
    # AGL.framework was removed from the macOS 26 SDK, but Qt's FindWrapOpenGL
    # still adds it to the OpenGL link interface (pulled in transitively by
    # Qt6::Gui), which makes linking fail with "framework 'AGL' not found".
    # Strip AGL from the interface; it is unused by this plugin.
    get_target_property(_draw2_wrapgl_libs WrapOpenGL::WrapOpenGL INTERFACE_LINK_LIBRARIES)
    if(_draw2_wrapgl_libs)
      list(FILTER _draw2_wrapgl_libs EXCLUDE REGEX "[ /]AGL([./]|$)")
      set_target_properties(WrapOpenGL::WrapOpenGL PROPERTIES INTERFACE_LINK_LIBRARIES "${_draw2_wrapgl_libs}")
    endif()
    unset(_draw2_wrapgl_libs)
  endif()

  target_compile_options(
    ${CMAKE_PROJECT_NAME}
    PRIVATE $<$<C_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header -Wno-comma>
  )

  set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
  )

  if(APPLE)
    # Qt's AUTOMOC generates moc_predefs.h by invoking the compiler with no
    # explicit deployment target. On Xcode 16+/clang 17+ the build environment
    # exports several *_DEPLOYMENT_TARGET variables at once, which clang rejects
    # with "conflicting deployment targets". The predefines are not required
    # here, so disable their generation to avoid the bare compiler invocation.
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES AUTOMOC_COMPILER_PREDEFINES OFF)
  endif()
endif()

target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/plugin-main.cpp)

set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})

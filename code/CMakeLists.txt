# GameLab 3 Tutorial
# Andreas Knote <andreas.knote@uni-wuerzburg.de>
# 2019-10-3

# Following modern CMake guidelines
#
# https://cliutils.gitlab.io/modern-cmake/
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/

cmake_minimum_required(VERSION 3.13)

project(GL3
	VERSION 0.0
	DESCRIPTION "The GameEngine example project for the 2019/20 class."
	LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/module")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/module")

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here
    
    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    if(APPLE)
        # Special workaround for OSX < 10.15
        # to support std::filesystem using brew
        # Prequrisites: `brew install llvm`
        link_directories(/usr/local/Cellar/llvm/9.0.0/lib)
        set(CMAKE_C_COMPILER /usr/bin/clang)
        set(CMAKE_CXX_COMPILER /usr/bin/clang++)
    endif()

    # Let's nicely support folders in IDE's
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Docs only available if this is the main app
    # http://www.doxygen.nl/download.html
    find_package(Doxygen)
    if(Doxygen_FOUND)
	add_subdirectory(docs)
    else()
	message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

# Dependencies 
add_subdirectory(extern)

# Our Engine Project
add_subdirectory(engine)

# Our Renderer Project
add_subdirectory(renderer)

# Our Physics Project
add_subdirectory(physics)

# Our Asset Management Project
add_subdirectory(asset_management)

# Our UI System Project
add_subdirectory(ui)

# Our Game Project 
add_subdirectory(resourceGame)

# Second Game Project 
add_subdirectory(blockGame)

# Space Shooter 
# Andreas Knote <andreas.knote@uni-wuerzburg.de>
# 2019-10-3

# Following modern CMake guidelines
#
# https://cliutils.gitlab.io/modern-cmake/
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/

# Define the asset path for development
set(ENGINE_DEV_ROOT_PATH "${PROJECT_SOURCE_DIR}/" CACHE PATH "Fallback path for engine resolving assets during development. Points to the directory containing the 'assets/' folder. Must end in a /.")

# Define the asset path for development
set(ENGINE_MAX_NUM_COMPONENTS "4096" CACHE STRING "(Default) size of fixed-size component pools.")
set(ENGINE_MAX_NUM_SYSTEMS "128" CACHE STRING "(Default) size of fixed-size system array.")

# Configure files based on variables 
configure_file(generated/defaults.h.in src/engine/generated/defaults.h @ONLY)
file(GLOB_RECURSE GENERATED_FILES_LIST CONFIGURE_DEPENDS "generated/**.in")

# Glob "okay" for CMAKE 3.12+ using CONFIGURE_DEPENDS flag!
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cxx")
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "src/**.h")
file(GLOB_RECURSE PUBLIC_HEADER_LIST CONFIGURE_DEPENDS "include/**.h")

# Add an executable target 
add_library(Engine ${SOURCE_LIST} ${HEADER_LIST} ${PUBLIC_HEADER_LIST} ${GENERATED_FILES_LIST})

# Require C++ 17 compiler
target_compile_features(Engine PRIVATE cxx_std_17)
target_compile_options(Engine PRIVATE
        $<$<AND:$<PLATFORM_ID:Darwin>,$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,8.0>>:-mmacosx-version-min=10.15>)

# Link the executable with required libraries
target_link_libraries(Engine PUBLIC glfw glad glm_static glm_header Renderer)
target_compile_definitions(Engine PUBLIC GLFW_INCLUDE_NONE)

# Add header directory
target_include_directories(Engine PUBLIC include/)
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/src/)

# IDE should put the headers in a nice place
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SOURCE_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Private Header" FILES ${HEADER_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Public Header" FILES ${PUBLIC_HEADER_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/generated" PREFIX "Generated Files" FILES ${GENERATED_FILES_LIST})

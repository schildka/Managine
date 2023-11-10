# Space Shooter 
# Andreas Knote <andreas.knote@uni-wuerzburg.de>
# 2019-10-3

# Following modern CMake guidelines
#
# https://cliutils.gitlab.io/modern-cmake/
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/

# Glob "okay" for CMAKE 3.12+ using CONFIGURE_DEPENDS flag!
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cxx")
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "src/**.h")

# Add an executable target 
add_executable(BlockGame ${SOURCE_LIST} ${HEADER_LIST} ${GENERATED_HEADER_LIST})

# Require C++ 17 compiler
target_compile_features(BlockGame PRIVATE cxx_std_17)

# Link the executable with required libraries 
target_link_libraries(BlockGame PRIVATE Engine Physics UI Renderer AssetManagement assimp nothings_stb)

# Add generated header directory
target_include_directories(BlockGame PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src/)

# IDE should put the headers in a nice place
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SOURCE_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Private Header" FILES ${HEADER_LIST})

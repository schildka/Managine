# Space Shooter 
# Andreas Knote <andreas.knote@uni-wuerzburg.de>
# 2019-10-3

# Following modern CMake guidelines
#
# https://cliutils.gitlab.io/modern-cmake/
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/


# Glob "okay" for CMAKE 3.12+ using CONFIGURE_DEPENDS flag!
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cpp")
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "src/**.h")
file(GLOB_RECURSE PUBLIC_HEADER_LIST CONFIGURE_DEPENDS "include/**.h")

# Add an executable target 
add_library(imgui ${SOURCE_LIST} ${HEADER_LIST} ${PUBLIC_HEADER_LIST} ${GENERATED_FILES_LIST})

# Require C++ 17 compiler
target_compile_features(imgui PRIVATE cxx_std_17)
target_compile_options(imgui PRIVATE
        $<$<AND:$<PLATFORM_ID:Darwin>,$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,8.0>>:-mmacosx-version-min=10.15>)

# Add header directory
target_include_directories(imgui PUBLIC include/)

# Add dependencies
target_link_libraries(imgui PUBLIC glfw glad)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

# IDE should put the headers in a nice place
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SOURCE_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Private Header" FILES ${HEADER_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Public Header" FILES ${PUBLIC_HEADER_LIST})

# GLFW Platform-Independence
add_subdirectory(glfw)

# GLAD GL Extension Loader
add_subdirectory(glad)

# GL Maths Library
add_subdirectory(glm)
# GLM is not (yet)following modern CMake Best Practice (TM)
# https://github.com/g-truc/glm/pull/951/commits/fff50cae0028249d68d14fa80592fc656c75bfae
# We create a INTERFACE target (no binaries) to inject the include paths
#
# We could have used the windows installer and installed it globally on the system
# At least probably... https://github.com/g-truc/glm/issues/947
add_library(glm_header INTERFACE)
target_include_directories(glm_header INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glm/)
            
# reactphysics3D
add_subdirectory(reactphysics3d)

# nothings/stb
add_library(nothings_stb INTERFACE)
target_include_directories(nothings_stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/nothings)

# tinyobjloader
add_library(tinyobjloader INTERFACE)
target_include_directories(tinyobjloader INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader/)

# imgui
add_subdirectory(imgui)

# assimp
add_subdirectory(assimp)

# irrklang
#find_package(irrklang REQUIRED)
add_library(irrklang INTERFACE)
target_include_directories(irrklang INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/irrklang/include/")
target_link_libraries(irrklang INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/irrklang/lib/Win32-visualStudio/irrKlang.lib")
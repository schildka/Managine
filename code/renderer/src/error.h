// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>


#include <glad/glad.h>
#include <string>
#include <sstream>

namespace engine::renderer::gl {

    static void throwOnError() {
        // Note: This is slow, but makes sure errors are actually caught
        // glFlush();
        bool errOccurred = false;
        GLenum error;
        std::stringstream ss;
        while ((error = glGetError()) != GL_NO_ERROR) {
            errOccurred = true;

            switch (error) {
                case GL_INVALID_ENUM:
                    ss << "GL_INVALID_ENUM" << std::endl;
                    break;
                case GL_INVALID_VALUE:
                    ss << "GL_INVALID_VALUE" << std::endl;
                    break;
                case GL_INVALID_OPERATION:
                    ss << "GL_INVALID_OPERATION" << std::endl;
                    break;
                case GL_OUT_OF_MEMORY:
                    ss << "GL_OUT_OF_MEMORY" << std::endl;
                    break;
            }
        }

        if (errOccurred)
            throw std::runtime_error(ss.str());
    }

}

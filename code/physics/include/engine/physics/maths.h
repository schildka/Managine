#pragma once

#include <reactphysics3d.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace engine::physics {

    void inline copy(rp3d::Transform const &in, glm::mat4 &out) {
        float matrix[16];
        in.getOpenGLMatrix(matrix);
        out = glm::make_mat4(matrix);
    }

    void inline copy(glm::mat4 const &in, rp3d::Transform &out) {
        float matrix[16];
        std::memcpy(matrix, glm::value_ptr(in), sizeof(matrix));
        out.setFromOpenGL(matrix);
    }

}

#pragma once

#include <glm/vec4.hpp>

namespace engine::renderer {

    class Framebuffer {
    public:
        Framebuffer(int numSamples, int width, int height);

        ~Framebuffer();

        void enable();

        void blit(int width, int height);

    private:
        unsigned int colorRenderTarget = 0;
        unsigned int depthRenderTarget = 0;
        unsigned int framebuffer = 0;

        int framebufferWidth, framebufferHeight;
        glm::vec4 clearColor{0.0f, 0.0f, 0.0f, 0.0f};
    };
}

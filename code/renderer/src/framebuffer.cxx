// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>

#include <engine/renderer/framebuffer.h>
#include "error.h"

#include <glad/glad.h>
#include <stdexcept>
#include <iostream>

namespace engine::renderer {

    Framebuffer::Framebuffer(int numSamples, int width, int height) : framebufferWidth(width),
                                                                      framebufferHeight(height) {
        std::cout << "Creating Framebuffer with " << numSamples << "x MSAA" << std::endl;

        glGenTextures(1, &colorRenderTarget);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorRenderTarget);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples, GL_RGBA8, framebufferWidth, framebufferHeight,
                                GL_TRUE);

        gl::throwOnError();
        glGenTextures(1, &depthRenderTarget);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthRenderTarget);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples, GL_DEPTH_COMPONENT24, framebufferWidth,
                                framebufferHeight, GL_TRUE);

        gl::throwOnError();
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorRenderTarget, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthRenderTarget, 0);

        gl::throwOnError();

        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Framebuffer is not complete!");
        }
    }

    Framebuffer::~Framebuffer() {
        glDeleteTextures(1, &colorRenderTarget);
        glDeleteTextures(1, &depthRenderTarget);
        glDeleteFramebuffers(1, &framebuffer);
    }

    void Framebuffer::enable() {
        // Enable Framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDepthMask(GL_TRUE);

        glViewport(0, 0, framebufferWidth, framebufferHeight);

        gl::throwOnError();
    }

    void Framebuffer::blit(int width, int height) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        gl::throwOnError();
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        gl::throwOnError();
        glDrawBuffer(GL_BACK);
        gl::throwOnError();
        glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, width, height, GL_COLOR_BUFFER_BIT,
                          GL_NEAREST);
        gl::throwOnError();
    }
}

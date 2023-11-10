#pragma once

#include <engine/renderer/lightTypes.h>

#include <glad/glad.h>
#include <stdexcept>
#include <string>

#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace engine::renderer {
    /** An OpenGl Shader handle class following the RAII principles. */
    class Shader {
    public:
        /** Creates a handle for a compiled shader. */
        Shader(const std::string source, GLenum glShaderType);

        ~Shader();

        Shader(Shader &&) = delete;

        Shader(const Shader &) = delete;

        /** Returns the id by which the shader can be referenced when using the OpenGL API. */
        GLuint getID() const;

    private:
        GLuint shaderID = 0;
    };

    /** An OpenGl Shader Program handle class following the RAII principles.*/
    class ShaderProgram {
    public:
        /** Creates a handle for a shader program linked to the supplied shaders. */
        ShaderProgram(const Shader &vertexShader, const Shader &fragmentShader, const Shader* geometryShader = nullptr);

        ~ShaderProgram();

        ShaderProgram(ShaderProgram &&) = delete;

        ShaderProgram(const ShaderProgram &) = delete;

		void setBool(const std::string &name, bool value) const;
		void setInt(const std::string &name, int value) const;
		void setFloat(const std::string &name, float value) const;
		void setVec2(const std::string &name, const glm::vec2 &value) const;
		void setVec2(const std::string &name, float x, float y) const;
		void setVec3(const std::string &name, const glm::vec3 &value) const;
		void setVec3(const std::string &name, float x, float y, float z) const;
		void setVec4(const std::string &name, const glm::vec4 &value) const;
		void setVec4(const std::string &name, float x, float y, float z, float w) const;
		void setMat2(const std::string &name, const glm::mat2 &mat) const;
		void setMat3(const std::string &name, const glm::mat3 &mat) const;
		void setMat4(const std::string &name, const glm::mat4 &mat) const;
		void setMat4T(const std::string &name, const glm::mat4 &mat) const;

		void bufferSharedMatricesUniforms();
		void bufferSharedVectorsUniforms();
		void bufferSharedLightsUniforms(int directionalLight, int numPointLights, int numSpotLights);
		void bufferSharedShadowsUniforms(int numSpotLights);

		void bindSharedMatricesUniforms(glm::mat4 projection, glm::mat4 view);
		void bindSharedVectorsUniforms(glm::vec3 camView);
		void bindSharedLightsUniforms(DirectionalLight light, std::vector<PointLight> pointLights, std::vector<SpotLight> spotLights, glm::ivec3 numberOfLights);
		void bindSharedShadowsUniforms(glm::mat4 lightSpaceMatrix[3], float cascadeClipSpace[3], std::vector<glm::mat4> spotLightSpaceMatrix);

        GLint GetAttributeLocation(std::string const &name);

        void use();

        GLuint getID() const;

    private:
        GLuint programID = 0;

		GLuint g_GlobalUniformsMatricesUBO;
		GLuint g_GlobalUniformsVectorsUBO;
		GLuint g_GlobalUniformsLightsUBO;
		GLuint g_GlobalUniformsShadowsUBO;

		static const GLuint g_iGlobalUniformMatricesBindingIndex = 0;
		static const GLuint g_iGlobalUniformLightsBindingIndex = 1;
		static const GLuint g_iGlobalUniformVectorsBindingIndex = 2;
		static const GLuint g_iGlobalUniformShadowsBindingIndex = 3;
		
    };
}

#include <engine/renderer/shader.h>

#include <iostream>

#include "error.h"

namespace engine::renderer {

    Shader::Shader(const std::string source, GLenum glShaderType) {
        shaderID = glCreateShader(glShaderType);

        // glShaderSource expects array of c-string
        // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glShaderSource.xhtml
        const char *sourceCharArray[1] = {source.c_str()};

        glShaderSource(shaderID, 1, sourceCharArray, NULL);
        glCompileShader(shaderID);

        // Error checking
        int success;
        char infoLog[512];
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
            throw std::runtime_error(std::string("Shader compilation failed!\n") + infoLog);
        }
    }

    Shader::~Shader() {
        glDeleteShader(shaderID);
    }

    GLuint Shader::getID() const {
        return shaderID;
    }

    ShaderProgram::ShaderProgram(const Shader &vertexShader, const Shader &fragmentShader, const Shader* geometryShader) {
        programID = glCreateProgram();

        glAttachShader(programID, vertexShader.getID());
        glAttachShader(programID, fragmentShader.getID());
		if(geometryShader != nullptr)glAttachShader(programID, geometryShader->getID());
        glLinkProgram(programID);

        int success;
        char infoLog[512];
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(programID, 512, NULL, infoLog);
            throw std::runtime_error(std::string("Shader Program linking failed!\n") + infoLog);
        }
    }

	// utility uniform functions
	// ------------------------------------------------------------------------
	void ShaderProgram::setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void ShaderProgram::setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void ShaderProgram::setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void ShaderProgram::setVec2(const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
	}
	void ShaderProgram::setVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void ShaderProgram::setVec3(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
	}
	void ShaderProgram::setVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void ShaderProgram::setVec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
	}
	void ShaderProgram::setVec4(const std::string &name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void ShaderProgram::setMat2(const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void ShaderProgram::setMat3(const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void ShaderProgram::setMat4(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void ShaderProgram::setMat4T(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_TRUE, &mat[0][0]);
	}

	void ShaderProgram::bufferSharedMatricesUniforms() {
		glGenBuffers(1, &g_GlobalUniformsMatricesUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, g_GlobalUniformsMatricesUBO);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, g_iGlobalUniformMatricesBindingIndex,
			g_GlobalUniformsMatricesUBO, 0, sizeof(glm::mat4) * 2);

		gl::throwOnError();
	}

	void ShaderProgram::bufferSharedVectorsUniforms() {
		glGenBuffers(1, &g_GlobalUniformsVectorsUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, g_GlobalUniformsVectorsUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, g_iGlobalUniformVectorsBindingIndex,
			g_GlobalUniformsVectorsUBO, 0, sizeof(glm::vec4));

		gl::throwOnError();
	}

	void ShaderProgram::bufferSharedLightsUniforms(int directionalLight, int numPointLights, int numSpotLights) {
		glGenBuffers(1, &g_GlobalUniformsLightsUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, g_GlobalUniformsLightsUBO);
		glBufferData(GL_UNIFORM_BUFFER, 64 + 16 + 80 * 5 + 96 * numSpotLights, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, g_iGlobalUniformLightsBindingIndex,
			g_GlobalUniformsLightsUBO, 0, 64 + 16 + 80 * 5 + 96 * numSpotLights);

		gl::throwOnError();
	}

	void ShaderProgram::bufferSharedShadowsUniforms(int numSpotLights) {
		glGenBuffers(1, &g_GlobalUniformsShadowsUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, g_GlobalUniformsShadowsUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3 + 3 * sizeof(glm::vec4) + sizeof(glm::mat4) * numSpotLights, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		glBindBufferRange(GL_UNIFORM_BUFFER, g_iGlobalUniformShadowsBindingIndex,
			g_GlobalUniformsShadowsUBO, 0, sizeof(glm::mat4) * 3 + 3 * sizeof(glm::vec4) + sizeof(glm::mat4) * numSpotLights);

		gl::throwOnError();
	}

	void ShaderProgram::bindSharedMatricesUniforms(glm::mat4 projection, glm::mat4 view) {
		glBindBuffer(GL_UNIFORM_BUFFER, g_GlobalUniformsMatricesUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view[0][0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		gl::throwOnError();
	}

	void ShaderProgram::bindSharedVectorsUniforms(glm::vec3 camView) {
		glBindBuffer(GL_UNIFORM_BUFFER, g_GlobalUniformsVectorsUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), &camView[0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		gl::throwOnError();
	}

	void ShaderProgram::bindSharedLightsUniforms(DirectionalLight light, std::vector<PointLight> pointLights, std::vector<SpotLight> spotLights, glm::ivec3 numberOfLights) {
		glBindBuffer(GL_UNIFORM_BUFFER, g_GlobalUniformsLightsUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &light);
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 12, &numberOfLights[0]);

		for (unsigned int i = 0; i < numberOfLights.x; i++)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, 64 + 16 + 80 * i, 76, &pointLights[i]);
		}

		for (unsigned int i = numberOfLights.x; i < 5; i++)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, 64 + 16 + 80 * i, 76, 0);
		}

		for (unsigned int i = 0; i < numberOfLights.y; i++)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, 64 + 16 + 80 * 5 + 96 * i, 96, &spotLights[i]);
		}

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		gl::throwOnError();
	}

	void ShaderProgram::bindSharedShadowsUniforms(glm::mat4 lightSpaceMatrix[3], float cascadeClipSpace[3], std::vector<glm::mat4> spotLightSpaceMatrix) {
		glBindBuffer(GL_UNIFORM_BUFFER, g_GlobalUniformsShadowsUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 3, &lightSpaceMatrix[0]);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3, sizeof(glm::vec4), &cascadeClipSpace[0]);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3 + sizeof(glm::vec4), sizeof(glm::vec4), &cascadeClipSpace[1]);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3 + sizeof(glm::vec4) * 2, sizeof(glm::vec4), &cascadeClipSpace[2]);

		for (unsigned int i = 0; i < spotLightSpaceMatrix.size(); i++)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3 + sizeof(glm::vec4) * 3, sizeof(glm::mat4), &spotLightSpaceMatrix[i]);
		}

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		/*
		const GLchar *bufferNames[3] = { "lightSpaceMatrix[0]", "cascadeEndClipSpace[0]", "spotLightSpaceMatrix[0]"};

		GLuint uniformBlockIndex = glGetUniformBlockIndex(programID, "GlobalShadows");

		GLint blockSize;
		glGetActiveUniformBlockiv(programID, uniformBlockIndex,
		GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
		GLubyte* blockBuffer = (GLubyte *)malloc(blockSize);

		GLuint uniformIndices[3];

		glGetUniformIndices(programID, 3, bufferNames, uniformIndices);

		GLint uniformOffsets[3];

		glGetActiveUniformsiv(programID, 3, uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);

		std::cout << uniformOffsets[0] << uniformOffsets[1] << uniformOffsets[2] << std::endl;
		*/

		gl::throwOnError();
	}

    GLint ShaderProgram::GetAttributeLocation(std::string const &name) {
        GLint attr = glGetAttribLocation(programID, name.c_str());
        return attr;
    }

    void ShaderProgram::use() {
        glUseProgram(programID);
    }

    GLuint ShaderProgram::getID() const {
        return programID;
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(programID);
    }
}

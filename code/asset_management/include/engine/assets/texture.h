#pragma once

#include <engine/assets/bitmap.h>

#include <glad/glad.h>

namespace engine::assets {

	/**
	* A Texture loader.
	* @param id Textures id.
	* @param width of textures.
	* @param height of textures.
	* @param hasTransparency if texture should be transparent.
	*/
	class Texture {
	public:

		Texture() {}

		/**
		* Initalize a cubemap or a single texture depending on users needs. Can be used for shadowmapping techniques.
		* @param width of texture
		* @param height of texture
		* @param isSingleDirectionTexture if texture should be a cubemap or not.
		*/
		Texture(unsigned int width, unsigned int height, bool isSingleDirectionTexture) {

			glGenTextures(1, &_id);

			if (isSingleDirectionTexture) {
				glBindTexture(GL_TEXTURE_2D, _id);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			}
			else {
				glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

				for (unsigned int i = 0; i < 6; ++i)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			}
		}

		/**
		* Initalize a cubemap.
		* @param faces a vectore of different path strings to textures.
		*/
		Texture(std::vector<std::string> faces) {

			glGenTextures(1, &_id);
			glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

			for (unsigned int i = 0; i < faces.size(); i++)
			{
				Bitmap map = Bitmap(faces[i]);
				if (map.data())
				{
					GLenum format;
					if (map.channels() == 1)
						format = GL_RED;
					else if (map.channels() == 3)
						format = GL_RGB;
					else if (map.channels() == 4)
						format = GL_RGBA;

					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0, format, map.width(), map.height(), 0, format, GL_UNSIGNED_BYTE, map.data()
					);
				}

			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}

		Texture(std::filesystem::path relativeAssetPath) {

			glGenTextures(1, &_id);

			Bitmap map = Bitmap(relativeAssetPath);
			if (map.data())
			{
				GLenum format;
				if (map.channels() == 1)
					format = GL_RED;
				else if (map.channels() == 3)
					format = GL_RGB;
				else if (map.channels() == 4)
					format = GL_RGBA;
				
				glBindTexture(GL_TEXTURE_2D, _id);
				glTexImage2D(GL_TEXTURE_2D, 0, format, map.width(), map.height(), 0, format, GL_UNSIGNED_BYTE, map.data());
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
		}

		void loadImage(std::filesystem::path relativeAssetPath) {
			glGenTextures(1, &_id);

			Bitmap map = Bitmap(relativeAssetPath);
			if (map.data())
			{
				GLenum format;
				if (map.channels() == 1)
					format = GL_RED;
				else if (map.channels() == 3)
					format = GL_RGB;
				else if (map.channels() == 4)
					format = GL_RGBA;

				glBindTexture(GL_TEXTURE_2D, _id);
				glTexImage2D(GL_TEXTURE_2D, 0, format, map.width(), map.height(), 0, format, GL_UNSIGNED_BYTE, map.data());

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
		}

		static Texture CreateTextureAttachment(int width, int height) {
			Texture texture = Texture();
			texture.width = width, texture.height = height;
			texture.hasTransparency = false;

			glGenTextures(1, &(texture._id));
			glBindTexture(GL_TEXTURE_2D, texture._id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture._id, 0);

			return texture;
		}

		unsigned int id() const {
			return _id;
		}

		unsigned int getWidth() const {
			return width;
		}

		unsigned int getHeight() const {
			return height;
		}

		bool getTransparency() const {
			return hasTransparency;
		}

	private:
		unsigned int _id;
		unsigned int width;
		unsigned int height;
		bool hasTransparency;
	};
}

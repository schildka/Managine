// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>

#pragma once

#include <engine/assets/assets.h>
#include <stb/stb_image.h>
#include <stdexcept>

namespace engine::assets {
    class Bitmap {
    public:
		Bitmap() {}

        Bitmap(std::filesystem::path relativeAssetPath) {
            auto p = resolveAssetPath(relativeAssetPath);

			// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
			// stbi_set_flip_vertically_on_load(true);

            _data = stbi_load(p.string().c_str(), &_width, &_height, &_channels, 0);

            if (_data == nullptr)
                throw std::runtime_error(std::string("Could not load file: ") + relativeAssetPath.string());
        }

		void load(std::filesystem::path relativeAssetPath) {
			auto p = resolveAssetPath(relativeAssetPath);

			// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
			// stbi_set_flip_vertically_on_load(true);

			_data = stbi_load(p.string().c_str(), &_width, &_height, &_channels, 0);

			if (_data == nullptr)
				throw std::runtime_error(std::string("Could not load file: ") + relativeAssetPath.string());
		}

        int width() const {
            return _width;
        }

        int height() const {
            return _height;
        }

        int channels() const {
            return _channels;
        }

        const unsigned char * data() const {
            return _data;
        }

        ~Bitmap() {
            stbi_image_free(_data);
        }

    private:
        int _width, _height, _channels;
        unsigned char *_data;
    };
}


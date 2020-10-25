#pragma once

#include <engine/assets/assets.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

namespace engine::assets {
	/**
	* A Text loader.
	* @param data Data of file.
	* @param fileSize Size of file.
	*/
    class Text {
    public:
        Text(std::filesystem::path assetPath) {
            auto path = resolveAssetPath(assetPath);

            // See http://0x80.pl/notesen/2019-01-07-cpp-read-file.html
            auto ss = std::ostringstream{};
            std::ifstream file(path);
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            ss << file.rdbuf();
            data = ss.str();

			// Get File size
			int pos = file.tellg();
			file.seekg(0, std::ios::end);
			int length = file.tellg();
			// Restore the position of the get pointer
			file.seekg(pos);

			fileSize = length;
        }

        const std::string get() const {
            return data;
        }

		const unsigned int getFileSize() const {
			return fileSize;
		}

    private:
        std::string data;
		unsigned int fileSize;
    };
}

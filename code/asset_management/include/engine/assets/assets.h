#pragma once

#include <engine/generated/defaults.h>

#include <filesystem>

namespace engine::assets {
    using namespace std::filesystem;

    inline path resolveAssetPath(path relativeAssetPath) {
        return canonical(ENGINE_DEV_ROOT_PATH / path("assets") / relativeAssetPath);
    }

	inline char separator()
	{
		#ifdef _WIN32
			return '\\';
		#else
			return '/';
		#endif
	}
}

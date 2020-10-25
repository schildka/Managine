#pragma once

#include <engine/assets/assets.h>

#include <irrKlang.h>

namespace engine::assets {
	/**
	* A Sound loader.
	* @param sound Holds sound file.
	*/
	class Sound {
	
	public:
		Sound() {}

		Sound(std::filesystem::path assetPath, irrklang::ISoundEngine* soundEngine) {
			auto path = resolveAssetPath(assetPath);
		
			sound = soundEngine->addSoundSourceFromFile(path.string().c_str());
		}

		irrklang::ISoundSource* getSound() {
			return sound;
		}

	private:
		irrklang::ISoundSource* sound;
	};
}

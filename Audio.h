#pragma once
#include <string>
#include <unordered_map>

#ifdef _WIN32

#include <AL/al.h>
#include <AL/alc.h>
#endif

class Audio {
public:
	static Audio& GetInstance();

	bool Initialize();
	void Shutdown();

	bool LoadSound(const std::string& name, const std::string& filepath);
	void PlaySound(const std::string& name, bool loop = false);
	void StopSound(const std::string& name);
	void StopAllSounds();

	void SetListenerPosition(float x, float y, float z);
	void SetSoundPosition(const std::string& name, float x, float y, float z);

private:
	Audio() = default;
	~Audio() = default;

	struct SoundData {
		ALuint buffer = 0;
		ALuint source = 0;
		bool isValid = false;
	};

	ALCdevice* device = nullptr;
	ALCcontext* context = nullptr;
	std::unordered_map<std::string, SoundData> sounds;

	bool LoadWAV(const std::string& filepath, ALuint& buffer);
};


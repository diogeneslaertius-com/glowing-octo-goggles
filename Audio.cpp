#include "Audio.h"
#include <iostream>
#include <fstream>
#include <vector>

Audio& Audio::GetInstance() {
	static Audio instance;
	return instance;
}

bool Audio::Initialize() {
	// Открываем устройство
	device = alcOpenDevice(nullptr);
	if (!device) {
		std::cerr << "Failed to open OpenAL device" << std::endl;
		return false;
	}

	// Создаем контекст
	context = alcCreateContext(device, nullptr);
	if (!context) {
		std::cerr << "Failed to create OpenAL context" << std::endl;
		alcCloseDevice(device);
		return false;
	}

	// Активируем контекст
	if (!alcMakeContextCurrent(context)) {
		std::cerr << "Failed to make OpenAL context current" << std::endl;
		alcDestroyContext(context);
		alcCloseDevice(device);
		return false;
	}

	std::cout << "OpenAL initialized successfully" << std::endl;
	return true;
}

void Audio::Shutdown() {
	StopAllSounds();

	// Удаляем все звуки
	for (auto& [name, sound] : sounds) {
		if (sound.isValid) {
			alDeleteSources(1, &sound.source);
			alDeleteBuffers(1, &sound.buffer);
		}
	}
	sounds.clear();

	// Закрываем OpenAL
	if (context) {
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		context = nullptr;
	}

	if (device) {
		alcCloseDevice(device);
		device = nullptr;
	}

	std::cout << "OpenAL shutdown complete" << std::endl;
}

bool Audio::LoadSound(const std::string& name, const std::string& filepath) {
	SoundData soundData;

	// Генерируем буфер и источник
	alGenBuffers(1, &soundData.buffer);
	alGenSources(1, &soundData.source);

	// Загружаем WAV файл
	if (!LoadWAV(filepath, soundData.buffer)) {
		std::cerr << "Failed to load sound: " << filepath << std::endl;
		alDeleteSources(1, &soundData.source);
		alDeleteBuffers(1, &soundData.buffer);
		return false;
	}

	// Привязываем буфер к источнику
	alSourcei(soundData.source, AL_BUFFER, soundData.buffer);
	soundData.isValid = true;

	sounds[name] = soundData;
	std::cout << "Loaded sound: " << name << " from " << filepath << std::endl;
	return true;
}

void Audio::PlaySound(const std::string& name, bool loop) {
	auto it = sounds.find(name);
	if (it != sounds.end() && it->second.isValid) {
		alSourcei(it->second.source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
		alSourcePlay(it->second.source);
	}
}

void Audio::StopSound(const std::string & name) {
	auto it = sounds.find(name);
	if (it != sounds.end() && it->second.isValid) {
		alSourceStop(it->second.source);
	}
}

void Audio::StopAllSounds() {
	for (auto& [name, sound] : sounds) {
		if (sound.isValid) {
			alSourceStop(sound.source);
		}
	}
}

void Audio::SetListenerPosition(float x, float y, float z) {
	alListener3f(AL_POSITION, x, y, z);
}

void Audio::SetSoundPosition(const std::string & name, float x, float y, float z) {
	auto it = sounds.find(name);
	if (it != sounds.end() && it->second.isValid) {
		alSource3f(it->second.source, AL_POSITION, x, y, z);
	}
}

bool Audio::LoadWAV(const std::string & filepath, ALuint & buffer) {
	// Простая загрузка WAV (только для несжатых PCM файлов)
	std::ifstream file(filepath, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Cannot open WAV file: " << filepath << std::endl;
		return false;
	}

	// Читаем WAV заголовок (упрощенная версия)
	char header[44];
	file.read(header, 44);

	if (file.gcount() != 44) {
		std::cerr << "Invalid WAV header size" << std::endl;
		return false;
	}

	// Извлекаем параметры из заголовка
	int sampleRate = *reinterpret_cast<int*>(&header[24]);
	short channels = *reinterpret_cast<short*>(&header[22]);
	short bitsPerSample = *reinterpret_cast<short*>(&header[34]);
	int dataSize = *reinterpret_cast<int*>(&header[40]);

	// Читаем аудио данные
	std::vector<char> audioData(dataSize);
	file.read(audioData.data(), dataSize);

	if (file.gcount() != dataSize) {
		std::cerr << "Failed to read audio data" << std::endl;
		return false;
	}

	// Определяем формат OpenAL
	ALenum format;
	if (channels == 1) {
		format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
	}
	else {
		format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
	}

	// Загружаем данные в буфер OpenAL
	alBufferData(buffer, format, audioData.data(), dataSize, sampleRate);

	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << "OpenAL error: " << error << std::endl;
		return false;
	}

	return true;
}
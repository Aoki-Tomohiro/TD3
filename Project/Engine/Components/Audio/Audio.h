#pragma once
#include <array>
#include <vector>
#include <set>
#include <fstream>
#include <wrl.h>
#include <xaudio2.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib,"xaudio2.lib")
#pragma comment(lib,"Mf.lib")
#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"Mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")

class Audio {
private:
	//エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	static const std::string kBaseDirectory;

public:
	//音声データの最大数
	static const int kMaxSoundData = 256;

	//チャンクヘッダ
	struct ChunkHeader {
		char id[4];//チャンク毎のID
		int32_t size;//チャンクサイズ
	};

	//RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk;//"RIFF"
		char type[4];//"WAVE"
	};

	//FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk;//"fmt"
		WAVEFORMATEX fmt;//波形フォーマット
	};

	//音声データ
	struct SoundData {
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		std::vector<BYTE> pBuffer;
		//バッファのサイズ
		size_t bufferSize;
		//名前
		std::string name;
		//オーディオハンドル
		uint32_t audioHandle;
	};

	//ボイスデータ
	struct Voice {
		uint32_t handle = 0;
		IXAudio2SourceVoice* sourceVoice = nullptr;
	};

	static Audio* GetInstance();

	void Finalize();

	void Initialize();

	uint32_t LoadAudioFile(const std::string& filename);

	void SoundUnload(SoundData* soundData);

	uint32_t PlayAudio(uint32_t audioHandle, bool loopFlag, float volume);

	void StopAudio(uint32_t voiceHandle);

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator = (const Audio&) = delete;

private:
	ComPtr<IXAudio2> xAudio2_ = nullptr;

	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	std::array<SoundData, kMaxSoundData> soundDatas_{};

	std::set<Voice*> sourceVoices_{};

	uint32_t audioHandle_ = -1;

	uint32_t voiceHandle_ = -1;
};
#pragma once
#include <array>
#include <string_view>
#include <fstream>

namespace simple_sf2
{
	/*
	 * Chunks:
	 */

	struct RiffChunk final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(m_chunkName.data(), static_cast<std::streamsize>(m_chunkName.size()));
			stream.read(reinterpret_cast<char*>(&m_chunkSize), 4);
		}

		[[nodiscard]] std::string_view GetName() const { return std::string_view{m_chunkName}; }
		[[nodiscard]] uint32_t GetSize() const { return m_chunkSize; }

	protected:
		std::array<char, 4> m_chunkName{};
		uint32_t m_chunkSize = 0u;
	};

	/*
	 * Info
	 */

	struct info_ifil final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(reinterpret_cast<char*>(&m_major), 2);
			stream.read(reinterpret_cast<char*>(&m_minor), 2);
		}

		int16_t m_major = 0i16;
		int16_t m_minor = 0i16;
	};

	/*
	 * Generators/Modulators
	 */

	struct rangesType final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(reinterpret_cast<char*>(&m_lo), 1);
			stream.read(reinterpret_cast<char*>(&m_hi), 1);
		}

		uint8_t m_lo = 0ui8;
		uint8_t m_hi = 0ui8;
	};

	constexpr uint32_t RANGES_TYPE_SIZE = 2u;

	struct genAmountType final
	{
		void Read(std::ifstream& stream)
		{
			m_ranges.Read(stream);
			stream.read(reinterpret_cast<char*>(&m_amountS16), 2);
			stream.read(reinterpret_cast<char*>(&m_amountU16), 2);
		}

		rangesType m_ranges{};
		int16_t m_amountS16 = 0i16;
		uint16_t m_amountU16 = 0ui16;
	};

	constexpr uint32_t GEN_AMT_TYPE_SIZE = RANGES_TYPE_SIZE + 4u;

	struct pdta_modulator final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(reinterpret_cast<char*>(&m_modSrcOper), 2);
			stream.read(reinterpret_cast<char*>(&m_modDestOper), 2);
			stream.read(reinterpret_cast<char*>(&m_modAmount), 2);
			stream.read(reinterpret_cast<char*>(&m_modAmtSrcOper), 2);
			stream.read(reinterpret_cast<char*>(&m_modTransOper), 2);
		}

		uint16_t m_modSrcOper = 0ui16;
		uint16_t m_modDestOper = 0ui16;
		int16_t m_modAmount = 0i16;
		uint16_t m_modAmtSrcOper = 0ui16;
		uint16_t m_modTransOper = 0ui16;
	};

	constexpr uint32_t PDTA_MODULATOR_SIZE = 10u;

	enum class EGeneratorType final : uint16_t
	{
		startAddrsOffset = 0ui16, endAddrsOffset, startloopAddrsOffset, endloopAddrsOffset, startAddrsCoarseOffset,
		modLfoToPitch, vibLfoToPitch, modEnvToPitch, initialFilterFc, initialFilterQ, modLfoToFilterFc,
		modEnvToFilterFc, endAddrsCoarseOffset, modLfoToVolume, unused1, chorusEffectsSend, reverbEffectsSend,
		pan, unused2, unused3, unused4, delayModLFO, freqModLFO, delayVibLFO, freqVibLFO, delayModEnv,
		attackModEnv, holdModEnv, decayModEnv, sustainModEnv, releaseModEnv, keynumToModEnvHold,
		keynumToModEnvDecay, delayVolEnv, attackVolEnv, holdVolEnv, decayVolEnv, sustainVolEnv,
		releaseVolEnv, keynumToVolEnvHold, keynumToVolEnvDecay, instrument, reserved1, keyRange,
		velRange, startloopAddrsCoarseOffset, keynum, velocity, initialAttenuation, reserved2,
		endloopAddrsCoarseOffset, coarseTune, fineTune, sampleID, sampleModes, reserved3, scaleTuning,
		exclusiveClass, overridingRootKey, unused5, endOper, GEN_MAX
	};

	struct pdta_generator final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(reinterpret_cast<char*>(&m_genOper), sizeof(EGeneratorType));
			m_genAmount.Read(stream);
		}

		EGeneratorType m_genOper = EGeneratorType::startAddrsOffset;
		genAmountType m_genAmount{};
	};

	constexpr uint32_t PDTA_GENERATOR_SIZE = 2u + GEN_AMT_TYPE_SIZE;

	struct pdta_bag final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(reinterpret_cast<char*>(&m_genNdx), sizeof(int32_t));
			stream.read(reinterpret_cast<char*>(&m_modNdx), sizeof(int32_t));
		}

		int32_t m_genNdx = 0;
		int32_t m_modNdx = 0;
	};

	constexpr uint32_t PDTA_BAG_SIZE = 8u;

	/*
	 * Presets
	 */

	struct pdta_phdr final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(m_presetName.data(), static_cast<std::streamsize>(m_presetName.size()));
			stream.read(reinterpret_cast<char*>(&m_preset), sizeof(int16_t));
			stream.read(reinterpret_cast<char*>(&m_bank), sizeof(int16_t));
			stream.read(reinterpret_cast<char*>(&m_presetBagNdx), sizeof(int16_t));
			stream.read(reinterpret_cast<char*>(&m_library), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&m_genre), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&m_morphology), sizeof(uint32_t));
		}

		std::array<char, 20> m_presetName{};
		int16_t m_preset = 0i16;
		int16_t m_bank = 0i16;
		int16_t m_presetBagNdx = 0i16;
		uint32_t m_library = 0u;
		uint32_t m_genre = 0u;
		uint32_t m_morphology = 0u;
	};

	constexpr uint32_t PDTA_PHDR_SIZE = 38u;

	/*
	 * Instruments:
	 */

	struct pdta_inst final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(m_instrumentName.data(), static_cast<std::streamsize>(m_instrumentName.size()));
			stream.read(reinterpret_cast<char*>(&m_instBagNdx), 2);
		}

		std::array<char, 20> m_instrumentName{};
		int16_t m_instBagNdx = 0i16;
	};

	constexpr uint32_t PDTA_INST_SIZE = 22u;

	/*
	 * Samples:
	 */

	enum struct ESampleLinkType final : uint16_t
	{
		MONO_SAMPLE = 1ui16,
		RIGHT_SAMPLE = 2ui16,
		LEFT_SAMPLE = 4ui16,
		LINKED_SAMPLE = 8ui16,
		ROM_MONO_SAMPLE = 0x8001ui16,
		ROM_RIGHT_SAMPLE = 0x8002ui16,
		ROM_LEFT_SAMPLE = 0x8004ui16,
		ROM_LINKED_SAMPLE = 0x8008ui16
	};

	struct pdta_shdr final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(m_sampleName.data(), static_cast<std::streamsize>(m_sampleName.size()));
			stream.read(reinterpret_cast<char*>(&m_sampleStart), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&m_sampleEnd), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&m_loopStart), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&m_loopEnd), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&m_sampleRate), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&m_originalPitch), sizeof(uint8_t));
			stream.read(reinterpret_cast<char*>(&m_pitchCorrection), sizeof(int8_t));
			stream.read(reinterpret_cast<char*>(&m_sampleLink), sizeof(uint16_t));
			stream.read(reinterpret_cast<char*>(&m_sampleType), sizeof(ESampleLinkType));
		}

		std::array<char, 20> m_sampleName{};
		uint32_t m_sampleStart = 0u;
		uint32_t m_sampleEnd = 0u;
		uint32_t m_loopStart = 0u;
		uint32_t m_loopEnd = 0u;
		uint32_t m_sampleRate = 0u;
		uint8_t m_originalPitch = 0ui8;
		int8_t m_pitchCorrection = 0i8;
		uint16_t m_sampleLink = 0ui16;
		ESampleLinkType m_sampleType = ESampleLinkType::MONO_SAMPLE;
	};

	constexpr uint32_t PDTA_SHDR_SIZE = 46u;
}
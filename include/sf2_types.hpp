#pragma once
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
			m_chunkName.clear();
			m_chunkName.resize(4);
			stream.read(m_chunkName.data(), static_cast<std::streamsize>(m_chunkName.size()));
			
			stream.read(reinterpret_cast<char*>(&m_chunkSize), 4);
		}

		[[nodiscard]] const std::string& GetName() const { return m_chunkName; }
		[[nodiscard]] uint32_t GetSize() const { return m_chunkSize; }

	protected:
		std::string m_chunkName;
		uint32_t m_chunkSize = 0u;
	};

	/*
	 * Info
	 */

	struct info_ifil final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(reinterpret_cast<char*>(&m_major), sizeof(int16_t));
			stream.read(reinterpret_cast<char*>(&m_minor), sizeof(int16_t));
		}

		int16_t m_major = 0i16;
		int16_t m_minor = 0i16;
	};

	/*
	 * Generators/Modulators
	 */

	enum class SFGenerator final : uint16_t
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
	
	struct rangesType final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(reinterpret_cast<char*>(&m_lo), sizeof(uint8_t));
			stream.read(reinterpret_cast<char*>(&m_hi), sizeof(uint8_t));
		}

		uint8_t m_lo = 0ui8;
		uint8_t m_hi = 0ui8;
	};
	
	struct genAmountType final
	{
		void Read(std::ifstream& stream)
		{
			m_ranges.Read(stream);
			m_amountS16 = static_cast<int16_t>(static_cast<int16_t>(m_ranges.m_hi) << 8u | m_ranges.m_lo);
			m_amountU16 = static_cast<uint16_t>(static_cast<uint16_t>(m_ranges.m_hi) << 8 | m_ranges.m_lo);
		}

		rangesType m_ranges{};
		int16_t m_amountS16 = 0i16;
		uint16_t m_amountU16 = 0ui16;
	};

	enum struct SFTransform final : uint16_t
	{
		kLinear = 0ui16, kAbsoluteValue = 2ui16,
	};

	struct SFModulator final
	{
		uint8_t m_index : 7;
		uint8_t m_controller : 1;
		uint8_t m_direction : 1;
		uint8_t m_polarity : 1;
		uint8_t m_type : 6;
	};
	
	struct pdta_modulator final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(reinterpret_cast<char*>(&m_modSrcOper), sizeof(SFModulator));
			stream.read(reinterpret_cast<char*>(&m_modDestOper), sizeof(SFGenerator));
			stream.read(reinterpret_cast<char*>(&m_modAmount), sizeof(int16_t));
			stream.read(reinterpret_cast<char*>(&m_modAmtSrcOper), sizeof(SFModulator));
			stream.read(reinterpret_cast<char*>(&m_modTransOper), sizeof(SFTransform));
		}

		SFModulator m_modSrcOper;
		SFGenerator m_modDestOper = SFGenerator::startAddrsOffset;
		int16_t m_modAmount = 0i16;
		SFModulator m_modAmtSrcOper;
		SFTransform m_modTransOper = SFTransform::kLinear;
	};

	constexpr size_t PDTA_MODULATOR_SIZE = sizeof(SFModulator) + sizeof(SFGenerator) + sizeof(int16_t) + sizeof(SFModulator) + sizeof(SFTransform);

	struct pdta_generator final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(reinterpret_cast<char*>(&m_genOper), sizeof(SFGenerator));
			m_genAmount.Read(stream);
		}

		SFGenerator m_genOper = SFGenerator::startAddrsOffset;
		genAmountType m_genAmount{};
	};

	constexpr size_t PDTA_GENERATOR_SIZE = sizeof(SFGenerator) + sizeof(uint8_t) + sizeof(uint8_t);

	struct pdta_bag final
	{
		void Read(std::ifstream& stream)
		{
			stream.read(reinterpret_cast<char*>(&m_genNdx), sizeof(int16_t));
			stream.read(reinterpret_cast<char*>(&m_modNdx), sizeof(int16_t));
		}

		int16_t m_genNdx = 0i16;
		int16_t m_modNdx = 0i16;
	};

	/*
	 * Presets
	 */

	struct pdta_phdr final
	{
		void Read(std::ifstream& stream)
		{
			m_presetName.clear();
			m_presetName.resize(20);
			stream.read(m_presetName.data(), static_cast<std::streamsize>(m_presetName.size()));
			
			stream.read(reinterpret_cast<char*>(&m_preset), sizeof(int16_t));
			stream.read(reinterpret_cast<char*>(&m_bank), sizeof(int16_t));
			stream.read(reinterpret_cast<char*>(&m_presetBagNdx), sizeof(int16_t));
			stream.read(reinterpret_cast<char*>(&m_library), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&m_genre), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&m_morphology), sizeof(uint32_t));
		}

		std::string m_presetName;
		int16_t m_preset = 0i16;
		int16_t m_bank = 0i16;
		int16_t m_presetBagNdx = 0i16;
		uint32_t m_library = 0u;
		uint32_t m_genre = 0u;
		uint32_t m_morphology = 0u;
	};

	constexpr size_t PDTA_PHDR_SIZE = 20 + sizeof(int16_t) + sizeof(int16_t) + sizeof(int16_t) + sizeof(uint32_t)
		+ sizeof(uint32_t) + sizeof(uint32_t);

	/*
	 * Instruments:
	 */

	struct pdta_inst final
	{
		void Read(std::ifstream& stream)
		{
			m_instrumentName.clear();
			m_instrumentName.resize(20);
			stream.read(m_instrumentName.data(), static_cast<std::streamsize>(m_instrumentName.size()));
			
			stream.read(reinterpret_cast<char*>(&m_instBagNdx), sizeof(int16_t));
		}

		std::string m_instrumentName;
		int16_t m_instBagNdx = 0i16;
	};

	constexpr size_t PDTA_INST_SIZE = 20 + sizeof(int16_t);

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
			m_sampleName.clear();
			m_sampleName.resize(20);
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

		std::string m_sampleName;
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

	constexpr size_t PDTA_SHDR_SIZE = 20 + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t)
		+ sizeof(uint32_t) + sizeof(uint8_t) + sizeof(int8_t) + sizeof(uint16_t) + sizeof(ESampleLinkType);
}

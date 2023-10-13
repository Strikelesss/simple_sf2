#pragma once
#include <filesystem>
#include "sf2_types.hpp"
#include <array>

namespace simple_sf2
{
	struct SF2Bank final
	{
		SF2Bank() = default;

		info_ifil m_versionTag{};
		std::string m_soundEngine{};
		std::string m_engineer{};
		std::string m_bankName{};
		std::string m_software{};

		std::vector<pdta_phdr> m_presets{};
		std::vector<pdta_bag> m_presetBags{};
		std::vector<pdta_generator> m_presetGens{};
		std::vector<pdta_modulator> m_presetMods{};
	
		std::vector<pdta_inst> m_instruments{};
		std::vector<pdta_bag> m_instrumentBags{};
		std::vector<pdta_generator> m_instrumentGens{};
		std::vector<pdta_modulator> m_instrumentMods{};
	
		std::vector<pdta_shdr> m_samples{};
		std::vector<uint16_t> m_sampleData{};
	};

	enum struct ESF2ReadResult final
	{
		READ_SUCCESS, FILE_NOT_EXIST
	};
	
	inline ESF2ReadResult ReadSF2(const std::filesystem::path& sf2File, SF2Bank& outBank)
	{
		if (std::filesystem::exists(sf2File))
		{
			std::ifstream stream(sf2File.c_str(), std::ios::binary);
			if (stream.is_open())
			{
				RiffChunk riff;
				riff.Read(stream);

				if (riff.GetName() == "RIFF")
				{
					std::array<char, 4> sfbk{};
					stream.read(sfbk.data(), sfbk.size());

					if (std::string_view{sfbk.data(), sfbk.size()} == "sfbk")
					{
						while (!stream.eof())
						{
							RiffChunk chunkList;
							chunkList.Read(stream);

							std::array<char, 4> chunkIDData{};
							stream.read(chunkIDData.data(), chunkIDData.size());

							const std::string_view chunkID{chunkIDData.data(), chunkIDData.size()};
							if (chunkID == "INFO")
							{
								const std::istream::pos_type readLocStart(stream.tellg());
								while (stream.tellg() - readLocStart != chunkList.GetSize() - 4)
								{
									RiffChunk subChunk;
									subChunk.Read(stream);

									if (subChunk.GetName() == "ifil")
									{
										outBank.m_versionTag.Read(stream);
									}
									else if (subChunk.GetName() == "isng")
									{
										outBank.m_soundEngine.resize(subChunk.GetSize());
										stream.read(outBank.m_soundEngine.data(),
											static_cast<std::streamsize>(outBank.m_soundEngine.size()));
									}
									else if (subChunk.GetName() == "IENG")
									{
										outBank.m_engineer.resize(subChunk.GetSize());
										stream.read(outBank.m_engineer.data(),
											static_cast<std::streamsize>(outBank.m_engineer.size()));
									}
									else if (subChunk.GetName() == "INAM")
									{
										outBank.m_bankName.resize(subChunk.GetSize());
										stream.read(outBank.m_bankName.data(),
											static_cast<std::streamsize>(outBank.m_bankName.size()));
									}
									else if (subChunk.GetName() == "ISFT")
									{
										outBank.m_software.resize(subChunk.GetSize());
										stream.read(outBank.m_software.data(),
											static_cast<std::streamsize>(outBank.m_software.size()));
									}
									else
									{
										// std::cout << "Skipping " << subChunk.GetName() << "!\n";
										stream.ignore(subChunk.GetSize());
									}
								}
							}
							else if (chunkID == "sdta")
							{
								const std::istream::pos_type readLocStart(stream.tellg());
								while (stream.tellg() - readLocStart != chunkList.GetSize() - 4)
								{
									RiffChunk subChunk;
									subChunk.Read(stream);

									outBank.m_sampleData.resize(subChunk.GetSize());
									stream.read(reinterpret_cast<char*>(outBank.m_sampleData.data()),
										static_cast<std::streamsize>(outBank.m_sampleData.size()));
								}
							}
							else if (chunkID == "pdta")
							{
								const std::istream::pos_type readLoc(stream.tellg());

								while (stream.tellg() - readLoc != chunkList.GetSize() - 4)
								{
									RiffChunk subChunk;
									subChunk.Read(stream);
									
									if (subChunk.GetName() == "phdr")
									{
										const size_t numPhdrs(subChunk.GetSize() / PDTA_PHDR_SIZE - 1);
										for (size_t i(0); i < numPhdrs; ++i)
										{
											pdta_phdr phdr{};
											phdr.Read(stream);

											outBank.m_presets.emplace_back(std::move(phdr));
										}

										pdta_phdr phdr_terminator{};
										phdr_terminator.Read(stream);
									}
									else if (subChunk.GetName() == "pbag" || subChunk.GetName() == "ibag")
									{
										const size_t numBags(subChunk.GetSize() / sizeof(pdta_bag) - 1);
										for (size_t i(0); i < numBags; ++i)
										{
											pdta_bag bag{};
											bag.Read(stream);

											if (subChunk.GetName() == "pbag")
											{
												outBank.m_presetBags.emplace_back(std::move(bag));
											}
											else
											{
												outBank.m_instrumentBags.emplace_back(std::move(bag));
											}
										}

										pdta_bag bag_terminator{};
										bag_terminator.Read(stream);
									}
									else if (subChunk.GetName() == "pgen" || subChunk.GetName() == "igen")
									{
										const size_t numGens(subChunk.GetSize() / PDTA_GENERATOR_SIZE - 1);
										for (size_t i(0); i < numGens; ++i)
										{
											pdta_generator gen{};
											gen.Read(stream);
											
											if (subChunk.GetName() == "pgen")
											{
												outBank.m_presetGens.emplace_back(std::move(gen));
											}
											else
											{
												outBank.m_instrumentGens.emplace_back(std::move(gen));
											}
										}

										pdta_generator gen_terminator{};
										gen_terminator.Read(stream);
									}
									else if (subChunk.GetName() == "pmod" || subChunk.GetName() == "imod")
									{
										const size_t numPMods(subChunk.GetSize() / PDTA_MODULATOR_SIZE - 1);
										for (size_t i(0); i < numPMods; ++i)
										{
											pdta_modulator mod{};
											mod.Read(stream);

											if (subChunk.GetName() == "pmod")
											{
												outBank.m_presetMods.emplace_back(std::move(mod));
											}
											else
											{
												outBank.m_instrumentMods.emplace_back(std::move(mod));
											}
										}

										pdta_modulator mod_terminator{};
										mod_terminator.Read(stream);
									}
									else if (subChunk.GetName() == "inst")
									{
										const size_t numInsts(subChunk.GetSize() / PDTA_INST_SIZE - 1);
										for (size_t i(0); i < numInsts; ++i)
										{
											pdta_inst inst{};
											inst.Read(stream);

											outBank.m_instruments.emplace_back(std::move(inst));
										}

										pdta_inst inst_terminator{};
										inst_terminator.Read(stream);
									}
									else if (subChunk.GetName() == "shdr")
									{
										const size_t numShdrs(subChunk.GetSize() / PDTA_SHDR_SIZE - 1);
										for (size_t i(0); i < numShdrs; ++i)
										{
											pdta_shdr shdr{};
											shdr.Read(stream);

											outBank.m_samples.emplace_back(std::move(shdr));
										}

										pdta_shdr shdr_terminator{};
										shdr_terminator.Read(stream);
									}
									else
									{
										stream.ignore(subChunk.GetSize());
									}
								}
							}
							else
							{
								stream.ignore(chunkList.GetSize());
							}
						}

						return ESF2ReadResult::READ_SUCCESS;
					}
				}
			}
		}
		
		return ESF2ReadResult::FILE_NOT_EXIST;
	}
}

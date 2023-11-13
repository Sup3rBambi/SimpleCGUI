#include "Cartridge.h"

Cartridge::Cartridge(const std::string& sFileName)
{
	struct Header {
		char name[4];
		uint8_t prg_rom_chunks;
		uint8_t chr_rom_chunks;
		uint8_t mapper1;
		uint8_t mapper2;
		uint8_t prg_ram_size;
		uint8_t tv_system1;
		uint8_t tv_system2;
		char unused[5];
	} header;

	imageValid = false;

	std::ifstream ifs;
	ifs.open(sFileName, std::ifstream::binary);
	if (ifs.is_open()) {
		ifs.read((char*)&header, sizeof(Header));
		if (header.mapper1 & 0x04)
			ifs.seekg(512, std::ios_base::cur);
		nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);

		uint8_t nFileType = 1;

		if (nFileType == 0) {
			
		}
		if (nFileType == 1) {
			nPRGBanks = header.prg_rom_chunks;
			vPRGMemory.resize(nPRGBanks * 16384);
			ifs.read((char*)vPRGMemory.data(), vPRGMemory.size());

			nCHRBanks = header.chr_rom_chunks;
			vCHRMemory.resize(nCHRBanks * 8192);
			ifs.read((char*)vCHRMemory.data(), vCHRMemory.size());
		}
		if (nFileType == 2) {

		}

		switch (nMapperID) {
		case 0:
			pMapper = std::make_shared<Mapper_0>(nPRGBanks, nCHRBanks);
			break;

		}

		imageValid = true;

		ifs.close();
	}
}

Cartridge::~Cartridge()
{

}

bool Cartridge::isImageValid()
{
	return imageValid;
}

bool Cartridge::cpuRead(uint16_t addr, uint8_t& data)
{
	uint32_t mapped_addr = 0;
	if (pMapper->cpuMapRead(addr, mapped_addr)) {
		data = vPRGMemory[mapped_addr];
		return true;
	}
	else
		return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;
	if (pMapper->cpuMapWrite(addr, mapped_addr)) {
		vPRGMemory[mapped_addr] = data;
		return true;
	}
	else
		return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t& data)
{
	uint32_t mapped_addr = 0;
	if (pMapper->ppuMapRead(addr, mapped_addr)) {
		data = vCHRMemory[mapped_addr];
		return true;
	}
	else
		return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;
	if (pMapper->ppuMapWrite(addr, mapped_addr)) {
		vCHRMemory[mapped_addr] = data;
		return true;
	}
	else
		return false;
}

void Cartridge::reset()
{
	// Note: This does not reset the ROM contents,
	// but does reset the mapper.
	if (pMapper != nullptr)
		pMapper->reset();
}
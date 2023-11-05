#include "CPU.h"

Memory::Memory() {
	data = new Byte8[MAX_MEM];
	for (unsigned int i = 0; i < MAX_MEM; i++) {
		this->data[i] = 0;
	}
}

Memory::~Memory() {
	delete[] data;
}

CPU::CPU(Memory& memory) {
	accumulator = registerX = registerY = 0;
	memory.data[0x4017] = 0x00;
	memory.data[0x4015] = 0x00;
	for (int i = 0; i < 16; i++) {
		memory.data[0x4000 + i] = 0x00;
	}
	for (int i = 0; i < 4; i++) {
		memory.data[0x4010 + i] = 0x00;
	}
	carryFlag, zeroFlag, interruptDisable, decimalMode, overflowFlag, negativeFlag, bFlag = 0;
	programCounter = 0x4020;
	stackPointer = 0xFE;
}

CPU::~CPU() {

}

Byte8 CPU::FetchByte(Memory& memory) {
	Byte8 value = memory.data[programCounter];
	programCounter++;
	return value;
}

Byte16 CPU::FetchByte16(Memory& memory) {
	Byte16 value = memory.data[programCounter];
	programCounter++;
	value |= (memory.data[programCounter] << 8);
	programCounter++;
	return value;
}

Byte8 CPU::ReadByte(Memory& memory, Byte16 address) {
	Byte8 value = memory.data[address];
	return value;
}

Byte8 CPU::ReadByteFromStack(Memory& memory) {
	stackPointer++;
	Byte8 value = memory.data[stackPointer + 0x0100];
	return value;
}

void CPU::WriteByte(Memory& memory, Byte8 address, Byte8& regist) {
	memory.data[address] = regist;
}

void CPU::WriteByteToStack(Memory& memory, Byte8& address, Byte8& regist) {
	memory.data[address + 0x0100] = regist;
	stackPointer--;
}

void CPU::Execute(Memory& memory) {
	bool isThereInstr = true;
	while (isThereInstr) {
		Byte8 instr = FetchByte(memory);
		switch (instr)
		{

		/* LDA Put memory in accumulator */
			
		case LDA_IM:
		{
			accumulator = FetchByte(memory);
			Accum_FLAGS();
		}break;
		case LDA_ZP:
		{
			Byte8 zeroPageAddr = FetchByte(memory);
			accumulator = ReadByte(memory, zeroPageAddr);
			Accum_FLAGS();
		}break;
		case LDA_ZPX:
		{
			Byte8 zeroPageAddr = FetchByte(memory);
			zeroPageAddr += registerX;
			accumulator = ReadByte(memory, zeroPageAddr);
			Accum_FLAGS();
		}break;
		case LDA_AB:
		{
			Byte16 address = FetchByte16(memory);
			accumulator = ReadByte(memory, address);
			Accum_FLAGS();
		}break;
		case LDA_ABX:
		{
			Byte16 address = FetchByte16(memory);
			address += registerX;
			accumulator = ReadByte(memory, address);
			Accum_FLAGS();
		}break;
		case LDA_ABY:
		{
			Byte16 address = FetchByte16(memory);
			address += registerY;
			accumulator = ReadByte(memory, address);
			Accum_FLAGS();
		}break;

		/* LDX Put memory in register X */

		case LDX_IM:
		{
			registerX = FetchByte(memory);
			LDX_TSX_FLAGS();
		}
		break;
		case LDX_ZP:
		{
			Byte8 zeroPageAddr = FetchByte(memory);
			registerX = ReadByte(memory, zeroPageAddr);
			LDX_TSX_FLAGS();
		}break;
		case LDX_ZPY:
		{
			Byte8 zeroPageAddr = FetchByte(memory);
			zeroPageAddr += registerY;
			registerX = ReadByte(memory, zeroPageAddr);
			LDX_TSX_FLAGS();
		}break;
		case LDX_AB:
		{
			Byte16 address = FetchByte16(memory);
			registerX = ReadByte(memory, address);
			LDX_TSX_FLAGS();
		}break;
		case LDX_ABY:
		{
			Byte16 address = FetchByte16(memory);
			address += registerY;
			registerX = ReadByte(memory, address);
			LDX_TSX_FLAGS();
		}break;

		/* LDY Put memory in register Y */

		case LDY_IM:
		{
			registerY = FetchByte(memory);
			LDY_FLAGS();
		}
		break;
		case LDY_ZP:
		{
			Byte8 zeroPageAddr = FetchByte(memory);
			registerY = ReadByte(memory, zeroPageAddr);
			LDY_FLAGS();
		}break;
		case LDY_ZPX:
		{
			Byte8 zeroPageAddr = FetchByte(memory);
			zeroPageAddr += registerX;
			registerY = ReadByte(memory, zeroPageAddr);
			LDY_FLAGS();
		}break;
		case LDY_AB:
		{
			Byte16 address = FetchByte16(memory);
			registerY = ReadByte(memory, address);
			LDY_FLAGS();
		}break;
		case LDY_ABX:
		{
			Byte16 address = FetchByte16(memory);
			address += registerX;
			registerY = ReadByte(memory, address);
			LDY_FLAGS();
		}break;

		/* STA Store accumulator in memory */

		case STA_ZP:
		{
			Byte8 address = FetchByte(memory);
			WriteByte(memory, address, accumulator);
		}break;
		case STA_ZPX:
		{
			Byte8 address = FetchByte(memory);
			address += registerX;
			WriteByte(memory, address, accumulator);
		}break;
		case STA_AB:
		{
			Byte16 address = FetchByte16(memory);
			WriteByte(memory, address, accumulator);
		}break;
		case STA_ABX:
		{
			Byte8 address = FetchByte16(memory);
			address += registerX;
			WriteByte(memory, address, accumulator);
		}break;
		case STA_ABY:
		{
			Byte8 address = FetchByte16(memory);
			address += registerY;
			WriteByte(memory, address, accumulator);
		}break;

		/* STX Store accumulator in memory */

		case STX_ZP:
		{
			Byte8 address = FetchByte(memory);
			WriteByte(memory, address, registerX);
		}break;
		case STX_ZPY:
		{
			Byte8 address = FetchByte(memory);
			address += registerY;
			WriteByte(memory, address, registerX);
		}break;
		case STX_AB:
		{
			Byte16 address = FetchByte16(memory);
			WriteByte(memory, address, registerX);
		}break;

		/* STY Store accumulator in memory */

		case STY_ZP:
		{
			Byte8 address = FetchByte(memory);
			WriteByte(memory, address, registerY);
		}break;
		case STY_ZPX:
		{
			Byte8 address = FetchByte(memory);
			address += registerX;
			WriteByte(memory, address, registerY);
		}break;
		case STY_AB:
		{
			Byte16 address = FetchByte16(memory);
			WriteByte(memory, address, registerY);
		}break;

		/* TAX Transfer accumulator to register X */

		case TAX:
		{
			registerX = accumulator;
			TAX_FLAGS();
		}break;
		case TAY:
		{
			registerY = accumulator;
			TAY_FLAGS();
		}break;
		case TXA:
		{
			accumulator = registerX;
			TXA_TYA_FLAGS();
		}break; 
		case TYA:
		{
			accumulator = registerY;
			TXA_TYA_FLAGS();
		}break;

		/* TSX Transfer stack pointer to register X */

		case TSX:
		{
			registerX = stackPointer;
			LDX_TSX_FLAGS();
		}break;

		/* TXS Transfer register X to stack pointer */

		case TXS:
		{
			stackPointer = registerX;
		}break;

		/* PHA Push a copy of the accumulator to the stack */

		case PHA:
		{
			WriteByteToStack(memory, stackPointer, accumulator);
		}break;

		/* PHP Push a copy of the status flags to the stack */

		case PHP:
		{
			Byte8 flags = bFlag;
			flags += (negativeFlag * 2);
			flags += (overflowFlag * pow(2, 2));
			flags += (breakCommand * pow(2, 3));
			flags += (decimalMode * pow(2, 4));
			flags += (interruptDisable * pow(2, 5));
			flags += (zeroFlag * pow(2, 6));
			flags += (carryFlag * pow(2, 7));

			WriteByteToStack(memory, stackPointer, flags);
		}break;

		/* PLA Pull a Byte8 from the stack to the accumulator */

		case PLA:
		{
			accumulator = ReadByteFromStack(memory);
			Accum_FLAGS();
		}break;

		/* PLP Pull a Byte8 from the stack to the flags */

		case PLP:
		{
			Byte8 flags = ReadByteFromStack(memory);
			carryFlag |= (flags & 0b10000000) > 0;
			zeroFlag |= (flags & 0b01000000) > 0;
			interruptDisable |= (flags & 0b00100000) > 0;
			decimalMode |= (flags & 0b00010000) > 0;
			breakCommand |= (flags & 0b00001000) > 0;
			overflowFlag |= (flags & 0b00000100) > 0;
			negativeFlag |= (flags & 0b00000010) > 0;
			bFlag |= (flags & 0b00000001) > 0;
		}break;
		default:
			// printf("Error : unknown instruction");
			isThereInstr = false;
			break;
		}
	}
}

void CPU::Accum_FLAGS() {
	zeroFlag = (accumulator == 0);
	negativeFlag = (accumulator & 0b10000000) > 0;
}

void CPU::LDX_TSX_FLAGS() {
	zeroFlag = (registerX == 0);
	negativeFlag = (registerX & 0b10000000) > 0;
}

void CPU::LDY_FLAGS() {
	zeroFlag = (registerY == 0);
	negativeFlag = (registerY & 0b10000000) > 0;
}

void CPU::TAX_FLAGS() {
	zeroFlag = (registerX == 0);
	negativeFlag = (registerX & 0b10000000) > 0;
}

void CPU::TAY_FLAGS() {
	zeroFlag = (registerY == 0);
	negativeFlag = (registerY & 0b10000000) > 0;
}

void CPU::TXA_TYA_FLAGS() {
	zeroFlag = (accumulator == 0);
	negativeFlag = (accumulator & 0b10000000) > 0;
}

#pragma once

class Cpu {

private:
	unsigned char PC = 0;
	unsigned char regs[8];
	unsigned int TC = 0;

	enum CpuState { IDLE, FETCH, DECODE, WAIT, WORKING, HALTED, FINISHED };
	enum InstructionType { ADD, ADD_IMMEDIATE, MULTIPLY, INVERT, BRANCH, LOAD_WORD, STORE_WORD, HALT };
	CpuState state = IDLE;

	//used to determine when an instruction has finished
	unsigned int ticksRemaining = 0;

	//used by clock to determine whether another cycle is needed
	bool workToDo;

	//values to hold the instruction data
	unsigned int instruction;
	unsigned int instructionType;
	unsigned int instructionDestinationRegister;
	unsigned int instructionSourceRegister;
	unsigned int instructionTargetRegister;
	unsigned int instructionImmediateValue;

	//Referenced while decoding instructions to determine ticks required per given instruction
	unsigned int instructionTicksTable[8] = {
		1, //Add instruction
		1, //Add immediate instruction
		2, //Multiply instruction
		1, //Invert instruction
		1, //Branch not taken
		1, //Load word
		1, //Store word
		1  //Halt
	};

public:

	//Reset the CPU
	int reset();

	//Used to set CPU register by number
	int setReg(int register, unsigned char byte);

	//Might never actually be used?
	int setRegByPointer(unsigned char* registerPointer, unsigned char byte);

	
	//sets PC
	int setPC(unsigned char byte);
	
	//Dumps data cpu internals
	int dump();



	//Getters
	unsigned char* getPC();
	unsigned char* getRegs();



	//Called by clock to do pre-tick activities
	int startTick();

	//Called by clock to execute cycle work
	int doCycleWork();

	//Called by clock to determine if another cycle is needed
	bool isMoreCycleWorkNeeded();

};

Cpu& getCpu();


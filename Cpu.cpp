#include "cs3421_emul.h"

//Reset the cpu registers
int Cpu::reset()
{
	for (int i = 0; i < 8; i++)
	{
		regs[i] = 0;
	}

	PC = 0;
	TC = 0;

	getMemory();
	state = IDLE;

	return 0;
}


//Set a specific cpu register
int
Cpu::setReg(int reg, unsigned char byte)
{
	regs[reg] = byte;

	return 0;
}

//Set a specific cpu register via pointer
int Cpu::setRegByPointer(unsigned char* registerPointer, unsigned char byte) {

	*registerPointer = byte;

	return 0;
}



//set the pc register
int
Cpu::setPC(unsigned char byte)
{
	PC = byte;

	return 0;
}

//dump the cpu registers
int
Cpu::dump()
{

	printf("PC: 0x%02X\n", PC);

	for (int i = 0; i < 8; i++)
	{
		printf("R%c: 0x%02X\n", i + 65, regs[i]);
	}

	printf("TC: %d\n", TC);

	printf("\n");
	return 0;
}



//getter function for the PC
unsigned char*
Cpu::getPC()
{
	return &PC;
}



//getter function for the regs
unsigned char*
Cpu::getRegs()
{
	return regs;
}



//do pre-tick activities
int
Cpu::startTick()
{

	switch (state)
	{
		//If we're idling, fetch the next instruction
	case IDLE:
		state = FETCH;
		workToDo = true;
		break;

	case FETCH:
		break;

	case DECODE:
		break;

	case FINISHED:
		break;

	case WAIT:
		break;

	case HALTED:
		workToDo = false;
		break;

	case WORKING:
		workToDo = true;
		break;

	default:
		break;

	}

	if (state != HALTED) { 
		TC++; 
		//printf("CPU Tick: %d\n", TC);  
		//getCache().cacheDump(); 
	}

	return 0;
}



//called repeatedly until no more work to do
int
Cpu::doCycleWork()
{
	switch (state)
	{

	case FETCH:
		//Fetch from instruction memory, NULL memDone because it finishes in 1 tick
		getIMemory().startFetch(PC, 1, &instruction, NULL);
		state = DECODE;
		break;

	case DECODE:

		//Masking out the instruction
		instructionType = instruction >> 17;
		instructionDestinationRegister = (instruction >> 14) & 0b111;
		instructionSourceRegister = (instruction >> 11) & 0b111;
		instructionTargetRegister = (instruction >> 8) & 0b111;
		instructionImmediateValue = instruction & 0xFF;

		ticksRemaining = instructionTicksTable[instructionType];

		//If the branch will actually be taken, we have to spend more ticks taking it
		if (instructionType == BRANCH) {

			switch (instructionDestinationRegister) {
			case 0:
				if (instructionSourceRegister == instructionTargetRegister)
					ticksRemaining = 2;
				break;

			case 1:
				if (instructionSourceRegister != instructionTargetRegister)
					ticksRemaining = 2;
				break;

			case 2:
				if (instructionSourceRegister < instructionTargetRegister)
					ticksRemaining = 2;
				break;
			}

		}

		state = WORKING;

		break;

	case WAIT:

		if (workToDo)
		{			//Once the value has been copied
			state = IDLE;
			workToDo = false;
		};

		break;

	case IDLE:			//If end up idling mid-tick, do nothing until next tick
		workToDo = false;
		break;

	case WORKING:

		if (workToDo) {
			ticksRemaining--;
		}

		if (ticksRemaining <= 0) {
			workToDo = true;
			state = FINISHED;
		}
		else {
			workToDo = false;
		}

		break;

	case FINISHED:

		switch (instructionType)
		{


			//Add instruction PROTOTYPE
		case ADD:
			regs[instructionDestinationRegister] =
				regs[instructionTargetRegister] +
				regs[instructionSourceRegister];
			state = IDLE;

			workToDo = false;

			break;

			//Add immediate instruction PROTOTYPE
		case ADD_IMMEDIATE:
			regs[instructionDestinationRegister] =
				regs[instructionSourceRegister] + instructionImmediateValue;
			state = IDLE;

			workToDo = false;

			break;

			//Multiply instruction PROTOTYPE
		case MULTIPLY:

			regs[instructionDestinationRegister] =

				(unsigned char)((regs[instructionSourceRegister] >> 4) & 0xF) * //mask top 4
				(unsigned char)((regs[instructionSourceRegister]) & 0xF); //mask bottom 4

			state = IDLE;

			workToDo = false;

			break;

			//Invert instruction PROTOTYPE
		case INVERT:
			regs[instructionDestinationRegister] =
				~regs[instructionSourceRegister];
			state = IDLE;

			workToDo = false;

			break;

			//BEQ instructions NEED TO IMPLEMENT THREE
		case BRANCH:
			switch (instructionDestinationRegister)
			{

				//BEQ
			case 0b000:
				if (regs[instructionSourceRegister] == regs[instructionTargetRegister]) {
					PC = instructionImmediateValue - 1; //Decrease by one, since the PC is always incremented
				}
				state = IDLE;
				break;

				//BNEQ
			case 0b001:
				if (regs[instructionSourceRegister] != regs[instructionTargetRegister]) {
					PC = instructionImmediateValue - 1; //Decrease by one, since the PC is always incremented
				}
				state = IDLE;
				break;

				//BLT
			case 0b010:
				if ((signed char)regs[instructionSourceRegister] < (signed char)regs[instructionTargetRegister]) {
					PC = instructionImmediateValue - 1; //Decrease by one, since the PC is always incremented
				}
				state = IDLE;
				break;

				//Should never be reached
			default:
				state = IDLE;
				break;

			}

			workToDo = false;

			break;

			//Load word instruction, sets workToDo to false
		case LOAD_WORD:
			getCache().startCacheFetch(regs[instructionTargetRegister], 1,
				&regs[instructionDestinationRegister],
				&workToDo);
			state = WAIT;
			break;

			//Save word instruction, sets workToDo to false
		case STORE_WORD:
			getCache().startCacheStore(regs[instructionTargetRegister], 1,
				&regs[instructionSourceRegister],
				&workToDo);
			state = WAIT;
			break;

			//Halt instruction
		case HALT:
			state = HALTED;
			workToDo = false;
			break;
		}

		PC++;

		break;

	case HALTED:
		//No longer responding to ticks
		workToDo = false;
		break;

	default:
		printf("This should never happen, dropped to bottom of CPU switch\n");
		state = IDLE;
		break;

	}

	return 0;
}


bool
Cpu::isMoreCycleWorkNeeded()
{
	return workToDo;
}



//getter function for the cpu
Cpu& getCpu()
{
	static Cpu cpu;
	return cpu;
};

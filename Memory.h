#pragma once
#include "cs3421_emul.h"

class Memory {

private:
	enum MemState { MEM_IDLE, MEM_WORKING, MEM_MOVE, MEM_CACHE_FLUSH };
	enum TransferType { MEM_FETCH, MEM_STORE };

	//Used to determine what to do during a cycle
	MemState state;

	//Used to track whether memory is sending or receiving bytes
	TransferType transferType;

	//The ticks required for a transfer to finish
	int requiredTicks = 5;

	//The current tick that memory is on
	int tick = 0;

	//The array to hold memory
	unsigned char* memory;

	//The size of memory in bytes
	unsigned int size;

	//Used by clock to determine whether next tick can execute
	bool workToDo;



	//Tracks the address that bytes should be sent/received from (index in memory array)
	unsigned int currentAddress;

	//Tracks how many bytes to transfer
	unsigned int currentCount;

	//Tracks where data is either coming from or going to
	unsigned char* currentForeignPointer;

	//Used to notify cache when load or store is completed
	bool* currentMemoryDonePointer;

	//Used by the pause function to track how long the cache flush takes
	int currentTickRequirement;





public:

	//Initialize memory
	int create(int memSize);

	//Set a specific byte in memory
	int set(int address, unsigned char byte);

	//Set all allocated memory to 0
	int reset();

	//Prints memory starting at the given address
	int dump(int startAddress, int count);
	
	//Used to free initialized memory
	int cleanup();



	//Called by the clock
	int startTick();

	//Called by the clock 
	int doCycleWork();

	//Used by the clock to determine when next tick can execute
	bool isMoreCycleWorkNeeded();



	//Called by cache, or indirectly by CPU to load memory
	int startFetch(unsigned int address, unsigned int count, unsigned char* dataPtr, bool* memDonePtr);

	//Called by cache, or indirectly by CPU to save to memory
	int startStore(unsigned int address, unsigned int count, unsigned char* dataPtr, bool* memDonePtr);

	bool usedByIO = false;



	//Called by the cache to perform a partial block transfer
	int acceptCacheFlush(int ticks);
};

Memory& getMemory();

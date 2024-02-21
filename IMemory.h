class IMemory {

private:

	//array to hold imemory data
	unsigned int* memory;

	//length of array
	unsigned int size;

public:

	//used to allocate memory
	int create(int memSize);

	//used to set specific value in imemory
	int set(int address, unsigned int value);

	int reset();

	//dumps internals
	int dump(int startAddress, int count);

	//called to deallocate malloc'd memory
	int cleanup();
	
	//called by CPU to request an instruction
	int startFetch(unsigned int address, unsigned int count, unsigned int* dataPtr, bool* memDonePtr);



	//called by clock to do pre-tick activities
	int startTick();

	//called by clock if more cycles are needed for imemory
	int doCycleWork();

	//called by clock to determine whether more cycles are needed
	bool isMoreCycleWorkNeeded();

};
IMemory& getIMemory();

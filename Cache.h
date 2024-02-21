#pragma once
#include "cs3421_emul.h"

#define CACHE_SIZE 8
#define FLUSH_TIME 5

class Cache {

private:
	//Used to determine whether cache functionality should be active
	bool cacheEnabled = false;

	
	//variables to track the information passed from function calls
	unsigned char currentAddress;
	unsigned char currentCacheAddress;
	bool* currentCPUMemDonePointer;
	unsigned char* currentCPUDataPointer;


	//cache line- tracks current block
	unsigned char CLO;



	//used for determining if cache needs to flush
	bool validDataInCache = false;

	//tracks whether any written data exists in the cache
	bool writtenDataInCache = false;

	//tracks whether a given flush was caused by a write, since it requires some post-op cleanup
	bool flushCausedByWrite = false;



	//Array holds flags determining which bytes of cache memory have been written
	//0 is valid, 1 is invalid, 2 is written
	int dataFlags[CACHE_SIZE];

	//Array holds the data of the cache
	unsigned char cacheData[CACHE_SIZE];



public:
	//Computes what the CLO should be for given data address
	int computeCLO(int address);

	//Computes what the address should be relative to our current Cache Line
	int computeRelativeAddress(int address);

	//Called by the CPU in attempt to read from the cache
	int startCacheFetch(unsigned int address, unsigned int count, unsigned char* dataPtr, bool* memDonePtr);

	//Called by Memory to finish the fetch to Cache
	int finishCacheFetch();

	//Called by the CPU in attempt to write to the cache
	int startCacheStore(unsigned int address, unsigned int count, unsigned char* dataPtr, bool* memDonePtr);



	//Mark all data in the cache as invalid
	int markInvalid();

	//Start the transaction of writing written cache memory to data memory
	int startFlush();

	//Called by memory when memory is ready to accept the written data
	int finishFlush();



	//Reset the cache
	int cacheReset();

	//Enable the cache
	int cacheOn();

	//Disable the cache
	int cacheOff();

	//Dump the contents of the cache
	int cacheDump();
};

//Getter function for the cache
Cache& getCache();


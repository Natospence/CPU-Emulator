#include "Cache.h"

//This method simply calculates what the CLO for a given address would be
int Cache::computeCLO(int address) {
	return address/CACHE_SIZE;
}

//This method calculates the address of a byte of memory relative to it's CLO
int Cache::computeRelativeAddress(int address) {
	return address % CACHE_SIZE;
}

//This method is called by the CPU to attempt to grab memory from the cache, if not, then from data memory
int Cache::startCacheFetch(unsigned int address, unsigned int count, unsigned char* dataPtr, bool* memDonePtr) {

	//Grab the transaction data, count is unimportant since it will always be 1
	currentAddress = address;
	currentCPUDataPointer = dataPtr;
	currentCPUMemDonePointer = memDonePtr;

	currentCacheAddress = computeRelativeAddress(address);

	*currentCPUMemDonePointer = false; //IS THERE ANY REASON WE SHOULDN'T DO THIS HERE?

	//If the CPU loads from 0xFF, we return a value of 0 and mark data as invalid
	if (cacheEnabled && currentAddress == 0xFF) {
		*currentCPUDataPointer = 0; //Return a value of 0
		markInvalid(); //Sets data as invalid and erases written flags
		*currentCPUMemDonePointer = true; //Notify the CPU
		return 0;
	}



	if (cacheEnabled) {

		//Check to see if we have a cache hit
		if (computeCLO(currentAddress) == CLO && validDataInCache) {
			*currentCPUDataPointer = cacheData[currentCacheAddress]; //Update the data to the requested data
			*currentCPUMemDonePointer = true; //Notify the CPU
		}
		else { //Otherwise, just load the new block to cache
			CLO = computeCLO(currentAddress);
			getMemory().startFetch(CLO * CACHE_SIZE, CACHE_SIZE, cacheData, NULL); //memDonePtr doesn't matter, we update it manually
		}

	}
	else { //If we're not running the cache, this should behave exactly as if the cache isn't involved.
		getMemory().startFetch(address, count, dataPtr, memDonePtr);
	}
	return 0;
}

//Called by Memory to finish the fetch to Cache
int Cache::finishCacheFetch() {

	if (cacheEnabled) {

		//We now have valid data in the cache
		validDataInCache = true;

		for (int i = 0; i < CACHE_SIZE; i++) {
			dataFlags[i] = 0;
		}

		//Update the appropriate CPU register to the requested value from data memory
		*currentCPUDataPointer = cacheData[currentCacheAddress];

	}

	//Notify the CPU
	*currentCPUMemDonePointer = true;

	return 0;
}

int Cache::startCacheStore(unsigned int address, unsigned int count, unsigned char* dataPtr, bool* memDonePtr) {

	//Grab the transaction data, count is unimportant since it will always be 1
	currentAddress = address;
	currentCPUDataPointer = dataPtr;
	currentCPUMemDonePointer = memDonePtr;

	currentCacheAddress = computeRelativeAddress(address);

	*currentCPUMemDonePointer = false; //IS THERE ANY REASON WE SHOULDN'T DO THIS HERE?

	//If the CPU saves to 0xFF, we flush the cache
	if (cacheEnabled && currentAddress == 0xFF) {
		startFlush(); //This will notify the CPU eventually
		return 0;
	}



	if (cacheEnabled) {

		//Check to see if we have a cache hit, or there is no written data
		if (computeCLO(currentAddress) == CLO || !writtenDataInCache) {

			//Make sure the CLO is correct in case we're writing on garbage data
			if (CLO != computeCLO(currentAddress)) {
				markInvalid();
			}
			CLO = computeCLO(currentAddress);

			cacheData[currentCacheAddress] = *dataPtr; //Update the data to the sent data
			dataFlags[currentCacheAddress] = 2; //Set the written flag
			writtenDataInCache = true;

			*currentCPUMemDonePointer = true; //Notify the CPU
		}
		else { //Otherwise, we need to dump the old written data
			flushCausedByWrite = true; //This is needed to save the byte the CPU was actually trying to save after the flush
			startFlush();	 
		}

	}
	else { //If we're not running the cache, this should behave exactly as if the cache isn't involved.
		getMemory().startStore(address, count, dataPtr, memDonePtr);
	}


	return 0;
}

//Marks cache data as invalid
int Cache::markInvalid() {

	validDataInCache = false;

	for (int i = 0; i < CACHE_SIZE; i++) {
		dataFlags[i] = 1;
	}
	writtenDataInCache = false;

	return 0;
}

//Start the transaction of writing written cache memory to data memory
int Cache::startFlush() {

	//Tell Memory to count for the flush
	getMemory().acceptCacheFlush(FLUSH_TIME);

	return 0;

}

//Called by memory when memory is ready to accept the written data
int Cache::finishFlush() {

	for (int i = 0; i < CACHE_SIZE; i++) { //For every byte that could've been written
		if (dataFlags[i] == 2) { //If it was written
			getMemory().set(CLO * CACHE_SIZE + i, cacheData[i]);//Copy it
			dataFlags[i] = 0; //Make it valid
		}
	}
	writtenDataInCache = false;

	//If the flush was caused by a write to cache, we still need to write the new byte
	if (flushCausedByWrite) {
		flushCausedByWrite = false;

		cacheData[currentCacheAddress] = *currentCPUDataPointer;

		dataFlags[currentCacheAddress] = 2;
		writtenDataInCache = true;

		CLO = computeCLO(currentAddress);

	}

	*currentCPUMemDonePointer = true; //Notify the CPU

	return 0;

}

//Reset the cache
int Cache::cacheReset() {
	CLO = 0;
	markInvalid();
	cacheOff();
	return 0;
}

//Enable the cache
int Cache::cacheOn() {
	cacheEnabled = true;
	return 0;
}

//Disable the cache
int Cache::cacheOff() {
	cacheEnabled = false;
	return 0;
}

//Print the contents of the cache
int Cache::cacheDump() {

	printf("CLO        : ");
	printf("0x%02X\n", CLO);

	printf("cache data : ");
	for (int i = 0; i < CACHE_SIZE; i++) {
		printf("0x%02X ", cacheData[i]);
	}
	printf("\n");

	printf("Flags      :   ");
	for (int i = 0; i < CACHE_SIZE; i++) {

		if (dataFlags[i] == 2) { //If the data in the cache has been written, write 'W'
			printf("W    ");
		}
		else if (dataFlags[i] == 0) { //If there's valid data in the cache, write 'V'
			printf("V    ");
		}
		else if (dataFlags[i] == 1){ //If there's invalid data in the cache, write 'I'
			printf("I    ");
		}

	}

	printf("\n\n");

	return 0;
}

//getter function for the cache
Cache& getCache()
{
	static Cache cache;
	return cache;
};
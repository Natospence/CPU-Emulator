#include "cs3421_emul.h"

int Clock::getTick() {
	return count;
}

int Clock::reset() {
	count = 0;
	return 0;
}

int Clock::tick(int increment) {

	//tick as many times as required
	for (int i = 0; i < increment; i++) {

		//begin a new tick
		getMemory().startTick();
		getCpu().startTick();
		getIODevice().startTick();

		bool workToDo = true;

		//while each component still has work to do	
		while (workToDo) {

			getCpu().doCycleWork();
			getMemory().doCycleWork();

			workToDo =
				getCpu().isMoreCycleWorkNeeded() ||
				getMemory().isMoreCycleWorkNeeded();


		}
		count++;
	}

	return 0;

}

int Clock::dump() {
	printf("Clock: %d\n\n", count);
	return 0;
}

Clock& getClock() {
	static Clock clock;
	return clock;
};

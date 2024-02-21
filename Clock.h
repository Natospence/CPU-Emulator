#pragma once

class Clock {

private:
	unsigned int count = 0;

public:

	//returns the current tick of the clock
	int getTick();

	//resets the ticks to 0
	int reset();

	//causes the clock to tick by a certain amount
	int tick(int increment);

	//dumps ticks
	int dump();
};

//Factory design pattern getter
Clock& getClock();

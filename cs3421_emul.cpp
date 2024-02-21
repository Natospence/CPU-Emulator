#include "cs3421_emul.h"
using namespace std;
int
main(int argc, char* argv[])
{

	//If a file wasn't supplied, exit
	if (argc < 2)
	{
		printf("Not enough arguments supplied\n");
		return 1;
	}

	//open the input stream
	ifstream infile(argv[1]);

	//If unable to open, complain
	if (!infile.is_open())
	{
		cerr << "error opening input file " << argv[1] << endl;
		return 1;
	}

	//string to store read-in string
	string current;

	//int to store read-in int
	unsigned int currentInt;

	//while we haven't reached the end of the file
	while (infile >> current)
	{

		//Else-if chain for parsing initial command

		//If a clock command
		if (current == "clock")
		{

			//grab next command
			infile >> current;

			//reset
			if (current == "reset")
			{
				getClock().reset();
			}

			//tick
			else if (current == "tick")
			{
				infile >> dec >> currentInt;
				getClock().tick(currentInt);
			}

			//dump
			else if (current == "dump")
			{
				getClock().dump();
			}
		}

		//if a cpu command
		else if (current == "cpu")
		{
			infile >> current;

			//reset
			if (current == "reset")
			{
				getCpu().reset();
			}

			//set
			else if (current == "set")
			{

				//Burn the "reg" part of the command
				infile >> current;

				//Grab the register name
				infile >> current;

				//since we're grabbing hexadecimal values with an attached '0x' and a space before it, we burn three characters
				if (current == "PC")
				{
					infile.get();
					infile.get();
					infile.get();
					infile >> hex >> currentInt;
					getCpu().setPC(currentInt);
				}

				else if (current == "RA")
				{
					infile.get();
					infile.get();
					infile.get();
					infile >> hex >> currentInt;
					getCpu().setReg(0, currentInt);
				}

				else if (current == "RB")
				{
					infile.get();
					infile.get();
					infile.get();
					infile >> hex >> currentInt;
					getCpu().setReg(1, currentInt);
				}

				else if (current == "RC")
				{
					infile.get();
					infile.get();
					infile.get();
					infile >> hex >> currentInt;
					getCpu().setReg(2, currentInt);
				}

				else if (current == "RD")
				{
					infile.get();
					infile.get();
					infile.get();
					infile >> hex >> currentInt;
					getCpu().setReg(3, currentInt);
				}

				else if (current == "RE")
				{
					infile.get();
					infile.get();
					infile.get();
					infile >> hex >> currentInt;
					getCpu().setReg(4, currentInt);
				}

				else if (current == "RF")
				{
					infile.get();
					infile.get();
					infile.get();
					infile >> hex >> currentInt;
					getCpu().setReg(5, currentInt);
				}

				else if (current == "RG")
				{
					infile.get();
					infile.get();
					infile.get();
					infile >> hex >> currentInt;
					getCpu().setReg(6, currentInt);
				}

				else if (current == "RH")
				{
					infile.get();
					infile.get();
					infile.get();
					infile >> hex >> currentInt;
					getCpu().setReg(7, currentInt);
				}
			}

			//dump
			else if (current == "dump")
			{
				getCpu().dump();
			}
		}

		//if a memory command
		else if (current == "memory")
		{
			infile >> current;

			//memory create, again, burning 3 characters because of hex value.
			if (current == "create")
			{
				infile.get();
				infile.get();
				infile.get();
				infile >> hex >> currentInt;
				getMemory().create(currentInt);
			}

			//memory reset
			else if (current == "reset")
			{
				getMemory().reset();
			}

			//memory dump
			else if (current == "dump")
			{
				infile.get();
				infile.get();
				infile.get();
				infile >> hex >> currentInt;
				int a = currentInt;
				infile.get();
				infile.get();
				infile.get();
				infile >> hex >> currentInt;
				int b = currentInt;
				getMemory().dump(a, b);
			}

			//memory set
			else if (current == "set")
			{

				//grab both hex values
				infile.get();
				infile.get();
				infile.get();
				infile >> hex >> currentInt;
				int start = currentInt;
				infile.get();
				infile.get();
				infile.get();
				infile >> hex >> currentInt;
				int count = currentInt;

				//grab each of the specified hex values and throw them in memory
				for (int i = 0; i < count; i++)
				{
					infile.get();
					infile.get();
					infile.get();
					infile >> hex >> currentInt;
					getMemory().set(start + i, currentInt);
				}
			}

		}
		else if (current == "imemory")
		{
			infile >> current;

			//memory create, again, burning 3 characters because of hex value.
			if (current == "create")
			{
				infile.get();
				infile.get();
				infile.get();
				infile >> hex >> currentInt;
				getIMemory().create(currentInt);
			}

			//memory reset
			else if (current == "reset")
			{
				getIMemory().reset();
			}

			//memory dump
			else if (current == "dump")
			{
				infile.get();
				infile.get();
				infile.get();
				infile >> hex >> currentInt;
				int a = currentInt;
				infile.get();
				infile.get();
				infile.get();
				infile >> hex >> currentInt;
				int b = currentInt;
				getIMemory().dump(a, b);
			}

			//memory set
			else if (current == "set")
			{

				//grab the start location value
				infile.get();
				infile.get();
				infile.get();
				infile >> hex >> currentInt;
				int start = currentInt;

				//Determine whether we're reading a file or inline values
				bool readingFile = false;
				infile >> current;
				if (current == "file")
				{
					readingFile = true;
					infile >> current;

					//open the new input stream
					ifstream infile2(current);
					if (!infile2.is_open())
					{
						cerr << "error opening input file " << current << endl;
						return 1;
					}
					int i = 0;
					while (infile2 >> hex >> currentInt)
					{
						getIMemory().set(start + i, currentInt);
						i++;
					}
					infile2.close();
				}
				if (!readingFile)
				{

					//grab the instruction count
					infile.get();
					infile.get();
					infile.get();
					infile >> hex >> currentInt;
					int count = currentInt;

					//grab each of the specified hex values and throw them in memory
					for (int i = 0; i < count; i++)
					{
						infile.get();
						infile.get();
						infile.get();
						infile >> hex >> currentInt;
						getIMemory().set(start + i, currentInt);
					}
				}
			}
		}
		else if (current == "cache") {

		infile >> current;

		if (current == "reset") {
			getCache().cacheReset();
		}
		else if (current == "on") {
			getCache().cacheOn();
		}
		else if (current == "off") {
			getCache().cacheOff();
		}
		else if (current == "dump") {
			getCache().cacheDump();
		}

		}
		else if (current == "iodev") {

		infile >> current;

		if (current == "reset") {
			getIODevice().reset();
		}
		else if (current == "load") {

			infile >> current;

			//open the new input stream
			ifstream infile2(current);
			if (!infile2.is_open())
			{
				cerr << "error opening input file " << current << endl;
				return 1;
			}
			while (infile2 >> dec >> currentInt)
			{

				unsigned char tick = currentInt;
				bool write = false;
				unsigned char address = 0;
				unsigned char data = 0;

				infile2 >> current;

				if (current == "write") {
					write = true;

					infile2.get();
					infile2.get();
					infile2.get();
					infile2 >> hex >> currentInt;
					address = currentInt;

					infile2.get();
					infile2.get();
					infile2.get();
					infile2 >> hex >> currentInt;
					data = currentInt;

				}
				else if (current == "read") {
					write = false;

					infile2.get();
					infile2.get();
					infile2.get();
					infile2 >> hex >> currentInt;
					address = currentInt;

				}
				else {
					printf("This shouldn't happen!! Parsing error from IODev file");
				}

				getIODevice().loadInstruction(tick, write, address, data);

			}
			infile2.close();


		}
		else if (current == "dump") {
			getIODevice().dump();
		}
		}


		}

	getMemory().cleanup();
	getIMemory().cleanup();
	return 0;
}

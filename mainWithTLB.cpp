/*
In the main, the first input file is used to create the segment tables 
and page tables in the physical memory. The second input file contains
the virtual addresses to be translated. If there is a 0 before the virtual 
address, the physical address is read from the physical memory. If there
is a 1 before the virtual address, the physical address is written 
to the physical memory.
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include "MemoryWithTLB.h"
using namespace std;

int main()
{
	ifstream infile("input1.txt");

	string line;

	stringstream ss;

	int p, s, f;

	getline(infile, line);
	ss << line;

	MemoryWithTLB m;

	while (ss >> s >> f)
		m.setST(s, f);

	ss.clear();
	getline(infile, line);
	ss << line;

	while (ss >> p >> s >> f)
		m.setPT(p, s, f);


	infile.close();
	ifstream infile1("input2.txt");
	int rW, VA;

	ss.clear();
	getline(infile1, line);
	ss << line;

	while (ss >> rW >> VA)
	{
		if (rW == 0)
			m.read(VA);

		else
			m.write(VA);
	}

	return 0;
}
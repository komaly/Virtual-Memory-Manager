/*
The MemoryWithTLB class creates and implements a 
virtual memory system using segmentation and paging.
It's primary function is to accept virtual addresses and
translate them into physical addresses. It also utilizes a 
Translation Look-Aside Buffer (TLB) to increase the
efficiency of the translation process.
*/

#ifndef MEMORYWITHTLB_H
#define MEMORYWITHTLB_H

#include <iostream>
using namespace std;

//The structure of an entry in the TLB
struct TLBEntry
{
	int LRU = -1;
	int sp = -1;
	int f = -1;
};

class MemoryWithTLB
{
	private:
		int* PM;
		int bitmap[32];
		TLBEntry TLB[4];

		int segmentNumber;
		int pageNumber;
		int offSet;

		int getSegmentNumber(int VA);
		int getPageNumber(int VA);
		int getOffset(int VA);
		int getSP(int VA);
		void initializeFramesToZeros(int frameLen, int PMIndex);
		int updateBitMapForPage();
		int allocatePage();
		int updateBitMapForPT();
		int allocatePT();
		void updateTLB(int sp, int w);
		int getTLBEntry(int sp);
		void updateLRU(int entry, int w);
		void setBitMapForST(int f);
		void setBitMapForPT(int f);
		void printTLB();
		void printBitMap();

	public:
		MemoryWithTLB();
		~MemoryWithTLB();
		void setST(int s, int f);
		void setPT(int s, int p, int f);
		int read(int VA);
		int write(int VA);
};

#endif

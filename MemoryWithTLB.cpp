#include <iostream>
#include <stdio.h>
#include <string.h>
#include <iomanip>
#include "MemoryWithTLB.h"
using namespace std;

//The physical memory and bitmap is initialized 
//in the constructor
MemoryWithTLB::MemoryWithTLB()
{
	PM = new int[524288];

	for (int i = 0; i < 524288; i++)
	{
		PM[i] = 0;
	}

	memset(bitmap, 0 , 128);

	bitmap[0] = (1 << 31);
}

//The physical memory is deleted in the destructor
MemoryWithTLB::~MemoryWithTLB()
{	
	delete[] PM;
}

//Retrieves the segment number from the virtual address
int MemoryWithTLB::getSegmentNumber(int VA)
{
	int mask = 0b00001111111110000000000000000000;
	return (VA & mask) >> 19;
}

//Retrieves the page number from the virtual address
int MemoryWithTLB::getPageNumber(int VA)
{
	int mask = 0b00000000000001111111111000000000;
	return (VA & mask) >> 9;
}

//Retrieves the offset from the virtual address
int MemoryWithTLB::getOffset(int VA)
{
	int mask = 0b00000000000000000000000111111111;
	return VA & mask;
}

//Retrieves both the segment number and page number from
//the virtual address.
int MemoryWithTLB::getSP(int VA)
{
	int mask = 0b00001111111111111111111000000000;
	return (VA & mask) >> 9;
}

//Initializes a frame in the physical memory with zeros
void MemoryWithTLB::initializeFramesToZeros(int frameLen, int PMIndex)
{
	if (frameLen + PMIndex >= 524288)
	{
		printf("%s", "Internal error: initializeFramesToZeros ");
		return;
	}

	for (int i = PMIndex; i < PMIndex + frameLen; i++)
		PM[i] = 0;
}

//Updates the bitmap when a new page is created
//to indicate that the given frame is no longer available
int MemoryWithTLB::updateBitMapForPage()
{
	for (int i = 0; i < 32; i++)
	{
		for (int j = 31; j >= 0; j--)
		{
			if ((bitmap[i] & (1 << j)) == 0)
			{
				bitmap[i] |= (1 << j);
				return (i * 32) + (31 - j);
			}
		}
	}
	return -1;
}

//Creates an empty page within the physical memory
int MemoryWithTLB::allocatePage()
{
	int frameIndex = updateBitMapForPage();

	if (frameIndex == -1)
		return -1;
	
	int PMIndex = frameIndex * 512;
	initializeFramesToZeros(512, PMIndex);
	return PMIndex;
}

//Updates the bitmap when a new page table is created
//to indicate that the given frame is no longer available
int MemoryWithTLB::updateBitMapForPT()
{
	for (int i = 0; i < 32; i++)
	{
		for (int j = 31; j > 0; j--)
		{
			if ((bitmap[i] & (1 << j)) == 0 && (bitmap[i] & (1 << (j - 1))) == 0) //if bit and bit next to it == 0
			{
				bitmap[i] |= (1 << j);
				bitmap[i] |= (1 << (j - 1));
				return (i * 32) + (31 - j);
			}
		}

		if ((bitmap[i] & 1) == 0 && (bitmap[i + 1] & (1 << 31)) == 0)
		{
			bitmap[i] |= 1;
			bitmap[i + 1] |= (1 << 31);
			return (i * 32) + 31;
		}
	}

	return -1;
}

//Creates an empty page table in the physical memory
int MemoryWithTLB::allocatePT()
{
	int frameIndex = updateBitMapForPT();

	if (frameIndex == -1)
		return -1;

	int PMIndex = frameIndex * 512;
	initializeFramesToZeros(1024, PMIndex);
	return PMIndex;
}

//Creates a new entry in the TLB
//with the given segment and page table numbers
//and the starting frame address, and updates the
//LRU's of all other TLB entries
void MemoryWithTLB::updateTLB(int sp, int f)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		if (TLB[i].sp != -1)
		{
			if (TLB[i].LRU == 0)
			{
				TLB[i].LRU = 3;
				TLB[i].sp = sp;
				TLB[i].f = f;
			}
			else
				TLB[i].LRU--;
		}
		else
		{
			TLB[i].LRU = 3;
			TLB[i].sp = sp;
			TLB[i].f = f;
			return;
		}		
		
	}
}

//Retrieves the TLB entry if the given segment 
//and page table number is in the TLB 
int MemoryWithTLB::getTLBEntry(int sp)
{
	for (int i = 0; i < 4; i++)
		if (TLB[i].sp == sp) //TLB hit
			return i;

	return -1;
}

//Updates the LRU (the number indicating the least recently 
//used address) of all addresses currently in the TLB
void MemoryWithTLB::updateLRU(int entry, int w)
{
	for (int j = 0; j < 4; j++)
	{
		if (TLB[j].sp != -1)
		{
			if (TLB[j].LRU > TLB[entry].LRU)
				TLB[j].LRU--;
		}
	}

	TLB[entry].LRU = 3;

}

//Updates the bitmap for the frame that the 
//given segment table is in
void MemoryWithTLB::setBitMapForST(int f)
{
	int frame = f/512;
	int intIndex = frame/32;
	int bitIndex = frame % 32;

	bitmap[intIndex] |= (1 << (31 - (bitIndex)));

	if (bitIndex == 31)
	{
		bitmap[intIndex + 1] |= (1 << 31);
	}

	else
		bitmap[intIndex] |= (1 << (31 - bitIndex - 1));

}

//Updates the bitmap for the frame that the 
//given page table is in
void MemoryWithTLB::setBitMapForPT(int f)
{
	int frame = f/512;
	bitmap[frame/32] |= (1 << (31 - (frame % 32)));
}

//Displays all entries currently in the
//Translation Look Aside Buffer (TLB)
void MemoryWithTLB::printTLB()
{
	for (int i = 0; i < 4; i++)
	{
		cout << endl;
		cout << "New entry: " << endl;
		cout << TLB[i].LRU << endl;
		cout << TLB[i].sp << endl;
		cout << TLB[i].f << endl;
	}
}

//Displays the contents of the bitmap, which shows
//which frames in the physical memory are free and
//which are not
void MemoryWithTLB::printBitMap()
{
	for (int i = 0; i < 32; i++)
		cout << "bit " << hex << bitmap[i] << endl;
}

//The segment table is initialized with the given information 
//and the bit map is updated to show that the given frame
//is no longer available
void MemoryWithTLB::setST(int s, int f)
{
	if (s >= 512 || s < 0)
	{
		printf("%s", "Internal error: setST");
		return;
	}

	if (f >= 524288 || (f > 0 && f < 512))
	{
		printf("%s", "Internal error: setST2");
		return;
	}
	
	PM[s] = f;

	if (f == 0 || f == -1)
		return;

	setBitMapForST(f);
}

//The page table is initialized with the given information 
//and the bit map is updated to show that the given frame
//is no longer available
void MemoryWithTLB::setPT(int p, int s, int f)
{
	if (p < 0 || p >= 1024)
	{
		printf("%s", "Internal error: setPT");
		return;
	}

	if (PM[s] == 0 || PM[s] == -1)
		return;
	
	PM[PM[s] + p] = f;
	
	if (f == 0 || f == -1)
		return;

	setBitMapForPT(f);
}

//Retrieves and displays the physical address. It first checks if the
//entry is already in the TLB; if it is, the TLB LRU (number indicating least recently used) 
//is updated for all entries and the physical address is displayed. If it is not in the TLB,
//it retrieves the physical address from the physical memory and
//updates the TLB with an entry specifying LRU, the segment and page number,
//and the starting frame address of the given address. The LRU of all other 
//TLB entries is then updated.
int MemoryWithTLB::read(int VA)
{
	int s = getSegmentNumber(VA);
	int p = getPageNumber(VA);
	int w = getOffset(VA);
	int sp = getSP(VA);

	int entry = getTLBEntry(sp);

	if (s >= 524288 || PM[s] + p >=524288)
		return -1;

	if (entry == -1) //TLB Miss
	{
		int STEntry = PM[s];
		int PTEntry = PM[PM[s] + p];

        //Either the page table or the page is not in the physical memory 
		if (STEntry == -1 || PTEntry == -1) 
		{
			printf ("%c %s ", 'm', "pf");
			fflush(stdout);
			return -1;
		}

        //Either the page table or the page does not exist
		if (STEntry == 0 || PTEntry == 0)
		{
			printf ("%c %s ", 'm', "err");
			fflush(stdout);
			return -1;
		}

		else
		{
			printf ("%c %d ", 'm', PTEntry + w);
			fflush(stdout);
		}

		updateTLB(sp, PTEntry);
		//printTLB();
		return PTEntry + w;
	}

	else //TLB hit
	{
		updateLRU(entry, w);
		//printTLB();
		int PA = TLB[entry].f + w;
		printf ("%c %d ", 'h', PA);
		fflush(stdout);
		return PA;
	}
}

//Writes the address to the physical memory and displays the physical address. 
//It first checks if the entry is already in the TLB; if it is, the TLB LRU (number 
//indicating least recently used) is updated for all entries and the physical address 
//is displayed. If it is not in the TLB, it retrieves the physical address from the 
//physical memory and updates the TLB with an entry specifying LRU, the segment and 
//page number, and the starting frame address of the given address. The LRU of all other 
//TLB entries is then updated.
int MemoryWithTLB::write(int VA)
{
	int s = getSegmentNumber(VA);
	int p = getPageNumber(VA);
	int w = getOffset(VA);
	int sp = getSP(VA);

	int entry = getTLBEntry(sp);

	if (s >= 524288 || PM[s] + p >=524288)
		return -1;

	if (entry == -1) //TLB Miss
	{
		int STEntry = PM[s];

        //The page table is not in the physical memory
		if (STEntry == -1) 
		{
			printf ("%c %s ", 'm', "pf");
			fflush(stdout);
			return -1;
		}

        //The page table does not exist, so a blank page table is created
		if (STEntry == 0)
		{
			int PMIndex = allocatePT();
			if (PMIndex == -1)
			{
				printf ("%c %s ", 'm', "err");
				fflush(stdout);
				return -1;
			}

			PM[s] = PMIndex;
			STEntry = PM[s];
		}

		int PTEntry = PM[PM[s] + p];

        //The page is not in the physical memory
		if (PTEntry == -1) 
		{
			printf ("%c %s ", 'm', "pf");
			fflush(stdout);
			return -1;
		}

        //The page does not exist so a blank page is created
		if (PTEntry == 0)
		{
			int PMIndex = allocatePage();
			if (PMIndex == -1)
			{
				printf ("%c %s ", 'm', "err");
				fflush(stdout);
				return -1;
			}

			PM[PM[s] + p] = PMIndex;
			PTEntry = PM[PM[s] + p];
			printf ("%c %d ", 'm', PTEntry + w);
			fflush(stdout);
		}

		else
		{
			printf ("%c %d ", 'm', PTEntry + w);
			fflush(stdout);
		}

		updateTLB(sp, PTEntry);
		//printTLB();
		return PTEntry + w;
	}

	else //TLB Hit
	{
		updateLRU(entry, w);
		//printTLB();
		int PA = TLB[entry].f + w;
		printf ("%c %d ", 'h', PA);
		fflush(stdout);
		return PA;
	}
}

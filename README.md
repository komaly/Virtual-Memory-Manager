The Virtual Memory Manager is a virtual memory system (VM) that uses segmentation and paging.
It creates segment and page tables in the physical memory, which is divided into 512 frames, 
and translates virtual addresses to physical addresses. It uses a Translation Look Aside Buffer 
(TLB) to increase the efficiency of the translation process. It also utilizes a bitmap to keep track
of which frames in the physical memory are free and which are not.

The program takes in 2 input files. The first input file contains numbers used to create the segment 
and page tables in the physical memory. 

The format of the first input file is:

s1 f1 s2 f2 ... sn fn
p1 s1 f1 p2 s2 f2 ... pm sm fm

The first line indicates that for each pair, the page table of segment "s" starts at address "f".
The second line indicates that for every 3 numbers, the page "p" of segment "s" starts at address "f".

The second input file contains numbers that have the following format:

a1 b1 a2 b2 ... an bn

The number "b" is the virtual address to be translated. The number "a" will always be 0 or 1. If it 
is 0, it means that the given virtual address will be read from memory. If it is 1, it means that 
the given address will be written to memory.

For every virtual address, the program outputs an "m" or a "h", followed by either an "err", "pf",
or a number. If it outputs "m", that means that entry was not in the TLB, and if it was "h" that 
means that the entry was in the TLB. It outputs "err" (error) if either the page table or the page
does not exist. It outputs "pf" (page fault) if either the page table or the page is not in the physical 
memory. If the program is writing a given address to memory, it outputs "err" if a blank page or 
page table is unable to be created. If there is no error or page fault, the physical address is outputed.




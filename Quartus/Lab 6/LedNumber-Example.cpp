#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
using namespace std;

//Property of Prof. J. Marpaung
//Not to be distributed elsewhere without a written consent from Prof. J. Marpaung
//If you see this code, please report it to Northeastern University
//All Rights Reserved

// Important Message from Prof. Marpaung
// Read the PDF to find the Physical ADDRESS of LEDR, SW and KEY.
// End Important Message

// Physical base/starting address of FPGA Devices 
const unsigned int LW_BRIDGE_BASE = 0xFF200000;  // Base offset 

// Physical final address of FPGA Devices 
const unsigned int FINAL_PHYSICAL_ADDRESS = 0xFFFEC700;  // Final offset 

// Length of memory-mapped IO window 
const unsigned int LW_BRIDGE_SPAN = FINAL_PHYSICAL_ADDRESS - LW_BRIDGE_BASE; //0x00DEC700; // Address map size

// Cyclone V FPGA device addresses
const unsigned int LEDR_OFFSET = //Physical ADDRESS of RED LED - LW_BRIDGE_BASE ; Write the value in Hex here
const unsigned int SW_OFFSET   = //Physical ADDRESS of SWITCH - LW_BRIDGE_BASE  ; Write the value in Hex here
const unsigned int KEY_OFFSET  = //Physical ADDRESS of PUSH BUTTON - LW_BRIDGE_BASE ; Write the value in Hex here

/** 
 * Write a 4-byte value at the specified general-purpose I/O location. 
 * 
 * @param pBase		Base address returned by 'mmap'. 
 * @param offset	Offset where device is mapped. 
 * @param value	Value to be written. 
 */ 
void RegisterWrite(char *pBase, unsigned int reg_offset, int value) 
{ 
	* (volatile unsigned int *)(pBase + reg_offset) = value; 
} 

/** 
 * Read a 4-byte value from the specified general-purpose I/O location. 
 * 
 * @param pBase		Base address returned by 'mmap'. 
 * @param offset	Offset where device is mapped. 
 * @return		Value read. 
 */ 
int RegisterRead(char *pBase, unsigned int reg_offset) 
{ 
	return * (volatile unsigned int *)(pBase + reg_offset); 
} 

void WriteAllLeds(char *pBase, int value)
{
    RegisterWrite(pBase, LEDR_OFFSET, value);
}

/** 
 * Initialize general-purpose I/O 
 *  - Opens access to physical memory /dev/mem 
 *  - Maps memory into virtual address space 
 * 
 * @param fd	File descriptor passed by reference, where the result 
 *			of function 'open' will be stored. 
 * @return	Address to virtual memory which is mapped to physical, or MAP_FAILED on error. 
  */ 
char *Initialize(int *fd) 
{
	// Open /dev/mem to give access to physical addresses
	*fd = open( "/dev/mem", (O_RDWR | O_SYNC));
	if (*fd == -1)			//  check for errors in openning /dev/mem
	{
        cout << "ERROR: could not open /dev/mem..." << endl;
        exit(1);
	}
	
   // Get a mapping from physical addresses to virtual addresses
   char *virtual_base = (char *)mmap (NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, *fd, LW_BRIDGE_BASE);
   if (virtual_base == MAP_FAILED)		// check for errors
   {
      cout << "ERROR: mmap() failed..." << endl;
      close (*fd);		// close memory before exiting
      exit(1);        // Returns 1 to the operating system;
   }
   return virtual_base;
}

/** 
 * Close general-purpose I/O. 
 * 
 * @param pBase	Virtual address where I/O was mapped. 
 * @param fd	File descriptor previously returned by 'open'. 
 */ 
void Finalize(char *pBase, int fd) 
{
	if (munmap (pBase, LW_BRIDGE_SPAN) != 0)
	{
      cout << "ERROR: munmap() failed..." << endl;
      exit(1);
	}
   close (fd); 	// close memory
}

int main() 
{ 
	// Initialize 
	int fd; 
	char *pBase = Initialize(&fd);
	
	
	// ************** Put your code here **********************
		
	int value = 0; 
	cout << "Enter an int value between 0 to 1023: " << endl; 
	cin >> value; 
	cout << "value to be written to LEDs = " << value << endl; 
	WriteAllLeds(pBase, value);
	int readLEDs = RegisterRead(pBase, LEDR_OFFSET);
	cout << "value of LEDS read = " << readLEDs << endl;
	
	// Done 
	
	Finalize(pBase, fd); 
}

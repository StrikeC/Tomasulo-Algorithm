/*
 *  ECE 474/774: Computer Architecture Project 2
 *  Author(s): Jonathon Edstrom & Tianqi Chen
 *  Date: 12/16/2016
 *
 *  Compile: gcc ROB.c -o rob -std=c99
 */
 
// Includes
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Constants
#define ASCII_OFFSET 48
#define DELAY_ADD 1
#define DELAY_SUB 1
#define DELAY_MUL 9
#define DELAY_DIV 39

// Structures
struct reorderBuffer
{
    uint8_t op;
    uint8_t dst; // indicates the destination in RF
    int32_t value;
    bool commit;
    bool exception;
};

struct instruction
{
    uint8_t op;
    uint8_t dst;
    uint8_t srcOne;
    uint8_t srcTwo;
};

struct reservationStation
{
    bool busy;
    uint8_t op;
    uint8_t dstRob; // indicates the destination in ROB
    int32_t vj;
    int32_t vk;
    uint8_t qj;
    uint8_t qk;
    bool disp;
};

struct integerAddUnit
{
    bool busy;
    int8_t cyclesRemaining;
    int32_t result;
    uint8_t dst; // indicates the reservation station where instructions come from
    bool broadcast;
};

struct integerMultiplyUnit
{
    bool busy;
    int8_t cyclesRemaining;
    int32_t result;
    uint8_t dst; // indicates the reservation station where instructions come from
    bool broadcast;
};

struct temporaryContainerForUpdate
{
	bool busy;
	uint8_t dst;
	int32_t result;
};

//#define DEBUG_MODE // comment out to disable debugging

/*
 *  Function: main
 *  Parameters: argc : total number of program arguments
 *              argv[] : array of string arguments
 *  Returns: int : exit code (0 == success)
 *  Description: program entry
 */
int main( int argc, char * argv[] )
{
    printf( "\n--- Reorder Buffer Algorithm Simulator - Jonathon Edstrom & Tianqi Chen ---\n" ); 

    if( argc != 2 ) // argc should be 2 for correct execution
    {
        // print argv[0] assuming it is the program name with the following usage hint to user
        printf( "usage: %s file_path\n", argv[0] );
    }
    
    else // correct number of arguments
    {
        
    }
    
    return 0;
}

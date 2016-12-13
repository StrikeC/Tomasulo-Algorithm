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
#define DELAY_DIV_EXCEPTION 37
//#define DEBUG_MODE // comment out to disable debugging

// Structures
struct reorderBuffer
{
	bool busy;
    uint8_t op;
    uint8_t dst; // indicates the destination in RF
    int32_t value;
    bool done;
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
    uint8_t dst; // indicates the destination in ROB
    int32_t vj;
    int32_t vk;
    uint8_t qj;
    uint8_t qk;
};

struct integerAddUnit
{
    bool busy;
    int8_t cyclesRemaining;
    int32_t result;
    uint8_t dst; // indicates the destination in ROB
};

struct integerMultiplyUnit
{
    bool busy;
    int8_t cyclesRemaining;
    int32_t result;
    uint8_t dst; // indicates the destination in ROB
	bool exception;
};

struct temporaryContainerForUpdate
{
	bool busy;
	bool exception;
	uint8_t dst;
	int32_t result;
};

// Global Declarations
uint8_t numberOfInstructions;
uint32_t numberOfCycles;
int32_t registerFile[8]; // 8-entry array of integers used as register file
int8_t registerAllocationTable[8]; // 8-entry array of integers used as RAT (0 means empty)
uint8_t instructionPosition = 0; // acts as a queue pointer
uint8_t issuePointer = 1; // issue pointer for re-order buffer, range 1 - 6
uint8_t commitPointer = 1; // commit pointer for re-order buffer, range 1 - 6
struct instruction instructions[10]; // 10-entry array of instruction records
struct reorderBuffer rob[7]; // 7-entry array of re-order buffer (ROB0-ROB6), don't use ROB0
struct reservationStation rs[6]; // 6-entry array of reservation stations (RS0-RS5), don't use RS0
struct integerAddUnit addUnit;
struct integerMultiplyUnit mulUnit;
struct temporaryContainerForUpdate temp;
char* strOpcodes[4] = { "Add", "Sub", "Mul", "Div" };
char* strTags[6] = { "", "RS1", "RS2", "RS3", "RS4", "RS5" }; // index 0 SHOULD be the empty string!

// Function Declarations
void checkIssue( uint8_t instructionIndex );
void checkDispatch();
void checkBroadcast();
void printSimulatorOutput();
void printUnitOutputs();
void checkUpdate();


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

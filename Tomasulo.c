/*
 *  ECE 474/774: Computer Architecture Project 1
 *  Author(s): Jonathon Edstrom
 *  Date: 10/29/16 - 10/31/16
 *
 *  Compile: gcc Tomasulo.c -o tomasulo -std=c99
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
#define DELAY_ADD 2
#define DELAY_SUB 2
#define DELAY_MUL 10
#define DELAY_DIV 40
#define DEBUG_MODE // comment out to disable debugging

// Structures
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
    uint8_t dst;
    bool broadcast;
};

struct integerMultiplyUnit
{
    bool busy;
    int8_t cyclesRemaining;
    int32_t result;
    uint8_t dst;
    bool broadcast;
};

// Global Declarations
uint8_t numberOfInstructions;
uint32_t numberOfCycles;
int32_t registerFile[8]; // 8-entry array of integers used as register file
int8_t registerAllocationTable[8]; // 8-entry array of integers used as RAT (-1 means empty)
uint8_t instructionPosition = 0; // acts as a queue pointer
struct instruction instructions[10]; // 10-entry array of instruction records
struct reservationStation rs[6]; // 6-entry array of reservation stations (RS0-RS5), don't use RS0
struct integerAddUnit addUnit;
struct integerMultiplyUnit mulUnit;

// Function Declarations
void checkIssue( uint8_t instructionIndex );
void checkDispatch();
void checkCapture();

/*
 *  Function: main
 *  Parameters: argc : total number of program arguments
 *              argv[] : array of string arguments
 *  Returns: int : exit code (0 == success)
 *  Description: program entry
 */
int main( int argc, char * argv[] )
{
    printf( "--- Tomasulo's Algorithm Simulator - Jonathon Edstrom ---\n\n" ); 

    if( argc != 2 ) // argc should be 2 for correct execution
    {
        // print argv[0] assuming it is the program name with the following usage hint to user
        printf( "usage: %s file_path\n", argv[0] );
        }
    else // correct number of arguments
    {
        FILE * inputFile;
        if( (inputFile = fopen( argv[1] , "r" )) == NULL )
        {
            printf( "Input file could not be opened... Exiting.\n" );
            exit( EXIT_FAILURE );
        }

        char line_buffer[BUFSIZ]; /* BUFSIZ is defined if you include stdio.h */
        uint8_t line_number = 0;
        line_number = 0;
        while( fgets(line_buffer, sizeof(line_buffer), inputFile) )
        {
            ++line_number;
            if( line_number == 1 ) // number of instructions line
            {
                numberOfInstructions = line_buffer[0] - ASCII_OFFSET;
                #ifdef DEBUG_MODE
                printf( "Number of Instructions: %u\n", numberOfInstructions );
                #endif
            }
            else if( line_number == 2 ) // number of cycles of simulation line
            {
                numberOfCycles = line_buffer[0] - ASCII_OFFSET;
                #ifdef DEBUG_MODE
                printf( "Number of Simulation Cycles: %u\n\n", numberOfCycles );
                #endif
            }
            else if( line_number >= 3 && line_number < (3 + numberOfInstructions) ) // instructions
            {
                instructions[line_number-3].op = line_buffer[0] - ASCII_OFFSET;
                instructions[line_number-3].dst = line_buffer[2] - ASCII_OFFSET;
                instructions[line_number-3].srcOne = line_buffer[4] - ASCII_OFFSET;
                instructions[line_number-3].srcTwo = line_buffer[6] - ASCII_OFFSET;
            }
            else // initial values for register file
            {
                registerFile[line_number-3-numberOfInstructions] = atoi( line_buffer );
            }
        }

        #ifdef DEBUG_MODE
        for( uint8_t i = 0; i < numberOfInstructions; i++ )
        {
            char operator;
            switch( instructions[i].op )
            {
                case 0:
                    operator = '+';
                    break;
                case 1:
                    operator = '-';
                    break;
                case 2:
                    operator = 'x';
                    break;
                case 3:
                    operator = '/';
                    break;
            }
            printf( "Instruction %u: R%u=R%u%cR%u\n", i, instructions[i].dst, instructions[i].srcOne, operator, instructions[i].srcTwo );
        }

        printf( "\n" );

        for( uint8_t i = 0; i < 8; i++ )
        {
            printf( "RF%u: %d\n", i, registerFile[i] );
        }

        printf( "\n" );
        #endif

        for( uint16_t i = 0; i < numberOfCycles; i++ )
        {
            #ifdef DEBUG_MODE
            printf( "--CYCLE %u--\n", numberOfCycles );
            #endif
        }
    }
    return 0;
}

/*
 *  Function: checkIssue
 *  Parameters: instructionIndex : index of the instruction to be checked for issuing to reservation station
 *  Returns: None
 *  Description: checks if an instruction is ready to be issued
 */
void checkIssue( uint8_t instructionIndex )
{
    // check RS1-RS3 (ADD/SUB R.S.)
    if( instructions[instructionIndex].op == 0 || instructions[instructionIndex].op == 1 )
    {
        bool issuedSuccessfully = false;
        if( !rs[1].busy )
        {
            rs[1].busy = true;
            rs[1].op = instructions[instructionIndex].op;
            rs[1].
            issuedSuccessfully = true;
        }

        if( !rs[2].busy && !issuedSuccessfully )
        {
            issuedSuccessfully = true;
        }

        if( !rs[3].busy && !issuedSuccessfully )
        {
            issuedSuccessfully = true;
        }

        if( issuedSuccessfully )
        {
            instructionPointer++; // move instruction queue to next instruction
        }
    }
    // check RS4-RS5 (MUL/DIV R.S.)
    else if( instructions[instructionIndex].op == 2 || instructions[instructionIndex].op == 3 )
    {
        bool issuedSuccessfully = false;
        if( !rs[4].busy )
        {
            issuedSuccessfully = true;
        }

        if( !rs[5].busy && !issuedSuccessfully )
        {
            issuedSuccessfully = true;
        }

        if( issuedSuccessfully )
        {
            instructionPointer++; // move instruction queue to next instruction
        }
    }
    // error: incorrect opcode
    else
    {
        printf( "Incorrect instruction opcode found while issuing instruction\n" );
        exit(EXIT_FAILURE);
    }
}

/*
 *  Function: checkDispatch
 *  Parameters: None
 *  Returns: None
 *  Description: checks if a reservation station is ready to dispatch
 */
void checkDispatch()
{
    if( rs[1].busy && rs[1].disp )
    {
        if( addUnit.busy )
        {
            #ifdef DEBUG_MODE
            printf( "RS1 cannot dispatch because ADD unit is busy\n" );
            #endif
        }
        else
        {

        }
    }

    if( rs[2].busy && rs[2].disp )
    {
        if( addUnit.busy )
        {
            #ifdef DEBUG_MODE
            printf( "RS2 cannot dispatch because ADD unit is busy\n" );
            #endif
        }
        else
        {

        }
    }

    if( rs[3].busy && rs[3].disp )
    {
        if( addUnit.busy )
        {
            #ifdef DEBUG_MODE
            printf( "RS3 cannot dispatch because ADD unit is busy\n" );
            #endif
        }
        else
        {

        }
    }

    if( rs[4].busy && rs[4].disp )
    {
        if( mulUnit.busy )
        {
            #ifdef DEBUG_MODE
            printf( "RS4 cannot dispatch because MUL unit is busy\n" );
            #endif
        }
        else
        {

        }
    }

    if( rs[5].busy && rs[5].disp )
    {
        if( mulUnit.busy )
        {
            #ifdef DEBUG_MODE
            printf( "RS5 cannot dispatch because MUL unit is busy\n" );
            #endif
        }
        else
        {

        }
    }
}

/*
 *  Function: checkCapture
 *  Parameters: None
 *  Returns: None
 *  Description: check if results of unit(s) are ready to broadcast/capture
 */
void checkCapture()
{
    bool broadcasting = false;
    if( mulUnit.busy )
    {
        mulUnit.cyclesRemaining--; // decrement cycles remaining
        if( mulUnit.cyclesRemaining == 0 ) // broadcast MUL unit result
        {
            broadcasting = true;
        }
    }    

    if( addUnit.busy && !broadcasting ) // broadcast ADD unit result if MUL unit isn't broadcasting
    {
        
    }
}

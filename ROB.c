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
	uint8_t rsDstAdd;
	uint8_t rsDstMul;
	uint8_t robDstCom;
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
struct reorderBuffer rob[6]; // 7-entry array of re-order buffer (ROB0-ROB6), don't use ROB0
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
        FILE * inputFile;
        if( (inputFile = fopen( argv[1], "r" )) == NULL )
        {
            printf( "Input file could not be opened... Exiting.\n" );
            exit( EXIT_FAILURE );
        }

        char line_buffer[BUFSIZ]; // BUFSIZ is defined if you include stdio.h
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
                numberOfCycles = atoi( line_buffer );
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

        // Loop for each cycle
        for( uint16_t i = 1; i <= numberOfCycles; i++ )
        {
            #ifdef DEBUG_MODE
            printf( "---CYCLE %u---\n", i );
            printf( "1. PRE-BROADCAST:\n" );
            printUnitOutputs();
            printSimulatorOutput();
            printf( "-------------\n\n" );
            #endif
            checkBroadcast();
			
            #ifdef DEBUG_MODE
            printf( "2. PRE-DISPATCH/POST-BROADCAST:\n" );
            printUnitOutputs();
            printSimulatorOutput();
            printf( "-------------\n\n" );
            #endif
			checkDispatch();
			
			#ifdef DEBUG_MODE
            printf( "3. PRE-ISSUE/POST-DISPATCH:\n" );
            printUnitOutputs();
            printSimulatorOutput();
            printf( "-------------\n\n" );
            #endif
			checkIssue( instructionPosition );
			
			#ifdef DEBUG_MODE
            printf( "4. PRE-UPDATE/POST-ISSUE:\n" );
            printUnitOutputs();
            printSimulatorOutput();
            printf( "-------------\n\n" );
            #endif
			checkUpdate();
			
			#ifdef DEBUG_MODE
            printf( "5. POST-UPDATE:\n" );
            printUnitOutputs();
            printSimulatorOutput();
            printf( "-------------\n\n" );
            #endif
        }
        
        printSimulatorOutput(); // Display output for reservation stations, RF, RAT and instruction queue
    }

    printf( "\n" );
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
    // return if no more instructions to issue
    if( instructionIndex > numberOfInstructions - 1 )
    {
        return;
    }
	// return if re-order buffer is full
	if( rob[issuePointer].busy )
	{
		return;
	}

    // check RS1-RS3 (ADD/SUB R.S.)
    if( instructions[instructionIndex].op == 0 || instructions[instructionIndex].op == 1 )
    {
        bool issuedSuccessfully = false;
        for ( uint8_t i = 1; i <= 3; i++) 
        {
            if( !rs[i].busy && !issuedSuccessfully )
            {
				issuedSuccessfully = true;
				
				// set re-order buffer
				rob[issuePointer].busy = true;
				rob[issuePointer].op = instructions[instructionIndex].op;
				rob[issuePointer].dst = instructions[instructionIndex].dst;
				
				// set reservation station
				rs[i].busy = true;
                rs[i].op = instructions[instructionIndex].op;
				rs[i].dst = issuePointer;
                // source one value/name transmit
                if( registerAllocationTable[instructions[instructionIndex].srcOne] == 0 )
                {
                    rs[i].vj = registerFile[instructions[instructionIndex].srcOne];
                }
                else
                {
                    rs[i].qj = registerAllocationTable[instructions[instructionIndex].srcOne];
                }
                
                // source two value/name transmit
                if( registerAllocationTable[instructions[instructionIndex].srcTwo] == 0 )
                {
                    rs[i].vk = registerFile[instructions[instructionIndex].srcTwo];
                }
                else
                {
                    rs[i].qk = registerAllocationTable[instructions[instructionIndex].srcTwo];
                }
				
				// update destination's RAT with ROB index
				registerAllocationTable[instructions[instructionIndex].dst] = issuePointer;
            }
        }

        if( issuedSuccessfully )
        {
            instructionPosition++; // move instruction queue to next instruction
			
			if( issuePointer == 6 )
			{
				issuePointer = 1; // reset issue pointer
			}
			else
			{
				issuePointer++; // move issue pointer to next ROB position
			}
        }
    }
    // check RS4-RS5 (MUL/DIV R.S.)
    else if( instructions[instructionIndex].op == 2 || instructions[instructionIndex].op == 3 )
    {
        bool issuedSuccessfully = false;
        for ( uint8_t i = 4; i <= 5; i++) 
        {
            if( !rs[i].busy && !issuedSuccessfully )
            {
				issuedSuccessfully = true;
				
				// set re-order buffer
				rob[issuePointer].busy = true;
				rob[issuePointer].op = instructions[instructionIndex].op;
				rob[issuePointer].dst = instructions[instructionIndex].dst;
				
				// set reservation station
                rs[i].busy = true;
                rs[i].op = instructions[instructionIndex].op;
				rs[i].dst = issuePointer;
                // source one value/name transmit
                if( registerAllocationTable[instructions[instructionIndex].srcOne] == 0 )
                {
                    rs[i].vj = registerFile[instructions[instructionIndex].srcOne];
                }
                else
                {
                    rs[i].qj = registerAllocationTable[instructions[instructionIndex].srcOne];
                }
                
                // source two value/name transmit
                if( registerAllocationTable[instructions[instructionIndex].srcTwo] == 0 )
                {
                    rs[i].vk = registerFile[instructions[instructionIndex].srcTwo];
                }
                else
                {
                    rs[i].qk = registerAllocationTable[instructions[instructionIndex].srcTwo];
                }

                // update destination's RAT with ROB index
				registerAllocationTable[instructions[instructionIndex].dst] = issuePointer; 
            
				
            }
        }

        if( issuedSuccessfully )
        {
            instructionPosition++; // move instruction queue to next instruction
			
			if( issuePointer == 6 )
			{
				issuePointer = 1; // reset issue pointer
			}
			else
			{
				issuePointer++; // move issue pointer to next ROB position
			}
        }
    }
    // error: incorrect opcode
    else
    {
        printf( "Incorrect instruction opcode: '%u' found while issuing instruction. Now exiting.\n", instructions[instructionIndex].op );
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
	if( rs[1].busy )
	{
		if( addUnit.busy )
		{
			#ifdef DEBUG_MODE
			printf( "RS1 cannot dispatch because ADD unit is busy\n" );
			#endif
		}
		else
		{
			if( rs[1].qj == 0 && rs[1].qk == 0 ) // check if tags have captured their values
			{
				//rs[1].disp = true;
				addUnit.busy = true;
				addUnit.dst = rs[1].dst;
				switch( rs[1].op )
				{
					case 0:
						addUnit.result = rs[1].vj + rs[1].vk;
						addUnit.cyclesRemaining = DELAY_ADD;
						break;
					case 1:
						addUnit.result = rs[1].vj - rs[1].vk;
						addUnit.cyclesRemaining = DELAY_SUB;
						break;
				}
				// reset reservation station
				rs[1].busy = false;
				rs[1].op = rs[1].dst = rs[1].vj = rs[1].vk = 0;
			}
			else
			{
				#ifdef DEBUG_MODE
				printf( "RS1 cannot dispatch before capture\n" );
				#endif
			}
		}
	}
	
	if( rs[2].busy )
	{
		if( addUnit.busy )
		{
			#ifdef DEBUG_MODE
			printf( "RS2 cannot dispatch because ADD unit is busy\n" );
			#endif
		}
		else
		{
			if( rs[2].qj == 0 && rs[2].qk == 0 ) // check if tags have captured their values
			{
				//rs[2].disp = true;
				addUnit.busy = true;
				addUnit.dst = rs[2].dst;
				switch( rs[2].op )
				{
					case 0:
						addUnit.result = rs[2].vj + rs[2].vk;
						addUnit.cyclesRemaining = DELAY_ADD;
						break;
					case 1:
						addUnit.result = rs[2].vj - rs[2].vk;
						addUnit.cyclesRemaining = DELAY_SUB;
						break;
				}
				// reset reservation station
				rs[2].busy = false;
				rs[2].op = rs[2].dst = rs[2].vj = rs[2].vk = 0;
			}
			else
			{
				#ifdef DEBUG_MODE
				printf( "RS2 cannot dispatch before capture\n" );
				#endif
			}
		}
	}
	
	if( rs[3].busy )
	{
		if( addUnit.busy )
		{
			#ifdef DEBUG_MODE
			printf( "RS3 cannot dispatch because ADD unit is busy\n" );
			#endif
		}
		else
		{
			if( rs[3].qj == 0 && rs[3].qk == 0 ) // check if tags have captured their values
			{
				//rs[3].disp = true;
				addUnit.busy = true;
				addUnit.dst = rs[3].dst;
				switch( rs[3].op )
				{
					case 0:
						addUnit.result = rs[3].vj + rs[3].vk;
						addUnit.cyclesRemaining = DELAY_ADD;
						break;
					case 1:
						addUnit.result = rs[3].vj - rs[3].vk;
						addUnit.cyclesRemaining = DELAY_SUB;
						break;
				}
				// reset reservation station
				rs[3].busy = false;
				rs[3].op = rs[3].dst = rs[3].vj = rs[3].vk = 0;
			}
			else
			{
				#ifdef DEBUG_MODE
				printf( "RS3 cannot dispatch before capture\n" );
				#endif
			}
		}
	}
	
	if( rs[4].busy )
	{
		if( mulUnit.busy )
		{
			#ifdef DEBUG_MODE
			printf( "RS4 cannot dispatch because MUL unit is busy\n" );
			#endif
		}
		else
		{
			if( rs[4].qj == 0 && rs[4].qk == 0 ) // check if tags have captured their values
			{
				//rs[4].disp = true;
				mulUnit.busy = true;
				mulUnit.dst = rs[4].dst;
				switch( rs[4].op )
				{
					case 2:
						mulUnit.result = rs[4].vj * rs[4].vk;
						mulUnit.cyclesRemaining = DELAY_MUL;
						break;
					case 3:
						if( rs[4].vk == 0)
						{
							mulUnit.exception = true;
							mulUnit.result = 0;
							mulUnit.cyclesRemaining = DELAY_DIV_EXCEPTION;
						}
						else
						{
							mulUnit.result = rs[4].vj / rs[4].vk;
							mulUnit.cyclesRemaining = DELAY_DIV;
						}
						break;
				}
				// reset reservation station
				rs[4].busy = false;
				rs[4].op = rs[4].dst = rs[4].vj = rs[4].vk = 0;
			}
			else
			{
				#ifdef DEBUG_MODE
				printf( "RS4 cannot dispatch before capture\n" );
				#endif
			}
		}
	}
	
	if( rs[5].busy )
	{
		if( mulUnit.busy )
		{
			#ifdef DEBUG_MODE
			printf( "RS5 cannot dispatch because MUL unit is busy\n" );
			#endif
		}
		else
		{
			if( rs[5].qj == 0 && rs[5].qk == 0 ) // check if tags have captured their values
			{
				//rs[5].disp = true;
				mulUnit.busy = true;
				mulUnit.dst = rs[5].dst;
				switch( rs[5].op )
				{
					case 2:
						mulUnit.result = rs[5].vj * rs[5].vk;
						mulUnit.cyclesRemaining = DELAY_MUL;
						break;
					case 3:
						if( rs[5].vk == 0)
						{
							mulUnit.exception = true;
							mulUnit.result = 0;
							mulUnit.cyclesRemaining = DELAY_DIV_EXCEPTION;
						}
						else
						{
							mulUnit.result = rs[5].vj / rs[5].vk;
							mulUnit.cyclesRemaining = DELAY_DIV;
						}
						break;
				}
				// reset reservation station
				rs[5].busy = false;
				rs[5].op = rs[5].dst = rs[5].vj = rs[5].vk = 0;
			}
			else
			{
				#ifdef DEBUG_MODE
				printf( "RS5 cannot dispatch before capture\n" );
				#endif
			}
		}
	}
}

/*
 *  Function: checkBroadcast
 *  Parameters: None
 *  Returns: None
 *  Description: check if results of unit(s) are ready to be freed.
 */
void checkBroadcast()
{
	bool broadcasting = false;
	if( mulUnit.busy )
	{
		if( mulUnit.cyclesRemaining == 0 ) // broadcast MUL unit result
		{
			broadcasting = true;
			
			// save values to temporaryContainer
			temp.busy = true;
			temp.exception = mulUnit.exception;
			temp.dst = mulUnit.dst;
			temp.result = mulUnit.result;
			
			// reset mulUnit
			mulUnit.busy = false;
			mulUnit.exception = false;
		}
		
		if( mulUnit.cyclesRemaining > 0 )
		{
			mulUnit.cyclesRemaining--; // decrement MUL unit cycles remaining
		}
	}
	
	if( addUnit.busy )
	{
		if( addUnit.cyclesRemaining == 0 && !broadcasting ) // broadcast ADD unit result if MUL unit isn't broadcasting
		{
			broadcasting = true;
			
			// save values to temporaryContainer
			temp.busy = true;
			temp.dst = addUnit.dst;
			temp.result = addUnit.result;
			
			// reset addUnit
			addUnit.busy = false;
		}
		else
		{
			if( broadcasting )
			{
				#ifdef DEBUG_MODE
				printf( "ADD Unit can't broadcast this cycle because MUL Unit is busy broadcasting...\n" );
				#endif
			}
		}
		
		if( addUnit.cyclesRemaining > 0 )
		{
			addUnit.cyclesRemaining--; // decrement ADD unit cycles remaining
		}
	}
}

/*
 *  Function: printSimulatorOutput
 *  Parameters: None
 *  Returns: None
 *  Description: displays current values for reservation stations, register file, register allocation table, and instruction queue
 */
void printSimulatorOutput()
{
	// print reservation station headers
	printf( "\n\tBusy\tOp\tVj\tVk\tQj\tQk\tDisp\n" );
	
	// print reservation station values
	for( uint8_t i = 1; i <= 5; i++ )
	{
		printf( "RS%u\t%u\t", i, rs[i].busy );
        if( rs[i].busy )
        {
            printf( "%s\t", strOpcodes[rs[i].op] );

            if( rs[i].qj == 0 ) // if tag is empty print the value of vj
            {
                printf( "%d\t", rs[i].vj );
            }
            else
            {
                printf( " \t" );
            }

            if( rs[i].qk == 0 ) // if tag is empty print the value of vk
            {
                printf( "%d\t", rs[i].vk );
            }
            else
            {
                printf( " \t" );
            }

            printf( "%s\t%s\t%u\n", strTags[rs[i].qj], strTags[rs[i].qk], rs[i].disp );
        }
        else
        {
            printf( " \t \t \t \t \t \n" );
        }
    }
	
	// print RF and RAT headers
	printf( "\n\tRF\t\tRAT\n" );

	// print RF and RAT values
	for( uint8_t i = 0; i <= 7; i++ )
	{
		printf( "%u:\t%d", i, registerFile[i] );
		
		if( registerAllocationTable[i] != 0 )
		{
			printf( "\t\t%s\n", strTags[registerAllocationTable[i]] );
		}
		else
		{
			printf( "\t\t \n" );
		}
	}
	
	// print instruction queue
	printf( "\nInstruction Queue\n" );
	for( uint8_t i = instructionPosition; i < numberOfInstructions; i++ )
	{
		printf( "%s R%u, R%u, R%u\n", strOpcodes[instructions[i].op], instructions[i].dst, instructions[i].srcOne, instructions[i].srcTwo );
	}
}

/*
 *  Function: printUnitOutputs
 *  Parameters: None
 *  Returns: None
 *  Description: displays current values for ADD and MUL units (used for debugging)
 */
void printUnitOutputs()
{
    // print add unit structure members
    printf( "ADD Unit:\n" );
    printf( "\tBusy: %u\n", addUnit.busy );
    printf( "\tCycles Remaining: %d\n", addUnit.cyclesRemaining );
    printf( "\tResult: %d\n", addUnit.result );
    printf( "\tDestination R.S.: %u\n", addUnit.dst );
    printf( "\tBroadcast: %u\n\n", addUnit.broadcast );

    // print mul unit structure members
    printf( "MUL Unit:\n" );
    printf( "\tBusy: %u\n", mulUnit.busy );
    printf( "\tCycles Remaining: %d\n", mulUnit.cyclesRemaining );
    printf( "\tResult: %d\n", mulUnit.result );
    printf( "\tDestination R.S.: %u\n", mulUnit.dst );
    printf( "\tBroadcast: %u\n\n", mulUnit.broadcast );
}

/*
 *  Function: checkUpdate
 *  Parameters: None
 *  Returns: None
 *  Description: check if results of unit(s) are ready to broadcast/capture
 */
void checkUpdate()
{
	if( temp.busy )
	{
		// update matching reservation station values (vj, vk) and clear tags (qj, qk)
		for( uint8_t i = 1; i <= 5; i++ )
		{
			if( rs[i].qj == temp.dst )
			{
				rs[i].qj = 0;
				rs[i].vj = temp.result;
			}
			
			if( rs[i].qk == temp.dst )
			{
				rs[i].qk = 0;
				rs[i].vk = temp.result;
			}
		}
		
		// update re-order buffer value
		rob[temp.dst].value = temp.result;
		
		// reset temporaryContainer
		temp.busy = false;
		temp.exception = false;
	}
}

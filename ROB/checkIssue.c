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
	if( issuePointer == 0)
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
                rs[i].busy = true;
                rs[i].op = rob[issuePointer].op = instructions[instructionIndex].op;
				rs[i].dst = issuePointer;
                rob[issuePointer].dst = instructions[instructionIndex].dst;

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
            
                issuedSuccessfully = true;
            }
        }

        if( issuedSuccessfully )
        {
            instructionPosition++; // move instruction queue to next instruction
			
			if( issuePointer == 6 )
			{
				issuePointer = 0; // ROB is full
			}
			else
			{
				issuePointer++; // move issue pointer in ROB to next position
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
                rs[i].busy = true;
                rs[i].op = rob[issuePointer].op = instructions[instructionIndex].op;
				rs[i].dst = issuePointer;
                rob[issuePointer].dst = instructions[instructionIndex].dst;

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
            
                issuedSuccessfully = true;
            }
        }

        if( issuedSuccessfully )
        {
            instructionPosition++; // move instruction queue to next instruction
			
			if( issuePointer == 6 )
			{
				issuePointer = 0; // ROB is full
			}
			else
			{
				issuePointer++; // move issue pointer in ROB to next position
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

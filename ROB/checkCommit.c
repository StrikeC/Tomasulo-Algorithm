/*
 *  Function: checkCommit
 *  Parameters: None
 *  Returns: Bool
 *  Description: check if values in re-order buffer are committed
 */
bool checkCommit()
{
	if( rob[commitPointer].exception ) // destroy everything and exit
	{
		// clear rob
		for( uint8_t i = 1; i <= 6; i++ )
		{
			rob[i].busy = rob[i].done = rob[i].exception = false;
			rob[i].op = rob[i].dst = rob[i].value = 0;
		}
		
		// clear rs
		for( uint8_t i = 1; i <= 5; i++ )
		{
			rs[i].busy = false;
			rs[i].op = rs[i].dst = rs[i].vj = rs[i].vk = rs[i].qj = rs[i].qk = 0;
		}
		
		// clear rat
		registerAllocationTable = {0,0,0,0,0,0,0,0};
		
		// return exit
		return true;
		
	}
	else
	{
		// reset rob
		temp.robDstCom = commitPointer;
		
		// update RF/RAT
		if( registerAllocationTable[rob[commitPointer].dst] = commitPointer)
		{
			registerAllocationTable[rob[commitPointer].dst] = 0;
			registerFile[rob[commitPointer].dst] = rob[commitPointer].value;
		}
		
		// move pointer to the next 
		commitPointer++;
	}
	
}

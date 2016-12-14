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
			temp.busyBro = true;
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
			temp.busyBro = true;
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

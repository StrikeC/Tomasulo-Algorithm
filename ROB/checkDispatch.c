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

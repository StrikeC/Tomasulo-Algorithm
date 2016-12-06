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

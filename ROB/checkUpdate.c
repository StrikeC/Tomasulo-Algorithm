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
		rob[temp.dst].done = true;
		
		// reset reservation station - from DISPATCH
		rs[temp.rsDstAdd].busy = false;
		rs[temp.rsDstAdd].op = rs[temp.rsDstAdd].dst = rs[temp.rsDstAdd].vj = rs[temp.rsDstAdd].vk = 0;
		rs[temp.rsDstMul].busy = false;
		rs[temp.rsDstMul].op = rs[temp.rsDstMul].dst = rs[temp.rsDstMul].vj = rs[temp.rsDstMul].vk = 0;
		
		// reset re-order buffer - from COMMIT
		rob[temp.robDstCom].busy = rob[temp.robDstCom].done = rob[temp.robDstCom].exception = false;
		rob[temp.robDstCom].op = rob[temp.robDstCom].dst = rob[temp.robDstCom].value = 0;
		
		// reset temporaryContainer
		temp.busy = false;
		temp.exception = false;
	}
}

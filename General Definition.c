/*
	General definition
	IQ(10 ROW MAX)
	|Operation|Destination|Source1|Source2|Issue|
	|0-3	  |1-8        |1-8    |1-8    |0/1  |
	
	RF(8 ROW MAX)
	|Value|RAT    |
	|x    |0/10-50|
	
	RS(5 ROW MAX)
	|Busy|Operation|Destination|Value1|Value2|Tag1   |Tag2   |Dispatch|Execution Done Cycle|Execution Done|Instruction Number|Result|
	|0/1 |0-3      |1-8        |x     |x     |0/10-50|0/10-50|0/1     |DC+n-1              |0/1           |0-9               |x     |
	
	addUnitOccupy: mark down if add unit is occupied now
	mulUnitOccupy: mark down if mul unit is occupied now

*/
int iq[10][5];
int rf[8][2];
int rs[5][11];
int addUnitOccupy = 0;
int mulUnitOccupy = 0;

int main() {
	int cycle = 1;
	while (cycle <= cycleReqst){
		issue();
		dispatch(cycle);
		execute(cycle);
		broadcast();
		//test if all instructions are issued with all RS are freed
		if (finish() = 1) break;
		
		cycle++;
	}
	//Result display code here ~later~
}

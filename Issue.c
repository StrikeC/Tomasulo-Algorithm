//Issue
void issue (){
	int pointer; // rf pointer
	for (int i=0;i<10;i++){
		if (iq[i][0]==0){
			//Start issue
			//ADD/SUB op
			if ((iq[i][1]==0)||(iq[i][1]==1)){
				for (int j=0;j<3;j++){
					//Find free RS
					if (rs[j][0] ==0){
						iq[i][0] = 1; //set IQ issue as 1
						rs[j][0] = 1; //set RS busy as 1
						rs[j][4] = i; //set RS instruction number
						rs[j][5] = iq[i][1]; //set RS operation
						rs[j][6] = iq[i][2]; //set RS destination
						//set RS value1/2 or tag1/2
						for (int s=3;s<=4;v++){ 
							pointer = iq[i][s]-1;
							if (rf[pointer][1]==0)
								rs[j][s+4] = rf[pointer][0];
							else
								rs[j][s+6] = rs[pointer][1];
						}
						
						break;
					}
				}
			}
			//MUL/DIV op
			else {
				for (int j=3;j<5;j++){
					//Find free RS
					if (rs[j][0] ==0){
						iq[i][0] = 1; //set IQ issue as 1
						rs[j][0] = 1; //set RS busy as 1
						rs[j][4] = i; //set RS instruction number
						rs[j][5] = iq[i][1]; //set RS operation
						rs[j][6] = iq[i][2]; //set RS destination
						//set RS value1/2 or tag1/2
						for (int s=3;s<=4;v++){ 
							pointer = iq[i][s]-1;
							if (rf[pointer][1]==0)
								rs[j][s+4] = rf[pointer][0];
							else
								rs[j][s+6] = rs[pointer][1];
						}
						
						break;
					}
				}
			}
			
			break;
		}
	}
}
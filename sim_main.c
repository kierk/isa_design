//#########################
// EC535 - Hw2_pt2
// kierke@bu.edu
// sim_main.c - simulator
//#########################

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


//#########################
//  Globals
//#########################
int instr_count = 1;  //might need to change
int exec_count = 0;
int cycle_count = 0;
int hit_count = 0;
int ldst_count = 0;
int8_t registers[6];
int8_t cache[256];
uint8_t v_cache[256] = {0};
int low_instr = 9999; //max number of instructions that program will work with
int high_instr = 0;
int zf = 0; //zero flag for comps


//#########################
//  Execution Function
//#########################

void exec_asm(char instr[], int8_t ops[][2]){
	int ip = low_instr;
//	printf("Low instr: %d, high instr: %d\n", low_instr, high_instr);

	while(ip <= high_instr){
		exec_count++;
		switch(instr[ip]){
			case 'm':
				registers[ops[ip][0]] = ops[ip][1];
				ip++;
				cycle_count++;
			break;
			case 'a':
				registers[ops[ip][0]] = registers[ops[ip][0]] + registers[ops[ip][1]];
				ip++; // cycles
				cycle_count++;
			break;
			case 'i':
				registers[ops[ip][0]] = registers[ops[ip][0]] + ops[ip][1];
				ip++;
				cycle_count++;
			break;
			case 'l':
				registers[ops[ip][0]] = cache[ ops[ip][1] ];
				if(v_cache[registers[ops[ip][1]]] == 0){
					v_cache[registers[ops[ip][1]]] = 1;
					cycle_count+=40;
				} else {
					cycle_count+=2;
					hit_count++;
				}
				ip++;
				ldst_count++;
			break;
			case 's':
				cache[ops[ip][0]] = registers[ops[ip][1]];
				if(v_cache[registers[ops[ip][0]]] == 0){
					v_cache[registers[ops[ip][0]]] = 1;
					cycle_count+=40;
				} else{
					cycle_count+=2;
					hit_count++;
				}
				ip++;
				ldst_count++;
			break;
			case 'c':
				if (registers[ ops[ip][0] ] == registers[ ops[ip][1] ])
					zf = 1;
				else
					zf = 0;
				ip++;
				cycle_count+=2;
			break;
			case 'j':
				if(zf == 1)
					ip = ops[ip][0];
				else
					ip++;
				cycle_count++;
			break;
			case 'J':
				printf("Infinite jumps..at %d, jumping to instr %d\n", ip, ops[ip][0]);
				ip = ops[ip][0];
				cycle_count++;
			break;
			default:
				printf("error:  %c\n", instr[ip]);
		}
	}
}


//#########################
//  Main Block
//#########################

int main(int argc, char* argv[]){

	//-----------------------
	// Find # of Instructs:
	FILE * asm_file_line = fopen(argv[1], "r");
	char ch;
	while(!feof(asm_file_line))
	{
	  ch = fgetc(asm_file_line);
	  if(ch == '\n')
	  {
	    instr_count++;
	  }
	}
	fclose(asm_file_line);

	//printf("Debugging.. past count %d\n", instr_count);
	//Compile array of instructions
	FILE * asm_file = fopen(argv[1], "r");

	char instr[1000];
	int8_t ops[1000][2];
	int i = 0;
	char op_code;
	char line[256];
	char* str_token;

	//---------------------------------
	// Time to Read the asm file:
	// Pull all of the instructions
	while (fgets(line, sizeof(line), asm_file)) {

		//Get rid of address
		str_token = strtok(line, "\t");

		//Set instruction start and end
		i = atoi(str_token);
		if(i < low_instr)
			low_instr = i;
		if(i > high_instr)
			high_instr = i;

		//Pull instr
		str_token = strtok(NULL, " ");


		if(strcmp("MOV", str_token) == 0){ 
			str_token = strtok(NULL, ",");
			str_token++;
			ops[i][0] = (int8_t)atoi(str_token);
			str_token = strtok(NULL, " ");
			ops[i][1] = (int8_t)atoi(str_token);
			instr[i] = 'm';
		}
		else if(strcmp("ADD", str_token) == 0){ 
			str_token = strtok(NULL, ",");
			str_token++;
			ops[i][0] = (int8_t)atoi(str_token);
			str_token = strtok(NULL, " ");
			if(str_token[0] = 'R'){
				str_token++;
				instr[i] = 'a';
			}
			else{
				instr[i] = 'i';
			}
			ops[i][1] = (int8_t)atoi(str_token);			

		}
		else if(strcmp("LD", str_token) == 0){ 
			str_token = strtok(NULL, ",");
			str_token++;
			ops[i][0] = (int8_t)atoi(str_token);

			str_token = strtok(NULL, "]");
			str_token+=3;
			ops[i][1] = (int8_t)atoi(str_token);
			instr[i] = 'l';
		}
		else if(strcmp("ST", str_token) == 0){ 
			str_token = strtok(NULL, "]");
			str_token += 2;
			ops[i][0] = (int8_t)atoi(str_token);

			str_token = strtok(NULL, "]");
			str_token += 3;
			ops[i][1] = (int8_t)atoi(str_token);
			instr[i] = 's';
		}
		else if(strcmp("CMP", str_token) == 0){ 
			str_token = strtok(NULL, ",");
			str_token++;
			ops[i][0] = (int8_t)atoi(str_token);

			str_token = strtok(NULL, " ");
			str_token++;
			ops[i][1] = (int8_t)atoi(str_token);
			instr[i] = 'c';
		}
		else if(strcmp("JE", str_token) == 0){ 
			str_token = strtok(NULL, " ");
			ops[i][0] = (int8_t)atoi(str_token);
			
			instr[i] = 'j';
		}
		else if(strcmp("JMP", str_token) == 0){ 
			str_token = strtok(NULL, " ");
			ops[i][0] = (int8_t)atoi(str_token);

			instr[i] = 'J';
		}
		else{
			printf("Issue with the instruction read..\n"); 
			exit(0);
		}


	}
	fclose(asm_file);

	//printf("Debugging.. past read %d\n");

	//----------------------------------
	// Execute instructions

	exec_asm(instr, ops);


	//----------------------------------
	// Print stats
	printf("Total number of instructions in the code: %d\n", instr_count);	
	printf("Total number of executed instructions: %d\n", exec_count);
	printf("Total number of clock cycles: %d\n", cycle_count);
	printf("Number of hits to local memory: %d\n", hit_count);
	printf("Total number of executed LD/ST instructions: %d\n", ldst_count);



	return 0;
}


//#########################
// 
//#########################
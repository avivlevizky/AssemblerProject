
#include "Insertion.h"
#include "Checkers.h"
#include "Assembler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*private function that create/expand the Instruction table by type Of Order*/
void createNewSpaceToITtable(int typeOfOrder)
{
	
	if (!instructions_table) 
    {
		instructions_table = (Instruction **)malloc(sizeof(Instruction *));
		allocate_check(instructions_table);
	}
	else
	{
		Instruction **temp;
		temp = (Instruction **)realloc(instructions_table, sizeof(Instruction*)*(IC + 1));
		allocate_check(temp);
		instructions_table = temp;
	}
	

	instructions_table[IC] = (Instruction *)malloc(sizeof(Instruction));
	allocate_check(instructions_table[IC]);

	switch (typeOfOrder)
	{
	    case 1: /*if type Order is InstructOrder*/
	    {
		    instructions_table[IC]->order = (InstructOrder *)malloc(sizeof(InstructOrder));
		    allocate_check(instructions_table[IC]->order);
	    	break;
	    }

	    case 2: /*if type Order is InstructData*/
	    {
		    instructions_table[IC]->order = (InstructData *)malloc(sizeof(InstructData));
	    	allocate_check(instructions_table[IC]->order);
	    	break;
	    }
	    case 3: /*if type Order is InstructRegister*/
	    {
	    	instructions_table[IC]->order = (InstructRegisters *)malloc(sizeof(InstructRegisters));
	    	allocate_check(instructions_table[IC]->order);
	    	break;
    	}
    }
}




/*private function that create/expand the Data table*/
void CreateNewSpaceForDT()
{

	if (!data_table)
	{
		data_table = (int *)malloc(sizeof(int));
		allocate_check(data_table);
	}
	else
	{
		int* temp_data;
		temp_data = (int *)realloc(data_table, sizeof(int)*(DC + 1));
		allocate_check(temp_data);
		data_table = temp_data;
	}
}





















/*private function ...*/
void insertToItForOperand(char * data, int operand, int isOriginOperand)
{
	int * value;
	char ** mat_data;
	InstructData dataOrder;
	InstructRegisters regOrder;

	value = NULL;
	/*If the given operand is register*/
	if (operand == 3)
	{
		regOrder.reg1 = 0;
		regOrder.reg2 = 0;
		value = isNumeric(data + 1);

		if (isOriginOperand)
			regOrder.reg1 = *value;
		else
			regOrder.reg2 = *value;

		createNewSpaceToITtable(3);
		*(((InstructRegisters *)(instructions_table[IC]->order))) = regOrder;
		instructions_table[IC]->type_order = 2;
		IC++;
		free(value);
		return;
	}

	/*If the given operand is label,mat or direct addressing*/
	if (operand == 0)
	{
		value = isNumeric(data + 1);
		dataOrder.value = *value;
		free(value);
	}

	dataOrder.type_coding = 0;
	createNewSpaceToITtable(2);
	*(((InstructData *)(instructions_table[IC]->order))) = dataOrder;
	instructions_table[IC]->type_order = 1;
	IC++;

	if (operand == 2)
	{
		mat_data = isValidMatrix(data);
		regOrder.reg1 = (*isNumeric((mat_data[1] + 1)));
		regOrder.reg2 = (*isNumeric((mat_data[2] + 1)));
        
		/*allocate new place registers coding*/
		InstructOrder *testo = (InstructOrder *)(instructions_table[0]->order);
		createNewSpaceToITtable(3);
		*(((InstructRegisters *)(instructions_table[IC]->order))) = regOrder;
		instructions_table[IC]->type_order = 2;
		IC++;
		freeLinkedList(mat_data, 3);
	}
}



void insertToItForOperandSecond(char * data, int operand)
{
	int index;
	char *label;
	if (operand == 2)
		label = (isValidMatrix(data))[0];
	else
		label = data;
	index = findSymbol(label);

	if (index == -1)
		insertNewError("Symbol isn't decleared in Line: %d");
	else
	{
		if (((operand == 1) && (((symbol_table[index]->type) == DATA) || ((symbol_table[index]->type) == STRING) || ((symbol_table[index]->type) == EXTERN))) || ((operand == 2) && ((symbol_table[index]->type) == MAT)))
		{
			InstructData *temp;

			temp = ((InstructData*)(instructions_table[IC]->order));
			temp->value = (symbol_table[index]->dec_value) + Total_IC;

			if ((symbol_table[index]->type) == EXTERN)
			{
				temp->value = 0;
				temp->type_coding = 1;
			}
			else
				temp->type_coding = 2;
		}
		else
			insertNewError("The symbol isn't matched to the type of the decleared symbol Line: %d");

		IC = IC + operand;
	}
}










/*Function that insert the symbol into the symbols tabel by the type: 16 or 17 or 18 is data;19 is .entry and 20 is .extern otherwise is instruction type. and update SC counter too*/
void insertSymbolToTable(char *data, int type)
{
	int isDotDot;
	Symbol* temp;

	isDotDot = 0;
	if (data[strlen(data) - 1] == ':')
		isDotDot = 1;

	temp = (Symbol*)malloc(sizeof(Symbol));
	allocate_check(temp);

	temp->label_name = (char *)calloc(sizeof(char), strlen(data) - isDotDot);
	allocate_check((temp->label_name));

	strcpy(temp->label_name, data);
	/*need to add check for the copy operation*/

	(temp->label_name)[strlen(temp->label_name) - isDotDot] = '\0';
	temp->type = type;

	if (type == EXTERN)
		temp->dec_value = 0;
	else
		temp->dec_value = DC;

	if (findSymbol(temp->label_name) != -1) {
		insertNewError("The symbol is already decleared in Line: %d");
		free(temp);
	}
	if (!symbol_table)
	{
		symbol_table = (Symbol **)malloc(sizeof(Symbol*));
		allocate_check(symbol_table);
	}
	else
	{
		Symbol ** temp_ST;
		temp_ST = (Symbol **)realloc(symbol_table,sizeof(Symbol*)*(SC + 1));
		allocate_check(temp_ST);
		symbol_table = temp_ST;
	}
	symbol_table[SC] = temp;
	SC++;
}







/* Function that insert the given data into the instructions_table, the function will update too the IC counter by matching to the length of the given data */
void FirstCheckingCommand(char ** command)
{
	int flag_symbol_type;

	/*if the given string list is null/empty */
	if (!(*command))
		return;
    
	/*In the case that the first string on the current command line is a label(symbol) */
	if ((isValidLabel((command[0]), 1)) && (((flag_symbol_type = isInstruction(command[1], 1)) >= 0)))
	{
		/*if the instrct type is an data or extern*/
		if (((flag_symbol_type >= DATA) && (flag_symbol_type <= MAT)) || (flag_symbol_type == EXTERN))
		{
			insertSymbolToTable(command[0], flag_symbol_type);
			insertToDT(&command[2], flag_symbol_type);
		}
		/*if the instrct type is an instruction*/
		else if (flag_symbol_type <= 15)
		{
			insertSymbolToTable(command[0], flag_symbol_type);
			insertToIT(&command[2], flag_symbol_type);   /*the command[2] is first operand*/
		}
	}
	else /*if the commands[0] isn't label*/
	{
		if ((flag_symbol_type = isInstruction(command[0], 0)) >= 0)
		{
			if ((flag_symbol_type >= DATA) && (flag_symbol_type <= MAT))
			{
				insertToDT(&command[1], flag_symbol_type);
				return;
			}
			if (flag_symbol_type >= 19)
			{
				if (flag_symbol_type == 20) /*if is .extern insruct type then we will enter the command into the symbol table*/
					insertSymbolToTable(command[1], flag_symbol_type);
			}
			else
				insertToIT(&command[1], flag_symbol_type);  /*the command[1] is first operand*/
		}
		else if (!isValidLabel((command[0]), 1))
			    insertNewError("Unidentified command line: %d");
	}
}







/*Function that insert data by the given Instruc_type argument into the data table*/
void insertToDT(char **data, int type)
{
	int* value;
	;
	int i;
	char *reader;

	i = 0;
	value = NULL;
	switch (type) {
	    case DATA: /*if the type is .data*/
	    {
		    while ((reader = data[i]))
		    {
		    	value = isNumeric(reader);
		    	if (!value)
			    {
				    insertNewError("The data defining isn't valid in Line: %d");
				    free(value);
				    return;
			    }
			    CreateNewSpaceForDT();
			    data_table[DC] = *value;
			    DC++;
			    i++;
		    }
	    }
	    break;
            
	    case STRING: /*if the type is .string*/
	    {
            char ch;
		    reader = data[0];

		    if ((!reader[0]) || (reader[0] != '"'))
	    	{
		    	insertNewError("The string defining isn't valid in Line: %d");
	    		return;
	    	}
	    	i = 1;
	    	while ((reader[i]) && (ch = reader[i]))
		    {
		    	if (reader[i] == '"')
		    	{
			    	if (reader[i + 1])
			    		insertNewError("The string defining isn't valid in Line: %d");
				    return;
			    }
		    	CreateNewSpaceForDT();
		    	data_table[DC] = ch;
		    	DC++;
		    	i++;
		    }
	    	insertNewError("The string defining missing \" token in Line: %d");
    	}
	    break;

	    case MAT: /*if the type is .mat*/
	    {
		    int n;
	    	n = isValidMatrixToData(data[0]);
    
	    	if (n == -1)
		    	return;

		    i = 0;

		    while ((reader = data[i + 1]))
	    	{
		    	value = isNumeric(reader);
		    	if (!value)
			    {
			    	insertNewError("The matrix defining isn't valid in Line: %d");
		    		free(value);
		    		return;
			    }
			    CreateNewSpaceForDT();
			    data_table[DC] = *value;
		    	free(value);
			    DC++;
			    i++;
		    }
	    	if (i != n) {
		    	insertNewError("The defining matrix values are not equel to declaring matrix: %d");
		    	free(value);
	    	}
    	}
    	break;
   	}
}




/*Update the given instruction command for the second checking*/
void updateInstruction(char **data, int Instruc_type)
{
	int orgOperand, destOperand;

	destOperand = -2;
	orgOperand = checkAddressingType(data[0]);

	if (orgOperand != -2)
		destOperand = checkAddressingType(data[1]);

	IC++;

	if ((orgOperand == 3) && (destOperand == 3))
	{
		IC++;
		return;
	}

	if (orgOperand != -2)
	{
		if ((orgOperand == 1) || (orgOperand == 2))
			insertToItForOperandSecond(data[0], orgOperand);
		else
			IC++;
	}

	if (destOperand != -2)
	{
		if ((destOperand == 1) || (destOperand == 2))
			insertToItForOperandSecond(data[1], destOperand);
		else
			IC++;
    }
}

    
    











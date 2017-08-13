
#include "Insertion.h"
#include "Checkers.h"
#include "Assembler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



/*private function that create create/expand the Instruction table by type Of Order*/
void createNewSpaceToITtable(int typeOfOrder)
{
    Instruction **temp;
    if(!instructions_table){
        instructions_table=(Instruction **)malloc(sizeof(Instruction *));
        allocate_check(instructions_table);
    }
    
    temp=(Instruction **)malloc(sizeof(Instruction**)*(IC+1));
    allocate_check(temp);
    
    memmove(temp,instructions_table,IC*sizeof(Instruction**));
    
    instructions_table=temp;
    instructions_table[IC]=(Instruction *)malloc(sizeof(Instruction));
    allocate_check(instructions_table[IC]);
    
    switch (typeOfOrder)
    {
        case 1: /*if type Order is InstructOrder*/
        {
            instructions_table[IC]->order=(InstructOrder *)malloc(sizeof(InstructOrder));
            allocate_check(instructions_table[IC]->order);
            break;
        }
            
        case 2: /*if type Order is InstructData*/
        {
            instructions_table[IC]->order=(InstructData *)malloc(sizeof(InstructData));
            allocate_check(instructions_table[IC]->order);
            break;
            
        }
        case 3: /*if type Order is InstructRegister*/
        {
            instructions_table[IC]->order=(InstructRegisters *)malloc(sizeof(InstructRegisters));
            allocate_check(instructions_table[IC]->order);
            break;
            
        }
    }
    
}


/*private function ...*/
void insertToItForOperand(char * data,int operand, int isOriginOperand)
{
    int * value;
    char ** mat_data;
    InstructData dataOrder;
    InstructRegisters regOrder;
    
    
    /*If the given operand is register*/
    if(operand==3)
    {
        regOrder.reg1=0;
        regOrder.reg2=0;
        value=isNumeric(data+1);
        
        if(isOriginOperand)
            regOrder.reg1=*value;
        else
            regOrder.reg2=*value;
        
        createNewSpaceToITtable(3);
        *(((InstructRegisters *)(instructions_table[IC]->order)))=regOrder;
        instructions_table[IC]->type_order=2;
        IC++;
        
        return;
    }
    
    
    /*If the given operand is label,mat or direct addressing*/
    if(operand==0)
    {
        value=isNumeric(data+1);
        dataOrder.value=*value;
    }
    
    
    dataOrder.type_coding=0;
    
    createNewSpaceToITtable(2);
    *(((InstructData *)(instructions_table[IC]->order)))=dataOrder;
    instructions_table[IC]->type_order=1;
    IC++;
    
    if(operand==2)
    {
        mat_data=isValidMatrix(data);
        regOrder.reg1=(*isNumeric((mat_data[1]+1)));
        regOrder.reg2=(*isNumeric((mat_data[2]+1)));
        
        
        /*allocate new place registers coding*/
        createNewSpaceToITtable(3);
        *(((InstructRegisters *)(instructions_table[IC]->order)))=regOrder;
        instructions_table[IC]->type_order=2;
        IC++;
        freeLinkedList(mat_data,3);
        
    }
    
}




void insertToItForOperandSecond(char * data,int operand)
{
    int index;
    char *label;
    if(operand==2)
        label=(isValidMatrix(data))[0];
    else
        label=data;
    index=findSymbol(label);

    if(index==-1)
        insertNewError("Symbol isn't decleared in Line: %d");
    else
    {
        ((InstructData*)(instructions_table[IC]->order))->value=symbol_table[index]->dec_value;
        if((symbol_table[index]->type)==EXTERN)
            ((InstructData*)(instructions_table[IC]->order))->type_coding=1;
        else
            ((InstructData*)(instructions_table[IC]->order))->type_coding=2;
    }
    
    IC=IC+operand;
    
}










/*Function that insert the symbol into the symbols tabel by the type: 16 or 17 or 18 is data;19 is .entry and 20 is .extern otherwise is instruction type. and update SC counter too*/
void insertSymbolToTable(char *data,int type)
{
    Symbol* temp;
    
    temp = (Symbol*)malloc(sizeof(Symbol));
    allocate_check(temp);
    
    temp->label_name=(char *)calloc(sizeof(char), strlen(data)-1);
    allocate_check((temp->label_name));
    
    strcpy(temp->label_name, data);
    /*need to add check for the copy operation*/
    
    (temp->label_name)[strlen(data)-1]='\0';
    temp->type = type;
    
    if(type==EXTERN)
        temp->dec_value=0;
    else
        temp->dec_value=DC;

    if(findSymbol(temp->label_name)!=-1)
        insertNewError("The symbol is already decleared in Line: %d");
    if (!symbol_table)
    {
        symbol_table = (Symbol **)malloc(sizeof(Symbol*));
        allocate_check(symbol_table);
    }
    else
    {
        symbol_table = (Symbol **)realloc(symbol_table,SC+1);
        allocate_check(symbol_table);
    }
    symbol_table[SC]=temp;

    SC++;
    
}










/* Function that insert the given data into the instructions_table, the function will update too the IC counter by matching to the length of the given data */
void insertToIT(char **data,int Instruc_type)
{
    int orgOperand,destOperand;
    InstructOrder order;
    
    destOperand=-2;
    orgOperand=checkAddressingType(data[0]);
    if(orgOperand!=-2)
        destOperand=checkAddressingType(data[1]);
    
    
    if((orgOperand==-1)||(destOperand==-1))
    {
    Failure: insertNewError("Invalid operands in Line: %d");
        return;
    }
    
    /*if the instruct type is dual places*/
    if ((Instruc_type<=SUB)||(Instruc_type==LEA))
    {
        if((orgOperand==-2)||(destOperand==-2))
            goto Failure;
        
        if(Instruc_type==LEA)
        {
            if((orgOperand==0)||(orgOperand==3))
                goto Failure;
        }
        else
        {
            if((Instruc_type!=CMP)&&(destOperand==0))
                goto Failure;
        }
    }
    
    else
    {   /*if the instruct type is sole place*/
        if (((Instruc_type>=NOT)&&(Instruc_type<=CLR))||((Instruc_type>=INC)&&(Instruc_type<=JSR)))
        {
            if((orgOperand==-2)||(destOperand!=-2))
                goto Failure;
            
            if((Instruc_type!=PRN)&&(destOperand==0))
                goto Failure;
        }
        else  /*In the condition of Instruc_type==rts or Instruc_type==stop */
        {
            if((orgOperand!=-2)||(destOperand!=-2))
                goto Failure;
        }
    }
    /*if the operand/s are valid then: */
    order.type_coding=0;
    order.origin_addressing=(orgOperand==-2) ? 0 : orgOperand;
    order.dest_addressing=(destOperand==-2) ? 0 : destOperand;
    order.opcode=Instruc_type;
    
    createNewSpaceToITtable(1);
    *(((InstructOrder *)(instructions_table[IC]->order)))=order;
    instructions_table[IC]->type_order=0;
    IC++;
    
    /*if both of the operands are registers*/
    if((orgOperand==3)&&(destOperand==3))
    {
        InstructRegisters regOrder;
        
        regOrder.reg1=(*isNumeric(data[0]+1));
        regOrder.reg2=(*isNumeric(data[1]+1));
        
        createNewSpaceToITtable(3);
        *(((InstructRegisters *)(instructions_table[IC]->order)))=regOrder;
        instructions_table[IC]->type_order=2;
        IC++;
        
    }
    else
    {
        if(orgOperand!=-2)
            insertToItForOperand(data[0],orgOperand,1);
        
        if(destOperand!=-2)
            insertToItForOperand(data[1],destOperand,0);
    }
    
    
}







/*Function that insert data by the given Instruc_type argument into the data table*/
void insertToDT(char **data,int type)
{
    int* value;
    int i;
    char *reader;
    
    i=0;
    
    switch (type) {
        case DATA: /*if the type is .data*/
        {
            while((reader=data[i]))
            {
                value=isNumeric(reader);
                if(!value)
                {
                    insertNewError("The data defining isn't valid in Line: %d");
                    return;
                }
                data_table=(int*)realloc(data_table,DC+1);
                data_table[DC]=*value;
                DC++;
                i++;
            }
            
        }
            break;
            
        case STRING: /*if the type is .string*/
        {
            char ch;
            /* need to check when reader is null */
            reader=data[0];
            
            if ((!reader[0])||(reader[0]!='"'))
            {
                insertNewError("The string defining isn't valid in Line: %d");
                return;
            }
            i=1;
            while((reader[i])&&(ch=reader[i]))
            {
                if(reader[i]=='"')
                {
                    if(reader[i+1])
                        insertNewError("The string defining isn't valid in Line: %d");
                    
                    return;
                    
                }
                data_table=(int*)realloc(data_table,DC+1);
                data_table[DC]= ch;
                DC++;
                i++;
            }
            insertNewError("The string defining missing \" token in Line: %d");
            
            
        }
            break;
            
        case MAT: /*if the type is .mat*/
        {
            int n;
            
            n=isValidMatrixToData(data[0]);
            
            if(n==-1)
            {
                insertNewError("The matrix defining isn't valid in Line: %d");
                return;
            }
            
            i=0;
            
            while((reader=data[i+1]))
            {
                value=isNumeric(reader);
                if(!value)
                {
                    insertNewError("The matrix defining isn't valid in Line: %d");
                    free(value);
                    return;
                }
                
                data_table=(int*)realloc(data_table,DC+1);
                data_table[DC]=*value;
                free(value);
                DC++;
                i++;
            }
            if (i!=n)
                insertNewError("The defining matrix values are not equel to declaring matrix: %d");
        }
            break;
    }
    
}

/*Update the given instruction command for the second checking*/
void updateInstruction(char **data,int Instruc_type)
{
    int orgOperand,destOperand;
    
    destOperand=-2;
    orgOperand=checkAddressingType(data[0]);
    
    if(orgOperand!=-2)
        destOperand=checkAddressingType(data[1]);

    
    IC++;

    
    if((orgOperand==1)||(orgOperand==2))
        insertToItForOperandSecond(data[0],orgOperand);
        
    if((destOperand==1)||(destOperand==2))
        insertToItForOperandSecond(data[1],destOperand);
    
    
}


    
    











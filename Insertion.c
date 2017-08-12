
#include "Insertion.h"
#include "Checkers.h"
#include "Assembler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



void insertToItForOperand(char ** data,int operand)
{
    int * value;
    char ** mat_data;
    InstructData dataOrder;
    InstructRegisters regOrder;
    
    
    
    if((operand==0)||(operand==3))
    {
        value=isNumeric(data[0]+1);
        dataOrder.value=*value;
    }
    
    
    dataOrder.type_coding=0;
    instructions_table[IC]=(InstructData *)malloc(sizeof(InstructData));
    instructions_table[IC]=&dataOrder;
    IC++;
    
    if(operand==2)
    {
        mat_data=isValidMatrix(data[0]);
        regOrder.reg1=(*isNumeric((mat_data[1]+1)));
        regOrder.reg2=(*isNumeric((mat_data[2]+1)));
        regOrder.type_coding=0;
        instructions_table[IC]=(InstructRegisters *)malloc(sizeof(InstructRegisters));
        instructions_table[IC]=&regOrder;
        IC++;
        freeLinkedList(data);

    }
    
}











/*Function that insert the symbol into the symbols tabel by the type: 16 or 17 or 18 is data;19 is .entry and 20 is .extern otherwise is instruction type. and update SC counter too*/
void insertSymbolToTable(char *data,int type)
{
    Symbol* temp;
    temp = (Symbol*)malloc(sizeof(Symbol));
    allocate_check(temp);
    temp->type = type;
    temp->label_name = data;
    temp->dec_value=DC;
    
    if(findSymbol(data)!=-1)
        insertNewError("The symbol is already decleared in Line: %d");
    if (!symbol_table)
    {
        symbol_table = (Symbol **)malloc(sizeof(Symbol*));
        allocate_check(symbol_table);
        *symbol_table = temp;
    }
    else
    {
        symbol_table = (Symbol **)realloc(symbol_table,SC+1);
        allocate_check(symbol_table);
        symbol_table[SC]=temp;
    }
    
    SC++;
    
}



/* Function that insert the given data into the instructions_table, the function will update too the IC counter by matching to the length of the given data */
void insertToIT(char **data,int Instruc_type)
{
    int orgOperand,destOperand;
    InstructOrder order;

    orgOperand=checkAddressingType(data[0]);
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
    /*if the instruct type is sole place*/
    if (((Instruc_type>=NOT)&&(Instruc_type<=CLR))||((Instruc_type>=INC)&&(Instruc_type<=JSR)))
    {
        if((orgOperand!=-2)||(destOperand==-2))
            goto Failure;
        
        if((Instruc_type!=PRN)&&(destOperand==0))
            goto Failure;
    }
    else  /*In the condition of Instruc_type==rts or Instruc_type==stop */
    {
        if((orgOperand!=-2)||(destOperand!=-2))
            goto Failure;
    }
    
    
    /*if the operand/s are valid then: */
    order.type_coding=0;
    order.origin_addressing=orgOperand;
    order.dest_addressing=destOperand;
    order.opcode=Instruc_type;
    
    
    instructions_table[IC]=(InstructOrder *)malloc(sizeof(InstructOrder));
    instructions_table[IC]=&order;
    IC++;

    if(orgOperand!=-2)
        insertToItForOperand(data,orgOperand);
    
    if(destOperand!=-2)
        insertToItForOperand(data,destOperand);
    
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
            // need to check when reader is null
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
    
    
}











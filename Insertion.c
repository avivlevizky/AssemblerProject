
#include "Insertion.h"
#include "Checkers.h"
#include "Assembler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*Function that insert the symbol into the symbols tabel by the type: 16 or 17 or 18 is data;19 is .entry and 20 is .extern otherwise is instruction type. and update SC counter too*/
void insertSymbolToTable(char *data,int type)
{
    Symbol* temp;
    temp = (Symbol*)malloc(sizeof(Symbol));
    allocate_check(temp);
    temp->type = type;
    temp->label_name = data;
    temp->dec_value=DC;
    
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
    /*if the instruct type is dual places*/
    if ((Instruc_type<=SUB)||(Instruc_type==LEA))
    {
        
        
        
        return;
    }
    /*if the instruct type is sole place*/
    if (((Instruc_type>=NOT)&&(Instruc_type<=CLR))||((Instruc_type>=INC)&&(Instruc_type<=JSR)))
    {
        
        
        
    }
    else  /*In the condition of Instruc_type==rts or Instruc_type==stop */
    {
        
        
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
                    insertNewError("The data defining isn't valid in Line: ");
                    return;
                }
                
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
            if ((data[1])||(reader[0]!='"'))
                {
                   insertNewError("The string defining isn't valid in Line: ");
                   return;
                }
            i=1;
            while((ch=reader[i]))
                {
                   data_table=(int*)realloc(data_table,DC+1);
                   data_table[DC]= ch;
                   DC++;
                   i++;
                }
        }
            break;
            
        case MAT: /*if the type is .mat*/
        {
            char *reader;
            int n,i;
            
            n=isValidMatrixToData(data[0]);
            i=0;
            if(n<1)
                {
                   return;
                }
            while((reader=data[i+1]))
                {
                   value=isNumeric(reader);
                if(!value)
                {
                    ("The data defining isn't valid in Line: ");
                    return;
                }
                
                data_table[DC]=*value;
                DC++;
                i++;
                }
            if (i!=n)
                insertNewError("The defining matrix values are not equel to declaring matrix");
        }
            break;
    }
    
}

/*Update the given instruction command for the second checking*/
void updateInstruction(char **data,int Instruc_type)
{
    
    
}













#include "Checkers.h"
#include "Assembler.h"

#include <string.h>

#include <stdio.h>


/*Private Aux function: which indicate if the given char is an alpha-bet charcter*/
int isGoodLetter (char toTest){
    
    int tester = (int)toTest;
    if( (tester>= 65 && tester<=90) || (tester>= 97 && tester<=122) || (tester>= 48 && tester<=57) )
    {return 1;}
    return 0;
}




/*******************************************************************************************************************************/



/*Bool Function that ppp checks if the label is valid by return boolean value, if will be an error the function will insert the match error into ErrorsAssembler table */
int isValidLabel(char * label)
{
    int flag;             /*flag=0 is not valid label; flag=1 is valid label */
    size_t i;
    i= strlen(label);
    flag=1;
    
    if(isInstruction(label,0)>=0)
        flag=0;
    else
    {
        if(i>30) flag=0;
        int k;
        if ((int)label[0]>= 48 && (int)label[0]<=57)
            return 0;
        for ( k = 0; k < i-2; k++)
        {
            if (!isGoodLetter(label[k]))
                flag=0;
        }
        k++;
        if (label[k]!=':')
            flag=0;
    }
    
    if (!flag)
        insertNewError("Invalid Label in line: ");
    return flag;
}




/*int Function that checks if the given order string is an vald instruction/data defining order: if is .data then ->16, if is .string then ->17, if is .mat then ->18 ,if is .entry then -> 19, if is .extern then -> 20 ,if is mov ->func.mov,otherwise the function will return -1*/
/*flagMessage: indicate if to print an error message or not*/
int isInstruction(char * order, int flagMessage)
{
    if (strcmp(order,"mov")==0){
        return 0;
    }
    if (strcmp(order,"cmp")==0){
        return 1;
    }
    if (strcmp(order,"add")==0){
        return 2;
    }
    if (strcmp(order,"sub")==0){
        return 3;
    }
    if (strcmp(order,"not")==0){
        return 4;
    }
    if (strcmp(order,"clr")==0){
        return 5;
    }
    if (strcmp(order,"lea")==0){
        return 6;
    }
    if (strcmp(order,"inc")==0){
        return 7;
    }
    if (strcmp(order,"dec")==0){
        return 8;
    }
    if (strcmp(order,"jmp")==0){
        return 9;
    }
    if (strcmp(order,"bne")==0){
        return 10;
    }
    if (strcmp(order,"red")==0){
        return 11;
    }
    if (strcmp(order,"prn")==0){
        return 12;
    }
    if (strcmp(order,"jsr")==0){
        return 13;
    }
    if (strcmp(order,"rts")==0){
        return 14;
    }
    if (strcmp(order,"stop")==0){
        return 15;
    }
    if (strcmp(order,".data")==0){
        return 16;
    }
    if (strcmp(order,".string")==0){
        return 17;
    }
    if (strcmp(order,".mat")==0){
        return 18;
    }
    if (strcmp(order,".entry")==0){
        return 19;
    }
    if (strcmp(order,".extern")==0){
        return 20;
    }
    
    if(flagMessage)
        insertNewError("Invalid instruction in line: ");
    return -1;
}


/*Int Function: search and return the index of the given label from the symbol table, if not exist return -1*/
int findDataInstruction(char * data)
{
    
}


/*Boolean Function: check by the given string which type of addressing type it is*/
int checkAddressingType(char * data)
{
    
    
}






























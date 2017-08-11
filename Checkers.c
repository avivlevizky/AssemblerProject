#include <stdlib.h>
#include "Checkers.h"
#include "Assembler.h"
#include <string.h>
#include <stdio.h>


/*Private Aux function: which indicate if the given char is an alpha-bet charcter*/
int isGoodLetter (char toTest){
    int tester;
    
    tester = (int)toTest;
    if( (tester>= 65 && tester<=90) || (tester>= 97 && tester<=122) || (tester>= 48 && tester<=57) )
    {return 1;}
    return 0;
}




/*******************************************************************************************************************************/



/*Bool Function that ppp checks if the label is valid by return boolean value, if will be an error the function will insert the match error into ErrorsAssembler table */
int isValidLabel(char * label,int flagDotDot)
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
        for ( k = 0; k < (i-1+(!flagDotDot)) && (flag); k++)
        {
            if (!isGoodLetter(label[k]))
                flag=0;
        }
        k++;
        if ((flagDotDot)&&(label[k]!=':'))
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
 
    return 0;
}




/*Boolean Function: check of the given string which type of addressing type it is*/
int isDirectOrRegister(char * data)
{
    if((data[0]=='#')||(data[0]=='r'))
    {
        int value;
        value=atoi((data+1));
        
        if((!value)&&(data[1]!='0')) /*if value ==0 and the next char after '#' isn't '0' then value is an error*/
            return -1;
        if((data[0]=='r')&&((value<0)||(value>7))) /*if the operand is reg and the domain value is out of bounds-> return -1*/
            return -1;
        
        return (data[0]=='#') ? 0 : 3;
    }
    return -1;
    
}






/*function that checks if the given string is a valid matrix : then return the string as array of strings otherwise return null*/
char ** isValidMatrix(char * mat)
{
    char ** matFixed;           /*dynamic matrix of strings*/
    
    int wordCounter,balance,i,chars_len;
    char reader,prevReader ;
    
    i=0;
    balance=0;
    prevReader = '\0';
    matFixed=NULL;
    reader='\0';
    chars_len=1;
    wordCounter=0;
    
    matFixed=(char **)malloc(sizeof(char *));
    matFixed[0]=(char *)calloc(1,sizeof(char));
    
    while((balance>=0)&&((reader=mat[i])!='\0'))
    {
        if(reader=='[')
        {
            if(wordCounter==0)
            {
                if(!isValidLabel(matFixed[0],0))
                {
                    insertNewError("Invalid Label of matrix in line: ");
                    freeLinkedList(matFixed);
                    return NULL;
                }
            }
            else
            {
                if(prevReader!=']')
                {
                    insertNewError("Invalid syntex in line: ");
                    freeLinkedList(matFixed);
                    return NULL;
                }
            }
        }
        
        if(reader==']')
        {
            if(isDirectOrRegister(matFixed[wordCounter])!=3)
            {
                insertNewError("Invalid register in index array ");
                freeLinkedList(matFixed);
                return NULL;
            }
            
        }
        
        if((reader==']')||(reader=='['))
        {
            if(chars_len>1)
            {
                wordCounter++;
                matFixed=(char **)realloc((char **)matFixed, (wordCounter+1)*sizeof(char *));
                matFixed[wordCounter]=(char *)calloc(1,sizeof(char));
                chars_len=1;
            }
            balance=(reader=='[') ? balance+1 : balance-1;
        }
        else
        {
            matFixed[wordCounter]=(char *)realloc((char *)(matFixed[wordCounter]), (chars_len+1)*sizeof(char));
            matFixed[wordCounter][chars_len-1]=reader;
            matFixed[wordCounter][chars_len]='\0';
            chars_len++;
        }
        prevReader=reader;
        i++;
    }
    
    
    if((balance!=0)||(wordCounter!=3))
    {
        insertNewError("Invalid syntex in line: ");
        freeLinkedList(matFixed);
        return NULL;
    }
    
    
    return matFixed;
}























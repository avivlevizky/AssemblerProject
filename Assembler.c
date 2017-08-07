
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Assembler.h"
#include "Checkers.h"
#include "Insertion.h"


extern Symbol ** symbol_table;               /*The symbols table*/
extern void ** instructions_table;   /* for data and instruction order*/
extern int * data_table;             /*Int dynamic array to store all the data instructions*/
extern char ** ErrorsAssembler;     /*Error in the compiling*/
extern unsigned IC;                 /*Instruction table counter*/
extern unsigned DC;                 /*Data table counter*/
extern unsigned SC;                 /*Symbol counter*/
extern unsigned EC;                 /*Error counter*/
extern unsigned LC;                 /*Line counter*/
unsigned word_counter;
FILE * fp;                           /*FILE pointer to the given assembly file*/


void FirstCheckingCommand(char **);
void SecondCheckingCommand(char ** command);


/*fucntion that checks if the new memory allocate was successed, if not the function will print to stderr a new error message and then will exit the program*/
void allocate_check(void * p)
{
    if(!p)
    {
        fprintf(stderr,"Error to allocate new memory\n");
        exit(0);
    }
    
}




/*fucntion that insert new assembler error into ErrorsAssembler table */
void insertNewError(char * error)
{
    char temp [1024];
    sprintf(temp, error, LC); // puts string into buffer
    
    if (!EC)
    {
        ErrorsAssembler=(char **)calloc(1, sizeof(char *));
        allocate_check((char **)ErrorsAssembler);            /*-------------Need to check if (char **)commands is valid------------*/
        
    }
    ErrorsAssembler[EC]=error;
    EC++;
    ErrorsAssembler=(char **)realloc((char **)ErrorsAssembler, (EC+1)*sizeof(char *));
    allocate_check((char *)ErrorsAssembler[EC]);
    
}




/*function that free the linked list of strings*/
void freeLinkedList(char ** list)
{
    while(list)
    {
        free((char *)*list);
        list++;
    }
}






/*get the whole command and transfer it to linked list, if returned ans>0 then there is an error in the input otherwise the input is valid*/
void CommandLineToLinkedList(int NumIteration)
{
    char ** command;           /*dynamic matrix of strings*/
    char reader;              /*char variable to iterate on content std*/
    int chars_len,word_counter,isComa;      /*chars_len: the char length of the current word; word_counter: indicate in the current number of word; isComa: indicate if coma was encourted more than one time*/
    
    LC++;
    reader='\0';
    word_counter=0;
    chars_len=1;
    isComa=0;
    command=(char **)malloc(sizeof(char *));
    allocate_check((char **)command);            /*-------------Need to check if (char **)commands is valid------------*/
    command[0]=(char *)calloc(1,sizeof(char));
    allocate_check((char *)command[0]);
    
    while(((reader=fgetc(fp))!=EOF)&&(reader!='\n'))
    {
        if((reader!=' ')&&((reader!=',')||(isComa)))
        {
            command[word_counter]=(char *)realloc((char *)(command[word_counter]), (chars_len+1)*sizeof(char));
            allocate_check((char *)command[word_counter]);
            command[word_counter][chars_len-1]=reader;
            command[word_counter][chars_len]='\0';
            chars_len++;
            isComa=0;
        }
        else
        {
            if(chars_len>1)
            {
                word_counter++;
                command=(char **)realloc((char **)command, (word_counter+1)*sizeof(char *));
                allocate_check((char **)command);
                command[word_counter]=(char *)calloc(1,sizeof(char));
                allocate_check((char *)command[word_counter]);
                chars_len=1;
                isComa=(reader==',');
            }
            
            
        }
    }
    /*Assign in the last+1 place a null (to indicate the end of the current command) */
    word_counter++;
    command=(char **)realloc((char **)command, (word_counter+1)*sizeof(char *));
    allocate_check((char **)command);
    command[word_counter]=NULL;
    
    if(reader=='\n')
    {
        if(NumIteration)
            FirstCheckingCommand(command);
        else
            SecondCheckingCommand(command);
        freeLinkedList(command);
        CommandLineToLinkedList(NumIteration);
    }
    else   /*if c is EOF*/
    {
        if(NumIteration)
            FirstCheckingCommand(command);
        else
            SecondCheckingCommand(command);
        freeLinkedList(command);
    }
}






/*checking and processing the current command line*/
void FirstCheckingCommand(char ** command)
{
    int flag_symbol_type;
    
    
    /*In the case that the first string on the current command line is a label(symbol) */
    if((isValidLabel(command[0]))&&((flag_symbol_type=isInstruction(command[1],1)>=0)))
    {
        /*if the instrct type is an data*/
        if((flag_symbol_type>15)&&(flag_symbol_type<19))
        {
            insertSymbolToTable(command[0],flag_symbol_type);
            insertToDT(&command[2],flag_symbol_type);
        }
        /*if the instrct type is an instruction*/
        else if(flag_symbol_type<=15)
        {
            insertSymbolToTable(command[0],flag_symbol_type);
            insertToIT(&command[2],flag_symbol_type);   /*the command[2] is first operand*/
        }
    }
    
    else /*if the commands[0] isn't label*/
    {
        if ((flag_symbol_type=isInstruction(command[0],1))>=0)
        {
            if (flag_symbol_type>=19)
            {
                if(flag_symbol_type==20) /*if is .extern insruct type then we will enter the command into the symbol table*/
                    insertSymbolToTable(command[1],flag_symbol_type);
            }
            else
            {
                insertToIT(&command[1],flag_symbol_type);  /*the command[1] is first operand*/
            }
        }
    }
}

/*The Secound check of the given command line*/
void SecondCheckingCommand(char ** command)
{
    int flag_symbol_type;
    int flag;  /*if there is a label(symbol) in the current given command line*/
    
    flag=0;
    if (isValidLabel(command[0]))
    {
        /*In the case that the first string on the current command line is a label(symbol) */
        flag=1;
    }
    
    if((flag_symbol_type=isInstruction(command[flag],1))==19)
    {
        /*In the case that the second string is .entry*/
        insertSymbolToTable(command[0],flag_symbol_type);   /*need to assign the current label(symbol) as an .entry in the symbol table*/
    }
    else if(flag_symbol_type<=15)
    {
        updateInstruction(&command[flag+1],flag_symbol_type);   /*the command[2] is first operand*/
    }
}



int main(int argc,char ** argv) {
    /*Function to check the validity of the inputed arguments*/
    /*********************************************************/
    IC=0;
    SC=0;
    EC=0;
    LC=0;
    DC=0;
    
    fp = fopen ("argv[0]", "r");
    
    if(!fp)
    {
        fprintf(stderr,"File address isn't valid\n");
        exit(0);
    }
    
    /*First checking of the assembly*/
    CommandLineToLinkedList(1);
    
    /*sets the file position to the beginning of the assembly file*/
    rewind(fp);
    
    if (EC>0)
    {
        /*Print all the compile error from ErrorsAssembler and exit*/
    }
    
    /*Second checking of the assembly*/
    IC=0;
    LC=0;
    CommandLineToLinkedList(2);
    
    if (EC>0)
    {
        /*Print all the compile error from ErrorsAssembler and exit*/
    }
    
    return 0;
    
    
    
    
}






























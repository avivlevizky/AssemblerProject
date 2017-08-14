
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Assembler.h"
#include "Checkers.h"
#include "Insertion.h"


Symbol ** symbol_table;               /*The symbols table*/
Instruction ** instructions_table;   /* for data and instruction order*/
char ** ErrorsAssembler;     /*Error in the compiling*/
int * data_table;             /*Int dynamic array to store all the data instructions*/
int * symbolType_to_entry;             /*table that save all the indexes of the entrys labels*/
unsigned SymbolEntry;                 /*Data table counter*/
unsigned IC;                 /*Instruction table counter*/
unsigned Total_IC;           /*total of Instructions after the first iteration*/
unsigned DC;                 /*Data table counter*/
unsigned SC;                 /*Symbol counter*/
unsigned EC;                 /*Error counter*/
unsigned LC;                 /*Line counter*/
FILE * fp;                    /*FILE pointer to the given assembly file*/


/*Prototyps*/
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


int lenOfNum(int n)
{
    int ans;
    
    ans=1;
    
    while((n=(n/10))>=1)
    {
        ans++;
    }
    return ans;
}






/*fucntion that insert new assembler error into ErrorsAssembler table */
void insertNewError(char * error)
{
    if (!EC)
    {
        ErrorsAssembler=(char **)malloc(sizeof(char *));
        allocate_check((char **)ErrorsAssembler);
    }
    ErrorsAssembler=(char **)realloc((char **)ErrorsAssembler, (EC+1)*sizeof(char *));
    allocate_check(ErrorsAssembler);
    
    ErrorsAssembler[EC]=(char *)malloc(strlen(error)+lenOfNum(LC)+1);
    sprintf(ErrorsAssembler[EC], error, LC); /*( puts string into buffer */
    ErrorsAssembler[EC][strlen(error)+lenOfNum(LC)]='\0';
    EC++;
    
}




/*function that free the linked list of strings*/
void freeLinkedList(char ** list,int length)
{
    int i;
    
    i=0;
    while((i<length)&&((list)!=NULL))
    {
        *list=NULL;
        free(*list);
        list++;
        i++;
        
    }
    
}




/*get the whole command and transfer it to linked list, if returned ans>0 then there is an error in the input otherwise the input is valid*/
void CommandLineToLinkedList(int NumIteration)
{
    char ** command;           /*dynamic matrix of strings*/
    char reader;              /*char variable to iterate on content std*/
    int chars_len,word_counter,isComa,isQuot,ignore;      /*chars_len: the char length of the current word; word_counter: indicate in the current number of word; isComa: indicate if coma was encourted more than one time ; isQueat: indicate if quotation marks apeared for ignore the creation of new string */
    
    LC++;
    ignore=0;
    reader='\0';
    word_counter=0;
    chars_len=1;
    isComa=0;
    isQuot=0;
    command=(char **)malloc(sizeof(char *));
    allocate_check(command);            /*-------------Need to check if (char **)commands is valid------------*/
    command[0]=(char *)calloc(1,sizeof(char));
    allocate_check(command[0]);

Loop: while(((reader=fgetc(fp))!=EOF)&&(reader!='\n'))
    {
        if((reader==';')&&(!word_counter)&&(chars_len==1))
            ignore=1;
        if((!ignore)&&((reader!=' ')&&((reader!=',')||(isComa))))
        {
            isQuot = (reader=='"') ? isQuot ^1 : isQuot;
            command[word_counter]=(char *)realloc((char *)(command[word_counter]), (chars_len+1)*sizeof(char));
            allocate_check(command[word_counter]);
            command[word_counter][chars_len-1]=reader;
            command[word_counter][chars_len]='\0';
            chars_len++;
            isComa=0;
        }
        else
        {
            if((!isQuot)&&(chars_len>1))
            {
                word_counter++;
                command=(char **)realloc((char **)command, (word_counter+1)*sizeof(char *));
                allocate_check(command);
                command[word_counter]=(char *)calloc(1,sizeof(char));
                allocate_check(command[word_counter]);
                chars_len=1;
                isComa=(reader==',');
            }
            
            
        }
    }
    if(ignore)
    {
        LC++;
        ignore=0;
        goto Loop;
    }
    /*Assign in the last+1 place a null (to indicate the end of the current command) */
    if(chars_len>1)
    {
        word_counter++;
        command=(char **)realloc((char **)command, (word_counter+1)*sizeof(char *));
        allocate_check(command);
    }
    command[word_counter]=NULL;
    
    if(reader=='\n')
    {
        if(NumIteration==1)
            FirstCheckingCommand(command);
        else
            SecondCheckingCommand(command);
        freeLinkedList(command,word_counter);
        CommandLineToLinkedList(NumIteration);
    }
    else   /*if c is EOF*/
    {
        if(NumIteration==1)
            FirstCheckingCommand(command);
        else
        {
            SecondCheckingCommand(command);
            while((SymbolEntry>0)&&(EC==0))
            {
                int index;
                index=symbolType_to_entry[--SymbolEntry];
                symbol_table[index]->type=ENTRY;
            }
        }
        
    }
}






/*checking and processing the current command line*/
void FirstCheckingCommand(char ** command)
{
    int flag_symbol_type;
    
    /*if the given string list is null/empty */
    if(!(*command))
        return;

    
    /*In the case that the first string on the current command line is a label(symbol) */
    if((isValidLabel((command[0]),1))&&(((flag_symbol_type=isInstruction(command[1],1))>=0)))
    {
        /*if the instrct type is an data or extern*/
        if(((flag_symbol_type>=DATA)&&(flag_symbol_type<=MAT))||(flag_symbol_type==EXTERN))
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
        if ((flag_symbol_type=isInstruction(command[0],0))>=0)
        {
            if((flag_symbol_type>=DATA)&&(flag_symbol_type<=MAT))
            {
                insertToDT(&command[1],flag_symbol_type);
                return;
            }

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
        else if(!isValidLabel((command[0]),1))
                insertNewError("Unidentified command line: %d");
        
    }
}

/*The Secound check of the given command line*/
void SecondCheckingCommand(char ** command)
{
    int flag_symbol_type;
    int flag;  /*if there is a label(symbol) in the current given command line*/
    
    /*if the given string list is null/empty */
    if(!(*command))
        return;

    flag=0;
    if (isValidLabel(command[0],1))
    {
        /*In the case that the first string on the current command line is a label(symbol) */
        flag=1;
    }
    
    if((flag_symbol_type=isInstruction(command[flag],1))==19)
    {
        /*In the case that the second string is .entry*/
        int index;
        
        index=findSymbol(command[flag+1]);
        
        if(index==-1)
            insertNewError("The entry symbol defining isn't valid: %d");
        else
        {
            symbolType_to_entry[SymbolEntry]=index;
            SymbolEntry++;
        }
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
    
    fp = fopen (argv[1], "r");
    
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
        int i;
        
        i=0;
        
        while(i<EC)
        {
            printf("%s\n",ErrorsAssembler[i]);
            i++;

        }
        return 1;

    }
    
    /*Second checking of the assembly*/
    symbolType_to_entry=(int *)malloc(sizeof(int)*SC);
    SymbolEntry=0;
    Total_IC=IC;
    IC=0;
    LC=0;
    CommandLineToLinkedList(2);
    free(symbolType_to_entry);
    
    if (EC>0)
    {
        /*Print all the compile error from ErrorsAssembler and exit*/
        int i;
        
        i=0;
        
        while(i<EC)
        {
            printf("%s\n",ErrorsAssembler[i]);
            i++;

        }
        return 2;

    }
    
    return 0;
    
    
    
    
}






























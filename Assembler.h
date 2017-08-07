#ifndef main_h
#define main_h

/*-------------------------------------------Defining Structs----------------------------------------------*/
typedef struct
{
    unsigned int type_coding: 2;
    unsigned int dest_addressing: 2;
    unsigned int origin_addressing: 2;
    unsigned int opcode: 4;
    
    
}InstructOrder;


typedef enum {MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,STOP}func;




typedef struct
{
    char * label_name;
    int dec_value;
    int type;      /*boolean variable: data order or instruction order*/
    
}Symbol;



void insertNewError(char * error);




/*-------------------------------------------Defining Global Variables-------------------------------------------*/




Symbol ** symbol_table;               /*The symbols table*/
void ** instructions_table;   /* for data and instruction order*/
//void ** data_table;           /**/
unsigned IC;                 /*Instruction table counter*/
//unsigned DC=0;                 /*Data table counter*/
unsigned SC;                 /*Symbol counter*/
char ** ErrorsAssembler;     /*Error in the compiling*/
unsigned EC;                 /*Error counter*/
unsigned LC;                 /*Line counter*/
unsigned DC;                 /*Data table counter*/





#endif /* main_h */

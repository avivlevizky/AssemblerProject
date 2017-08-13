#ifndef main_h
#define main_h

/*-------------------------------------------Defining Structs----------------------------------------------*/


typedef struct
{
    void * order;
    unsigned int type_order: 2;

}Instruction;



typedef struct
{
    unsigned int type_coding: 2;
    unsigned int dest_addressing: 2;
    unsigned int origin_addressing: 2;
    unsigned int opcode: 4;
}InstructOrder;


typedef struct
{
    char value;
    unsigned int type_coding: 2;
}InstructData;



typedef struct
{
    unsigned int reg1: 4;
    unsigned int reg2: 4;
}InstructRegisters;





typedef enum {MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,STOP,DATA,STRING,MAT,ENTRY,EXTERN}func;




typedef struct
{
    char * label_name;
    char dec_value;
    func type;      /*boolean variable: data order or instruction order*/
    
}Symbol;



void insertNewError(char * error);
void allocate_check(void * p);
void freeLinkedList(char ** list);





/*-------------------------------------------Defining Global Variables-------------------------------------------*/




Symbol ** symbol_table;               /*The symbols table*/
int * data_table;             /*Int dynamic array to store all the data instructions*/
Instruction ** instructions_table;   /* for data and instruction order*/
unsigned IC;                 /*Instruction table counter*/
unsigned SC;                 /*Symbol counter*/
char ** ErrorsAssembler;     /*Error in the compiling*/
unsigned EC;                 /*Error counter*/
unsigned LC;                 /*Line counter*/
unsigned DC;                 /*Data table counter*/














#endif /* main_h */

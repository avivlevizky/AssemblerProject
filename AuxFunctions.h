

#ifndef AuxFunctions_h
#define AuxFunctions_h

#include <stdio.h>

/*Bool Function that checks if the label is valid by return boolean value, if will be an error the function will insert the match error into ErrorsAssembler table */
int isValidLabel(char * label);

/*Bool Function that check if the first string from the command line is a lavel or not*/
int isLabel(char * order);

/*int Function that checks if the given order string is an vald instruction/data defining order: if is .data then ->16, if is .string then ->17, if is .mat then ->18 ,if is .entry then -> 19, if is .extern then -> 20 ,if is mov ->func.mov,otherwise the function will return -1*/
int isInstruction(char * order);

/*Function that insert the symbol into the symbols tabel by the type: 16 or 17 or 18 is data;19 is .entry and 20 is .extern otherwise is instruction type. and update SC counter too*/
void insertSymbolToTable(char **data,int type);

/* Function that insert the given data into the instructions_table, the function will update the IC counter by matching to the length of the given data */
void insertToIT(char **data,int Instruc_type);


/*Function that insert data by the given Instruc_type argument into the data table*/
void insertToDT(char **data,int Instruc_type);



/*Update the given instruction command for the second checking*/
void updateInstruction(char **data,int Instruc_type);

int isGoodLetter (char toTest);
#endif /* AuxFunctions_h */

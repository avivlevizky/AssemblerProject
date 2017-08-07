

#ifndef Checkers_h
#define Checkers_h

#include <stdio.h>

/*Bool Function that checks if the label is valid by return boolean value, if will be an error the function will insert the match error into ErrorsAssembler table */
int isValidLabel(char * label);


/*int Function that checks if the given order string is an vald instruction/data defining order: if is .data then ->16, if is .string then ->17, if is .mat then ->18 ,if is .entry then -> 19, if is .extern then -> 20 ,if is mov ->func.mov,otherwise the function will return -1*/
/*flagMessage: indicate if to print an error message or not*/
int isInstruction(char * order, int flagMessage);


/*Int Function: search and return the index of the given label from the symbol table, if not exist return -1*/
int findDataInstruction(char * data);

/*Boolean Function: check by the given string which type of addressing type it is*/
int checkAddressingType(char * data);



#endif /* Checkers_h */

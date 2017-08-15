#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

typedef enum {MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,STOP,DATA,STRING,MAT,ENTRY,EXTERN}func;

int isInstruction(char * order, int flagMessage);
void FirstCheckingCommand(char ** command);
void insertToDT(char **data,int type);
void SecondCheckingCommand(char ** command);
char ** isValidMatrix(char * mat);
void updateInstruction(char **data,int Instruc_type);

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


typedef struct
{
    char * label_name;
    unsigned int dec_value: 8;
    func type;      /*boolean variable: data order or instruction order*/
    
}Symbol;



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












void allocate_check(void * p)
{
    if(!p)
    {
        fprintf(stderr,"Error to allocate new memory\n");
        exit(0);
    }
    
}




int * isNumeric(char * data)
{
    int * value;
    int ans,i,signMinus,length;
    
    i=0;
    ans=0;
    signMinus=0;
    length=(int)strlen(data);
    value=(int *)malloc(sizeof(int));
    allocate_check(value);
    
    if(data[0]=='-')
        signMinus++;
    
    while((i+signMinus<length)&&(data[i+signMinus]>='0')&&(data[i+signMinus]<='9'))
    {
        ans=ans+((data[i+signMinus])-48)*pow(10,length-signMinus-i-1);
        
        i++;
    }
    if((i+signMinus)==length)
        *value= (signMinus) ? ans*(-1) : ans;
    else
    {
        free(value);
        value=NULL;
    }
    return value;
    
}


void freeLinkedList(char ** list,int length)
{
    int i;
    char ** temp;
    i=0;
    while(i<length && list)
    {
        free(list[i]);

        i++;

    }

    free(list);
}


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


void createNewSpaceToITtable(int typeOfOrder)
{
    /*Instruction **temp;*/
    if(!instructions_table){
        instructions_table=(Instruction **)malloc(sizeof(Instruction *));
        allocate_check(instructions_table);
    }
    
    /*temp=(Instruction **)malloc(sizeof(Instruction*)*(IC+1));
    allocate_check(temp);
    memmove(temp,instructions_table,IC*sizeof(Instruction**));
    instructions_table=temp;*/
    
    instructions_table=(Instruction **)realloc(instructions_table,sizeof(Instruction*)*(IC+1));
    allocate_check(instructions_table);
    
    instructions_table[IC]=(Instruction *)malloc(sizeof(Instruction));
    allocate_check(instructions_table[IC]);
    
    switch (typeOfOrder)
    {
        case 1: /*if type Order is InstructOrder*/
        {
            instructions_table[IC]->order=(InstructOrder *)malloc(sizeof(InstructOrder));
            allocate_check(instructions_table[IC]->order);
            break;
        }
            
        case 2: /*if type Order is InstructData*/
        {
            instructions_table[IC]->order=(InstructData *)malloc(sizeof(InstructData));
            allocate_check(instructions_table[IC]->order);
            break;
            
        }
        case 3: /*if type Order is InstructRegister*/
        {
            instructions_table[IC]->order=(InstructRegisters *)malloc(sizeof(InstructRegisters));
            allocate_check(instructions_table[IC]->order);
            break;
            
        }
    }
    
}







void insertToItForOperand(char * data,int operand, int isOriginOperand)
{
    int * value;
    char ** mat_data;
    InstructData dataOrder;
    InstructRegisters regOrder;
    
    
    /*If the given operand is register*/
    if(operand==3)
    {
        regOrder.reg1=0;
        regOrder.reg2=0;
        value=isNumeric(data+1);
        
        if(isOriginOperand)
            regOrder.reg1=*value;
        else
            regOrder.reg2=*value;
        
        createNewSpaceToITtable(3);
        *(((InstructRegisters *)(instructions_table[IC]->order)))=regOrder;
        instructions_table[IC]->type_order=2;
        IC++;
        free(value);
        return;
    }
    
    
    /*If the given operand is label,mat or direct addressing*/
    if(operand==0)
    {
        value=isNumeric(data+1);
        dataOrder.value=*value;
    }
    
    
    dataOrder.type_coding=0;
    
    createNewSpaceToITtable(2);
    *(((InstructData *)(instructions_table[IC]->order)))=dataOrder;
    instructions_table[IC]->type_order=1;
    IC++;
    
    if(operand==2)
    {
        mat_data=isValidMatrix(data);
        regOrder.reg1=(*isNumeric((mat_data[1]+1)));
        regOrder.reg2=(*isNumeric((mat_data[2]+1)));
        
        
        /*allocate new place registers coding*/
        InstructOrder *testo= (InstructOrder *)(instructions_table[0]->order);

        createNewSpaceToITtable(3);
        
        testo= (InstructOrder *)(instructions_table[0]->order);

        *(((InstructRegisters *)(instructions_table[IC]->order)))=regOrder;
        instructions_table[IC]->type_order=2;
        IC++;
        freeLinkedList(mat_data,3);

    }
    free(value);
}







int checkAddressingType(char * data)
{
    int addrType;
    
    
    if(!data)
        return -2;
    
    
    addrType=isDirectOrRegister(data);
    
    if(addrType==-1)
    {
        char ** mat_data = NULL;
        if((mat_data=isValidMatrix(data)))
            addrType=2;
        else
            addrType=(isValidLabel(data, 0))  ? 1 : -1;
        freeLinkedList(mat_data,3);

    }
    
    
    return addrType;
    
}








void insertToIT(char **data,int Instruc_type)
{
    int orgOperand,destOperand;
    InstructOrder order;
    
    destOperand=-2;
    orgOperand=checkAddressingType(data[0]);
    if(orgOperand!=-2)
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
    
    else
    {   /*if the instruct type is sole place*/
        if (((Instruc_type>=NOT)&&(Instruc_type<=CLR))||((Instruc_type>=INC)&&(Instruc_type<=JSR)))
        {
            if((orgOperand==-2)||(destOperand!=-2))
                goto Failure;
            
            if((Instruc_type!=PRN)&&(destOperand==0))
                goto Failure;
        }
        else  /*In the condition of Instruc_type==rts or Instruc_type==stop */
        {
            if((orgOperand!=-2)||(destOperand!=-2))
                goto Failure;
        }
    }
    /*if the operand/s are valid then: */
    order.type_coding=0;
    order.origin_addressing=(orgOperand==-2) ? 0 : orgOperand;
    order.dest_addressing=(destOperand==-2) ? 0 : destOperand;
    order.opcode=Instruc_type;
    
    createNewSpaceToITtable(1);
    *(((InstructOrder *)(instructions_table[IC]->order)))=order;
    instructions_table[IC]->type_order=0;
    IC++;
    
    /*if both of the operands are registers*/
    if((orgOperand==3)&&(destOperand==3))
    {
        InstructRegisters regOrder;
        
        regOrder.reg1=(*isNumeric(data[0]+1));
        regOrder.reg2=(*isNumeric(data[1]+1));
        
        createNewSpaceToITtable(3);
        *(((InstructRegisters *)(instructions_table[IC]->order)))=regOrder;
        instructions_table[IC]->type_order=2;
        IC++;
        
    }
    else
    {
        if(orgOperand!=-2)
            insertToItForOperand(data[0],orgOperand,1);

        if(destOperand!=-2)
            insertToItForOperand(data[1],destOperand,0);
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



void insertSymbolToTable(char *data,int type)
{
    int isDotDot;
    Symbol* temp;
    
    isDotDot=0;
    if(data[strlen(data)-1]==':')
        isDotDot=1;
    
    temp = (Symbol*)malloc(sizeof(Symbol));
    allocate_check(temp);
    
    
    temp->label_name=(char *)calloc(sizeof(char), strlen(data)-isDotDot);
    allocate_check((temp->label_name));
    
  
    
    strcpy(temp->label_name, data);
    /*need to add check for the copy operation*/
    
    (temp->label_name)[strlen(temp->label_name)-isDotDot]='\0';
    temp->type = type;

    if(type==EXTERN)
        temp->dec_value=0;
    else
        temp->dec_value=DC;
    
    if(findSymbol(temp->label_name)!=-1){
        insertNewError("The symbol is already decleared in Line: %d");
        free(temp);
    }
    if (!symbol_table)
    {
        symbol_table = (Symbol **)malloc(sizeof(Symbol*));
        allocate_check(symbol_table);
    }
    else
    {
        /*Symbol ** temp_ST;
        temp_ST=(Symbol **)malloc(sizeof(Symbol*)*(SC+1));
        allocate_check(temp_ST);
        memmove(temp_ST,symbol_table,sizeof(Symbol*)*(SC));
        symbol_table=temp_ST;*/
    }
    symbol_table[SC]=temp;
    SC++;
    
}



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
    if((!ignore)&&((reader!=' ')&&(reader!='\t')&&((reader!=',')||(isComa))))
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
            int i;
            SecondCheckingCommand(command);
            i=0;
            while((SymbolEntry>0)&&(EC==0))
            {
                int index;
                index=symbolType_to_entry[--SymbolEntry];
                symbol_table[index]->type=ENTRY;

            }
            while(i<SymbolEntry)
            {
                free(symbolType_to_entry[i]);
            }
            free(symbolType_to_entry);
        }

        freeLinkedList(command,word_counter);
    }
}




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




















int isGoodLetter (char toTest){
    int tester;
    
    tester = (int)toTest;
    if( (tester>= 65 && tester<=90) || (tester>= 97 && tester<=122) || (tester>= 48 && tester<=57) )
    {return 1;}
    return 0;
}








int isDirectOrRegister(char * data)
{
    if((data[0]=='#')||(data[0]=='r'))
    {
        int *value;
        value= isNumeric(data+1);
        
        
        if((!value)||((data[0]=='r')&&((*value<0)||(*value>7)))) /*if the operand is reg and the domain value is out of bounds-> return -1*/
            return -1;
        
        return (data[0]=='#') ? 0 : 3;
    }
    return -1;
    
}



/*Bool Function that ppp checks if the label is valid by return boolean value, if will be an error the function will insert the match error into ErrorsAssembler table */

int isValidLabel(char * label,int flagDotDot)
{
    int flag;             /*flag=0 is not valid label; flag=1 is valid label */
    size_t length;
    length= strlen(label);
    flag=1;
    
    if(isInstruction(label,0)>=0)
        flag=0;
    else
    {
        int k;
        
        if ((length>30)||(!isGoodLetter(label[0])))
            return 0;
        
        for (k = 0;k < (length-1+(!flagDotDot))&&(flag); k++)
        {
            if (!isGoodLetter(label[k]))
                flag=0;
        }
        if ((flagDotDot)&&(label[k]!=':'))
            flag=0;
    }
    
    return flag;
}




int isInstruction(char * order, int flagMessage)
{
    if(!order)
        goto Failure;
    
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
    
Failure:if(flagMessage)
            insertNewError("Invalid or missing instruction in line: %d");
    return -1;
}


int findSymbol(char * data)
{
    int i;
    
    if(!data)
        return -1;
    
    i=0;
    
    while(i<SC)
    {
        if(strcmp(symbol_table[i]->label_name,data)==0)
            return i;
        i++;
    }
    
    return -1;
}





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
    allocate_check(matFixed);
    
    matFixed[0]=(char *)calloc(1,sizeof(char));
    allocate_check(matFixed[0]);
    
    while((balance>=0)&&((reader=mat[i])!='\0'))
    {
        if(reader=='[')
        {
            if(wordCounter==0)
            {
                if(!isValidLabel(matFixed[0],0))
                {

                    freeLinkedList(matFixed,wordCounter);
                    return NULL;
                }
            }
            else
            {
                if(prevReader!=']')
                {

                    freeLinkedList(matFixed,wordCounter);
                    return NULL;
                }
            }
        }
        
        if(reader==']')
        {
            if(isDirectOrRegister(matFixed[wordCounter])!=3)
            {

                freeLinkedList(matFixed,wordCounter);
                return NULL;
            }
            
        }
        
        if((reader==']')||(reader=='['))
        {
            if(chars_len>1)
            {
                wordCounter++;

                char ** temp_mat;
                temp_mat=(char **)malloc(sizeof(char*)*(wordCounter+1));
                allocate_check(temp_mat);
                memmove(temp_mat,matFixed,sizeof(char*)*(wordCounter));
                matFixed=temp_mat;
                matFixed[wordCounter]=(char *)calloc(1,sizeof(char));
                allocate_check(matFixed[wordCounter]);
                chars_len=1;
            }
            balance=(reader=='[') ? balance+1 : balance-1;
        }
        else
        {
            char * temp_st;
            temp_st=(char *)malloc((chars_len+1)*sizeof(char));
            allocate_check(temp_st);
            memmove(temp_st,matFixed[wordCounter],(chars_len)*sizeof(char));
            matFixed[wordCounter]=temp_st;
            matFixed[wordCounter][chars_len-1]=reader;
            matFixed[wordCounter][chars_len]='\0';
            chars_len++;
        }
        prevReader=reader;
        i++;
    }
    
    if(chars_len>1)
    {
        wordCounter++;
        char ** temp_mat;
        temp_mat=(char **)malloc(sizeof(char*)*(wordCounter+1));
        allocate_check(temp_mat);
        memmove(temp_mat,matFixed,sizeof(char*)*(wordCounter));
        matFixed=temp_mat;
    }
    matFixed[wordCounter]=NULL;
    
    if((balance!=0)||(wordCounter!=3))
    {

        freeLinkedList(matFixed,wordCounter);
        return NULL;
    }
    
    return matFixed;
}








int isValidMatrixToData(char * mat)
{
    int * value;
    char * num;
    int balance,i,ans,chars_len,numOfBrac;
    char reader,prevReader;
    
    i=0;
    value=NULL;
    chars_len=1;
    balance=0;
    prevReader = '\0';
    reader='\0';
    ans=1;
    numOfBrac=0;
    
    num=(char *)calloc(1,sizeof(char));
    allocate_check(num);
    
    
    while((balance>=0)&&((reader=mat[i])!='\0'))
    {
        if(reader=='[')
        {
            if((i>0)&&(prevReader!=']'))
            {
                goto Failure;
            }
        }
        if(reader==']')
        {
            value=isNumeric(num);
            if((!value)||(*value<=0)) /*if value ==null and the next char after '#' isn't '0' then value is an error*/
            {
                goto Failure;
                
            }
            else
            {
                ans=ans*(*value);
                free(num);
                num=(char *)calloc(1,sizeof(char));
                allocate_check(num);
                chars_len=1;
            }
        }
        if((reader==']')||(reader=='['))
        {
            balance=(reader=='[') ? balance+1 : balance-1;
            numOfBrac++;
        }
        else
        {
            char * temp_st;

            temp_st=(char *)malloc((chars_len+1)*sizeof(char));
            allocate_check(temp_st);
            memmove(temp_st,num,(chars_len)*sizeof(char));
            num=temp_st;
            num[chars_len-1]=reader;
            num[chars_len]='\0';
            chars_len++;
        }
        prevReader=reader;
        i++;
    }
    
    
    if((balance!=0)||(numOfBrac!=4))
    {
    Failure: insertNewError("Invalid declaring matrix in Line: %d");
        free(num);
        if(value)
            free(value);
        return -1;
    }
    
    return ans;
}









void insertToDT(char **data,int type)
{
    int* value;
    int i;
    char *reader;
    
    i=0;
    value = NULL;
    switch (type) {
        case DATA: /*if the type is .data*/
        {
            while((reader=data[i]))
            {
                value=isNumeric(reader);
                if(!value)
                {
                    insertNewError("The data defining isn't valid in Line: %d");
                    free(value);
                    return;
                }

                int * temp_data;
                temp_data=(int *)malloc(sizeof(int)*(DC+1));
                allocate_check(temp_data);
                memmove(temp_data,data_table,sizeof(int)*DC);
                data_table=temp_data;
                data_table[DC]=*value;
                DC++;
                i++;
            }
            
        }
            break;
            
        case STRING: /*if the type is .string*/
        {
            char ch;

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
                int * temp_data;
                temp_data=(int *)malloc(sizeof(int)*(DC+1));
                allocate_check(temp_data);
                memmove(temp_data,data_table,sizeof(int)*DC);
                data_table=temp_data;
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
                return;
            
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

                int * temp_data;
                temp_data=(int *)malloc(sizeof(int)*(DC+1));
                allocate_check(temp_data);
                memmove(temp_data,data_table,sizeof(int)*DC);
                data_table=temp_data;
                data_table[DC]=*value;
                free(value);
                DC++;
                i++;
            }
            if (i!=n){
                insertNewError("The defining matrix values are not equel to declaring matrix: %d");
                free(value);}
        }
            break;
    }
    
}





void insertToItForOperandSecond(char * data,int operand)
{
    int index;
    char *label;
    if(operand==2)
        label=(isValidMatrix(data))[0];
    else
        label=data;
    index=findSymbol(label);
    
    if(index==-1)
        insertNewError("Symbol isn't decleared in Line: %d");
    else
    {
        if(((operand==1)&&(((symbol_table[index]->type)==DATA)||((symbol_table[index]->type)==STRING)||((symbol_table[index]->type)==EXTERN)))||((operand==2)&&((symbol_table[index]->type)==MAT)))
        {
            InstructData *temp;
            
            temp=((InstructData*)(instructions_table[IC]->order));
            temp->value=(symbol_table[index]->dec_value)+Total_IC;

            if((symbol_table[index]->type)==EXTERN)
            {
                temp->value=0;
                temp->type_coding=1;
            }
            else
                temp->type_coding=2;
        }
        else
            insertNewError("The symbol isn't matched to the type of the decleared symbol Line: %d");
        
        IC=IC+operand;
    }
}








/*Update the given instruction command for the second checking*/
void updateInstruction(char **data,int Instruc_type)
{
    int orgOperand,destOperand;
    
    destOperand=-2;
    orgOperand=checkAddressingType(data[0]);
    
    if(orgOperand!=-2)
        destOperand=checkAddressingType(data[1]);
    
    
    IC++;
    
    if((orgOperand==3)&&(destOperand==3))
    {
        IC++;
        return;
    }
    
    if(orgOperand!=-2)
    {
        if((orgOperand==1)||(orgOperand==2))
            insertToItForOperandSecond(data[0],orgOperand);
        else
            IC++;
    }
    
    if(destOperand!=-2)
    {
        if((destOperand==1)||(destOperand==2))
            insertToItForOperandSecond(data[1],destOperand);
        else
            IC++;
    }
   
    
}












int main() {

    int i;
    InstructOrder test0;
    InstructData test1;
    InstructRegisters test2;
    Instruction * wh;
    Symbol sy;
    LC=0;
    DC=0;
    i=0;
    fp = fopen ("test.txt", "r");
    
    if(!fp)
    {
        fprintf(stderr,"File address isn't valid\n");
        exit(0);
    }
    
    CommandLineToLinkedList(1);
    
    
    
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

    if (EC>0)
    {

    i=0;

    while(i<EC)
    {
        printf("%s\n",ErrorsAssembler[i]);
        i++;
    }
    return 2;

}



printf("%d\n",((InstructOrder *)instructions_table[0]->order)->type_coding);
     printf("%d\n",((InstructOrder *)instructions_table[0]->order)->origin_addressing);
     printf("%d\n",((InstructOrder *)instructions_table[0]->order)->dest_addressing);
     printf("%d\n",((InstructOrder *)instructions_table[0]->order)->opcode);
    
    
    
    
     putchar('\n');
    
    wh=instructions_table[1];
     printf("The type is: %d\n",wh->type_order);
     InstructData da=*((InstructData*)(wh->order));
     printf("%d\n",da.value);
     printf("%d\n",da.type_coding);
    
    putchar('\n');
    
    wh=instructions_table[2];
    printf("The type is: %d\n",wh->type_order);
    InstructRegisters reg=*((InstructRegisters*)(wh->order));
    printf("%d\n",reg.reg1);
    printf("%d\n",reg.reg2);
    
    putchar('\n');


    return 0;
}






















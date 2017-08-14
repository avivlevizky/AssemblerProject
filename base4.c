#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char * base4 (int i) {

    int k = 1;
    char * temp;
    char * ans = (char *)malloc(sizeof(char)*k);
    if (!ans) {
        printf("\ncannot allocate memory for base4 function\n");
        printf("\npreform exit\n");
        exit(0);
    }

    int res = i%4;
    i=i/4;

    if(res == 0){ans[k] = 'a';}
    if(res == 1){ans[k] = 'b';}
    if(res == 2){ans[k] = 'c';}
    if(res == 3){ans[k] = 'd';}

    k++;

    while (i > 0){
        printf("k is %d and i is %d res is %d ans ans is %s\n",k,i,res,ans);

        temp = (char*)realloc(ans,k);
        if (!temp) {
            printf("\ncannot allocate memory for base4 function - k is %d and i is %d\n",k,i);
            printf("preform exit\n");
            exit(0);
        }
        else
        {
            ans = temp;
            free(temp);
        }

        res = i%4;
        if(res == 0){ans[k++] = 'a';}
        else if(res == 1){ans[k++] = 'b';}
        else if(res == 2){ans[k++] = 'c';}
        else if(res == 3){ans[k++] = 'd';}
        i = i/4;

    }
    ans[k+1]='\0';

    return ans;
}


int main ( ){

    char *p;
    int i = 100;

    p = base4(i);
    printf("%s",p);


    return 1;

}










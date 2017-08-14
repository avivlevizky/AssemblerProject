


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

        temp = (char*)realloc(ans,k);
        if (!temp) {
            printf("\ncannot allocate memory for base4 function\n");
            printf("\npreform exit\n");
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

    return ans;
}














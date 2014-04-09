#include <stdio.h>
#include <string.h>

int fun1(int a)
{
    printf("11\n");
}


void fun2(int a, int b)
{
    printf("22\n");
}


void fun3(char *name)
{
    printf("33\n");
}




int main()
{
    void (*fun[])()={fun1,
    fun2,
    fun3};

    (*fun[0])(1);
    (*fun[1])(1,2);
    (*fun[2])("ss");

    int i=0;
    char str[] = "1 VM-ov2 linux 1123123";
    char delims[] = " ";
    char *result[4]={NULL, NULL, NULL, NULL};
    result[i] = strtok( str, delims );
    while( result[i] != NULL )
    {
        printf( "result is \"%s\"\n", result[i] );
        i++;
        result[i] = strtok( NULL, delims );
    }

    for(i=0;i<4;i++)
    {
        printf("%s\n",result[i]);
    }
    printf("%s\n", str);


    char sendString[1024];
    sprintf(sendString, "%-10d %-15ld %s", 23,3229628800,"vm2");
    printf("%s\n",sendString);
    long int la;
    printf(" int %d\n", sizeof( int));
    printf("long int %d\n", sizeof(long int));
    printf("long long int %d\n", sizeof(long long  int));

    return 1;
}






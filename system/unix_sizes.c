#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pwd.h>
#include <limits.h>

int main(int argc,char*argv[])
{
    printf("sizeof(time_t)=%ld\n",sizeof(time_t));
    printf("sizeof(suseconds_t)=%ld\n",sizeof(suseconds_t));
    printf("sizeof(uid_t)=%ld\n",sizeof(uid_t));
    printf("sizeof(gid_t)=%ld\n",sizeof(gid_t));
    printf("%ld\n",-9223372036854775807L-1);
    printf("%ld\n",LONG_MIN);
    return 0;
}

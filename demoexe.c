#include <stdio.h>
#include <unistd.h>
static char * envp[] = { "HOME=/", NULL };
static char * argv[] = { "-la",NULL};
int main(){
    pid_t pid;
    pid=fork();
    if(pid==0){
        execve("/usr/bin/ls",argv,envp);
        printf("Return not expected. Must be an execve error.n");
    }
    printf("Demo");
}

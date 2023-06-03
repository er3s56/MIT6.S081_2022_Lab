#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int sieve(int input_fd)
{
    int prime_pipe[2];
    int tmp = 0;

    // 0 for read, 1 for write
    pipe(prime_pipe);
    //printf("pid:%d fd0:%d fd1:%d\n", getpid(), prime_pipe[0], prime_pipe[1]);
    if (input_fd == -1)
    {
        for (int i = 2; i < 35; i++)
        {
            if (i == 2)
            {
                printf("prime %d\n", 2);
                continue;
            }

            if (i % 2 != 0)
            {
                write(prime_pipe[1], &i, 4);
            }

            if (i == 3)
            {
                if (0 == fork())
                {
                    close(prime_pipe[1]);
                    sieve(prime_pipe[0]);
                    //wait(0);
                    return 0;
                }
                else
                {
                    close(prime_pipe[0]);
                }
            }
        }
        close(prime_pipe[0]);
        close(prime_pipe[1]);
        return 0;
    }

    //if num satisfy the condition
  
    int first = 1;
    int fork_time = 0;
    int div = 0;
    
    while(4 == read(input_fd, &tmp, 4))
    {
        if (first == 1)
        {
            printf("prime %d\n", tmp);
            first = 0;
            div = tmp;
            continue;
        }
        
        if (tmp % div != 0)
        {
            write(prime_pipe[1], &tmp, 4);
        }

        if (fork_time == 0 && tmp % div != 0 )
        {
            if (0 == fork())
            {
                //printf("pid:%d input_fd:%d\n", getpid(), input_fd);
                //printf("pid:%d forktime\n", getpid());
                close(input_fd);
                close(prime_pipe[1]);
                fork_time = 1;
                sieve(prime_pipe[0]);
                wait(0);
                return 0;
            }
            else
            {
                fork_time = 1;
                close(prime_pipe[0]);
            }
            
        }

    }
    close(input_fd);
    close(prime_pipe[1]);
    return 0;
}

int main(int argc, char *argv[])
{
    sieve(-1);
    wait(0);
    exit(0);
}
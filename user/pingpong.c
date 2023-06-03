#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int
main(int argc, char *argv[])
{
  int ping_pipe[2];
  int pong_pipe[2];
  //int p;
  //int fd1, fd2, fd3, fd4;
  char num1 = 0;
  char num2 = 0;
  char tmp = 0;
  pipe(ping_pipe);
  pipe(pong_pipe);
  if (0 == fork())
  {
    num1 = 1;
    //printf("%d %d\n", getpid(), tmp);
    
    read(ping_pipe[0], &tmp, 1);
    if (tmp == 1)
    {
        printf("%d: received ping\n", getpid());
    }
    write(pong_pipe[1], &num1, 1);

    
  }
  else
  {
    num2 = 1;
    //printf("%d %d\n", getpid(), tmp);

    write(ping_pipe[1], &num2, 1);
    read(pong_pipe[0], &tmp, 1);
    if (tmp == 1)
    {
        printf("%d: received pong\n", getpid());
    }
  }

  // see the asm to checkout where to restore the return value

  exit(0);
}

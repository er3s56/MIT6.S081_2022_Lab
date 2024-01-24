#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int
main(int argc, char *argv[])
{
  struct sysinfo info;
  if (sysinfo(&info) < 0)
  {
    printf("error\n");
    exit(0);
  }

  printf("%d, %d\n", info.freemem, info.nproc);
  exit(0);
}
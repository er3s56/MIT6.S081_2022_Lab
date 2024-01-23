#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int
main(int argc, char *argv[])
{
  int sleep_t = 0;
  if (2 != argc)
  {
    printf("error\n");
    exit(0);
  }


  sleep_t = atoi(argv[1]);

  if (0 == sleep_t || sleep_t < 0)
  {
    printf("error sleep value\n");
    exit(0);
  }

  //printf("start sleep\n");
  sleep(sleep_t);
  //printf("end sleep\n");
  exit(0);
  return 0;
}

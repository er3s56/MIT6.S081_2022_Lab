#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

#define BUF_LEN 1000

int split2args(const char *str, int str_l, char *argv[], int argv_l)
{
    static char split_str[BUF_LEN];

    if (str_l > BUF_LEN)
    {
        printf("err!\n");
        return -1;
    }

    memcpy(split_str, str, str_l);
    int argv_i = 0;
    int left = 0, right = 0;
    while (left < str_l && split_str[left] == ' ')
        left++;

    right = left;

    while (left < str_l)
    {
        if (argv_i >= argv_l)
        {
            printf("too many arguments\n");
        }

        while (right < str_l && split_str[right] != ' ')
            right++;

        argv[argv_i] = &split_str[left];
        argv_i++;
        while (right < str_l &&  split_str[right] == ' ')
        {
            split_str[right] = 0;
            right++;
        }

        left = right;
    }

    return argv_i;

}            

void xargs(char *path, char *argv[])
{
    if (fork() == 0)
    {
        exec(path, argv);
        exit(0);
    }
    wait(0);
    return;
}

int main(int argc, char *argv[])
{
    static char buf[BUF_LEN];
    int left = 0, right = 0;
    int n = 0;
    char *xarg_argv[MAXARG];
    int buf_str_len = 0;
    if (argc < 2)
    {
        printf("xargs err use!\n");
        exit(0);
    }

    for (int i = 0; i < MAXARG; i++)
    {
        xarg_argv[i] = 0;
    }

    for (int i = 0; i < argc; i++)
    {
        xarg_argv[i] = argv[i];
    }

    while ((n = read(0, buf + buf_str_len, sizeof(buf))) > 0)
    {
        buf_str_len += n;
    }

    if (n < 0)
    {
        printf("error\n");
        exit(0);
    }

    // int n = read(0, buf, sizeof(buf));
    // printf("n:%d\n", n);
    // if (read(0, buf, sizeof(buf)))
    // {
    //     printf("too much input!\n");
    //     exit(0);
    // }
    if (buf_str_len >= BUF_LEN)
    {
        printf("error\n");
        exit(0);
    }
        
    while (left < buf_str_len)
    {
        char input[BUF_LEN];
        while (right < BUF_LEN && buf[right] != '\n')
            right++;
        if (right >= buf_str_len)
            break;
        memcpy(input, buf, right - left);
        right++;
        left = right;
        char *extra_arg[MAXARG];
        int extra_n = split2args(input, strlen(input), extra_arg, MAXARG);
        for (int i = 1;i < argc; i++)
        {
            xarg_argv[i - 1] = argv[i];
        }

        for (int i = 0; i < extra_n; i++)
        {
            xarg_argv[argc - 1 + i] = extra_arg[i];
        }
        
        xargs(xarg_argv[0], xarg_argv);
        
    }
    
    wait(0);
    exit(0);
    return 0;
}
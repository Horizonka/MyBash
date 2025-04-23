#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <wait.h>
#include <pwd.h>
#define  ARG_MAX  10
#define PATH_BIN "/home/stu/quzijie/test15/mybin/"
void  printf_info()
{
    char *user_str="$";
    int user_id=getuid();
    if(user_id==0)
    {
        user_str="#";
    }

    struct passwd *ptr=getpwuid(user_id);
    if(ptr==NULL)
    {
        printf("mybash1.0>>  ");
        fflush(stdout);
        return ;
    }
    char hostname[128]={0};
    if(gethostname(hostname,128)==-1)
    {
        printf("mybash1.0>>  ");
        fflush(stdout);
        return ;
    }
    char dir[256]={0};
    if(getcwd(dir,256)==NULL)
    {
        printf("mybash1.0>>  ");
        fflush(stdout);
        return ;
    }

    printf("\033[1;32m%s@%s\033[0m  \033[1;34m %s\033[0m%s ",ptr->pw_name,hostname,dir,user_str);
    fflush(stdout);
}

char *get_cmd(char *buff,char *myargv[])
{
    if(buff==NULL||myargv==NULL)
    {
        return NULL;
    }

    int i=0;
    char *s=strtok(buff," ");
    while(s!=NULL)
    {
        myargv[i++]=s;
        s=strtok(NULL," ");
    }

    return myargv[0];
}


// void run_cmd(char *path,char *myargv[])
// {
//     if(path==NULL ||myargv==NULL)
//     {
//         return ;
//     }
//     pid_t pid=fork();
//     if(pid==-1)
//     {
//         return ;
//     }
//     if(pid==0)
//     {
//       // execvp(path,myargv);
//       char pathname[128]={0};
//       if(strncmp(path,"/",1)==0||strncmp(path,"./",2)==0)
//       {
//           strcpy(pathname,path);
//       }
//       else
//       {
//           strcpy(pathname,PATH_BIN);
//           strcat(pathname,path);
//       }
//       execv(pathname,myargv);
//        perror("execvp   error!\n");
//        exit(0);
//     }

//     else
//      {
//          wait(NULL);
//      }
//   }


//   int main()
// {
//     while(1)
//     {
//     //    printf("stu@localhost        ~$");
//         printf_info();
//         //
//         fflush(stdout);

//         char buff[128]={0};
//         fgets(buff,128,stdin);//ls,ps -f,cp a.c b.c
//         buff[strlen(buff)-1]='\0';

//         char *myargv[ARG_MAX]={0};
//         char *cmd=get_cmd(buff,myargv);

//        if(cmd==NULL)
//          {
//              continue;
//         }
//        else    if(strcmp(cmd,"cd")==0)
//         {
//              //...
//              if(myargv[1]!=NULL)
//              {
//                if(chdir(myargv[1])==-1)
//                {
//                  perror("cd err!\n");
//                }
//              }
//         }
//         else if(strcmp(cmd,"exit")==0)
//         {
//             //exit(0);//OK,不建议
//             break;
//         }
//         else
//         {
//             //fork+exec;
//             run_cmd(cmd,myargv);
//         }
//     }

//     //...
//     exit(0);
// }



void run_cmd(char *path, char *myargv[]) {
    if (path == NULL || myargv == NULL) {
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork error");
        return;
    }

    if (pid == 0) { // 子进程
        char pathname[128] = {0};

        // 如果命令包含路径（如 "/bin/ls" 或 "./mycmd"），直接使用
        if (strncmp(path, "/", 1) == 0 || strncmp(path, "./", 2) == 0) {
            strcpy(pathname, path);
        } else {
            // 拼接默认路径
            strcpy(pathname, "/bin/");
            strcat(pathname, path);

            // 如果 /bin/ 下找不到，再尝试 /usr/bin/
            if (access(pathname, X_OK) != 0) {
                strcpy(pathname, "/usr/bin/");
                strcat(pathname, path);
            }
        }

        // 执行命令
        execvp(path, myargv); // 使用 execvp 支持 PATH 环境变量
        perror("execvp error"); // 如果 execvp 失败，打印错误信息
        exit(EXIT_FAILURE);
    } else { // 父进程
        wait(NULL); // 等待子进程结束
    }
}

int main() {
    while (1) {
        printf_info();

        char buff[128] = {0};
        fgets(buff, sizeof(buff), stdin);
        buff[strlen(buff) - 1] = '\0'; // 去掉换行符

        char *myargv[ARG_MAX] = {0};
        char *cmd = get_cmd(buff, myargv);

        if (cmd == NULL) {
            continue;
        } else if (strcmp(cmd, "cd") == 0) {
            if (myargv[1] != NULL) {
                if (chdir(myargv[1]) == -1) {
                    perror("cd error");
                }
            }
        } else if (strcmp(cmd, "pwd") == 0) {
            char dir[256] = {0};
            if (getcwd(dir, sizeof(dir)) != NULL) {
                printf("%s\n", dir);
            } else {
                perror("getcwd error");
            }
        } else if (strcmp(cmd, "exit") == 0) {
            break;
        } else {
            run_cmd(cmd, myargv);
        }
    }

    exit(0);
}
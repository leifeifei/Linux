/*
File name:Pipe_Ipc.c
Author:雷飞飞
Date:2016-8-12
Description:管道编程
在父子进程间通过两个管道进行双向通讯。父进程从标准输入输入文本数据，通过管道1将数据发
往子进程，子进程将所有小写字母变成大小字母，通过管道2将加工后的数据发会给父进程，父进程将
收到的数据输出到标准输出，重复上述过程，直到输入了自己指定的结束字符串为止。
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
  int fd[2];    //管道1
  int ret = pipe( fd );
  if ( ret == -1 )
  {
     perror("pipe fd:");
     exit( -1 );
  }
  int fd2[2];    //管道2
  ret = pipe( fd2 );
  if (ret == -1)
  {
    perror( "pipe fd2:" );
    exit( -1 );
  }
  
  pid_t pid = fork();    //创建进程
  if (pid < 0)
  {
    perror( "fork pid:" );
    exit( -1 );
  }
  if (pid == 0)    //子进程
  {
     close( fd[1] );    //关闭管道1写端
     close( fd2[0] );   //关闭管道2读端
     
     char buffer[1024];
     while (1)
     {
       bzero( buffer,sizeof(buffer) );
       ret = read( fd[0],buffer,sizeof(buffer) - 1 );    //从管道1读取数据 
       if (ret < 0)
       {
        perror( "read wrong pid == 0:" );
       }
       int i;
       for (i = 0; i < strlen(buffer); i++)    //将读到的小写字母，变成大写字母
       {
         if (buffer[i] >= 'a' && buffer[i] <= 'z')
         {
            buffer[i] -= 32;
         }
       }

       write( fd2[1],buffer,strlen(buffer) );    //将数据写入管道2
     }
     

    close( fd[0] );    //关闭管道1读端
    close( fd2[1] );   //关闭管道2写端
  }
  if (pid > 0)    //父进程
  {
    close( fd[0] );    //关闭管道1读端
    close( fd2[1] );   //关闭管道2写端
    
    char buffer[1024];
    while (1)
    {
      bzero( buffer,sizeof(buffer) );
      gets( buffer );
      write( fd[1],buffer,strlen(buffer) );    //写数据到管道1
      if ( strcmp(buffer,"bye") == 0 )
      {
        break;
      }
      ret = read( fd2[0],buffer,sizeof(buffer) -1 );    //从管道2读取数据
      buffer[ret + 1] = '\0';
      printf("%s\n",buffer );
    }
    
    close( fd[1] );    //关闭管道1写端
    close( fd2[0] );   //关闭管道2读端

    wait(NULL);
  }
  return 0;
}

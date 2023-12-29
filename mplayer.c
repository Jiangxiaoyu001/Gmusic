/****************************************************
*Description: a free tool for personal musical
*Data: Thu Oct 26 2023 12:30
*Author: jiangxiaoyu
****************************************************/


#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <gtk/gtk.h>	
#include "./baseWindow.h"                     
#include <pthread.h>

int main(int argc, char *argv[])
{
	int fd_pipe[2]; 
	pid_t pid; int ret;

	song_name_list();
	dir_lrc();
	dir_skin();

	if (pipe(fd_pipe) < 0) {       
		perror("pipe");
		return;
	}

	system("clear");

	ret=mkfifo("./FILE/fifo",0666);
	 if(ret!=0){
		 perror("==mkfifo");
		// return 0;
	 }

	pid=fork();
	if(pid<0) {
		perror("fork");
		return 0;
	}else if(pid==0) {
		close(fd_pipe[0]);
		dup2(fd_pipe[1],1);   //重定向到命名管道
		execlp("mplayer","mplayer","-ac","mad","-idle","-slave","-quiet","-input","file=./FILE/fifo","./song/luck.mp3",NULL); //子进程中启动播放器
	} else {
		gtk_thread_init();	
		gtk_init(&argc, &argv); // 初始化 
		pthread_mutex_init(&mutex,NULL);
		printf("========================\n\n");
		fp=open("./FILE/fifo",O_WRONLY);   // 父进程打开管道
		if (fp == -1) {
			perror("opem:");
			return;
		}
		window_show();
	}
	return 0;
}	

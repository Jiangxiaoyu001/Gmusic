#include<stdio.h>
#include "../window/mywindow.h"
#include "../other/sungtk_interface.h" 
#include<gtk/gtk.h>
#include <string.h>
#include "../lrc_source/lyrics_process.h"     //歌词解析头文件
#include<glade/glade.h> // glade 所需头文件
#include "../gb2312_source/gb2312_ucs2.h"
#include <pthread.h>

/****************************************************
*    函数功能: 给标签初始化
*
*
*******************************************************/
  void set_label(){                        
	gtk_label_set_text(GTK_LABEL(label5),"sorry no lrc");   //歌词初始化
	gtk_label_set_text(GTK_LABEL(label3),"please wait");  //歌名初始化
	gtk_label_set_text(GTK_LABEL(label2),"no soner");    //歌手初始化
  }

void send_cmd(char *cmd){      //通过有名管道向mplayer发送命令
	int re;
	re=write(fp,cmd,strlen(cmd));
	if(re==-1){
		perror("write cmd");
		return ;
	}
}
void * pipe_read_lrc_pthread(){    //专门处理歌词
	LRC_PTR lrc;
	LRC *head = NULL;
	char *buff1;
	char *buff2;
	int k,i;
	char lrc_buff[100];
	char lrc_buff1[100];
	char lrc_buf[100];
	while(1){
		if(my_lock==0){
			for(k=0;k<lrc_num;k++){
				buff1=lrc_list[k];
				buff2=song_list[j];
				sscanf(buff1,"%[^.]",lrc_buff);
				sscanf(buff2,"%[^.]",lrc_buff1);
				if(strcmp(lrc_buff,lrc_buff1)==0){
					bzero(lrc_buf,sizeof(lrc_buf));
					sprintf(lrc_buf,"./lyrics/%s",lrc_list[k]);
					head = dispose_lrc(lrc_buf, &lrc);
					if(head==NULL)
						printf("this is no lrc\n");
					else {
						for(i=0;i<lrc.lrc_arry_size;i++){
							if(play.time_pos==(lrc.lrc_arry[i]->time)/1000){
								char utf8[100] = "";                          
								char utf8_temp[100] = "";
								bzero(utf8_temp,100);
								gb2312_to_utf8(lrc.lrc_arry[i]->lrc, utf8_temp); //解码
								gdk_threads_enter();
								gtk_label_set_text(GTK_LABEL(label5),utf8_temp);
								gdk_threads_leave(); 
								printf("%s\n",utf8_temp);
							}
							usleep(10*1000);
						}   //时间是毫秒ms                         
					}
				}
			}	
		}
	}
	free_lrc_arry(&lrc);//释放歌词解析(dispose_lrc)后的资源
	return ;
}

void *pipe_read_pthread()
{
	char *p1,*p2;
	char buf[100];
	char buf1[20];

	while(TRUE) {

		usleep(10*1000);
		bzero(buf, sizeof(buf));
		bzero(buf1, sizeof(buf1));
		read(fd_pipe[0], buf, sizeof(buf)-1);
		p1 = strtok(buf, "=");
		p2 = strtok(NULL, "=");

		if (strcmp(p1, "ANS_PERCENT_POSITION") == 0) {    //ok  //当前播放进度
			play.percent_pos=atoi(p2);

			gdk_threads_enter();
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar1),((float)(play.percent_pos)/100));
			gdk_threads_leave(); 
		} else if(strcmp(p1, "ANS_LENGTH") == 0) {             //OK  歌曲时间总长度
			play.time_length=atoi(p2);
			bzero(buf1, sizeof(buf1));
			sprintf(buf1, "%02d:%02d", (play.time_length)/60, (play.time_length)%60);

			gdk_threads_enter();
			gtk_label_set_text(GTK_LABEL(label2), buf1);	
			gdk_threads_leave(); 
		} else if (strcmp(p1, "ANS_TIME_POSITION") == 0) {            //OK   当前播放时间
			bzero(buf1, sizeof(buf1));
			play.time_pos=atoi(p2);
			sprintf(buf1, "%02d:%02d", (play.time_pos)/60, (play.time_pos)%60);

			gdk_threads_enter();
			gtk_label_set_text(GTK_LABEL(label1), buf1);
			gdk_threads_leave();

			if(play.percent_pos >= 97) {	 //当前进度比较
				j++;
				if(j == song_num)
					song_num=0;

				set_label();
				sungtk_clist_set_foreground(clist, "blue");     //设置前景颜色
				sungtk_clist_set_row_color(clist, j,"green");   //颜色反显同步
				sprintf(buf,"loadfile ./song/%s\n",song_list[j]);
				write(fp,buf,strlen(buf));
			}
		} else if (strcmp(p1, "ANS_FILENAME") == 0) {   //OK 3     //获取歌名                                  
			bzero(buf1, sizeof(buf1));
			strcpy(buf1, p2+1);
			p1=strtok(buf1, ".");

			gdk_threads_enter();
			gtk_label_set_text(GTK_LABEL(label3),p1);
			gdk_threads_leave(); 
		} else if (strcmp(p1, "ANS_META_ARTIST") ==0 ) {          //OK 3     //获取歌手名

			if (p2[0] == p2[1]) {
				gdk_threads_enter();
				gtk_label_set_text(GTK_LABEL(label4),"no soner");
				gdk_threads_leave(); 
			} else {
				char utf8[100] = "";                          
				char utf8_temp[100] = "";
				bzero(buf1, strlen(buf1));
				bzero(utf8_temp, 100);
				strcpy(buf1, p2+1);
				p1 = strtok(buf1, "'");

				gb2312_to_utf8(p1, utf8_temp); //解码

				gdk_threads_enter();
				gtk_label_set_text(GTK_LABEL(label4),utf8_temp);   //打印歌手名
				gdk_threads_leave(); 
			}
		}
	}
}
/*****************************************************************
*     函数功能:发送获取歌词播放时间命令线程：
*
*********************************************************************/

	void *get_pos_pthread(){    //发送获取歌词播放时间命令线程：
		while(1){
			if(my_lock==0 ){	
				pthread_mutex_lock(&mutex);
				usleep(100*1000);
				send_cmd("get_percent_pos\n");   //获取播放进度
				usleep(100*1000);
				send_cmd("get_time_pos\n");   //获取播放时间
				usleep(100*1000);
				send_cmd("get_time_length\n");  //获取歌曲总时间
				usleep(100*1000);
				send_cmd("get_file_name\n");  //获取歌名
				usleep(100*1000);
				send_cmd("get_meta_artist\n");  //获取歌手信息
				pthread_mutex_unlock(&mutex);
			} 	
		usleep(100*1000);
		}
	return NULL;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

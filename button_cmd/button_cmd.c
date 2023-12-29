#include <stdio.h>
#include <string.h>
#include "../window/mywindow.h"
#include "../baseWindow.h"
#include <glade/glade.h> // glade 所需头文件

#include "../other/sungtk_interface.h" 
#include "../pthread/my_pthread.h"

int window_num=1;

/********************************************************
*   函数功能：按钮的回调函数
*
***********************************************************/
void gtk_next(GtkWidget *button, gpointer data)
{
	char *strr;
	strr=(char *)data;
	char *string="pause\n";
	 if(strcmp(strr,"pause")==0){   	 //播放/暂停
		pthread_mutex_lock(&mutex);
		 strr="pause\n";
		 my_lock++;
		 if(my_lock==1){
			load_image(image2,"./image/pause.png",w2,h2); //暂停
			write(fp,strr,strlen(strr));
		 }
		 if(my_lock==2){
			 my_lock=0;
			 load_image(image2,"./image/play.png",w2,h2); //播放
			write(fp,strr,strlen(strr));
		}
		pthread_mutex_unlock(&mutex);   //解锁
	}
	if(strcmp(strr,"sound_add")==0){  //音量加
		pthread_mutex_lock(&mutex);
		strr="volume 10\n";
		write(fp,strr,strlen(strr));
			if(my_lock==1)
				write(fp,string,strlen(string));            //判断是否是暂停状态
		pthread_mutex_unlock(&mutex);   //解锁
		return;
	}
	if(strcmp(strr,"sound_less")==0){  //音量减小
		pthread_mutex_lock(&mutex);
		strr="volume -10\n";
		write(fp,strr,strlen(strr));
		if(my_lock==1)
			write(fp,string,strlen(string));         //判断是否是暂停状态
		pthread_mutex_unlock(&mutex); 
		return;
	}
 	if(strcmp(strr,"quiet")==0){ 
        	pthread_mutex_lock(&mutex);
		if(num==0){
			strr="mute 0\n";              //取消静音
			load_image(image6,"./image/mute0.png",w6,h6); //音量图片
			write(fp,strr,strlen(strr));
			if(my_lock==1)
				write(fp,string,strlen(string));        //判断是否是暂停状态
			num=1;
		pthread_mutex_unlock(&mutex);
			return ;
		}
		if(num==1){
			pthread_mutex_lock(&mutex);
			strr="mute 1\n";             //静音
			load_image(image6,"./image/mute1.png",w6,h6); //音量图片
			write(fp,strr,strlen(strr));
			if(my_lock==1)
				write(fp,string,strlen(string));         //判断是否是暂停状态
			num=0;
			pthread_mutex_unlock(&mutex);
			return ;
		}
	}	 
	if(strcmp(strr,"fast_forward")==0){  //快进
		pthread_mutex_lock(&mutex);   //加锁
		strr="seek 10\n";
		write(fp,strr,strlen(strr));
		if(my_lock==1)
			write(fp,string,strlen(string));           //判断是否是暂停状态
		pthread_mutex_unlock(&mutex);   //解锁
		return;
	
	}
	if(strcmp(strr,"rewind")==0){  //回退
		pthread_mutex_lock(&mutex);   //加锁
		strr="seek -10\n";
		write(fp,strr,strlen(strr));
		if(my_lock==1)
			write(fp,string,strlen(string));            //判断是否是暂停状态
		pthread_mutex_unlock(&mutex);   //解锁
	}
	return;
}

/******************************************************
*    函数功能：切换皮肤
*
*******************************************************/
void skin_chage(GtkButton * button,gpointer data){        //切换皮肤
	pthread_mutex_lock(&mutex);   //加锁
		char *str;
		char buffer[100];
		str=(char *)data;
		printf("*********************%s\n",str);
		if(strcmp(str,"skin_change")==0){
			skin++;
			if(skin>skin_num-1)
				skin=0;
				bzero(buffer,sizeof(buffer));
				sprintf(buffer,"./skin/%s",skin_list[skin]);
				chang_background(window,900,480,buffer);   //设置背景图片 
		}
		pthread_mutex_unlock(&mutex);   //解锁
	}
	/************************************************************************
	*   函数功能：进度条的点击事件
	*
	*************************************************************************/
	void song_progress_event(GtkWidget *widget, GdkEventMotion *event, gpointer gui_data)
	{
		pthread_mutex_lock(&mutex);   //加锁
		gint x = event->x;
		char buf[200];
		float s;
		static int event_num=0;
		event_num=(x-20);
		s=((float)((event_num)*(play.time_length)/800));
		bzero(buf,strlen(buf));
		sprintf(buf,"seek %d\n",(int )s-play.time_pos);
		write(fp,buf,strlen(buf));
		pthread_mutex_unlock(&mutex);   //解锁
	}
	/*******************************************************************
*      函数功能： 实现歌曲的切换,暂停,静音等                       *                                   
*      参数类型： gtk_next  按钮的回调函数                         *                              
*                 GtkWidget *button  ：判断按钮                    *
*                  gpointer data   ：  回调函数的参数              *
*      完成时间 ： 2019年1月16日周三                               *                       
*                                                                  *
*******************************************************************/
void song_chage(GtkButton * button,gpointer data)
{
	pthread_mutex_lock(&mutex);   //加锁
	char *str;
	str=(char *)data;
	row=j;
	printf("*********************%s\n",str);
	char buf[200];
	set_label();
	char *string="pause\n";
	if(strcmp(str,"up")==0)
	{
		j--;
		if(j<0)
			j=song_num-1;
		sungtk_clist_select_row(clist, j, "red");       //选择当前行颜色
		sprintf(buf,"loadfile ./song/%s\n",song_list[j]);
		write(fp,buf,strlen(buf));
		//printf("上一首歌：%s\n",song_list[j]);
		if(my_lock==1)
			write(fp,string,strlen(string));          //判断是否是暂停状态   切换歌曲时以防冲突
		pthread_mutex_unlock(&mutex);   //解锁
		return;
	}
	if(strcmp(str,"next")==0){
		j++;
		if(j>song_num-1)
			j=0;
		sungtk_clist_select_row(clist, j, "green");       //选择当前行颜色
		sprintf(buf,"loadfile ./song/%s\n",song_list[j]);
		write(fp,buf,strlen(buf));
		//printf("下一首歌：%s\n",song_list[j]);
		if(my_lock==1)
			write(fp,string,strlen(string));
		pthread_mutex_unlock(&mutex);   //解锁
		return;
	}
}

 void window_hide(GtkButton * button,gpointer data){

	if(window_num==0){
 		pthread_mutex_lock(&mutex);   //加锁
		gtk_widget_show_all(window);
		window_num=1;
		pthread_mutex_unlock(&mutex);   //解锁
		return;
	}
	if(window_num==1){
		pthread_mutex_lock(&mutex);   //加锁
		gtk_widget_hide_all(list_event);
		window_num=0;
		pthread_mutex_unlock(&mutex);   //解锁
		return;
	}
 
 }
















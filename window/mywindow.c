#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
 #include<glade/glade.h> // glade 所需头文件
#include<gtk/gtk.h>
	
#include "../other/sungtk_interface.h"

#include <dirent.h>

#include "gb2312_ucs2.h"


typedef struct song_info{

 char buf[100]; 
 char *time_minute;
 char *time_second;
 int  time_length;
 char *song_progress;      //歌曲进度
 char *song_time;
 char *song_time_info;     //歌曲时间信息
  char *song_time_length;  //歌曲总时间
 int time;
 int minute;
 int second;
 }SONG;

SONG  *song; 
 
 
static char buffer[100];
SunGtkCList *clist;    
 int fp,i,song_num;
char *str;
char *song_list[100];      //歌曲清单
static int my_lock=1;

int fd_pipe[2];
pthread_t  tid1,tid2,tid3;
 
 

	void send_cmd(char *cmd)      //通过有名管道向mplayer发送命令
	{
		int re;
		re=write(fp,cmd,strlen(cmd));
		if(re==-1){
			perror("write cmd");
			return ;
		}
	}
 

 
/*******************************************************************
*      函数功能： 实现歌曲的切换,暂停,静音等                       *                                   *
*      参数类型： gtk_next  按钮的回调函数                         *                              *
*                 GtkWidget *button  ：判断按钮                    *
*                  gpointer data   ：  回调函数的参数              *
*      完成时间 ： 2019年1月16日周三                               *                       *
*                                                                  *
********************************************************************/

void gtk_next(GtkWidget *button, gpointer data){
	
	char *strr;
	strr=(char *)data;
	static int num=1;
	if(strcmp(strr,"sound_add")==0){  //音量加
		strr="volume 10\n";
		write(fp,strr,strlen(strr));
	}
		if(strcmp(strr,"sound_less")==0){  //音量减小
		strr="volume -10\n";
		write(fp,strr,strlen(strr));
	}
	
	if(strcmp(strr,"quiet")==0){ 
		if(num==0){
			strr="mute 0\n";              //取消静音
			//extern void load_image(image2,"./button/button5.jpg",extern w2,h2); //音量图片
			write(fp,strr,strlen(strr));
			num=1;
			return ;
		}
		if(num==1){
			strr="mute 1\n";             //静音
			
	
			write(fp,strr,strlen(strr));
			num=0;
			return ;
		}
	}
	
	 if(strcmp(strr,"fast_forward")==0){  //快进
		strr="seek 20\n";
		write(fp,strr,strlen(strr));
	}
	 if(strcmp(strr,"rewind")==0){  //回退
		strr="seek 20\n";
		write(fp,strr,strlen(strr));
	}

	 if(strcmp(strr,"pause")==0){     //播放/暂停
		strr="pause\n";
		if(my_lock=1){
			write(fp,strr,strlen(strr));
			my_lock=0;
			return ;
		}
		if(my_lock=0){
			write(fp,strr,strlen(strr));   //播放/暂停
			my_lock=1;
		return ;
		}
	}
}

void song_name_(GtkButton * button,gpointer data)
{
	str=(char *)data;
	static int j=0;
	printf("*********************%s\n",str);
	char buf[200];
	if(strcmp(str,"up")==0)
	{
		j--;
		if(j<0)
			j=song_num;
		system("clear");
		sprintf(buf,"loadfile ./song/%s\n",song_list[j]);
		
		write(fp,buf,strlen(buf));
		printf("上一首歌：\n");
	}
	if(strcmp(str,"next")==0)
	{
		j++;
		if(j>song_num)
			j=0;
		sprintf(buf,"loadfile ./song/%s\n",song_list[j]);
		printf("********************\n");
		write(fp,buf,strlen(buf));
		printf("下一首歌：\n");
	}
}



//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk事件盒子kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
	
void load_image(GtkWidget *image, const char *file_path, const int w, const int h )
{
	gtk_image_clear( GTK_IMAGE(image) ); // 清除图像
	GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(file_path, NULL); // 创建图片资源
	GdkPixbuf *dest_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf, w, h,
	GDK_INTERP_BILINEAR); // 指定大小
	gtk_image_set_from_pixbuf(GTK_IMAGE(image), dest_pixbuf); // 图片控件重新设置一张图
	//片(pixbuf)
	g_object_unref(src_pixbuf); // 释放资源
	g_object_unref(dest_pixbuf); // 释放资源
}
	
	
	
/************************************************************************
*      函数功能： 设置主窗口,按钮的图片,背景图片等                     	*                                   *
*      参数类型： widget: 主窗口                       		            *                              *
*                 w, h: 图片的大小                   		            *
*                 path： 图片路径              		                    *
*      完成时间 ： 2019年1月16日周三                               		*                       *
*************************************************************************/

 void chang_background(GtkWidget *widget, int w, int h, const gchar *path)
{
	gtk_widget_set_app_paintable(widget, TRUE); //允许窗口可以绘图
	gtk_widget_realize(widget);
	/* 更改背景图时，图片会重叠
	* 这时要手动调用下面的函数，让窗口绘图区域失效，产生窗口重绘制事件（即 expose 事件）。
	*/
	gtk_widget_queue_draw(widget);
	GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(path, NULL); // 创建图片资源对象
	// w, h 是指定图片的宽度和高度
	GdkPixbuf *dst_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf, w, h,
	GDK_INTERP_BILINEAR);
	GdkPixmap *pixmap = NULL;
	/* 创建 pixmap 图像;
	* NULL：不需要蒙版;
		* 123： 0~255，透明到不透明
*/
	gdk_pixbuf_render_pixmap_and_mask(dst_pixbuf, &pixmap, NULL, 128);
	// 通过 pixmap 给 widget 设置一张背景图，最后一个参数必须为: FASLE
	gdk_window_set_back_pixmap(widget->window, pixmap, FALSE);
	// 释放资源
	g_object_unref(src_pixbuf);
	g_object_unref(dst_pixbuf);
	g_object_unref(pixmap);
}



/********************************************************************
*      函数功能：设置点击信号，切换歌曲
*
*
*************************************************************************/


gboolean col_callback(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	int row=(int)data;    //点击的行数
	
	char buf[100];
	char *sp;

	printf("row========%d\n",row);
	printf("%s\n",song_list[row]);
	sungtk_clist_select_row(clist, row, "red");       //选择当前行颜色
	//sungtk_clist_set_text_size(clist, 20);                  //设置字体大小
	sprintf(buf,"loadfile ./song/%s\n",song_list[row]); 
	write(fp,buf,strlen(buf));

}




/***********************************************************************
*      函数功能: 显示歌曲名字到分栏列表
*                 转码格式显示,显示中文不乱码
*
*
**********************************************************************/
void song_name(){                      
                             
	int i;
	char buf[100];
	char utf8[100] = "";
	sungtk_clist_set_foreground(clist, "blue");
	for(i=0;i<song_num;i++)
	{	
		memset(buf,0,sizeof(buf));
		strcpy(buf,song_list[i]);
		str_to_utf8(buf, utf8);
		sungtk_clist_append(clist, buf);
		
		
	}
}





/****************************************************
*      函数功能：实现滚动窗口
*      
*
*********************************************************/
	
void song_list_event(GtkWidget *widget, GdkEventMotion *event, gpointer gui_data)
{
	SunGtkCList *clist=(SunGtkCList *)gui_data;
	static int now_x,now_y;
	static int old_x,old_y;	
	int i;
	old_x = now_x;
	old_y = now_y;
	gint x = event->x;
	gint y = event->y;
	now_x = x;	
	now_y = y;
	static int event_num=0;
	event_num+=(now_y-old_y);
	//printf("song_list= %s\n", song_list[event_num]);
	if(event_num < -220)                          //最右端
	{
		event_num = -220;
	}
	if(event_num > 20)                          //最右端
	{
		event_num = 0;
	}

	gtk_fixed_move((GtkFixed*)clist->fixed,clist->vbox,0,event_num);
	
}


	void window(){
	

	GladeXML *gxml=glade_xml_new("./glade/demo.glade",NULL,NULL);   // 父进程打开管道

	GtkWidget *window = glade_xml_get_widget(gxml,"window1");// 加载窗口
		
	GtkWidget *list_event=glade_xml_get_widget(gxml,"eventbox1"); //加载事件框
	
	//lable:显示时间进度等
	
	GtkWidget *label1=glade_xml_get_widget(gxml,"label1"); //获取歌曲当前播放时间
	gtk_label_set_text(GTK_LABEL(label1),"00:00");
	GtkWidget *label2=glade_xml_get_widget(gxml,"labe12");  //获得歌的总长度
	gtk_label_set_text(GTK_LABEL(label2),"hhh");  

	
	
	GtkWidget *progressbar1=glade_xml_get_widget(gxml,"progressbar1");  //加载进度条
	
	
	clist=sungtk_clist_new();    //创建分栏列表
	
	
	
	//设置点击信号
	sungtk_clist_set_select_row_signal(clist, "button-press-event",col_callback);
	g_signal_connect(list_event, "motion_notify_event", G_CALLBACK(song_list_event),clist);
	
	
	
	GtkWidget *fixed=glade_xml_get_widget(gxml,"fixed2");
	
	 //设置分栏列表的宽高
	 // sungtk_clist_set_row_height(clist, 480);
	 // sungtk_clist_set_col_width(clist, 180);
	

	gtk_container_add(GTK_CONTAINER(list_event),clist->fixed);
	song_name();  //显示歌曲信息到列表中

	
	//g_signal_connect(list_event, "motion_notify_event", G_CALLBACK(song_list_event));
	
		//  图片放入
	GtkWidget * image1 =glade_xml_get_widget(gxml,"image1");  //上一首

	GtkWidget * image3 =glade_xml_get_widget(gxml,"image3");  //下一首

	GtkWidget * image2 =glade_xml_get_widget(gxml,"image2");//播放/暂停

	GtkWidget * image4 =glade_xml_get_widget(gxml,"image4");   //快进

	GtkWidget * image5 =glade_xml_get_widget(gxml,"image5"); //回退
	
	GtkWidget * image6 =glade_xml_get_widget(gxml,"image6");   //静音
	
	
	gtk_widget_show_all(window); // 显示所有控件

	//获取图片控件的宽度和高度，必须放在 gtk_widget_show_all()函数后
	int w1 = image1->allocation.width;    
	int h1 = image1->allocation.height;       //上一首

	int w2 = image2->allocation.width;       //播放/暂停
	int h2 = image2->allocation.height;

	int w3 = image3->allocation.width;     //下一首
	int h3 = image3->allocation.height;


	int w4 = image4->allocation.width;         //快进
	int h4 = image4->allocation.height;

	int w5 = image5->allocation.width;        // 回退
	int h5 = image5->allocation.height;
	
	
	int w6 = image6->allocation.width;        // 静音
	int h6 = image6->allocation.height;
	

	load_image(image1,"./picture/back1.png",w1,h1);  //上一首
	load_image(image2,"./button/play1.jpg",w2,h2); //播放/暂停
	load_image(image3,"./picture/front1.png",w3,h3);  //下一首
	load_image(image4,"./picture/front.png",w4,h4);   //快进
	load_image(image5,"./picture/back.png",w5,h5);     // 回退
	load_image(image6,"./picture/muteoff.jpg",w6,h6);     // 静音
	
	chang_background(window,900,480,"./skin/back1.jpg");   //设置背景图片 
	
	GtkWidget * button_previous =glade_xml_get_widget(gxml,"button_previous");//上一首
	GtkWidget * button_next =glade_xml_get_widget(gxml,"button_next");//下一首
	
	//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
	g_signal_connect(button_previous, "clicked", G_CALLBACK(song_name_), "up");
	//上一首
	g_signal_connect(button_next, "clicked", G_CALLBACK(song_name_), "next");
	//下一首
	//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
	
	
	GtkWidget * button_pause =glade_xml_get_widget(gxml,"button_pause");//播放/暂停
	GtkWidget * fast_forward =glade_xml_get_widget(gxml,"fast_forward");//快进
	GtkWidget * rewind =glade_xml_get_widget(gxml,"rewind");//后退
	
	
	g_signal_connect(button_pause, "pressed", G_CALLBACK(gtk_next),"pause");  //播放/暂停 
	g_signal_connect(fast_forward, "pressed", G_CALLBACK(gtk_next),"fast_forward");//快进
	g_signal_connect(rewind, "pressed", G_CALLBACK(gtk_next),"rewind"); //后退
	
	
	GtkWidget * sound1 =glade_xml_get_widget(gxml,"sound1");//音量+
	GtkWidget * sound2 =glade_xml_get_widget(gxml,"sound2");//音量—
	GtkWidget * quiet =glade_xml_get_widget(gxml,"quiet");//静音
	g_signal_connect(sound1, "pressed", G_CALLBACK(gtk_next),"sound_add");  //音量+
	g_signal_connect(sound2, "pressed", G_CALLBACK(gtk_next),"sound_less");//音量—
	g_signal_connect(quiet, "pressed", G_CALLBACK(gtk_next),"quiet"); //静音
	
	
	g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);//销毁窗口
		
	gtk_main();
	
	}
	
//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk事件盒子kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk


  void dir(void)
{
	int i=0;
	DIR *dir;
	struct dirent *dirp;
	if((dir=opendir("./song"))==NULL)
	{
		printf("Open dir tmpdir fail\n");
		//exit(1);
	}
	while((dirp=readdir(dir))!=NULL)
	{
		if(strstr(dirp->d_name,"mp3")!=NULL)
		{
			song_list[i]=(char*)malloc(sizeof(dirp->d_name)+1);
			strcpy(song_list[i],dirp->d_name);
			i++;
			song_num++;
		}  

			
	}
	printf("<<<<<<<<<<<<<<<<<<<歌曲数目:%d\n",song_num);
	closedir(dir);
}

	void *pipe_read_pthread(){     								
		int size;
		//char msg_buf[100];
		while(1){
			memset(buffer, 0 , sizeof(buffer)) ;

			size=read(fd_pipe[0],buffer,sizeof(buffer)) ;     //读取mplayer发过来的歌曲信息
			if(size==-1){
				perror("read pipe");
				exit(1);
			}

			if( size == 0)//如果没有读到信息，则返回继续读取
				continue;
			buffer[size]='\0';//使信息变成字符串，便于处理
			printf("******************buf=%s\n",buffer);
			
			if(strncmp(buffer,"ANS_META",8) ==0){ //获取歌曲信息
				 buffer[(strlen(buffer)-2)]='\0';//多减一个去掉引号
			 }
			 
			 
			 printf(">>>>>>>>>>>>>>>>>>>>>>hh\n");
		}
		

	return ;
	}

	
   //信息处理函数*********************************************
	

	/*****************************************************************
	*     函数功能:发送获取歌词播放时间命令线程：
	*
	*********************************************************************/

	void *get_pos_pthread()    //发送获取歌词播放时间命令线程：
	{
	while(1)
	{
		if(my_lock==1 )     //播放的时候
		{
			sleep(1);
			send_cmd("get_percent_pos\n");   //获取播放进度
			sleep(1);
			send_cmd("get_time_pos\n");   //获取播放时间
			sleep(1);
			send_cmd("get_time_length\n");  //获取歌曲总时间
		} 
	}
	return NULL;
	}
	

	
void *pipe_read_dispose_pthread()
{
 while(1)
 {
   if(strncmp(buffer,"ANS_PERCENT_POSITION", 20)==0) //获取进度信息
   {
     //song->percent_dispose(buffer);
	 song->song_progress=strtok(buffer,"=");       //切出歌曲进度
   }

   else if(strncmp(buffer,"ANS_TIME_POSITION", 17) ==0) //获取歌曲当前播放时间
   {
    //time_dispose(buffer);
	song->song_time=strtok(NULL,"="); 
	song->time=atoi(song->song_time);
	song->minute=song->time/60;        //获取当前播放的歌曲时间 :minute
	song->second=song->time%60;       //获取当前播放的歌曲时间 :second
	
	printf("hhhhhhhhhhhhhhhhhhhhhh\n");
	
	sprintf(song->song_time_info,"%02d:%02d",song->minute,song->second);
	//sprintf(song->time_second,"%d",song->second);
	//gtk_label_set_text(GTK_LABEL(label1),"song->song_time_info");
	
   }
   else if(strncmp(buffer,"ANS_LENGTH",10) ==0) //获得歌的总长度
   {
     //length_dispose(buf);
	 song->song_time=strtok(NULL,"=");
	song->time_length=atoi(song->song_time);
	sprintf(song->song_time_length,"%02d:%02d",(song->time_length)/60,(song->time_length)%60);
	//gtk_label_set_text(GTK_LABEL(label2),"song->song_time_length");  
   }
 }

return NULL;

}

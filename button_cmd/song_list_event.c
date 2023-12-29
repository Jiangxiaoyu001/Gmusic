#include<stdio.h>
#include<gtk/gtk.h>
#include<string.h>
#include <dirent.h>
#include<stdlib.h>
#include "../mywindow.h"    
#include "../other/sungtk_interface.h"

void song_name_list(void)
{	
	int i = 0; DIR *dir;
	struct dirent *dirp;

	if (!(dir = opendir("./song"))) {
		perror("Open dir tmpdir fail");
		return;
	}
	/* itear song dir store all song-name */
	while ((dirp = readdir(dir)) != NULL) {
		
		if (strstr(dirp->d_name, "mp3") != NULL) {
			song_list[i] = (char*)malloc(sizeof(dirp->d_name) + 1);
			memcpy(song_list[i], dirp->d_name, strlen(dirp->d_name));
			printf("song_list:%s\n",song_list[i]);
			i++;song_num++;
		}  	
	}

	closedir(dir);
}
    
/********************************************************************
*      函数功能：设置点击信号，切换歌曲
*
*
*************************************************************************/
gboolean col_callback(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	row=(int)data;    //点击的行数
	char buf[100];
	char *sp;
	j=row;
	printf("row========%d\n",row);
	printf("%s\n",song_list[row]);
	sungtk_clist_select_row(clist, row, "red");       //选择当前行颜色
	//sungtk_clist_set_text_size(clist, 20);                  //设置字体大小
	sprintf(buf,"loadfile ./song/%s\n",song_list[row]); 
	write(fp,buf,strlen(buf));
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
	if(event_num < -220){                         //最右端
		event_num = -220;
	}
	if(event_num > 20){                         //最右端
		event_num = 0;
	}
	gtk_fixed_move((GtkFixed*)clist->fixed,clist->vbox,0,event_num);
}

// 读取歌词文件
void dir_lrc(void){	
	int k=0;
	DIR *dir;
	struct dirent *dirp;
	if((dir=opendir("./lyrics"))==NULL)
		printf("Open dir tmpdir fail\n");
	while((dirp=readdir(dir))!=NULL){
		if(strstr(dirp->d_name,"lrc")!=NULL){
			lrc_list[k]=(char*)malloc(sizeof(dirp->d_name)+1);
			strcpy(lrc_list[ik],dirp->d_name);
			printf("lrc_list=%s\n",lrc_list[k]);
			k++;
			lrc_num++;
		}  	
	}
	printf("<<<<<<<<<<<<<<<<<<<歌曲数目:%d\n",lrc_num);
	closedir(dir);
}       

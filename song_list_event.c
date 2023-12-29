#include<stdio.h>
#include<gtk/gtk.h>
#include<string.h>
#include <dirent.h>
#include<stdlib.h>
#include "./baseWindow.h"    
#include "./other/sungtk_interface.h"  

void dir(void){	
	int i=0;
	DIR *dir;
	struct dirent *dirp;
	if((dir=opendir("./song"))==NULL)
		printf("Open dir tmpdir fail\n");
	while((dirp=readdir(dir))!=NULL){
		if(strstr(dirp->d_name,".mp3")!=NULL){
			song_list[i]=(char*)malloc(sizeof(dirp->d_name)+1);
			strcpy(song_list[i],dirp->d_name);
			printf("song_list[%d]=%s\n",i,song_list[i]);
			i++;
			song_num++;
		}  	
	}
	printf("<<<<<<<<<<<<<<<<<<<������Ŀ:%d\n",song_num);
	closedir(dir);
}    

//��ȡ����ļ�
void dir_lrc(void){	
	int k=0;
	DIR *dir;                                   
	struct dirent *dirp;
	if((dir=opendir("./lyrics"))==NULL)
		printf("Open dir tmpdir fail\n");
	while((dirp=readdir(dir))!=NULL){
		if(strstr(dirp->d_name,"lrc")!=NULL){
			lrc_list[k]=(char*)malloc(sizeof(dirp->d_name)+1);
			strcpy(lrc_list[k],dirp->d_name);
			printf("lrc_list=%s\n",lrc_list[k]);
			k++;
			lrc_num++;
		}  	
	}
	printf("<<<<<<<<<<<<<<<<<<<������Ŀ:%d\n",lrc_num);
	closedir(dir);
}    

//��ȡƤ��ͼƬ   
void dir_skin(void){	
	int k=0;
	DIR *dir;                                   
	struct dirent *dirp;
	if((dir=opendir("./skin"))==NULL)
		printf("Open dir tmpdir fail\n");
		while((dirp=readdir(dir))!=NULL){
			if(strstr(dirp->d_name,"jpg")!=NULL){
				skin_list[k]=(char*)malloc(sizeof(dirp->d_name)+1);
				strcpy(skin_list[k],dirp->d_name);
				printf("skin_list=%s\n",skin_list[k]);
				k++;
				skin_num++;
			}  	
		}
	printf("<<<<<<<<<<<<<<<<<<<������Ŀ:%d\n",skin_num);
	closedir(dir);
}        	       
/********************************************************************
*      �������ܣ����õ���źţ��л�����
*
*
*************************************************************************/
gboolean col_callback(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	row=(int)data;
	char buf[100];
	char *sp;
	j=row;
	set_label();
	printf("row========%d\n",row);
	printf("%s\n",song_list[row]);
	sungtk_clist_select_row(clist, row, "green");       //ѡ��ǰ����ɫ
	//sungtk_clist_set_text_size(clist, 20);                  //���������С
	sprintf(buf,"loadfile ./song/%s\n",song_list[row]); 
	write(fp,buf,strlen(buf));
}
/****************************************************
*      �������ܣ�ʵ�ֹ�������
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
	if(event_num < -220){                         //���Ҷ�
		event_num = -220;
	}
	if(event_num > 20){                         //���Ҷ�
		event_num = 0;
	}
	gtk_fixed_move((GtkFixed*)clist->fixed,clist->vbox,0,event_num);
}

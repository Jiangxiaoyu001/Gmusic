#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
 #include<glade/glade.h> // glade ����ͷ�ļ�
#include<gtk/gtk.h>
	
#include "../other/sungtk_interface.h"

#include <dirent.h>

#include "gb2312_ucs2.h"


typedef struct song_info{

 char buf[100]; 
 char *time_minute;
 char *time_second;
 int  time_length;
 char *song_progress;      //��������
 char *song_time;
 char *song_time_info;     //����ʱ����Ϣ
  char *song_time_length;  //������ʱ��
 int time;
 int minute;
 int second;
 }SONG;

SONG  *song; 
 
 
static char buffer[100];
SunGtkCList *clist;    
 int fp,i,song_num;
char *str;
char *song_list[100];      //�����嵥
static int my_lock=1;

int fd_pipe[2];
pthread_t  tid1,tid2,tid3;
 
 

	void send_cmd(char *cmd)      //ͨ�������ܵ���mplayer��������
	{
		int re;
		re=write(fp,cmd,strlen(cmd));
		if(re==-1){
			perror("write cmd");
			return ;
		}
	}
 

 
/*******************************************************************
*      �������ܣ� ʵ�ָ������л�,��ͣ,������                       *                                   *
*      �������ͣ� gtk_next  ��ť�Ļص�����                         *                              *
*                 GtkWidget *button  ���жϰ�ť                    *
*                  gpointer data   ��  �ص������Ĳ���              *
*      ���ʱ�� �� 2019��1��16������                               *                       *
*                                                                  *
********************************************************************/

void gtk_next(GtkWidget *button, gpointer data){
	
	char *strr;
	strr=(char *)data;
	static int num=1;
	if(strcmp(strr,"sound_add")==0){  //������
		strr="volume 10\n";
		write(fp,strr,strlen(strr));
	}
		if(strcmp(strr,"sound_less")==0){  //������С
		strr="volume -10\n";
		write(fp,strr,strlen(strr));
	}
	
	if(strcmp(strr,"quiet")==0){ 
		if(num==0){
			strr="mute 0\n";              //ȡ������
			//extern void load_image(image2,"./button/button5.jpg",extern w2,h2); //����ͼƬ
			write(fp,strr,strlen(strr));
			num=1;
			return ;
		}
		if(num==1){
			strr="mute 1\n";             //����
			
	
			write(fp,strr,strlen(strr));
			num=0;
			return ;
		}
	}
	
	 if(strcmp(strr,"fast_forward")==0){  //���
		strr="seek 20\n";
		write(fp,strr,strlen(strr));
	}
	 if(strcmp(strr,"rewind")==0){  //����
		strr="seek 20\n";
		write(fp,strr,strlen(strr));
	}

	 if(strcmp(strr,"pause")==0){     //����/��ͣ
		strr="pause\n";
		if(my_lock=1){
			write(fp,strr,strlen(strr));
			my_lock=0;
			return ;
		}
		if(my_lock=0){
			write(fp,strr,strlen(strr));   //����/��ͣ
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
		printf("��һ�׸裺\n");
	}
	if(strcmp(str,"next")==0)
	{
		j++;
		if(j>song_num)
			j=0;
		sprintf(buf,"loadfile ./song/%s\n",song_list[j]);
		printf("********************\n");
		write(fp,buf,strlen(buf));
		printf("��һ�׸裺\n");
	}
}



//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk�¼�����kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
	
void load_image(GtkWidget *image, const char *file_path, const int w, const int h )
{
	gtk_image_clear( GTK_IMAGE(image) ); // ���ͼ��
	GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(file_path, NULL); // ����ͼƬ��Դ
	GdkPixbuf *dest_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf, w, h,
	GDK_INTERP_BILINEAR); // ָ����С
	gtk_image_set_from_pixbuf(GTK_IMAGE(image), dest_pixbuf); // ͼƬ�ؼ���������һ��ͼ
	//Ƭ(pixbuf)
	g_object_unref(src_pixbuf); // �ͷ���Դ
	g_object_unref(dest_pixbuf); // �ͷ���Դ
}
	
	
	
/************************************************************************
*      �������ܣ� ����������,��ť��ͼƬ,����ͼƬ��                     	*                                   *
*      �������ͣ� widget: ������                       		            *                              *
*                 w, h: ͼƬ�Ĵ�С                   		            *
*                 path�� ͼƬ·��              		                    *
*      ���ʱ�� �� 2019��1��16������                               		*                       *
*************************************************************************/

 void chang_background(GtkWidget *widget, int w, int h, const gchar *path)
{
	gtk_widget_set_app_paintable(widget, TRUE); //�����ڿ��Ի�ͼ
	gtk_widget_realize(widget);
	/* ���ı���ͼʱ��ͼƬ���ص�
	* ��ʱҪ�ֶ���������ĺ������ô��ڻ�ͼ����ʧЧ�����������ػ����¼����� expose �¼�����
	*/
	gtk_widget_queue_draw(widget);
	GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(path, NULL); // ����ͼƬ��Դ����
	// w, h ��ָ��ͼƬ�Ŀ�Ⱥ͸߶�
	GdkPixbuf *dst_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf, w, h,
	GDK_INTERP_BILINEAR);
	GdkPixmap *pixmap = NULL;
	/* ���� pixmap ͼ��;
	* NULL������Ҫ�ɰ�;
		* 123�� 0~255��͸������͸��
*/
	gdk_pixbuf_render_pixmap_and_mask(dst_pixbuf, &pixmap, NULL, 128);
	// ͨ�� pixmap �� widget ����һ�ű���ͼ�����һ����������Ϊ: FASLE
	gdk_window_set_back_pixmap(widget->window, pixmap, FALSE);
	// �ͷ���Դ
	g_object_unref(src_pixbuf);
	g_object_unref(dst_pixbuf);
	g_object_unref(pixmap);
}



/********************************************************************
*      �������ܣ����õ���źţ��л�����
*
*
*************************************************************************/


gboolean col_callback(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	int row=(int)data;    //���������
	
	char buf[100];
	char *sp;

	printf("row========%d\n",row);
	printf("%s\n",song_list[row]);
	sungtk_clist_select_row(clist, row, "red");       //ѡ��ǰ����ɫ
	//sungtk_clist_set_text_size(clist, 20);                  //���������С
	sprintf(buf,"loadfile ./song/%s\n",song_list[row]); 
	write(fp,buf,strlen(buf));

}




/***********************************************************************
*      ��������: ��ʾ�������ֵ������б�
*                 ת���ʽ��ʾ,��ʾ���Ĳ�����
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
	if(event_num < -220)                          //���Ҷ�
	{
		event_num = -220;
	}
	if(event_num > 20)                          //���Ҷ�
	{
		event_num = 0;
	}

	gtk_fixed_move((GtkFixed*)clist->fixed,clist->vbox,0,event_num);
	
}


	void window(){
	

	GladeXML *gxml=glade_xml_new("./glade/demo.glade",NULL,NULL);   // �����̴򿪹ܵ�

	GtkWidget *window = glade_xml_get_widget(gxml,"window1");// ���ش���
		
	GtkWidget *list_event=glade_xml_get_widget(gxml,"eventbox1"); //�����¼���
	
	//lable:��ʾʱ����ȵ�
	
	GtkWidget *label1=glade_xml_get_widget(gxml,"label1"); //��ȡ������ǰ����ʱ��
	gtk_label_set_text(GTK_LABEL(label1),"00:00");
	GtkWidget *label2=glade_xml_get_widget(gxml,"labe12");  //��ø���ܳ���
	gtk_label_set_text(GTK_LABEL(label2),"hhh");  

	
	
	GtkWidget *progressbar1=glade_xml_get_widget(gxml,"progressbar1");  //���ؽ�����
	
	
	clist=sungtk_clist_new();    //���������б�
	
	
	
	//���õ���ź�
	sungtk_clist_set_select_row_signal(clist, "button-press-event",col_callback);
	g_signal_connect(list_event, "motion_notify_event", G_CALLBACK(song_list_event),clist);
	
	
	
	GtkWidget *fixed=glade_xml_get_widget(gxml,"fixed2");
	
	 //���÷����б�Ŀ��
	 // sungtk_clist_set_row_height(clist, 480);
	 // sungtk_clist_set_col_width(clist, 180);
	

	gtk_container_add(GTK_CONTAINER(list_event),clist->fixed);
	song_name();  //��ʾ������Ϣ���б���

	
	//g_signal_connect(list_event, "motion_notify_event", G_CALLBACK(song_list_event));
	
		//  ͼƬ����
	GtkWidget * image1 =glade_xml_get_widget(gxml,"image1");  //��һ��

	GtkWidget * image3 =glade_xml_get_widget(gxml,"image3");  //��һ��

	GtkWidget * image2 =glade_xml_get_widget(gxml,"image2");//����/��ͣ

	GtkWidget * image4 =glade_xml_get_widget(gxml,"image4");   //���

	GtkWidget * image5 =glade_xml_get_widget(gxml,"image5"); //����
	
	GtkWidget * image6 =glade_xml_get_widget(gxml,"image6");   //����
	
	
	gtk_widget_show_all(window); // ��ʾ���пؼ�

	//��ȡͼƬ�ؼ��Ŀ�Ⱥ͸߶ȣ�������� gtk_widget_show_all()������
	int w1 = image1->allocation.width;    
	int h1 = image1->allocation.height;       //��һ��

	int w2 = image2->allocation.width;       //����/��ͣ
	int h2 = image2->allocation.height;

	int w3 = image3->allocation.width;     //��һ��
	int h3 = image3->allocation.height;


	int w4 = image4->allocation.width;         //���
	int h4 = image4->allocation.height;

	int w5 = image5->allocation.width;        // ����
	int h5 = image5->allocation.height;
	
	
	int w6 = image6->allocation.width;        // ����
	int h6 = image6->allocation.height;
	

	load_image(image1,"./picture/back1.png",w1,h1);  //��һ��
	load_image(image2,"./button/play1.jpg",w2,h2); //����/��ͣ
	load_image(image3,"./picture/front1.png",w3,h3);  //��һ��
	load_image(image4,"./picture/front.png",w4,h4);   //���
	load_image(image5,"./picture/back.png",w5,h5);     // ����
	load_image(image6,"./picture/muteoff.jpg",w6,h6);     // ����
	
	chang_background(window,900,480,"./skin/back1.jpg");   //���ñ���ͼƬ 
	
	GtkWidget * button_previous =glade_xml_get_widget(gxml,"button_previous");//��һ��
	GtkWidget * button_next =glade_xml_get_widget(gxml,"button_next");//��һ��
	
	//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
	g_signal_connect(button_previous, "clicked", G_CALLBACK(song_name_), "up");
	//��һ��
	g_signal_connect(button_next, "clicked", G_CALLBACK(song_name_), "next");
	//��һ��
	//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
	
	
	GtkWidget * button_pause =glade_xml_get_widget(gxml,"button_pause");//����/��ͣ
	GtkWidget * fast_forward =glade_xml_get_widget(gxml,"fast_forward");//���
	GtkWidget * rewind =glade_xml_get_widget(gxml,"rewind");//����
	
	
	g_signal_connect(button_pause, "pressed", G_CALLBACK(gtk_next),"pause");  //����/��ͣ 
	g_signal_connect(fast_forward, "pressed", G_CALLBACK(gtk_next),"fast_forward");//���
	g_signal_connect(rewind, "pressed", G_CALLBACK(gtk_next),"rewind"); //����
	
	
	GtkWidget * sound1 =glade_xml_get_widget(gxml,"sound1");//����+
	GtkWidget * sound2 =glade_xml_get_widget(gxml,"sound2");//������
	GtkWidget * quiet =glade_xml_get_widget(gxml,"quiet");//����
	g_signal_connect(sound1, "pressed", G_CALLBACK(gtk_next),"sound_add");  //����+
	g_signal_connect(sound2, "pressed", G_CALLBACK(gtk_next),"sound_less");//������
	g_signal_connect(quiet, "pressed", G_CALLBACK(gtk_next),"quiet"); //����
	
	
	g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);//���ٴ���
		
	gtk_main();
	
	}
	
//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk�¼�����kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk


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
	printf("<<<<<<<<<<<<<<<<<<<������Ŀ:%d\n",song_num);
	closedir(dir);
}

	void *pipe_read_pthread(){     								
		int size;
		//char msg_buf[100];
		while(1){
			memset(buffer, 0 , sizeof(buffer)) ;

			size=read(fd_pipe[0],buffer,sizeof(buffer)) ;     //��ȡmplayer�������ĸ�����Ϣ
			if(size==-1){
				perror("read pipe");
				exit(1);
			}

			if( size == 0)//���û�ж�����Ϣ���򷵻ؼ�����ȡ
				continue;
			buffer[size]='\0';//ʹ��Ϣ����ַ��������ڴ���
			printf("******************buf=%s\n",buffer);
			
			if(strncmp(buffer,"ANS_META",8) ==0){ //��ȡ������Ϣ
				 buffer[(strlen(buffer)-2)]='\0';//���һ��ȥ������
			 }
			 
			 
			 printf(">>>>>>>>>>>>>>>>>>>>>>hh\n");
		}
		

	return ;
	}

	
   //��Ϣ������*********************************************
	

	/*****************************************************************
	*     ��������:���ͻ�ȡ��ʲ���ʱ�������̣߳�
	*
	*********************************************************************/

	void *get_pos_pthread()    //���ͻ�ȡ��ʲ���ʱ�������̣߳�
	{
	while(1)
	{
		if(my_lock==1 )     //���ŵ�ʱ��
		{
			sleep(1);
			send_cmd("get_percent_pos\n");   //��ȡ���Ž���
			sleep(1);
			send_cmd("get_time_pos\n");   //��ȡ����ʱ��
			sleep(1);
			send_cmd("get_time_length\n");  //��ȡ������ʱ��
		} 
	}
	return NULL;
	}
	

	
void *pipe_read_dispose_pthread()
{
 while(1)
 {
   if(strncmp(buffer,"ANS_PERCENT_POSITION", 20)==0) //��ȡ������Ϣ
   {
     //song->percent_dispose(buffer);
	 song->song_progress=strtok(buffer,"=");       //�г���������
   }

   else if(strncmp(buffer,"ANS_TIME_POSITION", 17) ==0) //��ȡ������ǰ����ʱ��
   {
    //time_dispose(buffer);
	song->song_time=strtok(NULL,"="); 
	song->time=atoi(song->song_time);
	song->minute=song->time/60;        //��ȡ��ǰ���ŵĸ���ʱ�� :minute
	song->second=song->time%60;       //��ȡ��ǰ���ŵĸ���ʱ�� :second
	
	printf("hhhhhhhhhhhhhhhhhhhhhh\n");
	
	sprintf(song->song_time_info,"%02d:%02d",song->minute,song->second);
	//sprintf(song->time_second,"%d",song->second);
	//gtk_label_set_text(GTK_LABEL(label1),"song->song_time_info");
	
   }
   else if(strncmp(buffer,"ANS_LENGTH",10) ==0) //��ø���ܳ���
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

#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include <pthread.h>
#include<glade/glade.h> /* glade header files */
#include<gtk/gtk.h>	

#include "./other/sungtk_interface.h"                   
#include "./window/mywindow.h"
#include "./lrc_source/lyrics_process.h"
#include "./gb2312_source/gb2312_ucs2.h"
#include "./button_cmd/button_cmd.h"
#include "./pthread/my_pthread.h"

MUS play = {0};

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
char buffer[100];   
GtkWidget *label1;
GtkWidget *label2;
GtkWidget *label3;
GtkWidget *label4;
GtkWidget *label5;
GtkWidget *progressbar1;
GtkWidget * image8,*image2,*image6;
GtkWidget *window;
GtkWidget *list_event;
SunGtkCList *clist;
char *song_list[100];
char *lrc_list[100];
char *skin_list[100];                                //song_num
int lrc_num,row; 
int skin_num,skin=0;
int w2,h2,w6,h6,w8,h8;
int fd_pipe[2],ret,num=1;
int fp,song_num,row,my_lock=0,j=0;        
pthread_t  tid1,tid2,tid3;


struct songHelperHandle {
	char **song_list;
	char **lrc_list;
	char **skin_list;
} songHandle = {NULL};

void gtk_thread_init(void)
{
	if (FALSE == g_thread_supported())
		g_init(NULL);

	gdk_threads_init();        
}

/***********************************************************************
*      ��������: ��ʾ�������ֵ������б�
*                 ת���ʽ��ʾ,��ʾ���Ĳ�����
*
*
**********************************************************************/
void song_name(){                            //song_list_event                 
	int i;
	char buf[100];
	sungtk_clist_set_foreground(clist, "blue");           //����ǰ����ɫ
	sungtk_clist_select_row(clist, row, "green");       //ѡ��ǰ����ɫ
	for(i=0;i<song_num;i++){
		memset(buf,0,strlen(buf));
		strcpy(buf,song_list[i]);
		sungtk_clist_append(clist, buf);  //�Ѹ�����ӵ������б���
		sungtk_clist_set_row_color(clist, j,"green");   //��ɫ����ͬ��   ���õ�ǰ��
		//gtk_label_set_markup(GTK_LABEL(buf), "��ͨ <b>�Ӵ�</b> <i>��б</i> <u>�»���</u>");
	}
}

/************************************************************************
*      �������ܣ� ����������,��ť��ͼƬ,����ͼƬ��                     	*                                   *
*      �������ͣ� widget: ������                       		            *                              
*                 w, h: ͼƬ�Ĵ�С                   		            *
*                 path�� ͼƬ·��              		                    *
*      ���ʱ�� �� 2019��1��16������                               		*                       
*************************************************************************/
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

void chang_background(GtkWidget *widget, int w, int h, const gchar *path)
{
	gtk_widget_set_app_paintable(widget, TRUE); //�����ڿ��Ի�ͼ
	gtk_widget_realize(widget);
	// ���ı���ͼʱ��ͼƬ���ص�
	// ��ʱҪ�ֶ���������ĺ������ô��ڻ�ͼ����ʧЧ�����������ػ����¼����� expose �¼�����
	gtk_widget_queue_draw(widget);
	GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(path, NULL); // ����ͼƬ��Դ����
	// w, h ��ָ��ͼƬ�Ŀ�Ⱥ͸߶�
	GdkPixbuf *dst_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf, w, h,
	GDK_INTERP_BILINEAR);
	GdkPixmap *pixmap = NULL;
	gdk_pixbuf_render_pixmap_and_mask(dst_pixbuf, &pixmap, NULL, 128);
	gdk_window_set_back_pixmap(widget->window, pixmap, FALSE);
	g_object_unref(src_pixbuf);
	g_object_unref(dst_pixbuf);
	g_object_unref(pixmap);
}

void window_show()
{
	GladeXML *gxml = glade_xml_new("./glade/demo.glade",NULL,NULL);   // �����̴򿪹ܵ�
	window = glade_xml_get_widget(gxml,"window1");// ���ش���	
	list_event=glade_xml_get_widget(gxml,"eventbox1"); //�����¼���
	GtkWidget *fixed=glade_xml_get_widget(gxml,"fixed2");
	GtkWidget *fixed1=glade_xml_get_widget(gxml,"fixed1");
	label1=glade_xml_get_widget(gxml,"label1"); //��ȡ������ǰ����ʱ��
	label2=glade_xml_get_widget(gxml,"label2");  //��ø���ܳ���
	label3=glade_xml_get_widget(gxml,"label3");  //��ø�������
	label4=glade_xml_get_widget(gxml,"label4");  //��ø�����Ϣ
	label5=glade_xml_get_widget(gxml,"label5");  //��ø����Ϣ
	GtkWidget *progress_event=glade_xml_get_widget(gxml,"eventbox2"); //�����¼���2
	progressbar1=glade_xml_get_widget(gxml,"progressbar1");  //���ؽ�����
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar1),0);//��������ʼ��

	gdk_threads_enter();   //gtk�̼߳���
	gtk_label_set_text(GTK_LABEL(label1),"00:00");
	gtk_label_set_text(GTK_LABEL(label2),"00:00");
	gtk_label_set_text(GTK_LABEL(label3),"office.mp3");
	gtk_label_set_text(GTK_LABEL(label4),"please wait");
	gtk_label_set_text(GTK_LABEL(label5),"no lrc");
	gdk_threads_leave(); 	

	clist=sungtk_clist_new();    //���������б�
	sungtk_clist_set_select_row_signal(clist, "button-press-event",col_callback);  //���õ���ź�
	g_signal_connect(list_event, "motion_notify_event", G_CALLBACK(song_list_event),clist);
	gtk_container_add(GTK_CONTAINER(list_event),clist->fixed);

	song_name();  //��ʾ������Ϣ���б���

	GtkWidget * image1 =glade_xml_get_widget(gxml,"image1");  //��һ��
	GtkWidget * image3 =glade_xml_get_widget(gxml,"image3");  //��һ��
    image2 =glade_xml_get_widget(gxml,"image2");//����/��ͣ
	GtkWidget * image4 =glade_xml_get_widget(gxml,"image4");   //���
	GtkWidget * image5 =glade_xml_get_widget(gxml,"image5"); //����
    image6 =glade_xml_get_widget(gxml,"image6");   //����
	GtkWidget * image7 =glade_xml_get_widget(gxml,"image7");   //С����
	image8 =glade_xml_get_widget(gxml,"image8");   //����
	GtkWidget * image9 =glade_xml_get_widget(gxml,"image9"); //����
	GtkWidget * image10 =glade_xml_get_widget(gxml,"image10"); //������
	GtkWidget * image11 =glade_xml_get_widget(gxml,"image11"); //������

	gtk_widget_show_all(window); /* show window */

	//��ȡͼƬ�ؼ��Ŀ�Ⱥ͸߶ȣ�������� gtk_widget_show_all()������
	int w1 = image1->allocation.width;    
	int h1 = image1->allocation.height;       //��һ��
	 w2 = image2->allocation.width;       //����/��ͣ
	 h2 = image2->allocation.height;
	int w3 = image3->allocation.width;     //��һ��
	int h3 = image3->allocation.height;
	int w4 = image4->allocation.width;         //���
	int h4 = image4->allocation.height;
	int w5 = image5->allocation.width;        // ����
	int h5 = image5->allocation.height;
	w6 = image6->allocation.width;        // ����
	h6 = image6->allocation.height;
	int w7 = image7->allocation.width;        // С����
	int h7 = image7->allocation.height;
	 w8 = image7->allocation.width;        // Ƥ��
	 h8 = image8->allocation.height;
	int w9 = image9->allocation.width;        // С����
	int h9 = image9->allocation.height;
	int w10 = image10->allocation.width;        // ������
	int h10 = image10->allocation.height;
	int w11 = image11->allocation.width;        // ������
	int h11 = image11->allocation.height;
	load_image(image1,"./image/previous.png",w1,h1);  //��һ��
	load_image(image2,"./image/play.png",w2,h2); //����/��ͣ
	load_image(image3,"./image/next.png",w3,h3);  //��һ��
	load_image(image4,"./picture/front.png",w4,h4);   //���
	load_image(image5,"./picture/back.png",w5,h5);     // ����
	load_image(image6,"./image/mute0.png",w6,h6);     // ����
	load_image(image7,"./photos/window1.jpg",w7,h7);     // С����
	load_image(image8,"./image/skin.png",90,90);     // ����Ƥ��
	load_image(image9,"./button/lrc.png",w9,h9);     // ���ط����б�
	load_image(image10,"./image/sound0.png",w10,h10);     // ������ͼƬ
	load_image(image11,"./image/sound1.png",w11,h11);     // ������ͼƬ
	chang_background(window,900,480,"./skin/back1.jpg");   //���ñ���ͼƬ 
	GtkWidget *button_skin =glade_xml_get_widget(gxml,"button_skin");//Ƥ����ť
	g_signal_connect(button_skin, "clicked", G_CALLBACK(skin_chage), "skin_change");	//Ƥ��
	GtkWidget * button_previous =glade_xml_get_widget(gxml,"button_previous");//��һ��
	GtkWidget * button_next =glade_xml_get_widget(gxml,"button_next");//��һ��
	g_signal_connect(button_previous, "clicked", G_CALLBACK(song_chage), "up");	//��һ��
	g_signal_connect(button_next, "clicked", G_CALLBACK(song_chage), "next");//��һ��
	GtkWidget * button_pause =glade_xml_get_widget(gxml,"button_pause");//����/��ͣ
	GtkWidget * fast_forward =glade_xml_get_widget(gxml,"fast_forward");//���
	GtkWidget * rewind =glade_xml_get_widget(gxml,"rewind");//����
	g_signal_connect(button_pause, "pressed", G_CALLBACK(gtk_next),"pause");  //����/��ͣ 
	g_signal_connect(fast_forward, "pressed", G_CALLBACK(gtk_next),"fast_forward");//���
	g_signal_connect(rewind, "pressed", G_CALLBACK(gtk_next),"rewind"); //����
	GtkWidget * sound1 =glade_xml_get_widget(gxml,"sound1");//����+
	GtkWidget * sound2 =glade_xml_get_widget(gxml,"sound2");//������
	GtkWidget * quiet =glade_xml_get_widget(gxml,"quiet");//����
	
	/**********************************************************************
	*  �������ܣ��Ա�ǩ���г�ʼ��
	*          �� ��ǩ��ɫ�����������
	*
	***********************************************************************/
	sungtk_widget_set_font_color(label1, "black", FALSE);  //���ñ�ǩ��ɫ  ��ǰʱ��
	sungtk_widget_set_font_size(label1, 13, FALSE);  //���ñ�ǩ�����С
	sungtk_widget_set_font_color(label2, "black", FALSE);  //���ñ�ǩ��ɫ    ��ʱ��
	sungtk_widget_set_font_size(label2, 13, FALSE);  //���ñ�ǩ�����С
	sungtk_widget_set_font_color(label3, "blue", FALSE);  //���ñ�ǩ��ɫ   ����
	sungtk_widget_set_font_size(label3, 15, FALSE);  //���ñ�ǩ�����С
	sungtk_widget_set_font_color(label4, "blue", FALSE);  //���ñ�ǩ��ɫ   ����
	sungtk_widget_set_font_size(label4, 15, FALSE);  //���ñ�ǩ�����С
	sungtk_widget_set_font_color(label5, "purple", FALSE);  //���ñ�ǩ��ɫ    ���
    sungtk_widget_set_font_size(label5, 15, FALSE); //���ñ�ǩ�����С
	GtkWidget * button_window =glade_xml_get_widget(gxml,"button_window");//����
	g_signal_connect(button_window, "pressed", G_CALLBACK(window_hide),"hide"); 
	g_signal_connect(sound1, "pressed", G_CALLBACK(gtk_next),"sound_add");  //����+
	g_signal_connect(sound2, "pressed", G_CALLBACK(gtk_next),"sound_less");//������
	g_signal_connect(quiet, "pressed", G_CALLBACK(gtk_next),"quiet"); //����
	g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);//���ٴ���

	//���������ú���
	g_signal_connect(progress_event, "button-press-event", G_CALLBACK(song_progress_event), progressbar1);


	if((ret=pthread_create(&tid1,NULL,get_pos_pthread,NULL))==-1){
		perror("pthread");
		return ;
	} 
	if((ret=pthread_create(&tid2,NULL,pipe_read_pthread,NULL))==-1){
		 perror("pthread");
		 return ;
	 } 
	 if((ret=pthread_create(&tid3,NULL,pipe_read_lrc_pthread,NULL))==-1){
		 perror("pthread");
		 return ;                     
	 } 
	 
	gtk_main();

}	

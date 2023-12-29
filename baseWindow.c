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
*      函数功能: 显示歌曲名字到分栏列表
*                 转码格式显示,显示中文不乱码
*
*
**********************************************************************/
void song_name(){                            //song_list_event                 
	int i;
	char buf[100];
	sungtk_clist_set_foreground(clist, "blue");           //设置前景颜色
	sungtk_clist_select_row(clist, row, "green");       //选择当前行颜色
	for(i=0;i<song_num;i++){
		memset(buf,0,strlen(buf));
		strcpy(buf,song_list[i]);
		sungtk_clist_append(clist, buf);  //把歌曲添加到分栏列表中
		sungtk_clist_set_row_color(clist, j,"green");   //颜色反显同步   设置当前行
		//gtk_label_set_markup(GTK_LABEL(buf), "普通 <b>加粗</b> <i>倾斜</i> <u>下划线</u>");
	}
}

/************************************************************************
*      函数功能： 设置主窗口,按钮的图片,背景图片等                     	*                                   *
*      参数类型： widget: 主窗口                       		            *                              
*                 w, h: 图片的大小                   		            *
*                 path： 图片路径              		                    *
*      完成时间 ： 2019年1月16日周三                               		*                       
*************************************************************************/
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

void chang_background(GtkWidget *widget, int w, int h, const gchar *path)
{
	gtk_widget_set_app_paintable(widget, TRUE); //允许窗口可以绘图
	gtk_widget_realize(widget);
	// 更改背景图时，图片会重叠
	// 这时要手动调用下面的函数，让窗口绘图区域失效，产生窗口重绘制事件（即 expose 事件）。
	gtk_widget_queue_draw(widget);
	GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(path, NULL); // 创建图片资源对象
	// w, h 是指定图片的宽度和高度
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
	GladeXML *gxml = glade_xml_new("./glade/demo.glade",NULL,NULL);   // 父进程打开管道
	window = glade_xml_get_widget(gxml,"window1");// 加载窗口	
	list_event=glade_xml_get_widget(gxml,"eventbox1"); //加载事件框
	GtkWidget *fixed=glade_xml_get_widget(gxml,"fixed2");
	GtkWidget *fixed1=glade_xml_get_widget(gxml,"fixed1");
	label1=glade_xml_get_widget(gxml,"label1"); //获取歌曲当前播放时间
	label2=glade_xml_get_widget(gxml,"label2");  //获得歌的总长度
	label3=glade_xml_get_widget(gxml,"label3");  //获得歌曲名字
	label4=glade_xml_get_widget(gxml,"label4");  //获得歌手信息
	label5=glade_xml_get_widget(gxml,"label5");  //获得歌词信息
	GtkWidget *progress_event=glade_xml_get_widget(gxml,"eventbox2"); //加载事件框2
	progressbar1=glade_xml_get_widget(gxml,"progressbar1");  //加载进度条
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar1),0);//进度条初始化

	gdk_threads_enter();   //gtk线程加锁
	gtk_label_set_text(GTK_LABEL(label1),"00:00");
	gtk_label_set_text(GTK_LABEL(label2),"00:00");
	gtk_label_set_text(GTK_LABEL(label3),"office.mp3");
	gtk_label_set_text(GTK_LABEL(label4),"please wait");
	gtk_label_set_text(GTK_LABEL(label5),"no lrc");
	gdk_threads_leave(); 	

	clist=sungtk_clist_new();    //创建分栏列表
	sungtk_clist_set_select_row_signal(clist, "button-press-event",col_callback);  //设置点击信号
	g_signal_connect(list_event, "motion_notify_event", G_CALLBACK(song_list_event),clist);
	gtk_container_add(GTK_CONTAINER(list_event),clist->fixed);

	song_name();  //显示歌曲信息到列表中

	GtkWidget * image1 =glade_xml_get_widget(gxml,"image1");  //上一首
	GtkWidget * image3 =glade_xml_get_widget(gxml,"image3");  //下一首
    image2 =glade_xml_get_widget(gxml,"image2");//播放/暂停
	GtkWidget * image4 =glade_xml_get_widget(gxml,"image4");   //快进
	GtkWidget * image5 =glade_xml_get_widget(gxml,"image5"); //回退
    image6 =glade_xml_get_widget(gxml,"image6");   //静音
	GtkWidget * image7 =glade_xml_get_widget(gxml,"image7");   //小窗口
	image8 =glade_xml_get_widget(gxml,"image8");   //静音
	GtkWidget * image9 =glade_xml_get_widget(gxml,"image9"); //回退
	GtkWidget * image10 =glade_xml_get_widget(gxml,"image10"); //音量加
	GtkWidget * image11 =glade_xml_get_widget(gxml,"image11"); //音量减

	gtk_widget_show_all(window); /* show window */

	//获取图片控件的宽度和高度，必须放在 gtk_widget_show_all()函数后
	int w1 = image1->allocation.width;    
	int h1 = image1->allocation.height;       //上一首
	 w2 = image2->allocation.width;       //播放/暂停
	 h2 = image2->allocation.height;
	int w3 = image3->allocation.width;     //下一首
	int h3 = image3->allocation.height;
	int w4 = image4->allocation.width;         //快进
	int h4 = image4->allocation.height;
	int w5 = image5->allocation.width;        // 回退
	int h5 = image5->allocation.height;
	w6 = image6->allocation.width;        // 静音
	h6 = image6->allocation.height;
	int w7 = image7->allocation.width;        // 小窗口
	int h7 = image7->allocation.height;
	 w8 = image7->allocation.width;        // 皮肤
	 h8 = image8->allocation.height;
	int w9 = image9->allocation.width;        // 小窗口
	int h9 = image9->allocation.height;
	int w10 = image10->allocation.width;        // 音量加
	int h10 = image10->allocation.height;
	int w11 = image11->allocation.width;        // 音量减
	int h11 = image11->allocation.height;
	load_image(image1,"./image/previous.png",w1,h1);  //上一首
	load_image(image2,"./image/play.png",w2,h2); //播放/暂停
	load_image(image3,"./image/next.png",w3,h3);  //下一首
	load_image(image4,"./picture/front.png",w4,h4);   //快进
	load_image(image5,"./picture/back.png",w5,h5);     // 回退
	load_image(image6,"./image/mute0.png",w6,h6);     // 静音
	load_image(image7,"./photos/window1.jpg",w7,h7);     // 小窗口
	load_image(image8,"./image/skin.png",90,90);     // 更换皮肤
	load_image(image9,"./button/lrc.png",w9,h9);     // 隐藏分栏列表
	load_image(image10,"./image/sound0.png",w10,h10);     // 音量加图片
	load_image(image11,"./image/sound1.png",w11,h11);     // 音量减图片
	chang_background(window,900,480,"./skin/back1.jpg");   //设置背景图片 
	GtkWidget *button_skin =glade_xml_get_widget(gxml,"button_skin");//皮肤按钮
	g_signal_connect(button_skin, "clicked", G_CALLBACK(skin_chage), "skin_change");	//皮肤
	GtkWidget * button_previous =glade_xml_get_widget(gxml,"button_previous");//上一首
	GtkWidget * button_next =glade_xml_get_widget(gxml,"button_next");//下一首
	g_signal_connect(button_previous, "clicked", G_CALLBACK(song_chage), "up");	//上一首
	g_signal_connect(button_next, "clicked", G_CALLBACK(song_chage), "next");//下一首
	GtkWidget * button_pause =glade_xml_get_widget(gxml,"button_pause");//播放/暂停
	GtkWidget * fast_forward =glade_xml_get_widget(gxml,"fast_forward");//快进
	GtkWidget * rewind =glade_xml_get_widget(gxml,"rewind");//后退
	g_signal_connect(button_pause, "pressed", G_CALLBACK(gtk_next),"pause");  //播放/暂停 
	g_signal_connect(fast_forward, "pressed", G_CALLBACK(gtk_next),"fast_forward");//快进
	g_signal_connect(rewind, "pressed", G_CALLBACK(gtk_next),"rewind"); //后退
	GtkWidget * sound1 =glade_xml_get_widget(gxml,"sound1");//音量+
	GtkWidget * sound2 =glade_xml_get_widget(gxml,"sound2");//音量—
	GtkWidget * quiet =glade_xml_get_widget(gxml,"quiet");//静音
	
	/**********************************************************************
	*  函数功能：对标签进行初始化
	*          ： 标签颜色，字体等设置
	*
	***********************************************************************/
	sungtk_widget_set_font_color(label1, "black", FALSE);  //设置标签颜色  当前时间
	sungtk_widget_set_font_size(label1, 13, FALSE);  //设置标签字体大小
	sungtk_widget_set_font_color(label2, "black", FALSE);  //设置标签颜色    总时间
	sungtk_widget_set_font_size(label2, 13, FALSE);  //设置标签字体大小
	sungtk_widget_set_font_color(label3, "blue", FALSE);  //设置标签颜色   歌名
	sungtk_widget_set_font_size(label3, 15, FALSE);  //设置标签字体大小
	sungtk_widget_set_font_color(label4, "blue", FALSE);  //设置标签颜色   歌手
	sungtk_widget_set_font_size(label4, 15, FALSE);  //设置标签字体大小
	sungtk_widget_set_font_color(label5, "purple", FALSE);  //设置标签颜色    歌词
    sungtk_widget_set_font_size(label5, 15, FALSE); //设置标签字体大小
	GtkWidget * button_window =glade_xml_get_widget(gxml,"button_window");//隐藏
	g_signal_connect(button_window, "pressed", G_CALLBACK(window_hide),"hide"); 
	g_signal_connect(sound1, "pressed", G_CALLBACK(gtk_next),"sound_add");  //音量+
	g_signal_connect(sound2, "pressed", G_CALLBACK(gtk_next),"sound_less");//音量—
	g_signal_connect(quiet, "pressed", G_CALLBACK(gtk_next),"quiet"); //静音
	g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);//销毁窗口

	//进度条调用函数
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

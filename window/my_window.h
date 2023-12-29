#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
 #include<glade/glade.h> // glade 所需头文件
#include<gtk/gtk.h>



/*功能： 给创建好的图片控件重新设计一张图片
* image： 图片控件
* file_path: 图片的路径
* w, h： 指定图片的宽度和高度
*/
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
	
	
	/* 功能： 设置背景图
* widget: 主窗口
* w, h: 图片的大小
* path： 图片路径
*/
static void chang_background(GtkWidget *widget, int w, int h, const gchar *path)
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

	int  my_window(){

	//deal_auto_button(GtkWidget *image3, gpointer data);
	
	GladeXML *gxml=glade_xml_new("./glade/demo.glade",NULL,NULL);

	GtkWidget *window = glade_xml_get_widget(gxml,"window1");// 加载窗口

		//  图片放入
	GtkWidget * image1 =glade_xml_get_widget(gxml,"image1");  //上一首

	GtkWidget * image2 =glade_xml_get_widget(gxml,"image2");  //下一首

	GtkWidget * image3 =glade_xml_get_widget(gxml,"image3");//播放/暂停

	GtkWidget * image =glade_xml_get_widget(gxml,"image");   //快退

	GtkWidget * image4 =glade_xml_get_widget(gxml,"image4"); //快进


	GtkWidget *button1=gtk_button_new_with_label("后退");
	//g_signal_connect(button1,"pressed",G_CALLBACK(deal_pressed),"欢迎");
	gtk_container_add(GTK_CONTAINER(window),button1);


	GtkWidget *button2=gtk_button_new_with_label("快进");
	//g_signal_connect(button1,"pressed",G_CALLBACK(deal_pressed),"欢迎");
	gtk_container_add(GTK_CONTAINER(window),button2);



	GtkWidget *sungtk_scrolled_window_new();
	//创建一个滚动窗口
	//sungtk_widget_set_font_color(GtkWidget *widget, const char *color_buf, gboolean is_button);

	gtk_widget_show_all(window); // 显示所有控件

	//获取图片控件的宽度和高度，必须放在 gtk_widget_show_all()函数后
	int w1 = image1->allocation.width;
	int h1 = image1->allocation.height;

	int w2 = image2->allocation.width;
	int h2 = image2->allocation.height;

	int w3 = image3->allocation.width;
	int h3 = image3->allocation.height;


	int w4 = image4->allocation.width;
	int h4 = image4->allocation.height;

	int w = image->allocation.width;
	int h = image->allocation.height;

	load_image(image1,"./picture/previous.bmp",w1,h1);  //上一首
	load_image(image,"./picture/previous.bmp",w,h);// 快退
	load_image(image2,"./picture/next.bmp",w2,h2);  //下一首
	load_image(image4,"./picture/next.bmp",w4,h4);  //快进
	load_image(image3,"./picture/play.bmp",w3,h3);  //播放/暂停

	chang_background(window,800,480,"./picture/tu.jpg");   //设置背景图片 

	return 0;
	}

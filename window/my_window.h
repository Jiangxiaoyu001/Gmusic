#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
 #include<glade/glade.h> // glade ����ͷ�ļ�
#include<gtk/gtk.h>



/*���ܣ� �������õ�ͼƬ�ؼ��������һ��ͼƬ
* image�� ͼƬ�ؼ�
* file_path: ͼƬ��·��
* w, h�� ָ��ͼƬ�Ŀ�Ⱥ͸߶�
*/
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
	
	
	/* ���ܣ� ���ñ���ͼ
* widget: ������
* w, h: ͼƬ�Ĵ�С
* path�� ͼƬ·��
*/
static void chang_background(GtkWidget *widget, int w, int h, const gchar *path)
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

	int  my_window(){

	//deal_auto_button(GtkWidget *image3, gpointer data);
	
	GladeXML *gxml=glade_xml_new("./glade/demo.glade",NULL,NULL);

	GtkWidget *window = glade_xml_get_widget(gxml,"window1");// ���ش���

		//  ͼƬ����
	GtkWidget * image1 =glade_xml_get_widget(gxml,"image1");  //��һ��

	GtkWidget * image2 =glade_xml_get_widget(gxml,"image2");  //��һ��

	GtkWidget * image3 =glade_xml_get_widget(gxml,"image3");//����/��ͣ

	GtkWidget * image =glade_xml_get_widget(gxml,"image");   //����

	GtkWidget * image4 =glade_xml_get_widget(gxml,"image4"); //���


	GtkWidget *button1=gtk_button_new_with_label("����");
	//g_signal_connect(button1,"pressed",G_CALLBACK(deal_pressed),"��ӭ");
	gtk_container_add(GTK_CONTAINER(window),button1);


	GtkWidget *button2=gtk_button_new_with_label("���");
	//g_signal_connect(button1,"pressed",G_CALLBACK(deal_pressed),"��ӭ");
	gtk_container_add(GTK_CONTAINER(window),button2);



	GtkWidget *sungtk_scrolled_window_new();
	//����һ����������
	//sungtk_widget_set_font_color(GtkWidget *widget, const char *color_buf, gboolean is_button);

	gtk_widget_show_all(window); // ��ʾ���пؼ�

	//��ȡͼƬ�ؼ��Ŀ�Ⱥ͸߶ȣ�������� gtk_widget_show_all()������
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

	load_image(image1,"./picture/previous.bmp",w1,h1);  //��һ��
	load_image(image,"./picture/previous.bmp",w,h);// ����
	load_image(image2,"./picture/next.bmp",w2,h2);  //��һ��
	load_image(image4,"./picture/next.bmp",w4,h4);  //���
	load_image(image3,"./picture/play.bmp",w3,h3);  //����/��ͣ

	chang_background(window,800,480,"./picture/tu.jpg");   //���ñ���ͼƬ 

	return 0;
	}

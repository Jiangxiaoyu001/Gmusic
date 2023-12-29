 #include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "sungtk_interface.h"
int song_num=17;
gboolean col_callback(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	int row=(int)data;
	printf("row========%d\n",row);
}

void song_list_event(GtkWidget *widget, GdkEventMotion *event, gpointer gui_data)
{
	SunGtkCList *clist=(SunGtkCList *)gui_data;
	static int now_x,now_y;
	static int old_x,old_y;	
	old_x = now_x;
	old_y = now_y;
	gint x = event->x;
	gint y = event->y;
	now_x = x;	
	now_y = y;
	static int event_num=0;
	event_num+=(now_y-old_y);
	printf("event_num = %d\n", event_num);
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


int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	GladeXML *gxml = glade_xml_new("./test.glade",NULL,NULL);
	GtkWidget *window =glade_xml_get_widget(gxml,"window");
	GtkWidget *list_event=glade_xml_get_widget(gxml,"eventbox1");
	SunGtkCList *clist=sungtk_clist_new();
	sungtk_clist_set_select_row_signal(clist, "button-press-event",col_callback);
	GtkWidget *fixed=glade_xml_get_widget(gxml,"fixed2");
	
	gtk_container_add(GTK_CONTAINER(list_event),clist->fixed);
	int i;
	char buf[100];
	for(i=0;i<song_num;i++)
	{
		bzero(buf,100);
		sprintf(buf,"--%d--",i);
		sungtk_clist_append(clist, buf);
	}
	g_signal_connect(list_event, "motion_notify_event", G_CALLBACK(song_list_event),clist);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}

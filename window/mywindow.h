#ifndef __MYWINDOW_H__
#define __MYWINDOW_H__
#include<gtk/gtk.h>
#include<glade/glade.h> // glade 所需头文件
#include "../other/sungtk_interface.h"

extern GtkWidget *window;	
extern GtkWidget * image6;
extern int fp,song_num,row,my_lock,j;
extern char *lrc_list[100];
extern int lrc_num;
extern int song_num;
extern char *song_list[100]; 
extern int fd_pipe[2];
extern GtkWidget *label1;
extern GtkWidget *label2;
extern GtkWidget *label3;
extern GtkWidget *label4;
extern GtkWidget *label5;
extern GtkWidget *progressbar1;
extern GtkWidget *list_event;
extern GtkWidget * image8,*image2,*image6;
extern GtkWidget *window;
extern  pthread_t  tid1,tid2,tid3;                 
extern pthread_mutex_t mutex;
extern char *skin_list[100];
extern int skin;
extern int w2, h2, w6, h6, num;
extern int skin_num;
extern SunGtkCList *clist;

extern void window_show();
extern void song_name();
extern void gtk_thread_init();
extern gboolean col_callback(GtkWidget *widget, GdkEventButton *event, gpointer data);
extern void song_list_event(GtkWidget *widget, GdkEventMotion *event, gpointer gui_data);


typedef struct music {
	char file_name[10];
	int time_pos;
	int time_length;
	int percent_pos;
}MUS;
extern MUS play;

#endif

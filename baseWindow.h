#ifndef __MYWINDOW_H__
#define __MYWINDOW_H__
#include<gtk/gtk.h>
#include<glade/glade.h> // glade 所需头文件
extern gboolean col_callback(GtkWidget *widget, GdkEventButton *event, gpointer data);
extern void song_list_event(GtkWidget *widget, GdkEventMotion *event, gpointer gui_data);
extern GtkWidget *window;	
extern GtkWidget * image6;
void song_name();
extern void gtk_thread_init();
extern int fp,song_num,row,my_lock,j;
extern char *lrc_list[100];
extern int lrc_num;
extern int song_num;
extern char *song_list[100]; 
extern int fd_pipe[2];
extern  pthread_t  tid1,tid2,tid3;
extern pthread_mutex_t mutex;
extern char *skin_list[100];
extern int skin_num;
#endif

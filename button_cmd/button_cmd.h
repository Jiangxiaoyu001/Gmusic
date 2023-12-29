#ifndef __BUTTON_CMD_H__
#define __BUTTON_CMD_H__
#include<gtk/gtk.h>
void gtk_next(GtkWidget *button, gpointer data);
void skin_chage(GtkButton * button,gpointer data);
void song_progress_event(GtkWidget *widget, GdkEventMotion *event, gpointer gui_data);
void song_chage(GtkButton * button,gpointer data);
void window_hide(GtkButton * button,gpointer data);
#endif
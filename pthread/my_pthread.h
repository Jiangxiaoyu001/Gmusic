#ifndef __MY_PTHREAD_H__
#define __MY_PTHREAD_H__
void * pipe_read_lrc_pthread();
void send_cmd(char *cmd);
void *pipe_read_pthread();
void *get_pos_pthread();
void set_label();
#endif
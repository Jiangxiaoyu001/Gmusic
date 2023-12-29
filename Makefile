CC = gcc  
MAINC = mplayer.c
MAIN =./other/sungtk_interface.c
DEMO=./gb2312_source/gb2312_ucs2.c
EXEC = Gmusical
WINDOW = baseWindow.c
MY=song_list_event.c
MD=./lrc_source/lyrics_process.c
BUTTON=./button_cmd/button_cmd.c
CFLAGS = `pkg-config --cflags --libs gtk+-2.0 libglade-2.0`
PTHREAD=./pthread/my_pthread.c

main:  
	$(CC)  $(MAINC) $(MAIN)  $(DEMO)  $(WINDOW) $(MY)  $(MD) $(PTHREAD)  $(DIR) $(BUTTON)  -o $(EXEC) $(CFLAGS) -lgthread-2.0
clean:
	rm $(EXEC)      -rf

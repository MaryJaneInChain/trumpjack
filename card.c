#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "card.h"

GdkPixbuf** init_card(){
	GdkPixbuf** cards_pixbuf = (GdkPixbuf**)calloc(13, sizeof(GdkPixbuf*));
	GdkPixbuf* total_pixbuf;
	GError** error = NULL;
	int total_width;
	int total_height;
	int single_width;

	total_pixbuf = gdk_pixbuf_new_from_file("cards.svg", error);
	total_width = gdk_pixbuf_get_width(total_pixbuf);
	total_height = gdk_pixbuf_get_height(total_pixbuf);
	single_width = total_width/13;
	
	for(int i=0;i<12;i++){
		cards_pixbuf[i] = gdk_pixbuf_new_subpixbuf(total_pixbuf, i*single_width, 0, single_width, total_height);	
	}
	cards_pixbuf[12] = NULL;

	return cards_pixbuf; 
}

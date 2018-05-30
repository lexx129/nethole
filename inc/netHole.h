#ifndef NETHOLE_H
#define NETHOLE_H

#include <gtk/gtk.h>
#include <getopt.h>
// #include <gdk/gdk.h>
#ifndef STR_H
#include "structures.h"
#endif
#ifndef UTILS_H
#include "temp_utils.h"
#endif

#define BUFSIZE 1024   /* General character buffer size */

GtkWidget       *mainWindow;
GtkWidget       *ipSetsWindow;
GtkWidget       *portSetsWindow;

void netHole_init(int argc, char **argv);

gboolean ui_add_fhost(gpointer data);

gboolean ui_delete_fhost(gpointer data);

gboolean update_ifaces_model(gpointer data);


/*
*   Названия столбцов основной таблицы с ложными хостами
*/
typedef enum {
	COLUMN_NUMBER,
    COLUMN_IPADDR,
    COLUMN_STATUS,
    COLUMN_SOURCE,
    COLUMN_ROLE,
    COLUMN_ROLETEXT,
    COLUMN_ID
} column_types;

void status_to_string(int i, gchar *result);

GtkWidget* getChild(gchar *to_find, GtkContainer *parent);

#endif //(NETHOLE_H)
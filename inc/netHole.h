#ifndef NETHOLE_H
#define NETHOLE_H

#include <gtk/gtk.h>
// #include <gdk/gdk.h>
#ifndef STR_H
#include "structures.h"
#endif
#ifndef UTILS_H
#include "temp_utils.h"
#endif

GtkWidget       *mainWindow;
GtkWidget       *ipSetsWindow;
GtkWidget       *portSetsWindow;

void netHole_init(int argc, char **argv);

gboolean update_fhost_table(gpointer data);


/*
*   Названия столбцов основной таблицы с ложными хостами
*/
typedef enum {
    COLUMN_NUMBER,
    COLUMN_IPADDR,
    COLUMN_STATUS,
    COLUMN_SOURCE,
    COLUMN_ROLE
} column_types;

void status_to_string(int i, gchar *result);

#endif //(NETHOLE_H)
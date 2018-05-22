#include <gtk/gtk.h>


#include "netHole.h"

void netHole_init(int argc, char** argv)
{
	GtkBuilder      *builder; 
    
    gtk_init(&argc, &argv);
    builder = gtk_builder_new_from_file("glade/window_main.glade");
    mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
 
    gtk_widget_show(mainWindow);                
    gtk_main();
}

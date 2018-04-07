#include <gtk/gtk.h>

#include "netHole.h"

GtkWidget       *mainWindow;
GtkWidget       *ipSetsWindow;
GtkWidget       *portSetsWindow;
//GtkBuilder      *builder; 
 
int main(int argc, char *argv[])
{    
  
    GtkBuilder      *builder; 
    
    gtk_init(&argc, &argv);
    // builder = gtk_builder_new();
    builder = gtk_builder_new_from_file("glade/window_main.glade");
    // gtk_builder_add_from_file (builder, "glade/window_main.glade", NULL);
 
    mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
 
    g_object_unref(builder);
 
    gtk_widget_show(mainWindow);                
    gtk_main();
 
    return 0;
}

// called when window is closed
void on_window1_destroy()
{
    gtk_main_quit();
}

void on_btn_changeIpAddrSet_clicked()
{
    if (gtk_widget_get_realized(ipSetsWindow))
        gtk_window_present(GTK_WINDOW(ipSetsWindow));
    else {    
    
        // if (gtk_window_is_active(GTK_WINDOW(ipSetsWindow)))
        //     printf("%s\n", "window is active");
        // else printf("%s\n", "window is not active");

        GtkBuilder      *builder; 
                  
        builder = gtk_builder_new_from_file("glade/window_ipSets.glade");

        // builder = gtk_builder_new();
        // gtk_builder_add_from_file (builder, "glade/window_ipSets.glade", NULL);

        ipSetsWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_ipAddrSets"));
            
        gtk_builder_connect_signals(builder, NULL);
        g_object_unref(builder);
             
        gtk_widget_show(ipSetsWindow);    
    }
}

void on_btn_changePortsSet_clicked()
{
    if (gtk_widget_get_realized(portSetsWindow))
        gtk_window_present(GTK_WINDOW(portSetsWindow));
    else
    {
        GtkBuilder      *builder; 
                    
        builder = gtk_builder_new_from_file("glade/window_portSets.glade");

        // builder = gtk_builder_new();
        // gtk_builder_add_from_file (builder, "glade/window_ipSets.glade", NULL);

        portSetsWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_portSets"));
            
        gtk_builder_connect_signals(builder, NULL);
        g_object_unref(builder);
             
        gtk_widget_show(portSetsWindow);    
    }
}


void on_window_ipAddrSet_destroy()
{
    gtk_widget_destroy(ipSetsWindow);
}


void on_window_portSets_destroy()
{
    gtk_widget_destroy(portSetsWindow);
}
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
    builder = gtk_builder_new_from_file("glade/window_main.glade");
    mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
 
    gtk_widget_show(mainWindow);                
    gtk_main();
 
    return 0;
}

/* 
*   Обработчик закрытия главного окна
*/
void on_window_main_destroy()
{
    gtk_main_quit();
}


/*
*   Обработчик кнопки настройки множеств IP-адресов
*/
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

/*
*   Обработчик кнопки настройки множеств портов
*/
void on_btn_changePortsSet_clicked()
{

    /*проверяем, отрисовывалось ли уже это окно */
    if (gtk_widget_get_realized(portSetsWindow))         
        gtk_window_present(GTK_WINDOW(portSetsWindow));
    else
    {
        GtkBuilder      *builder; 
                    
        builder = gtk_builder_new_from_file("glade/window_portSets.glade");
        portSetsWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_portSets"));
        gtk_builder_connect_signals(builder, NULL);
        g_object_unref(builder);
             
        gtk_widget_show(portSetsWindow);    
    }
}

/*
*   Закрыто окно настройки множеств IP-адресов
*/
void on_window_ipAddrSet_destroy()
{
    gtk_widget_destroy(ipSetsWindow);
}

/*
*   Закрыто окно настройки множеств портов
*/
void on_window_portSets_destroy()
{
    gtk_widget_destroy(portSetsWindow);
}

/*
*   Настройки - Общие - Подсеть
*/
void on_entry_subnet_changed(GtkEditable *subnetEntry)
{
    gint res;
    // printf("%s\n", "Callback!");
    gchar *contents = gtk_editable_get_chars(subnetEntry, 0, -1);
    res = g_printf("%s\n", contents);
    g_free(contents);
}

/*
*   Настройки - Общие - Маска подсети
*/
void on_entry_netmask_changed(GtkEditable *subnetEntry)
{
    gint res;
    gchar *contents = gtk_editable_get_chars(subnetEntry, 0, -1);
    res = g_printf("%s\n", contents);
    g_free(contents);
}

/*
*   Настройки - Ловушка - Размер окна
*/
void on_spnbtn_throttleSize_value_changed(GtkSpinButton *throttleSize)
{
    gint value;
    gint res;
    value = gtk_spin_button_get_value_as_int(throttleSize);
    res = g_printf("%d\n", value);
}

/*
*   Настройки - Ловушка - Активность хостов
*/
void on_scl_tapitActivity_value_changed(GtkRange *tarpitActivity)
{
    gint value;
    gint res;
    GtkAdjustment *adj;
    adj = gtk_range_get_adjustment(tarpitActivity);
    value = gtk_adjustment_get_value(adj);
    res = g_printf("%d\n", value);
}

/*
*   Обработчик, который получает список сетевых интерфейсов
*   для комбобокса при отрисовке вкладки с настройками
*/
void on_combobox_interface_map(GtkWidget *interfaces, GtkListStore *ifacesModel)
{
    
    // GtkTreeStore *ifacesModel;
    GtkTreeIter iter;
    gboolean next;
    gchar *ifaceName;
    gint rowCount;
    gchar *path;

    //  ifacesModel = gtk_combo_box_get_model(GTK_COMBO_BOX(interfaces));
   
    /*проверка, заполнен уже список или нет*/
    next = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(ifacesModel), &iter);

    /*итератор не установился -> список не заполнен*/
    if(!next){
    /*тут будет код, 
            получающий список сетевых устройств*/    
    }

    while (next)
    {
        gtk_tree_model_get(GTK_TREE_MODEL(ifacesModel), &iter, 0, &ifaceName, -1);
        g_print("IFace #%d name: %s\n", rowCount, ifaceName);
        g_free(ifaceName);
        next = gtk_tree_model_iter_next(GTK_TREE_MODEL(ifacesModel), &iter);
        rowCount++;
    }
    
}

void on_combobox_interface_changed(GtkComboBox *ifaces, GtkListStore *ifacesModel)
{
    GtkTreeIter iter;
    gboolean next;
    gchar *ifaceName;

    next = gtk_combo_box_get_active_iter(ifaces, &iter);
    if (next)
    {
        gtk_tree_model_get(GTK_TREE_MODEL(ifacesModel), &iter, 0, &ifaceName, -1);
        g_print("Selected iface: %s\n", ifaceName);
        g_free(ifaceName);
    }
    else
        g_print("Didn't got iterator :(");

}


// static gchar* format_value_callback(GtkScale    *scale,
//                                     gdouble     value)
// {
//     return g_strdup_printf("-->\%0.*g<--",
//                             gtk_scale_get_digits(scale), value);
// }
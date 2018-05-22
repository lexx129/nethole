//#include <gtk/gtk.h>

#include "netHole.h"

GtkWidget       *mainWindow;
GtkWidget       *ipSetsWindow;
GtkWidget       *portSetsWindow;
// GtkBuilder      *builder; 
GtkWidget       *fhostsTree;
GMutex          mutex_interface;
 
int main(int argc, char *argv[])
{    
    gtk_init(&argc, &argv);
    GtkBuilder *builder;
    // init_fakeHosts_table();

    builder = gtk_builder_new_from_file("glade/window_main.glade");
    mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));

    gtk_builder_connect_signals(builder, NULL);
    fhostsTree = GTK_WIDGET(gtk_builder_get_object(builder, 
                                                    "tree_fakeHosts"));

    g_thread_new("background", init_fakeHosts_table, NULL);
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
*   Функция, вызываемая из другого потока для обновления 
*   таблицы ложных хостов
*/
gboolean update_fhost_table(gpointer data) /* data - указатель на новый хост */
{
    g_mutex_lock(&mutex_interface); 
    fake_host_t *new_host = data;
    GtkTreeModel *fhostsModel;
    GtkTreePath *path;
    GtkTreeIter iter;
    gboolean next;       
    
    /*
    *   Сначала проверим, что лежит в полученной модели
    */
    fhostsModel = gtk_tree_view_get_model(fhostsTree);
    gint i = 0;
    if (!gtk_tree_model_get_iter_first(fhostsModel, &iter))
    {
        g_print("***Fake hosts' roles model is empty!***\n");
    }

    gint rows = gtk_tree_model_iter_n_children(fhostsModel, NULL); /* Получим текущее кол-во столбцов.. */
    g_print("Now there is %d rows in a table\n", rows);
    path = gtk_tree_path_new_from_indices(rows - 1, -1);
    gtk_tree_model_get_iter(fhostsModel, &iter, path); /* ..чтобы получить указатель на крайний элемент */

    gtk_list_store_append(GTK_LIST_STORE(fhostsModel), &iter);
      
    gchar status[64];
    status_to_string(new_host->status, status); /* Переведем статус в строковый вид */
    gchar role[32] = "Без роли";

    gtk_list_store_set(GTK_LIST_STORE(fhostsModel), &iter, COLUMN_NUMBER, rows + 1,
                                COLUMN_IPADDR, ip_ntoa(&new_host->fake_host_addr),
                                COLUMN_STATUS, status,
                                COLUMN_SOURCE, ip_ntoa(&new_host->source_addr),
                                COLUMN_ROLE, role,
                                -1); /* запишем строку с данными нового хоста */
    
    g_mutex_unlock(&mutex_interface);
    return G_SOURCE_REMOVE;
}

void status_to_string(int i, gchar *result)
{
    // g_print("Got status %d\n ", i);
   
    switch (i) {
        case IDLE:
            sprintf(result, "Ожидание соединения");
            break;
        case IN_PROGRESS:
            sprintf(result, "Идет захват");
            break;
        case CAPTURED:
            sprintf(result, "Захвачен");
            break;
    }
    return;
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
   
    // printf("%s\n", "Callback!");
    gchar *contents = gtk_editable_get_chars(subnetEntry, 0, -1);
    g_print("New subnet: %s\n", contents);
    g_free(contents);
}

/*
*   Настройки - Общие - Маска подсети
*/
void on_entry_netmask_changed(GtkEditable *subnetEntry)
{
    gchar *contents = gtk_editable_get_chars(subnetEntry, 0, -1);
    g_print("New netmask: %s\n", contents);
    g_free(contents);
}

/*
*   Настройки - Ловушка - Размер окна
*/
void on_spnbtn_throttleSize_value_changed(GtkSpinButton *throttleSize)
{
    gint value;
    value = gtk_spin_button_get_value_as_int(throttleSize);
    g_print("New throttleSize: %d\n", value);
}

/*
*   Настройки - Ловушка - Активность хостов
*/
void on_scl_tapitActivity_value_changed(GtkRange *tarpitActivity)
{
    gint value;
    GtkAdjustment *adj;
    adj = gtk_range_get_adjustment(tarpitActivity);
    value = gtk_adjustment_get_value(adj);
    g_print("New tarpitActivity = %d %\n", value);
}

/*
*   Обработчик, который получает список сетевых интерфейсов
*   для комбобокса при отрисовке вкладки с настройками
*/
void on_combobox_interface_map(GtkWidget *interfaces,
                                GtkListStore *ifacesModel)
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

/*
*   Настройки - Общие - Сетевой интерфейс
*/
void on_combobox_interface_changed(GtkComboBox *ifaces,
                                    GtkListStore *ifacesModel)
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

/*
*   Обработчик изменения роли ложного хоста
*/
void on_roleRenderer_changed(GtkCellRendererCombo *roleCombo,
                                // gchar *path;
                                GtkTreeIter *newIter,
                                GtkListStore *rolesModel)
{
    GtkListStore *model;
    gboolean next;
    gchar *newrole;
    gint n;

    // next = gtk_tree_model_get_iter(GTK_TREE_MODEL(rolesModel), newIter);
    // if (next){
    g_print("Pointed model size: %d\n", sizeof(*rolesModel));
    g_object_get(roleCombo, "model", &model, NULL);
    g_print("Fetched model size: %d\n", sizeof(*model));
    g_print("Iter size: %d\n", sizeof(*newIter));
    if (newIter != NULL)
    {
        gtk_tree_model_get(GTK_TREE_MODEL(rolesModel), newIter, 0, &newrole, -1);
        g_print("Selected role: %s\n", newrole);
    }
    else g_print("iter is empty");


    // }
    // else
    //     g_print("Problem when getting the iterator");
    // g_free(newrole);
    g_object_unref(model);
}

// void on_tree_fakeHosts_map(GtkWidget *fakeHosts, 
//                             GtkListStore *roles)
// {
//     GtkListStore *newStore;
//     GtkTreeViewColumn *newroleColumn;
//     GtkTreeIter iter;
//     gint columnNumber;
//     int i;   

//     GtkCellRenderer *render; 

//     newStore = gtk_list_store_new(5, 
//                                     G_TYPE_INT,
//                                     G_TYPE_STRING,
//                                     G_TYPE_STRING,
//                                     G_TYPE_STRING,
//                                     G_TYPE_STRING);
//     // roles = gtk_list_store_new(1,
//     //                             G_TYPE_STRING);

//     gchar *addr;
//     gchar *status;
//     gchar *source;
//     gchar *role;

//     /*
//     *   Создаем новые записи для таблицы ложных хостов
//     */

//     for (i = 0; i < 10; i++)
//     {
//         addr = create_addr(i, TRUE);
//         // g_print("%d. Addr is %s\n", i, addr);
//         status = create_status(i);
//         // g_print("%d. Status is %s\n", i, status);
//         source = create_addr(i, FALSE);
//         // g_print("%d. Source addr is %s\n", i,  source);
//         // role = create_role(i);
//         // g_print("%d. Role is %s\n*****\n", i, role);

//         gtk_list_store_append(newStore, &iter);
//         gtk_list_store_set(newStore, &iter, COLUMN_NUMBER, i+1,
//                             COLUMN_IPADDR, addr,
//                             COLUMN_STATUS, status,
//                             COLUMN_SOURCE, source,
//                             // COLUMN_ROLE, role,
//                             -1);

//         g_free(addr);
//         g_free(status);
//         g_free(source);
//         // g_free(role);
//     }


//     newroleColumn = gtk_tree_view_get_column(GTK_TREE_VIEW(fakeHosts), 4);
//     render = gtk_cell_renderer_combo_new();
//     create_roles(roles); /*Заполняем список ролей*/

//     g_object_set(render, "model", roles, "text-column", 0, "has-entry", FALSE,
//                 "editable", TRUE, (char *)NULL);
//     newroleColumn = gtk_tree_view_column_new_with_attributes("Роль", render, "text", 1, NULL);
//     // gtk_tree_view_remove_column(GTK_TREE_VIEW(fakeHosts), 4);
//     gtk_tree_view_append_column(GTK_TREE_VIEW(fakeHosts), newroleColumn);

//     gboolean next;    
//     int j;
//     next = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(newStore), &iter);

//     while (next)
//     {
//         for (j = 0; j < 10; j++)
//         { 
//             gtk_tree_model_get(GTK_TREE_MODEL(newStore), &iter, 
//                                                          1, &addr,
//                                                          2, &status,
//                                                          3, &source,
//                                                          4, &role,
//                                                          -1);
//             g_print("Host #%d: addr=%s, status:%s, source:%s, role:%s \n", 
//                 j, addr, status, source, role);
//             g_free(addr);
//             g_free(status);
//             g_free(source);
//             g_free(role);
//             next = gtk_tree_model_iter_next(GTK_TREE_MODEL(newStore), &iter);
//             //rowCount++;
//         }
//     }
//             // gtk_tree_view_set_model(GTK_TREE_VIEW(fakeHosts), 
//                                         // GTK_TREE_MODEL(newStore));
// }




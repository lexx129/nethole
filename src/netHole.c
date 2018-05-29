//#include <gtk/gtk.h>

#include "netHole.h"

GtkWidget       *mainWindow;
GtkWidget       *additWindow;
GtkWidget       *ipSetsWindow;
GtkWidget       *portSetsWindow;
// GtkBuilder      *builder; 
GtkWidget       *fhostsTree;
GtkWidget       *ifacesCombo;
GMutex          mutex_interface;
 
int main(int argc, char *argv[])
{    
    gtk_init(&argc, &argv);
    GtkBuilder *builder;
    // init_fakeHosts_table();

    builder = gtk_builder_new_from_file("glade/window_main.glade");
    mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    additWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_additional"));

    gtk_builder_connect_signals(builder, NULL);
    fhostsTree = GTK_WIDGET(gtk_builder_get_object(builder, 
                                                    "tree_fakeHosts"));
    ifacesCombo = GTK_WIDGET(gtk_builder_get_object(builder,
                                                    "combobox_iface"));

    /* 
    *  Все, что не связано с интерфейсом напрямую, 
    *  будет работать в другом потоке
    */
    g_thread_new("background", init_background, NULL);

    g_object_unref(builder);
 
    gtk_widget_show(additWindow);
    gtk_widget_show(mainWindow);      

    gtk_main();
 
    return 0;
}

/*
*   Сначала функции для локального обновления виджетов и 
*   связанных с ними объектов (для вызова из других потоков). 
*   За ними -- обработчики сигналов виджетов.
*/

/*
*   Функция, вызываемая из другого потока для обновления 
*   таблицы ложных хостов
*/
gboolean ui_add_fhost(gpointer data) /* data - указатель на новый хост */
{
    g_mutex_lock(&mutex_interface); 
    fake_host_t *new_host = data;
    GtkTreeModel *fhostsModel;
    GtkTreePath *path;
    GtkTreeIter iter;
    
    fhostsModel = gtk_tree_view_get_model(GTK_TREE_VIEW(fhostsTree));
   
    /* Получим текущее кол-во записей.. */
    gint rows = gtk_tree_model_iter_n_children(fhostsModel, NULL); 
    path = gtk_tree_path_new_from_indices(rows - 1, -1);

    if (gtk_tree_path_get_depth(path) == 0) /* Если таблица пуста, */
        gtk_tree_model_get_iter_first(fhostsModel, &iter); /* запросим указатель на первую строку */
    else
        gtk_tree_model_get_iter(fhostsModel, &iter, path); /* иначе -- на последнюю запись*/

    gtk_list_store_append(GTK_LIST_STORE(fhostsModel), &iter);
      
    gchar status[64];
    status_to_string(new_host->status, status); /* Переведем статус в строковый вид */
    gchar role[32] = "Без роли";
   
    gtk_list_store_set(GTK_LIST_STORE(fhostsModel), &iter,
                                COLUMN_NUMBER, rows + 1,
                                COLUMN_IPADDR, ip_ntoa(&new_host->fake_host_addr),
                                COLUMN_STATUS, status,
                                COLUMN_SOURCE, ip_ntoa(&new_host->source_addr),
                                COLUMN_ROLETEXT, role,
                                COLUMN_ID, new_host->id,
                                -1); /* запишем строку с данными нового хоста */
    
    g_mutex_unlock(&mutex_interface);
    return G_SOURCE_REMOVE;
}

gboolean ui_delete_fhost(gpointer data)
{
    fake_host_t *to_delete = (fake_host_t*)data;
    GtkTreeModel *fhostsModel;
    GtkTreeIter iter;
    gboolean next;

    int number;
    uint16_t curr_id;

    g_mutex_lock(&mutex_interface); 
    fhostsModel = gtk_tree_view_get_model(GTK_TREE_VIEW(fhostsTree));
    next = gtk_tree_model_get_iter_first(fhostsModel, &iter);
    if (!next){
        g_print("**Couldn't get fake hosts model**\n");
    }
    while (next)
    {   
    /* Нельзя вычитать только айди, вычитываем первый стоблик за компанию */     
        gtk_tree_model_get(fhostsModel, &iter, COLUMN_NUMBER, &number,
                            COLUMN_ID, &curr_id, -1);
        // g_print("Processing %d row with id = %d\n", number, curr_id);
        if (curr_id == to_delete->id){
            if (gtk_list_store_remove(GTK_LIST_STORE(fhostsModel), &iter)){
                g_print("Have to delete host with id %d\n \
                    Deleted fhost with id %d\n", to_delete->id, curr_id);
                next = FALSE;
                break;
            }
            else
                g_print("Unable to delete given row\n");
        } else
            // g_free(curr_id);
            next = gtk_tree_model_iter_next(fhostsModel, &iter);
        

    }
    g_mutex_unlock(&mutex_interface);
    return G_SOURCE_REMOVE;
}

void status_to_string(int i, gchar *result)
{
    // g_print("Got status %d\n ", i);
   
    switch (i) {
        case IDLE:
            sprintf(result, "Ожидание\nсоединения");
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

gboolean update_ifaces_model(gpointer data)
{
    GtkTreeModel *ifacesModel;
    GtkTreePath *path;
    GtkTreeIter iter;

    g_mutex_lock(&mutex_interface); 

    ifacesModel = gtk_combo_box_get_model(GTK_COMBO_BOX(ifacesCombo));

    gint rows = gtk_tree_model_iter_n_children(ifacesModel, NULL);
   // g_print("Now there is %d rows in a table\n", rows);
    path = gtk_tree_path_new_from_indices(rows - 1, -1);
    if (gtk_tree_path_get_depth(path) > 0)
        gtk_tree_model_get_iter(ifacesModel, &iter, path); 
    else
        gtk_tree_model_get_iter_first(ifacesModel, &iter);

    gtk_list_store_append(GTK_LIST_STORE(ifacesModel), &iter);

    gtk_list_store_set(GTK_LIST_STORE(ifacesModel), &iter, 0, (gchar*) data, -1); 
    
    g_mutex_unlock(&mutex_interface);

    return G_SOURCE_REMOVE;
}



/***********************
        Обработчики
***********************/

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
    if (gtk_widget_get_realized(ipSetsWindow)) /* Если окно уже отрисовано.. */
        gtk_window_present(GTK_WINDOW(ipSetsWindow)); /* .. поместим его поверх остальных */
    else {    
        GtkBuilder      *builder; 

        builder = gtk_builder_new_from_file("glade/window_ipSets.glade");
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

    /* проверяем, отрисовывалось ли уже это окно */
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

/* Настройки - Включить ловушку */
void on_switch_main_state_set(GtkSwitch *widget, gboolean state,
                                GtkLabel *status)
{
    if (state)
        gtk_label_set_text(status, "Статус ловушки: активна");
    else
        gtk_label_set_text(status, "Статус ловушки: остановлена");
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
    g_print("New tarpitActivity = %d \n", value);
}

/*
*   Настройки - Общие - Сетевой интерфейс
*/
void on_combobox_iface_changed(GtkComboBox *ifaces,
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
        g_print("Could't get iface combobox iterator");

}

/*
*   Обработчик изменения роли ложного хоста
*/
// void on_roleRenderer_changed(GtkCellRendererCombo *roleCombo,
//                                 GtkTreeIter *newIter,
//                                 GtkListStore *rolesModel)
// {
//     GtkListStore *model;
//     gboolean next;
//     gchar *newrole;
//     gint n;

//     // next = gtk_tree_model_get_iter(GTK_TREE_MODEL(rolesModel), newIter);
//     // if (next){
//     g_print("Pointed model size: %d\n", sizeof(*rolesModel));
//     g_object_get(roleCombo, "model", &model, NULL);
//     g_print("Fetched model size: %d\n", sizeof(*model));
//     g_print("Iter size: %d\n", sizeof(*newIter));
//     if (newIter != NULL)
//     {
//         gtk_tree_model_get(GTK_TREE_MODEL(rolesModel), newIter, 0, &newrole, -1);
//         g_print("Selected role: %s\n", newrole);
//     }
//     else g_print("iter is empty");


//     // }
//     // else
//     //     g_print("Problem when getting the iterator");
//     // g_free(newrole);
//     g_object_unref(model);
// }

/* Обработчик кнопки тестового удаления выбранного хоста из таблицы */
void on_button_delSelectedHost_clicked(GtkButton *button, 
                                    gpointer data)
{
    GtkTreeSelection    *selection;
    GtkTreeModel        *fhostsModel;
    GList               *selected;
    GtkTreeIter         iter;

    GtkTreeView *fhostsView = (GtkTreeView*)data;

    selection = gtk_tree_view_get_selection(fhostsView);
    fhostsModel = gtk_tree_view_get_model(fhostsView);

    /* получим GList с путями до выделенных элементов */
    selected = gtk_tree_selection_get_selected_rows(selection, 
                                        &fhostsModel);

    GList *curr;
    for (curr = selected; curr != NULL; curr = curr->next)
    {  
        uint16_t curr_id;
        gtk_tree_model_get_iter(fhostsModel, &iter, curr->data);
        gtk_tree_model_get(fhostsModel, &iter, 
                            COLUMN_ID, &curr_id, -1);
        g_print("Delete button is clicked.\nGonna delete host with id %d\n", curr_id);
        delete_selected_fhost(curr_id);
    }
}

void on_button_delRandHost_clicked()
{
    delete_random_fhost();
}


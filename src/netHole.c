//#include <gtk/gtk.h>

#include "netHole.h"

GtkWidget       *mainWindow;
GtkWidget       *additWindow;
GtkWidget       *ipSetsWindow;
GtkWidget       *portSetsWindow;
// GtkBuilder      *builder; 
GtkWidget       *fhostsTree;
GMutex          mutex_interface;

char dev[BUFSIZE]="";
 
int main(int argc, char *argv[])
{    
    GtkBuilder *builder;

    gtk_init(&argc, &argv);
    
    // init_fakeHosts_table();

    builder = gtk_builder_new_from_file("glade/window_main.glade");
    mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    additWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window_additional"));

    gtk_builder_connect_signals(builder, NULL);
    fhostsTree = GTK_WIDGET(gtk_builder_get_object(builder, 
                                                    "tree_fakeHosts"));
    // ifacesCombo = GTK_WIDGET(gtk_builder_get_object(builder,
    //                                                 "combobox_iface"));
    /* 
    *  Все, что не связано с интерфейсом напрямую, 
    *  будет работать в другом потоке -- фоновом
    */
    g_thread_new("background", init_background, NULL);
    netHole_init(argc, argv);

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

static int
read_number (u_char *p)
{
  char buf[BUFSIZE]="";
  char *invalid = NULL;
  int result = 0;

  strlcpy(buf, p, sizeof(buf));     /* Copy, watching for buffer overflow */
  result = strtol(buf, &invalid, 10); /* Convert to integer, checking validity */
  if ((strlen(invalid) > 0) || (result < 0)) {
    strlcpy(buf, invalid, sizeof(buf));
    // warnx("*** Invalid integer input: %s", buf);
    // input_error = TRUE;
  }
  return(result);
}

void netHole_init(int argc, char** argv)
{

    int option_index = 0;    /* Option index */  
    int usernet = FALSE;    /* True if user-specified subnet for capturing */
    char subnet[BUFSIZE]="";
    char netmask[BUFSIZE]="";
    // char dev[BUFSIZE]="";       /* Input device name */
    uint32_t throttlesize;
    gboolean isHardCapture = FALSE;
    gboolean isRandWindow = FALSE;

    int c = 0;         /* Index for getopt */


    static struct option long_options[] =
    {
      {"network",           required_argument,  0, 'n'},
      {"mask",              required_argument,  0, 'm'},
      {"device",                required_argument,  0, 'i'},
      {"throttle-size",             required_argument,  0, 't'},
      {"arp-timeout",               required_argument,  0, 'r'},
      {"exclude-resolvable-ips",        no_argument,        0, 'X'},
      {"disable-capture",       no_argument,        0, 'x'},
      {"hard-capture",              no_argument,        0, 'h'},
      {"auto-hard-capture",         no_argument,        0, 'H'},
      {"persist-mode-only",         no_argument,        0, 'P'},
      {"no-resp-synack",            no_argument,        0, 'a'},
      {"no-resp-excluded-ports",        no_argument,        0, 'f'},
      {"rand-mac",          required_argument,  0, 'M'}, 
      {"rand-window",           no_argument,        0, 'W'}, 
      {"tcp-opt",           no_argument,        0, 'C'}, 
      {"no-arp-sweep",              no_argument,        0, '3'},
      {"demo",         no_argument,   0,    'e'},
      {0, 0, 0, 0}
    };

    while(TRUE){
        c = getopt_long(argc, argv, "n:m:i:j:I:E:qF:t:r:sM:XWSCxhRHp:bPaflvoOVTdz?2:3De",
                long_options, &option_index);

        if (c == EOF)       /* If at end of options, then stop */
          break;

        switch(c) {
            case 'n': /* Подсеть */
                usernet = TRUE;
                strlcpy(subnet, optarg, sizeof(subnet));
                break;
            case 'm': /* Маска подсети */
                usernet = TRUE;
                strlcpy(netmask, optarg, sizeof(netmask));
                break;
            case 'i': /* Сетевой интерфейс */
                strlcpy(dev, optarg, sizeof(dev));
                /* lbio_init will do further checking */
                break;    
            case 't': /* Размер окна */
                throttlesize = read_number(optarg);

                break;
            case 'h': /* Жесткий захват */
                isHardCapture = TRUE;
                break;
            case 'W': /* Случайный размер окна */
                isRandWindow = TRUE;
                break;
        }

    }
    if (usernet){
        GtkWidget *subnetEntry;
        GtkWidget *netmaskEntry;
        subnetEntry = getChild("entry_subnet", mainWindow);
        if (!subnetEntry) /* Проверить, что виджет был найден */
            g_print("Couldn't find 'entry_subnet'\n");                       
        else
            gtk_entry_set_text(subnetEntry, subnet);
        netmaskEntry = getChild("entry_netmask", mainWindow);
        if (!subnetEntry)
            g_print("Couldn't find 'entry_netmask'\n");                       
        else
            gtk_entry_set_text(netmaskEntry, netmask);
    }   

    if (throttlesize){
        GtkWidget *windowSizeSpin;
        windowSizeSpin = getChild("spnbtn_throttleSize", mainWindow);
        if (!windowSizeSpin)
            g_print("Couldn't find window size spinButton\n");
        gtk_spin_button_set_value(windowSizeSpin, throttlesize);
    }

    if (isHardCapture){
        GtkWidget *randWindowCheck;
        randWindowCheck = getChild("chk_randWindow", mainWindow);
        if (randWindowCheck)
            gtk_toggle_button_set_active(randWindowCheck, TRUE);
    }
}

gboolean set_active_iface()
{
    if (strcmp(dev, "") != 0){
        // g_print("Setting iface from parameter..\n");
        GtkTreeModel    *ifacesModel;
        GtkWidget       *ifacesCombo;
        GtkTreeIter     curr;
        gboolean        has_next;
        gchar           *curr_iface_name;
    /* Функция выполняется быстрее, чем фоновый поток успевает заполнить список сетевых интерфейсов */
        ifacesCombo = getChild("combobox_iface", mainWindow);
        if (!ifacesCombo)
            g_print("Couldn't find ifaces combobox");
        g_mutex_lock(&mutex_interface); 
     
        ifacesModel = gtk_combo_box_get_model(GTK_COMBO_BOX(ifacesCombo));
        gint rows = gtk_tree_model_iter_n_children(ifacesModel, NULL);
        g_print("Got the ifaces model, it has %d positions\n", rows);
        has_next = gtk_tree_model_get_iter_first(ifacesModel, &curr);
        if (!has_next)
            g_print("Couldn't get iterator\n");

        while(has_next){
            gtk_tree_model_get(ifacesModel, &curr, 0, &curr_iface_name, -1);
            if (strcmp(curr_iface_name, dev) == 0){
                gtk_combo_box_set_active_iter(ifacesCombo, &curr);
                g_print("Setting %s as active iface\n", curr_iface_name);
                has_next = FALSE;
            }
            else
                has_next = gtk_tree_model_iter_next(ifacesModel, &curr);
        }
        g_mutex_unlock(&mutex_interface);
    }
    return G_SOURCE_REMOVE;
}

/* 
*   Ищет дочерний виджет по имени в родительском parent
*   Возвращает указатель на виджет, если находит его.
*   Иначе возвращает NULL
*/
GtkWidget* getChild(gchar *to_find, GtkContainer *parent)
{
    GList *children;
    GList *curr;

    GQueue *to_visit = g_queue_new();
    GList *visited = NULL;

    g_queue_push_tail(to_visit, parent);
    // visited = g_list_append(parent);
    
    while (!g_queue_is_empty(to_visit)){
        GtkWidget *temp = g_queue_pop_head(to_visit);
        // g_print("Processing widget named %s.\t", gtk_widget_get_name(temp));
        if (g_list_find(visited, temp) == NULL){
            // g_print("Marking it as visited\n");
            visited = g_list_append(visited, temp);
            if (GTK_IS_CONTAINER(temp)){
                // g_print("Children:\n");
                children = gtk_container_get_children(temp);
                for (curr = children; curr != NULL; curr = curr->next){
                    g_queue_push_tail(to_visit, curr->data);
                    // g_print("\t%s\n", gtk_widget_get_name(curr->data));
                    // g_print("Pushed element %s to queue\n", gtk_widget_get_name(curr->data));
                    if (strcmp(to_find, gtk_widget_get_name(curr->data)) == 0)
                        return curr->data;
                }
            } 
            // else g_print("No children\n");
        }
        // else g_print("It was visited earlier\n");
    }
    // g_print("queue is empty!\n");
    return NULL;
}

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
    GtkTreeModel    *ifacesModel;
    GtkTreePath     *path;
    GtkTreeIter     iter;
    GtkWidget       *ifacesCombo;


    g_mutex_lock(&mutex_interface); 
    ifacesCombo = getChild("combobox_iface", mainWindow);
    if (!ifacesCombo)
        g_print("Couldn't find ifaces combobox");
    

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



/*******************************************************************************
                                    Обработчики
********************************************************************************/

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


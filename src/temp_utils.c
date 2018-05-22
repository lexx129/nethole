#include <gtk/gtk.h>
#include "temp_utils.h"

#ifndef NETHOLE_H
#include "netHole.h"
#endif
// #include "structures.h"

/*
*   Несколько функций для тестового
*   заполнения таблицы ложных хостов
*/

gpointer init_fakeHosts_table()
{
    rand = rand_open();
    if (NULL == rand){
        g_print("Couldn't initialise random");
    }

    fake_host_t *host1 = create_fake_host();
    fake_host_t *host2 = create_fake_host();

    LIST_INIT(&fake_hosts);

    LIST_INSERT_HEAD(&fake_hosts, host1, host_next);
    LIST_INSERT_AFTER(host1, host2, host_next);

    fake_host_t *iter;
    int i = 0;
    LIST_FOREACH(iter, &fake_hosts, host_next) {
        g_idle_add(update_fhost_table, iter);

        g_print("Generated %d host. IP: %s\nMAC: %s\n \
Status: %d\nSource: %s\n****\n", i, 
                ip_ntoa(&iter->fake_host_addr), eth_ntoa(&iter->fake_host_mac),
                iter->status, ip_ntoa(&iter->source_addr));
        i++;
    }
}

fake_host_t *create_fake_host()
{
    fake_host_t *host = (fake_host_t *)malloc(sizeof(fake_host_t));
    rand_get(rand, &host->fake_host_addr, IP_ADDR_LEN);
    rand_get(rand, &host->fake_host_mac, ETH_ADDR_LEN);
    
    host->status = IDLE;
    
    rand_get(rand, &host->source_addr, IP_ADDR_LEN);

    return host;
}

gchar* create_addr(int i, gboolean flag)
{
    gchar* result = malloc(32);
    if (result == NULL)
        g_print("Alloc error");
    
    if (flag)
    {
        sprintf(result, "1.1.1.%d", i); 
    }
    else 
    {
        i+=32;
        sprintf(result, "10.10.0.%d", i);
    }
    return result;
}

gchar* create_status(int i)
{
    gchar* result = malloc(40);
    const gchar* statuses[] = {
        "idle", "capturing", "session_captured"
    };
    sprintf(result, "%s", statuses[i%3]);
    return result;
}

/*
*   Если ни одной роли ложных хостов
*   не задано, нужно их создать
*/
// void create_roles(GtkListStore *source)
// {
//     GtkTreeIter iter;
   
//     gboolean next;
//     gint i;
//     const gchar* roles[] = {
//         "workstation", "mail-server", "file-server", "\0"
//     };
//     gchar *role;
//     i = 0;
//     /*
//     *   Сначала проверим, что лежит в полученной модели
//     */
//     next = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(source), &iter);
//     if (!next)
//     {
//         g_print("***Fake hosts' roles model is empty!***\n");
//     }

//     while (next)
//     {
//          gtk_tree_model_get(GTK_TREE_MODEL(source), &iter, 
//                             0, &role,
//                             -1);
//             g_print("%d old role :%s \n", i, role);
//             g_free(role);
//             next = gtk_tree_model_iter_next(GTK_TREE_MODEL(source), &iter);
//             i++;
//     }   

//     /*
//     *   Запишем новые роли в модель
//     */
//     i = 0;

//     while (strcmp(roles[i], "\0"))
//     {
//         // g_print("**Started filling in the roles model**\n");
//         gtk_list_store_insert_with_values(source, &iter, -1, 0, roles[i], -1);
//         // gtk_list_store_append(source, &iter);
//         // gtk_list_store_set(source, &iter,
//         //                     0, roles[i],
//         //                     -1);
//         i++;
//     }

//     /*
//     *   Проверим, заполнилась ли роль
//     */
//     i = 0;
//     next = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(source), &iter);
//     if (!next)
//     {
//         g_print("***Fake hosts' roles model is still empty!***\n");
//     }

//     while (next)
//     {
//          gtk_tree_model_get(GTK_TREE_MODEL(source), &iter, 
//                             0, &role,
//                             -1);
//             g_print("%d new role :%s \n", i, role);
//             g_free(role);
//             next = gtk_tree_model_iter_next(GTK_TREE_MODEL(source), &iter);
//             i++;
//     }
  
//     // gchar* result = malloc(40);
    
//     // sprintf(result, "%s", roles[i%3]);

//     // return result;
// }




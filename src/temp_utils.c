#include <gtk/gtk.h>
#include "temp_utils.h"

#ifndef NETHOLE_H
#include "netHole.h"
#endif //NETHOLE_H
// #include "structures.h"



/*
*   Несколько функций для тестового
*   заполнения таблицы ложных хостов
*/

gpointer init_background()
{
    get_interfaces();
    init_fakeHosts_table();

    return NULL;
}

gpointer init_fakeHosts_table()
{
    LIST_INIT(&fake_hosts);

    generate_fhosts(fake_hosts);
   
    fake_host_t *iter;
    int i = 0;
    LIST_FOREACH(iter, &fake_hosts, host_next) {
        g_idle_add(ui_add_fhost, iter);

//         g_print("Generated %d host.\nID: %d\n IP: %s\nMAC: %s\n \
// Status: %d\nSource: %s\n****\n", i, iter->id,
//                 ip_ntoa(&iter->fake_host_addr), eth_ntoa(&iter->fake_host_mac),
//                 iter->status, ip_ntoa(&iter->source_addr));
        i++;
    }
    return NULL;
}



/* 
*  Генератор случайных ложных хостов 
*  для тестового заполнения таблицы
*/    
void generate_fhosts(struct fake_host_list *list){
    uint8_t rand_am; /* Сколько хостов генерим */
    int j;
    fhosts_amount = 0;
   
    rand = rand_open();
    if (NULL == rand){
        g_print("Couldn't initialise random");
    }
    rand_am = rand_uint8(rand) % MAX_RHOSTS_TO_GENERATE;
    g_print("Generating %d hosts...\n", (int)rand_am);
    

    for(j = 0; j < (int)rand_am; j++){
        fake_host_t *new_host = create_fake_host();

        if (LIST_FIRST(&fake_hosts) == NULL){
            // g_print("List is empty\n");
            LIST_INSERT_HEAD(&fake_hosts, new_host, host_next);
            // last_host = host1;
        } else {
            // g_print("List wasn't empty\n");
           LIST_INSERT_AFTER(last_host, new_host, host_next);
           
        }
        last_host = new_host;
        fhosts_amount++;
    }
}

fake_host_t *create_fake_host()
{
    uint8_t rand_stat;
    // rand_stat = malloc(sizeof(uint8_t));
    rand_stat = rand_uint8(rand)%3;

    fake_host_t *host = (fake_host_t *)malloc(sizeof(fake_host_t));
    host->id = rand_uint16(rand);
    rand_get(rand, &host->fake_host_addr, IP_ADDR_LEN);
    rand_get(rand, &host->fake_host_mac, ETH_ADDR_LEN);
    
    host->status = rand_stat;
    // host->status = IDLE;
   
    rand_get(rand, &host->source_addr, IP_ADDR_LEN);

    // free(rand_stat);
    return host;
}

void get_interfaces()
{
    struct ifaddrs *addrs,*tmp;

    getifaddrs(&addrs);
    tmp = addrs;

    while (tmp)
    {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET){
            printf("%s\n", tmp->ifa_name);

            /*
            *   Возможно, стоит складывать имена интерфейсов в контейнер
            *   чтобы вызвать функцию обновления списка в форме единожды
            */
            g_idle_add_full(G_PRIORITY_HIGH_IDLE, update_ifaces_model,
             (gpointer)tmp->ifa_name, NULL);
        }

        tmp = tmp->ifa_next;
    }
    // g_idle_add(update_ifaces_model, );
    freeifaddrs(addrs);
    g_idle_add(set_active_iface, NULL);
}


/* 
*   Будет вызываться для удаления хоста из 
*   фонового списка и из модели интерфейса
*/
void delete_random_fhost()
{
    uint8_t to_del;
    int size = 0;
    fake_host_t *iter;

    to_del = rand_uint8(rand) % fhosts_amount;
    g_print ("Host number %d is randomly selected\n", to_del);
    int counter = 0;

    LIST_FOREACH(iter, &fake_hosts, host_next){
        if (counter == to_del) {
            g_print("Gonna delete host with ID: %d\nIP: %s\n", 
                    iter->id, ip_ntoa(&iter->fake_host_addr));
            g_idle_add(ui_delete_fhost, iter);
        }
        counter++;
    }
}

void delete_selected_fhost(uint16_t id)
{
    // g_print("Got to the background! Processing...\n");
    fake_host_t *iter;
    LIST_FOREACH(iter, &fake_hosts, host_next){
        if (iter->id == id)
            g_print("Gonna delete host with ID: %d\nIP: %s", 
                    iter->id, ip_ntoa(&iter->fake_host_addr));
    }
}

/* 
*   Заготовка функции для проверки идентичности
*   содержимого списков ложных хостов
*   интерфейсного и фонового потоков
*/
gboolean check_fhost_containers(){

}
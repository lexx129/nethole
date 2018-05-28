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
    init_fakeHosts_table();
    get_interfaces();

    return NULL;
}

gpointer init_fakeHosts_table()
{
    uint8_t rand_am;
    
    rand = rand_open();
    if (NULL == rand){
        g_print("Couldn't initialise random");
    }

    rand_am = rand_uint8(rand) % 50;
    g_print("gonna generate %d hosts\n", (int)rand_am);


    fake_host_t *last_host; /* Указатель на последний элемент в списке */

    LIST_INIT(&fake_hosts);

    int j;

    /* 
    *  Генератор случайных ложных хостов 
    *  для тестового заполнения таблицы
    */
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
    }
   
    fake_host_t *iter;
    int i = 0;
    LIST_FOREACH(iter, &fake_hosts, host_next) {
        g_idle_add(update_fhost_table, iter);

        g_print("Generated %d host.\nID: %d\n IP: %s\nMAC: %s\n \
Status: %d\nSource: %s\n****\n", i, iter->id,
                ip_ntoa(&iter->fake_host_addr), eth_ntoa(&iter->fake_host_mac),
                iter->status, ip_ntoa(&iter->source_addr));
        i++;
    }
    return NULL;
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
    g_print("rand_role %d\n", rand_stat);
    
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
            g_idle_add(update_ifaces_model, (gpointer)tmp->ifa_name);
        }

        tmp = tmp->ifa_next;
    }
    // g_idle_add(update_ifaces_model, );
    freeifaddrs(addrs);
}


/* 
*   Будет вызываться для удаления хоста из 
*   фонового списка и из модели интерфейса
*/
void delete_fhost(uint16_t id)
{
    g_print("Got to the background! Processing...\n");
    fake_host_t *iter;
    LIST_FOREACH(iter, &fake_hosts, host_next){
        if (iter->id == id)
            g_print("Gonna delete host with ID: %d\nIP: %s", 
                    iter->id, ip_ntoa(&iter->fake_host_addr));
    }
}
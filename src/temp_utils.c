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



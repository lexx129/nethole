#ifndef UTILS_H
#define UTILS_H

#include <gtk/gtk.h>
#include <dnet.h>
#include <sys/types.h>
#include <ifaddrs.h>

#ifndef NETHOLE_H
#include "netHole.h"
#endif 

#include "structures.h"

#define MAX_RHOSTS_TO_GENERATE 	50;

rand_t      *rand;
uint8_t		fhosts_amount;
fake_host_t *last_host;     /* Указатель на последний элемент в списке */

gchar* create_addr(int i, gboolean flag);

gchar* create_status(int i);

void create_roles(GtkListStore *source);

gpointer init_background();

gpointer init_fakeHosts_table();

fake_host_t *create_fake_host();

void get_interfaces();

void delete_random_fhost();

void delete_selected_fhost(uint16_t id);

#endif //UTILS_H
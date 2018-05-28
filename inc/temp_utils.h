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



rand_t      *rand;

gchar* create_addr(int i, gboolean flag);

gchar* create_status(int i);

void create_roles(GtkListStore *source);

gpointer init_background();

gpointer init_fakeHosts_table();

fake_host_t *create_fake_host();

void get_interfaces();

#endif //UTILS_H
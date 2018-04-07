#include <gtk/gtk.h>


#define NETHOLE_TYPE (nethole_get_type ())
G_DECLARE_FINAL_TYPE (netHole, example_app, EXAMPLE, APP, GtkApplication)


netHole     *netHole_new         (void);

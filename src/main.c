#include <gtk/gtk.h>

#include "netHole.h"

int
main (int argc, char *argv[])
{
  return g_application_run (G_APPLICATION (netHole_new ()), argc, argv);
}
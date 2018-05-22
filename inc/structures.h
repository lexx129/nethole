#ifndef STR_H
#define STR_H

#include <dnet.h>
#include "queue.h"

#define FP_NAME_LENGTH 16 /* maximal fake-port name length */
#define FP_SET_NAME_LENGTH 16 /* maximal port-set name length */
#define ETH_ADDR_LEN	6	/* MAC address length */
#define IP_ADDR_LEN	4		/* IP address length */

// enum port_constants {
// 	PORT_IGNORE = 0,    /* Ignore this port */
// 	PORT_MONITOR = 1,   /* Monitor this port for activity */
// 	PORT_NUM = 65535,
// 	PORT_MAX = 255,   /* Max count possible for a given port */
// 	PORT_NOISE = 6,   /* 1st n SYNs to a given port are considered noise */
// 	PORT_USR_AMOUNT = 5,    /* Uses in RESTRICT PORT mode */
// 	PORT_SYS_AMOUNT = 5,    /* Uses in RESTRICT PORT mode */
// 	PORT_SYSTEM_MAX = 1024  /* System ports (0 - 1023) */
// };

// enum sizes {
// 	RANDSIZE2 = 12,   /* Keep 12 different random numbers */
// 	BUFSIZE = 1024,   /* General character buffer size */
// 	BPFSIZE = 65536,    /* Holds entire bpf filter */
// 	IP_INP_SIZE = 20,   /* Max length of IP addr when input: xxx.xxx.xxx.xxx/nn */
// 	ONE_K = 1024,     /* For conversions */
// 	HIST_MIN = 4,     /* Keep 5 minutes of bandwidth history */
// 	MAX_NUM_ADAPTER = 10,         /* Windows only: max num of adapters in list */
// 	PGM_NAME_SIZE = 50    /* Ident tag for syslog */
// };

typedef enum 
{
	IDLE,
	IN_PROGRESS,
	CAPTURED

} statuses;

struct fake_port_s {
  	char      name[FP_NAME_LENGTH];
  	uint16_t  port_number;
};
typedef struct fake_port_s fake_port;

/* List of ports for one fake port set */
 
struct fake_port_set_s{
  	char                name[FP_SET_NAME_LENGTH];
  	struct fake_port_s  *incl_ports;
};
typedef struct fake_port_set_s fake_ports_set;


/*! Representation of a fake host */

struct fake_host_s {
	ip_addr_t   fake_host_addr;
 	eth_addr_t  fake_host_mac;
  	statuses    status;
  	ip_addr_t   source_addr;
  	fake_ports_set  role;
  	LIST_ENTRY(fake_host_s) host_next;
};

typedef struct fake_host_s fake_host_t;

// /* Fake hosts list */
// struct fake_host_list {
//   	fake_host_t   host;
//   	LIST_ENTRY(fake_host_list) host_next;
// };

LIST_HEAD(fake_host_list, fake_host_s) fake_hosts;

#endif //STR_H
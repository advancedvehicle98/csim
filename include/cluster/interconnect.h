#ifndef __CLUSTER_INTERCONNECT_H
#define __CLUSTER_INTERCONNECT_H


#include <cluster/node.h>
#include <cluster/timing.h>

#include <stdint.h>


#define MAX_CLUSTER_FILE_LINE_SIZE 100

#define BUS_CONNECTION ( "bus" )
#define P2P_CONNECTION ( "p2p" )


typedef enum __interconnect_type_t {
	IC_TYPE_BUS,
	IC_TYPE_P2P,
	IC_TYPE_UNKNOWN
} interconnect_type_t;


typedef struct __bus_connection_t {
	size_t  connected_nodes_count;
	struct _node_t *connected_nodes;
} bus_connection_t;


typedef struct __p2p_connection_t {
	struct _node_t *node1, *node2;
} p2p_connection_t;


typedef struct _interconnect_t {
	quantum_t delay_max, delay_min;

	// это уже для указателя на структуру, типа
	// p2p_connection_t, bus_connection_t и т.д.
	interconnect_type_t  type;
	void                *specs;
} interconnect_t;

typedef struct _interconnect_list_t {
	interconnect_t ic;
	struct _interconnect_list_t *next;
} interconnect_list_t;


#endif // ! __CLUSTER_INTERCONNECT_H

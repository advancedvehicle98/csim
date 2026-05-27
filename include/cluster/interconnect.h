#ifndef __CLUSTER_INTERCONNECT_H
#define __CLUSTER_INTERCONNECT_H


#include <cluster/node.h>
#include <cluster/timing.h>


#define BUS_CONNECTION ( "bus" )
#define P2P_CONNECTION ( "p2p" )


typedef enum __interconnect_type_t {
	IC_TYPE_BUS,
	IC_TYPE_P2P
} interconnect_type_t;


typedef struct __bus_connection_t {
	size_t  connected_nodes_count;
	node_t *connected_nodes;
} bus_connection_t;


typedef struct __p2p_connection_t {
	node_t *node1, *node2;
} p2p_connection_t;


typedef struct _interconnect_t {
	quantum_t delay_max, delay_min;

	// это уже для указателя на структуру, типа
	// p2p_connection_t, bus_connection_t и т.д.
	interconnect_type_t  type;
	void                *specs;
} interconnect_t;


#endif // ! __CLUSTER_INTERCONNECT_H

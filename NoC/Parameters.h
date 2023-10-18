#pragma once

// Simulation cycles
#define SIMULATION_CYCLES 500
#define WARMUP_CYCLES 7
#define MEASUREMENT_CYCLES 50
#define DRAIN_CYCLES 0

// Cycles
#define CYCLES_ROUTER_RC 1
#define CYCLES_ROUTER_VA 1
#define CYCLES_ROUTER_SA 1
#define CYCLES_ROUTER_ST 1
#define CYCLES_LINK 1
#define MIN_BUFFER_SIZE CYCLES_ROUTER_RC+CYCLES_ROUTER_VA+CYCLES_ROUTER_SA+CYCLES_ROUTER_ST+CYCLES_LINK
// all those cycles above are not tested with values higher than 1

// PE.h
#define DATA_PRECISION float // float(4B) / DATA_PRECISION(8B) / long DATA_PRECISION(8B)
#define MAX_XID 16

// Network.h
#define NETWORK_DIMENSION_X 2
#define NETWORK_DIMENSION_Y 2
#define ROUTER_NUMBER NETWORK_DIMENSION_X * NETWORK_DIMENSION_Y
// choose network type:
#define MESH
//#define TORUS

#define REGISTER_DEPTH 2

// Flit size
#define PACKET_SIZE 16 // bytes
#define FLIT_SIZE 4 // bytes, link width

// Virtual channel
#define VC_NUMBER 2
#define BUFFER_SIZE MIN_BUFFER_SIZE // credit stalls if less than MIN_BUFFER_SIZE

// Injection rate (packet per cycle)
#define INJECTION_RATE 0.1



//#define NI_CYCLES 5
//#define CHANNEL_CYCLES 1
//#define DRAM_CYCLES 10
//#define PE_CYCLES 5
//#define LINKS_CYCLES 1
//
//


//// Memory cycle
//#define MEMORY_READ_CYCLES 
//#define MEMORY_WRITE_CYCLES 
//
//// PE cycle 
//#define CHANNEL
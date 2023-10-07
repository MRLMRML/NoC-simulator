#pragma once

// PE.h
#define DATA_PRECISION float // float(4B) / DATA_PRECISION(8B) / long DATA_PRECISION(8B)
#define MAX_XID 16

// Network.h
#define NETWORK_DIMENSION_X 3
#define NETWORK_DIMENSION_Y 3
#define ROUTER_NUMBER NETWORK_DIMENSION_X * NETWORK_DIMENSION_Y
// choose network type:
#define MESH
//#define TORUS

// Cycles
#define CYCLES_ROUTER_RC 1
#define CYCLES_ROUTER_VA 1
#define CYCLES_ROUTER_SA 1
#define CYCLES_ROUTER_ST 1

//#define NI_CYCLES 5
//#define CHANNEL_CYCLES 1
//#define DRAM_CYCLES 10
//#define PE_CYCLES 5
//#define LINKS_CYCLES 1
//
//
//#define SIMULATION_CYCLES 500

//// Memory cycle
//#define MEMORY_READ_CYCLES 
//#define MEMORY_WRITE_CYCLES 
//
//// PE cycle 
//#define CHANNEL

//// Flit size
#define FLIT_SIZE 16 // bytes

// Virtual channel
#define VC_NUMBER 2
#define BUFFER_SIZE 4
#pragma once
//#define DEBUG

// Network.h
#define NETWORK_DIMENSION_X 4
#define NETWORK_DIMENSION_Y 4
#define ROUTER_NUMBER NETWORK_DIMENSION_X * NETWORK_DIMENSION_Y
// choose network type:
#define MESH
//#define TORUS

#define NETWORK_BANDWIDTH 1 // flit per cycle

#define REGISTER_DEPTH 2

// Packet and Flit size
#define PACKET_SIZE 16 // bytes
#define FLIT_SIZE 4 // bytes, link width
#define FLIT_NUMBER_PER_PACKET PACKET_SIZE / FLIT_SIZE

// Virtual channel
#define VC_NUMBER 2
#define BUFFER_SIZE 8 // credit stalls if less than MIN_BUFFER_SIZE

// Traffic pattern
//#define CUSTOMIZE
//#define PERMUTATION
#define DRAIN_NODE 0
#define UNIFORM

// Packet number to send in total
#define PACKET_NUMBER 5

// Injection rate
//#define PACKET_PER_CYCLE
#define FLIT_PER_CYCLE
#define INJECTION_RATE 0.1f

// PE.h
#define DATA_PRECISION float // float(4B) / DATA_PRECISION(8B) / long DATA_PRECISION(8B)
#define MAX_XID 16

// Router timing, fixed, might support other values even though not tested yet
#define EXECUTION_TIME_ROUTER_RC 1.0f
#define PERIOD_ROUTER_RC 1.0f
#define EXECUTION_TIME_ROUTER_VA 1.0f
#define PERIOD_ROUTER_VA 1.0f
#define EXECUTION_TIME_ROUTER_SA 1.0f
#define PERIOD_ROUTER_SA 1.0f
#define EXECUTION_TIME_ROUTER_ST 1.0f
#define PERIOD_ROUTER_ST 1.0f

// Link timing, fixed, PERIOD = EXECUTION_TIME = 1, need to change link behaviour to support higher value
#define EXECUTION_TIME_LINK 1.0f 
#define PERIOD_LINK EXECUTION_TIME_LINK

// Node timing, PERIOD >= EXECUTION_TIME
#define EXECUTION_TIME_DOPPLERNODE_INJECTTRAFFIC 1.0f
#if defined (PACKET_PER_CYCLE)
#define PERIOD_DOPPLERNODE_INJECTTRAFFIC static_cast<float>(1 / INJECTION_RATE)
#endif
#if defined (FLIT_PER_CYCLE)
#define PERIOD_DOPPLERNODE_INJECTTRAFFIC static_cast<float>(FLIT_NUMBER_PER_PACKET / INJECTION_RATE)
#endif
#define EXECUTION_TIME_DOPPLERNODE_COLLECTTRAFFIC 1.0f
#define PERIOD_DOPPLERNODE_COLLECTTRAFFIC EXECUTION_TIME_DOPPLERNODE_COLLECTTRAFFIC

// Minimum VC buffer size avoiding credit stalls
#define MIN_BUFFER_SIZE EXECUTION_TIME_ROUTER_RC+EXECUTION_TIME_ROUTER_VA+EXECUTION_TIME_ROUTER_SA+EXECUTION_TIME_ROUTER_ST+EXECUTION_TIME_LINK

// Simulation cycles
#define SIMULATION_CYCLES 300
#define WARMUP_CYCLES 100
#define MEASUREMENT_CYCLES 100
#define DRAIN_CYCLES SIMULATION_CYCLES - WARMUP_CYCLES - MEASUREMENT_CYCLES

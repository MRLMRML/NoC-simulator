# NoC simulator
Version 1.0

2024/01/10

MRLMRML

This is a cycle accurate simulator for conventional Network-on-Chip (NoC).

## Features

The simulator supports mesh and torus topologies.

It only supports XY-routing algorithm in current version.

Independent and adjustable router numbers in X and Y dimension.

Adjustable flit number per packet.

Adjustable virtual channel number at each input port and also adjustable size of each virtual channel.

Two traffic patterns, uniform random traffic and permutation (all-to-one) traffic, are supported, with periodic injection process.

Injection rate has two mode, packet per cycle and flit per cycle.

The router used in the network is a canonical VC router with best-effort service, i.e., round-robin arbitration policy is adopted.

It has traditional four pipeline stages, Route Computation (RC), Virtual channel Allocation (VA), Switch Allocation (SA), and Switch Traversal (ST).

Each of the stage, as well as the Link Traversal (LT), takes one clock cycle.

In measurement, three phases, warm-up phase, measurement phase, and drain phase are implemented while the cycles for each are adjustable.

For the algorithm details in the router and other critical design choices, please check the figures under directory .\\NoC\\Design

## To use this simulator

Clone this project and open it in Visual Studio 2022.

**Change the C++ version to C++20.**

Open Parameters.h and adjust parameters as you wish.

Open main.cpp and change the global variable g_dataFolderPath to the path where the folder Data is.

Then compile and run the program.

The average latency and throughput will be printed out in the terminal.

To see the detailed informantion of each packets, open the Data folder.

PacketRecord.csv records all the packets generated and their detailed information.

TrafficData.csv records all the packets measured, i.e., sent in measurement phase.
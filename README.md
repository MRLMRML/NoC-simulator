# NoC
A NoC simulator version 1.0

This is a simulator for conventional Network-on-Chip (NoC).

The simulator supports mesh and torus topologies.

It only supports XY-routing algorithm.

Independent and adjustable router numbers in X and Y dimension.

Adjustable flit number per packet.

Adjustable virtual channel number at each input port and the size of each virtual channel.

Two traffic patterns, uniform random traffic and permutation (all-to-one) traffic, are supported, with periodic injection process.

Injection rate has two mode, packet per cycle and flit per cycle.

The router used in the network is a canonical VC router with best-effort service.

It has traditional four pipeline stages, Route Computation (RC), Virtual channel Allocation (VA), Switch Allocation (SA), and Switch Traversal (ST).

Each of the stage, as well as the Link Traversal (LT), takes one clock cycle.

In measurement, three phases, warm-up phase, measurement phase, and drain phase are implemented while the cycles for each are adjustable.

The details of router algorithms implemented is shown in the following picture:

![Router algorithm in one pic](.\\NoC\\Figures\\Router algorithms in one pic.png)

The simulator used Clock class to support parallelism of the system, e.g., multiple VCs in one router.

![Clock system supporting parallelism](.\\NoC\\Figures\\Clock system supporting parallelism.png)

The simulator used enable and buffer to support system level pipelining:

![Enable and buffer feature supporting system level pipelining](.\\NoC\\Figures\\Enable and buffer feature supporting system level pipelining.png)

The measurement setup:

![Measurement](.\\NoC\\Figures\\Measurement.png)

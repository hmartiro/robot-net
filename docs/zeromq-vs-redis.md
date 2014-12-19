## ZeroMQ vs Redis
This document is research for the selection of a communication platform for robot-net.

### Goal
The purpose of this component is to enable rapid, reliable, and elegant communication
between the various nodes of the network, including controllers, sensors, and actuators
(robot drivers). It will act as the core of robot-net to create a standardized infrastructure
for robot control.

**Requirements:**

 * Very low latency (< 1ms, ideally < 0.1ms)
 * Easy to use, minimal client code
 * Handles rapid streams of data and commands (> 10kHz)
 * Handles high-priority events like state changes without delays
 * Handles 10 node system on commodity hardware
 * Logging, monitoring capabilities
 * Robust to failure of nodes
 * Simple to update communication protocol
 * Free and open-source

### Introduction
Given these requirements and available technologies, the final two choices for this
component come down to [ZeroMQ](http://zeromq.org/) and [Redis](http://redis.io/).
They are both best-in-class, but very different tools.

**ZeroMQ**

ZeroMQ is a high-performance asynchronous messaging library for distributed or concurrent
applications. It acts like a message queue, but without any requirements for an intermediate
broker. It uses a minimalistic socket-like API, and can use TCP, PGM, or IPC (Unix-style socket)
transports. It has several messaging patterns like request-reply, publish-subscribe, push-pull,
and exclusive pair, that provide differing protocols and behaviors. Performance tests
over 10Gb Ethernet are [here](http://zeromq.org/results:10gbe-tests-v031), showing a throughput
of 2.8 million msg/s at 10 bytes messages and 1.4 million msg/s at 100 bytes messages. Latency
stays around a constant 33us for messages under 4000 bytes. Maximum bandwidth possible is ~2.5Gb/s.
[This](http://zeromq.org/results:copying) document shows the effects of copying data on latency
at the application level, generally increasing latency by a factor of about 30%.
[Bindings](http://zeromq.org/bindings:_start) exist for every major language. A possible alternative
to ZeroMQ is the fork [nanomsg](http://nanomsg.org/documentation-zeromq.html).

**Redis**

Redis is an advanced key-value store, or data structure server. It works in-memory, with optional
persistency. The primary data type is a string, but Redis also supports hashes, lists, sets, sorted
sets, bitmaps, and hyperloglogs. It is one of the most popular key-value stores, and performance is
at the very top when on-disk durability is not required. Redis runs as a centralized server, and clients
communicate using the [Redis Serialization Protocol](http://redis.io/topics/protocol), which is a
request-response model using TCP. Interestingly, it also acts separately as a
[publish/subscribe](http://redis.io/topics/pubsub) server. Performance
[benchmarks](http://redis.io/topics/benchmarks) show typical throughputs of 30-100k requests/s,
and around 200-400k for pipelining (batch requests, not relevant for high-frequency sampling).
However, on my machine, I see more like 100-200k requests/s, and 700-1000k requests/s with pipelining. These
results are about constant for data under 1000 bytes over 1Gb Ethernet. Average latency (over localhost) seems
to be around 150us. Redis [clients](http://redis.io/clients) exist for every major language.

## Comparison

**Ties**

 * Both are widely used and proven in high-performance production applications.
 * Both have permissive licenses.
 * Both have bindings for many languages.
 * Both can handle a distributed system with many nodes.
 * Both can be used synchronously or asynchronously (blocking and non-blocking calls).
 * Both can be abstracted away from the end user.
 * Both can have a logger node that monitors all messages being sent.
 * Both can have one pathway for rapid data streams, and one for high-priority events.

**ZeroMQ Advantages**

 * ZeroMQ creates direct links between distributed nodes, whereas Redis is a central node which
   must be written to then read from. In addition, Redis always uses request-reply for calls.
   Since network calls act as the bottleneck for both, it is literally impossible for Redis to
   match the raw throughput or latency of ZeroMQ. It will take four network calls to send
   a piece of data to the store and to read it back, vs one over a direct ZeroMQ pubsub.
 * ZeroMQ is fully distributed, so there is no server required, cutting down on a process.
 * ZeroMQ is more robust, because no single node can bring down the whole system.

**Redis Advantages**

 * Redis keeps all data in a centralized place, where it is trivial to monitor and persist to disk if needed.
 * Redis knows about data types, whereas ZeroMQ deals only with serialized binary data.
 * Redis requires less effort to implement properly.
 * Redis can use the key-value store for data, and the pubsub for events. Using the key-value store decouples
   the sender of data from the reader, which is a good thing.

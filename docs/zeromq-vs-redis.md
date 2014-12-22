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

## Qualitative Comparison

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
 * ZeroMQ can use multithreading to utilize all CPU cores for reading from different nodes.

**Redis Advantages**

 * Redis keeps all data in a centralized place, where it is trivial to monitor and persist to disk if needed.
 * Redis knows about data types, whereas ZeroMQ deals only with serialized binary data.
 * Redis requires less effort to implement properly.
 * Redis can use the key-value store for data, and the pubsub for events. Using the key-value store decouples
   the sender of data from the reader, which is a good thing.
 * Redis is single-threaded, so no locks necessary (but potentially slows down proportionally to the number of nodes).

## Quantitative Comparison

Basic benchmark programs were written in C++ for both ZeroMQ and Redis. Both send messages of the form
"Hello at 1419140353074", where the number is the current epoch time in milliseconds. This lets us test
throughput and latency together. The string message has a size of approximately 22 bytes. For both tests,
there is a sending and receiving process, both communicating over the loopback TCP interface.

All complete benchmarks are available [here](https://github.com/hmartiro/robot-net/tree/master/tests).

Results for one writer, one reader:

 * ZeroMQ, Pub/Sub: 481,000 msg/s, latency <1 ms
 * ZeroMQ, Pair: 584,000 msg/s, latency <1 ms
 * Redis, Get/Set (synchronous): 23,600 sets/s, 24,000 gets/s, latency <1 ms
 * Redis, Get/Set (asynchronous via libevent custom timer): 78,400 sets/s, 78,100 gets/s, latency <1 ms
 * Redis Pub/Sub (async via libevent): 59,000 msg/s, latency <1 ms

Results for one writer, four readers (using `time parallel -j 4 ./build/COMMAND_NAME -- 1 2 3 4`):

 * ZeroMQ, Pub/Sub: 410,000 msg/s, latency 0-15 ms, avg ~3-4 ms (goes down to <1 ms if we limit the publisher to
   what the subscribers can consume)
 * Redis Get/Set (async): 72,800 sets/s, 40,700 gets/s, latency 0-21 ms, avg ~1-2 ms
 * Redis Pub/Sub (async): 67,800 msg/s, latency 0-2 ms, avg <1 ms (repeat tests confirm this is faster than with
   one writer, reason unknown)

## Conclusion

The decision comes down to the difference between a fully distributed solution with raw speed (ZeroMQ) versus a
centralized solution with more accessibility (Redis). Using Redis with an event library and asynchronous looping
greatly speeds it up, but ZeroMQ still has about 6x higher messaging rate. Both ZeroMQ and Redis can be sped up by
pipelining (batching) requests - it is unclear if this is helpful, if each node is dealing with data regarding a
separate robot. Redis definitely makes a lot of things easier - logging, persistence, pipelining, event loops.
However, it is a single-threaded centralized server and will slow down when _many_ nodes are reading, whereas a
distributed system will be affected less by congestion. All tests were run on a local machine, but we can imagine
that most applications of this library will run on a local network, so network latencies should never be too bad.
Both solutions should fulfil the requirements. The big question is how much throughput we actually need to handle.
If we assume four robots at 10 kHz each, we see that Redis should be able to handle this just fine. Therefore,
because it will be faster to implement a complete solution, the current conclusion is to proceed with Redis until
we run into an important use case for which it is prohibitively slow.

# shriek
A distributed key-value store. This was implemented for the [Bradfield School of Computer Science Distributed Systems](https://bradfieldcs.com/courses/distributed-systems/) class.

## Open tasks
See the `TODO.md` file.

## Use case
At the beginning of class, we were asked to pick a specific use case. These
are ones that I am considering, until architectural decisions force me to pick one:
- A service / configuration discovery management system, like Consul or etcd
- An distributed cache, like Redis but with implicit sharding and multi-threading ability.
- A distributed NoSQL database like MongoDB or Cassandra
- A distributed database that is designed to operate in highly partitioned networks; in the
considered use-case, clients would be responders / public servants responding to a natural
disaster and need to have a consistent record in spite of an unreliable network. New nodes
need to come on line with batches of queued writes while simultaneously receiving writes.

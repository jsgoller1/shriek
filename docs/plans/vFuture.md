Shriek, future versions
---

This document holds some ideas for future iterations of Shriek.

## Merkle trees for anti-entropy
Each Shriek node could use a modified storage system to engage in anti-entropy.
This system could consist of three core storage data structures:
 - a Merkle tree, constructed from hashes of key/value pairs
 - a hashtable that stores the actual key-value pairs
 - an array of pointers to hashtable entries, from which the

## Advanced object versioning
The Dynamo paper mentions that different versions of objects and be stored across nodes
and will be resolved on read; it could be possible to implement Lamport clocks for each
object and do resolution based on their version. Perhaps something like:
- when an object is created, its version is initialized to zero.
- when an object is updated, the existing timestamps are queried and the highest one
is chosen as the new version, and then incremented; the new version is then transmitted
to each node (which does not itself increment the timestamp)
- when an object is read from a node, its version is incremented by 1. This new version
number is then propagated to each other node (which does not itself incrememnt the timestamp)

## Other ideas
- Better backups / writes to disk in case a node crashes?
- Support for multiple threads
- Slab allocation / garbage collection

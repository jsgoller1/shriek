Shriek, v2+ roadmap
---
# Statement (of the problem)
Per direction from the Bradfield assignments, the next things we need to support
for our K/V projects are:
- Multiple nodes, either in a leader or leaderless topology
- Data replication; losing a single node should not cause permanent data loss
- Data partitioning; each node should not need to be a full replica
- Disk serialization; we should be able to write data in memory to an on-disk format and vice versa
- A protocol that supports all of the above, plus getting / setting gets

# Understand / Plan
We will presume that the network Shriek operates in is _generally_ reliable (nodes may
occasionally become unreachable, but not to the extent that each node is unable to
reach every other node in its preference list). To facilitate the above needs,
Shriek v2 will implement some of [Dynamo's](https://www.allthingsdistributed.com/files/amazon-dynamo-sosp2007.pdf) features:
- O(1) routing
- Object versioning (via Unix timestamps) and read-repair
- Consistent hashing for partitioning
- Sloppy quorum for reads and writes, with hinted handoff

A backup strategy that is not part of Dynamo will be implemented. Dynamo's anti-entropy
mechanism via Merkle trees and background gossip is not planned for implementation.
See each of individual version files for the specific implementation plans of each version.

# Execute
Versioned releases will be created for each individual version above.

# Review
Forthcoming

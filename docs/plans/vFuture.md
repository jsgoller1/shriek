Shriek, future versions
---

This document holds some ideas for future iterations of Shriek.

## Merkle trees for anti-entropy
Each Shriek node could use a modified storage system to engage in anti-entropy.
This system could consist of three core storage data structures:
 - a Merkle tree, constructed from hashes of key/value pairs
 - a hashtable that stores the actual key-value pairs
 - an array of pointers to hashtable entries, from which the

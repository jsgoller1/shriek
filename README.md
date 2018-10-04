# shriek
A distributed key-value store. This was implemented for the [Bradfield School of Computer Science Distributed Systems](https://bradfieldcs.com/courses/distributed-systems/) class.

## Demo
```
make shell    # Pull the test environment docker container
cd shriek     # Enter the workspace
make server   # Makes client, creates a test message, creates server, reads message, and executes behavior.
```

## Docs
The `docs/` subdirectory has all planned features and versions documented, as well as documentation
for each version of the protocol.

## Use case
At the beginning of class, we were asked to pick a specific use case or existing K/V store
to model our projects on - I chose Amazon Dynamo.

## Protocol design
See `PROTOCOL.md`.

## Credits / special thanks
- [Brian "Beej Jorgensen" Hall](https://beej.us/guide/bgnet/html/multi/index.html) for his immortal guide to
network programming; I copied some of his code snippets for my initial networking-related functions.
- [rxi](https://github.com/rxi/log.c)'s logging library
- [Werner Vogels et. al.](https://www.allthingsdistributed.com/files/amazon-dynamo-sosp2007.pdf) for the Dynamo whitepaper
- [Oz](https://github.com/ozan) and [Elliott](https://github.com/robot-dreams) for their constructive feedback and code reviews during the Bradfield course

// TODO
-----
1. Towards a Distributed Key-Value Store
  - Goal: Build a local key value store
  - KRs:
    - Setting a value for a key
    - Persisting that data to disk
    - Retrieving the data again by key.
    - Stretch: should aim to support multiple local “clients” operating on the same data store.
  - Suggested path:
    - REPL
      - Read/write from STDIN
      - Exits on SIGINT
    - Set keys and values in memory
      - `set foo=bar` and `get foo` -> `bar`
    - Persist data to disk
      - Come up with serialization format (doesn't need to be great, just functional)
    - Support multiple clients
      - Set in one tab, get in another
    - Test and harden

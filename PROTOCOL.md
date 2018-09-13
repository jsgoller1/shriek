Shriek protocol
---
# Encoding format
Shriek adopts an encoding format starting with v1. In this encoding format, there are
four types:
* Integers
* Strings
* Arrays of integers
* Arrays of strings

Every Shriek message has the following structure:
```
VERSION (2 bytes)
followed by any number of
TYPE (1 byte) | LENGTH (3 bytes) | DATA (0 to 2^24 bytes)
```


# Protocol versions

## v0
The v0 protocol is a stateless protocol. The only requirement
is that every message receives a reply, unless the connection dies.

Every message is of the following format, in binary:
```
VERSION (2 bytes)
ACTION (4 bytes)
KEY_SIZE (4 bytes)
VAL_SIZE (4 bytes)
KEY (optional, KEY_SIZE bytes)
VAL (optional, VAL_SIZE bytes)
```

#### Message fields
* `VERSION` - the version of the protocol in use. Both bytes are set to 0.
* `ACTION` - the intended behavior by the sender. v1 consists of 3 actions:
  * `GET` - the sender is requesting a key.
  * `SET`- the sender is assigning a new value to a key.
  * `REPLY` - the message is a reply to a previously sent one
* `KEY_SIZE` - the size, in bytes, of the key field
* `VAL_SIZE` - the size, in bytes, of the val field
* `KEY` - ASCII data representing the key
* `VAL` - ASCII data representing the value

#### Protocol behavior
* If `GET` is sent, the server should send back a `REPLY` with the same `KEY_SIZE` and `KEY` fields.
If the data in `KEY` represents a key in the server's hashtable, the associatied value should be sent in
`VAL` and the size of `VAL` should be sent in `VAL_SIZE`. Otherwise, `VAL_SIZE` should be 0 and no data should
be placed in `VAL`.
* If a `SET` is sent, the server should send back a `REPLY` with `KEY_SIZE` 1 and `VAL_SIZE` 0. `KEY` should be
set to `0xFF` if an error occurred, and `0x00` if the key was successfully installed for the value on the server.
* `REPLY` may never be sent first; it can only be sent back to a node that has
already sent a `GET` or a `SET` first.

## v1

The v1 protocol is a stateless protocol. Every message receives a reply, unless the connection dies.
In the v2



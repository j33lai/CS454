# CS654 Assignment 3 Documentation

## Group members: Andrew Palmer, Junyu Lai



### Marshalling/unmarshalling of data

We implement a **Message** class, which handles the marshalling, and unmarshalling of messages.

#### Serialization and Deserialization

Serialization is implemented using fixed-length fields in a byte array for the whole message. Each index position has a specific meaning and the array has a specific structure that is shared across the system. The array is ordered as follows:

ArgTypes (4 bytes) — Reason Code (4 bytes) — Port (4 bytes) — Host (128 bytes) — Name (64 bytes)

The message is deserialized according to this order as well.

The argument type sizes are specified as follows:

| Argument type | Size (bytes) |
| ------------- | ------------ |
| Character     | 1            |
| Short         | 2            |
| Integer       | 4            |
| Long          | 8            |
| Double        | 8            |
| Float         | 4            |

#### Sending and receiving (talk about buffer)

<u>TODO</u>

### Structure of binder database

Procedures in our system are represented by a **FuncStorage** object, which has the necessary properties for a function. The relevant properties for the binder are: *function name*, *function argument types*, the *list of servers* (represented by a string for the host, and an integer for the port) that have registered this function. The binder contains a Map data structure, indexed by function name, where the values are vectors of FuncStorage objects.

### Function overloading

The Map data structured is indexed by function name, and if there is no corresponding key existing for a currently registering function, this means that a new entry will have to be made in the database. Accordingly, a new entry with the function name, and server info is created.

If the function already exists in the map, then the corresponding list of servers is retrieved from the database, and scanned. If the server also exists, then no changes are made to the database. If the server does not exist, then it is added to the list of servers for this function.

#### Comparing functions

To check registering functions for equality, we implement the function `vector<int> reorderArgTypes(vector<int> & types)` . This function is used to reorder the *output* argument in the argTypes array so that two functions can be compared accordingly. This is done to handle cases where a user specifies the output argument after the first index in the argTypes array.

Two functions are considered equal iff:

- They have the same names,
- They have the same arity,
- They have the same output type,
- They have the same argument types, and
- They have the same argument order


### Round-robin scheduling

` Binder::getServerRR(string func_name, int func_id)` is responsible for fetching the next server that can handle a given function. The binder maintains a token that indicates the server that last executed a procedure. When handling a client request, the token is incremented by 1, and then the binder loops sequentially over the list of servers for one that can process the given function. If a server is deleted, then the token just advances to the next server in the sequence.



### Termination procedure

#### Client

When a client calls `rpcTerminate()`, it sends a terminate call to the binder. In the event that a single client calls `rpcTerminate()`, if there are any RPC procedures currently being executed, they exit immediately with the appropriate exit code.

#### Binder

The binder has a single boolean variable, **terminate**, that tracks whether or not it has received a terminate request from a client.

The binder also maintains a list of servers that it has an active connection to.

On the receipt of a termination call, the binder sets the terminate variable to true and begins sending out termination requests to servers. After a termination request is sent out to a server, the binder removes it from the list. Once the list of servers is empty, the binder itself closes its socket and breaks its infinite loop.

#### Server

Upon receiving a terminate call from the binder, a server closes all of its sockets and terminates.

### Optimizations

<u>TODO</u>

### Error codes

| System | Description                              | Error code |
| ------ | ---------------------------------------- | ---------- |
|        |                                          |            |
| Server | Server has no connection to binder, i.e. `rpcExecute()` without calling `rpcInit()` |            |
| Server | Unable to register procedure             |            |
|        |                                          |            |
| Binder | Procedure has not been registered        |            |
| Binder | Unable to register procedure             |            |
| Binder | Procedure found, but no matching server  |            |

### Missing functionality

N/A

### Caching mechanism

<u>TODO</u>

### Extra functionality

<u>TODO</u>

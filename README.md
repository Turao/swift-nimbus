# swift-nimbus

!["Thats not just a broom Harry! That a Nimbus 2000!"](http://farm4.staticflickr.com/3767/10996775514_e2ecbaeec8_z.jpg)


Dropbox-like project @ Operating Systems II / UFRGS


## NimbusClient
Client Application. Isolates Aplication-level tasks such as argv parsing from the object-oriented classes below.

#### Client
Handles the Client Application API calls (e.g. connectServer and syncClient)
  - on constructor:
    1. Creates a session with the server.

#### User
Holds properties related to users


---

## NimbusServer
Server Application. Isolates Aplication-level tasks such as argv parsing from the object-oriented classes below.

#### Server
Handles requests from clients.
  - on constructor:
    1. Creates a master socket (used by the sessions manager to listen for incoming user connections). 
    2. Creates a SessionsManager to handle incoming user connections.

#### SessionsManager
Handles incoming user connections and checks for inactive sessions caused by disconnects.
Has 2 threads:
  - Connections Handler:
    1. Calls accept() from time to time to check if there is a connection pending.
    2. If there is a pending connection, instantiates a new session (with the socket created by/returned from accept()) and pushes it into its sessions vector
  - Scrubber:
    1. Iterates through the sessions vector looking for inactive sessions (isAlive() == false)
    2. If there is one, delete (session destroys its socket - and socket closes the connection, don't worry) and remove from the sessions vector
 
 
 ---
  
## Both

#### Session
Abstracts the connection between User-Server.
Has 2 threads:
  - tail:
    1. Reads the socket from time to time, showing it's content.
    2. If read() returns 0, the connection was closed. Thus, we mark the session to be destroyed.
  - writer:
    1. Reads input from user (stdin) from time to time and sends to the other endpoint.

#### Socket
Abstracts the UNIX sockets API.

#### NimbusFile
Abstracts File handling operations

#### Utilities
Mandatory class, we may use it (or we may not. who knows...)

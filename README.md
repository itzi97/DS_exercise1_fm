# Distributed Systems File Manager

A C++ client-server file manager implementing remote file operations via message passing in a distributed systems exercise.

**Academic Project Details:**
- **Degree**: Software Engineering
- **University**: U-tad
- **Subject**: Distributed Systems Programming

Supports file management (list, create, delete, etc.) on a remote server folder through TCP sockets and structured messages.

## Key Features
- Client-server architecture for remote file ops (ls, mkdir, rm, etc.).
- Message passing protocol with enums for commands.
- Two branches: Basic (direct client-server) and Broker-enabled (intermediary connection management).
- Multi-client server support.
- Error handling and response acknowledgments.

## Tech Stack
- **Language**: C++17
- **Networking**: POSIX sockets
- **Build**: Cmake & Makefile
- **Libraries**: Standard C++ (threads, filesystem, iostream)

## Setup & Run

Dependencies managed through `flake.nix` using the [Nix package
manager](https://nixos.org/download/):

```sh
nix flake update
nix develop # enters local shell with dependencies
```

To setup and run the project having dependencies installed:

```sh
# 1. Clone repo
git clone https://github.com/itzi97/DS_exercise1_fm.git
cd DS_exercise1_fm

# 2. Build with cmake
cmake .
make

# 3. Run broker, server and client
```

## Branch part-1

Includes the basic implementation of client/server architecture of the program,
distributing the `filemanager.h` to work in two different programs those being
`client` and `server`.

## Branch part-2

Includes the ORB implementation of `filemanager.h` that manages the
client/server connections. Creates another executable (`broker`) that is
launched in the ORB machine which connects `client` programs to respective
`server` programs.

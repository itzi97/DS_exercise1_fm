# Distributed Systems Programming Exercise 1 - File Manager

Assignment for Distributed Systems Programming where the `filemanager.h` class
is implemented to work in a distributed system.

## Dependencies & Compilation

Dependencies managed through `flake.nix` using the (Nix package
manager)[https://nixos.org/download/]:

```sh
nix flake update
nix develop # enters local shell with dependencies
```

To compile:

```sh
cmake
make
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

# dmn
Run a command as a daemon.

## Description
**dmn** runs a given command as a daemon for either infinite or definite iterations and optionally logs output to a given file. The pid of the created process is printed upon creation so that it can be managed as needed by the user.

## Usage
  **-c**    : Command to execute. Required.
  **(-i)**  : Number of iterations to repeat the command before ending daemon process. Defaults to infinite.
  **(-f)**  : Write output to the given file.

## Build
 - Build : `make dmn`
 - Clean : `make clean`

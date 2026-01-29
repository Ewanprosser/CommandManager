# Command Manager Message Parser
C++17 implementation of a text-based message parser for Command Manager messages used in wind tunnel control systems.

## Features
- Parsing of text based commands, 10 character OpCodes followed by optional message contents 
- Parameter extraction and validation
- History tracking of last 5 commands
- Ignores unknown OpCodes 

## Build
Built using C++17 in Visual Studio 2019.

## Run
Build and run the solution. Example messages execute automatically, followed by optional manual input.
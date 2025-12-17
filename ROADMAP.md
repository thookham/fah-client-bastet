# Roadmap for fah-client-bastet

## Project: Folding@home Client (Bastet)

This is the codebase for the v7 Folding@home client, responsible for downloading "Work Units" (WU) and running "Cores" (scientific simulation engines) to process them.

## Status

- **Analysis**: Initial deep dive complete.
- **Sanitization**: Checked and clean.

## Golden Nuggets

These files control the lifecycle of the protein folding simulation:

### 1. Work Unit Management (`src/fah/client/Unit.cpp`)

* **Why**: Defines the `Unit` class, which wraps a single chunk of work.
- **Key Logic**:
  - State machine for a Unit (Downloading -> Running -> Uploading).
  - Handling of "Passkeys" for user credit.
  - Deadline enforcement (expiration).

### 2. Core Execution (`src/fah/client/Core.cpp`)

* **Why**: The client doesn't fold proteins itself; it launches a "Core" (like gromacs).
- **Key Logic**:
  - `start()`: Launches the focused scientific core process.
  - `heartbeat()`: Monitors the core to ensure it hasn't hung or crashed.
  - Communication via local sockets or files.

### 3. Client Orchestrator (`src/client.cpp`)

* **Why**: The entry point.
- **Key Logic**:
  - Initializes the "Slot" manager (CPU/GPU slots).
  - Connects to the Assignment Server (AS) to get work.

## Scaffold

- See `SCAFFOLD/NOTES.md` for personal scratchpad.

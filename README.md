# Multi-Instance Accessor in C

A simple and efficient C program demonstrating how to manage multiple object instances using ID-based access and POSIX signal handling.

This is **not** a set system — it allows multiple instances with the same ID.  
The first matching instance is returned when queried.

---

## Features
- Multi-instance registration with dynamic memory.
- ID-based lookup system (no uniqueness enforcement).
- Signal-based runtime testing:
  - `SIGUSR1`, `SIGUSR2`, `SIGINT` (CTRL+C), `SIGHUP`
- Clean switch-case signal handler (no nested if-else).
- Exit behavior on `SIGHUP` after displaying associated instance.

---

## Usage

### Compile

```bash
gcc multi.c -o multi

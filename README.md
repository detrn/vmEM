# MiniVM16

## Overview

This project started as a way to experiment and practice with the contents of the book *Computer Systems: A Programmer's Perspective*.

The development has been heavily inspired by the **Y86 architecture** presented in the book.

The program is what the name suggests: a mini virtual machine with a **16-bit architecture**.

The VM features:
- **15 general-purpose registers**
- **13 instruction families** for programming in Y86 assembly

For me, this represents the biggest project that I have written, and improvements or the signaling of potential errors are welcomed.

## Building

MiniVM16 is written in C and has no external dependencies beyond the C standard library. To build the project, all that is needed is a C compiler and `make`.

```bash
make
```

After building the project, run it with:

```bash
./minivm16 [path_to_your_file]
```

## Usage

The following is an example program written in Y86 assembly:

```asm
IRMOVW $0, %RAX
IRMOVW $1, %RBX
IRMOVW $10, %RCX
IRMOVW $1, %RDX

LOOP:
    RRMOVW %RBX, %RSI
    ADDW %RAX, %RSI
    RRMOVW %RBX, %RAX
    RRMOVW %RSI, %RBX

    SUBW %RDX, %RCX
    JG LOOP

HALT
```

This is an example program that computes the **tenth number of the Fibonacci sequence**. It is written in Y86 assembly and showcases some of its features.

The output of the program is going to be shown by the function `debug_dump_all()`, which is already called in `main.c`.

The result is going to be in register `%RAX`, just as shown below:

```text
-- Debug Print Screen --
The VM worked for 64 steps and it has stopped at PC address 0x001d. Status: VM_HALT. Flags: ZF=1, SF=0, OF=0
Registrii schimbati:
%rax: 0x0000 -> 0x0037
%rdx: 0x0000 -> 0x0001
%rbx: 0x0000 -> 0x0059
%rsi: 0x0000 -> 0x0059
```

The number of steps reports how many full cycles the CPU has completed (the CPU is not pipelined; it executes the instructions sequentially).

The function also outputs the changes that have been made inside the memory. The function doesn't differentiate between the changes made by loading the instructions and the ones made during execution by the VM.

## Documentation

More detailed information about the architecture and instruction set is available in the following documents:

- [Instruction Set Architecture](docs/isa.md) — Registers, instruction formats, opcodes, and instruction semantics.
- [VM Architecture](docs/architecture.md) — Internal organization and execution model.

## Limitations

I have tried to test the program and it has passed my tests. However, I am sure that there are still bugs to be found.

Please signal any errors that you may find at **mc838@student.ugal.ro**.

The assembler is **case-sensitive** and will not recognize lowercase instructions. This is a design choice just because that's the way I like it.

The assembler also doesn't accept numbers in the **hex representation**. The user has to convert them into decimal; otherwise, it produces an encoding error.

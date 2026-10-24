# MiniVM16 Instruction Set Architecture (ISA)

## 1. Overview

MiniVM16 is a 16-bit virtual machine heavily inspired by the Y86 architecture presented in *Computer Systems: A Programmer's Perspective*. The CPU features 15 general-purpose registers that can hold 16-bit values. Memory is byte-addressable and has a size of 64 KiB. Words are stored in little-endian format, and instructions have variable lengths.

## 2. Registers

| Register | ID |
|---|---|
| `%RAX` | `0x0` |
| `%RCX` | `0x1` |
| `%RDX` | `0x2` |
| `%RBX` | `0x3` |
| `%RSP` | `0x4` |
| `%RBP` | `0x5` |
| `%RSI` | `0x6` |
| `%RDI` | `0x7` |
| `%R8` | `0x8` |
| `%R9` | `0x9` |
| `%R10` | `0xA` |
| `%R11` | `0xB` |
| `%R12` | `0xC` |
| `%R13` | `0xD` |
| `%R14` | `0xE` |
| `NO_REG` | `0xF` |

`RSP` represents the stack pointer register and is used by the `POPW`, `PUSHW`, `RET`, and `CALL` instructions.

`NO_REG` is used to indicate that a register must not be used in certain instructions.

## 3. Memory Model

Memory starts at address `0x0000` and ends at `0xFFFF`. All memory is shared between the stack and the rest of the program, so stack corruption is possible.

Words are 16 bits wide and stored in little-endian format. For example, the word `0x1234` is stored as follows:

| Address | Value |
|---|---|
| `0x0000` | `0x34` |
| `0x0001` | `0x12` |

For a write to be valid, the entire value must fit within the available memory space. For example, a word write at address `0xFFFF` fails and produces an error.

## 4. Condition Codes

The CPU features three flags that can be used in conditional jumps or moves:

| Flag | Name | Description |
|---|---|---|
| `ZF` | Zero flag | Set when the result of the last instruction that updated the flags is zero. |
| `SF` | Sign flag | Set when the sign bit of that result is set, indicating a negative value in signed representation. |
| `OF` | Overflow flag | Set when that instruction produces signed arithmetic overflow. |

The `OF` flag is only relevant for signed arithmetic operations.

## 5. Instruction Set

The ISA features 13 instruction types. Some instructions have multiple function encodings (`ifun`) that change their behavior.

| Instruction | Opcode (`icode`) | Operands | Description |
|---|---|---|---|
| `HALT` | `0x0` | None | Stops CPU execution. |
| `NOP` | `0x1` | None | Performs no operation. |
| `RRMOVW` | `0x2` | `rA:rB` | Copies the contents of `rA` to `rB`. |
| `IRMOVW` | `0x3` | `V:rB` | Copies the immediate value `V` into `rB`. |
| `RMMOVW` | `0x4` | `rA:D[rB]` | Copies the contents of `rA` into memory at the address `rB + D`. |
| `MRMOVW` | `0x5` | `D[rA]:rB` | Copies the contents of memory at address `rA + D` into `rB`. |
| `OPW` | `0x6` | `rA, rB` | Performs an arithmetic or logical operation depending on `ifun`. |
| `JXX` | `0x7` | `label/value` | Sets the program counter to `V` or the address of the label, depending on the jump condition. |
| `CALL` | `0x8` | `V` | Sets the program counter to `V`, decrements `RSP` by two bytes, and stores the return address on the stack. |
| `RET` | `0x9` | None | Loads the return address from memory at `[RSP]` into `PC`, then increments `RSP` by two bytes. |
| `PUSHW` | `0xA` | `rA` | Decrements `RSP` by two bytes, then stores the value of `rA` at the top of the stack. |
| `POPW` | `0xB` | `rA` | Stores the value at the top of the stack in `rB`, then increments `RSP` by two bytes. |
| `CMOVXX` | `0xC` | `rA:rB` | Conditionally copies the contents of `rA` to `rB`. |

**Operand convention:** Instructions follow the `source:destination` convention.

**Condition flags:** Only `OPW` instructions may modify the CPU's flags.

**Labels:** If a label used by `JXX` remains unresolved, the assembler produces an error.

### OPW function codes

| Function | `ifun` | Description |
|---|---|---|
| `ADD` | `0x0` | Adds `rA` to `rB`. |
| `SUB` | `0x1` | Subtracts `rA` from `rB`. |
| `ANDW` | `0x2` | Performs bitwise AND on `rA` and `rB`. |
| `XORW` | `0x3` | Performs bitwise XOR on `rA` and `rB`. |

### JXX function codes

| Function | `ifun` | Condition |
|---|---|---|
| `JMP` | `0x0` | Unconditional jump. |
| `JLE` | `0x1` | `(SF XOR OF) OR ZF` — less than or equal to zero. |
| `JL` | `0x2` | `SF XOR OF` — less than zero. |
| `JE` | `0x3` | `ZF` — equal to zero. |
| `JNE` | `0x4` | `!ZF` — not equal to zero. |
| `JGE` | `0x5` | `SF == OF` — greater than or equal to zero. |
| `JG` | `0x6` | `!ZF AND !(SF XOR OF)` — greater than zero. |

### CMOVXX function codes

| Function | `ifun` |
|---|---|
| `CMOVLE` | `0x1` |
| `CMOVL` | `0x2` |
| `CMOVE` | `0x3` |
| `CMOVNE` | `0x4` |
| `CMOVGE` | `0x5` |
| `CMOVG` | `0x6` |

## 6. Instruction Encoding

MiniVM16 uses variable-length instructions ranging from 1 to 4 bytes. The first byte of every instruction contains the instruction code (`icode`) and function code (`ifun`).

The upper 4 bits represent `icode`, while the lower 4 bits represent `ifun`.

| Bits 7–4 | Bits 3–0 |
|---|---|
| `icode` | `ifun` |

Depending on the instruction, additional bytes may contain register identifiers (`rA`, `rB`) and a 16-bit immediate value or address (`valC`).

### 6.1 Register Encoding

Instructions that require register operands use a register byte immediately after the opcode.

| Bits 7–4 | Bits 3–0 |
|---|---|
| `rA` | `rB` |

Each register identifier occupies 4 bits. The value `0xF` represents `NO_REG` and is used when a register field is unused.

For `PUSHW` and `POPW`, the register operand is encoded in the `rA` field, while `rB` is set to `NO_REG`.

### 6.2 Immediate Value Encoding

Immediate values and addresses are represented using 16 bits and stored in little-endian format.

For example, the value `0x1234` is encoded as:

| Byte | Value |
|---|---|
| Low byte | `0x34` |
| High byte | `0x12` |

### 6.3 Instruction Formats

The following table describes the binary representation and size of each instruction.

| Instruction | Byte 0 | Byte 1 | Byte 2 | Byte 3 | Size |
|---|---|---|---|---|---|
| `HALT` | `00` | — | — | — | 1 |
| `NOP` | `10` | — | — | — | 1 |
| `RRMOVW` | `20` | `rA:rB` | — | — | 2 |
| `IRMOVW` | `30` | `F:rB` | `valC[7:0]` | `valC[15:8]` | 4 |
| `RMMOVW` | `40` | `rA:rB` | `valC[7:0]` | `valC[15:8]` | 4 |
| `MRMOVW` | `50` | `rA:rB` | `valC[7:0]` | `valC[15:8]` | 4 |
| `OPW` | `6:ifun` | `rA:rB` | — | — | 2 |
| `JXX` | `7:ifun` | `valC[7:0]` | `valC[15:8]` | — | 3 |
| `CALL` | `80` | `valC[7:0]` | `valC[15:8]` | — | 3 |
| `RET` | `90` | — | — | — | 1 |
| `PUSHW` | `A0` | `rA:F` | — | — | 2 |
| `POPW` | `B0` | `rA:F` | — | — | 2 |
| `CMOVXX` | `C:ifun` | `rA:rB` | — | — | 2 |

All fixed byte values are hexadecimal. In the table, `6:ifun`, `7:ifun`, and `C:ifun` represent a single byte formed by combining the instruction code with its function code.

The function codes for `OPW`, `JXX`, and `CMOVXX` are defined in Section 5.

### 6.4 Instruction Validation

During the Fetch stage, the CPU validates the instruction code, function code, and register identifiers.

An invalid instruction encoding produces the `CPU_INS` status.

The CPU also checks whether the instruction fits within the available memory space. An instruction that exceeds the memory boundary produces the `CPU_ADDR` status.

## 7. Processor Status

The processor has four states: CPU_AOK which means that everything is good. This is the only state in which the cpu will run. The following states are error states: CPU_ADDR (it means the cpu has encountered an invalid address error),
CPU_INS (the cpu has encountered an unknown instruction). CPU_HALT halts the cpu execution. 

## 8. Execution Semantics

This section defines the effects of each instruction on the CPU registers, memory, condition codes, and program counter.

The following notation is used:

| Notation | Description |
|---|---|
| `R[r]` | The 16-bit value stored in register `r`. |
| `M₁₆[a]` | The 16-bit word stored in memory at address `a`. |
| `PC` | The program counter. |
| `PC_next` | The address immediately following the current instruction. |
| `V` | A 16-bit immediate value or target address. |
| `D` | A 16-bit displacement. |
| `ZF`, `SF`, `OF` | The CPU condition flags. |
| `←` | Assignment. |

All register arithmetic uses 16-bit results. Arithmetic results that exceed 16 bits wrap modulo 2¹⁶. Operations involving memory must access valid addresses.

### 8.1 Data Movement

| Instruction | Execution Semantics |
|---|---|
| `RRMOVW rA, rB` | `R[rB] ← R[rA]` |
| `IRMOVW V, rB` | `R[rB] ← V` |
| `RMMOVW rA, D[rB]` | `M₁₆[R[rB] + D] ← R[rA]` |
| `MRMOVW D[rA], rB` | `R[rB] ← M₁₆[R[rA] + D]` |

### 8.2 Arithmetic and Logical Operations

| Instruction | Execution Semantics |
|---|---|
| `ADDW rA, rB` | `R[rB] ← (R[rB] + R[rA]) mod 2¹⁶` |
| `SUBW rA, rB` | `R[rB] ← (R[rB] - R[rA]) mod 2¹⁶` |
| `ANDW rA, rB` | `R[rB] ← R[rB] & R[rA]` |
| `XORW rA, rB` | `R[rB] ← R[rB] ^ R[rA]` |

All `OPW` instructions update the condition flags based on the result:

- `ZF ← 1` if the result is zero; otherwise `0`.
- `SF ← 1` if bit 15 of the result is set; otherwise `0`.
- `OF ← 1` if a signed arithmetic overflow occurs during `ADDW` or `SUBW`; otherwise `0`.

Other instructions do not modify the condition flags.

### 8.3 Control Flow

The following conditions are used by conditional jumps and moves:

| Condition | Expression |
|---|---|
| Less than or equal | `(SF ^ OF) \|\| ZF` |
| Less than | `SF ^ OF` |
| Equal | `ZF` |
| Not equal | `!ZF` |
| Greater than or equal | `!(SF ^ OF)` |
| Greater than | `!ZF && !(SF ^ OF)` |

For conditional jumps:

| Instruction | Execution Semantics |
|---|---|
| `JMP V` | `PC ← V` |
| `JLE V` | `PC ← V` if less than or equal |
| `JL V` | `PC ← V` if less than |
| `JE V` | `PC ← V` if equal |
| `JNE V` | `PC ← V` if not equal |
| `JGE V` | `PC ← V` if greater than or equal |
| `JG V` | `PC ← V` if greater than |

If a conditional jump is not taken, `PC ← PC_next`.

For conditional moves:

| Instruction | Execution Semantics |
|---|---|
| `CMOVLE rA, rB` | `R[rB] ← R[rA]` if less than or equal |
| `CMOVL rA, rB` | `R[rB] ← R[rA]` if less than |
| `CMOVE rA, rB` | `R[rB] ← R[rA]` if equal |
| `CMOVNE rA, rB` | `R[rB] ← R[rA]` if not equal |
| `CMOVGE rA, rB` | `R[rB] ← R[rA]` if greater than or equal |
| `CMOVG rA, rB` | `R[rB] ← R[rA]` if greater than |

If the condition is not satisfied, the destination register remains unchanged.

### 8.4 Stack Operations

The stack grows towards lower memory addresses. Each stack operation transfers a 16-bit word and changes `RSP` by two bytes.

| Instruction | Execution Semantics |
|---|---|
| `PUSHW rA` | `R[RSP] ← R[RSP] - 2` |
| | `M₁₆[R[RSP]] ← R[rA]` |
| `POPW rA` | `R[rA] ← M₁₆[R[RSP]]` |
| | `R[RSP] ← R[RSP] + 2` |
| `CALL V` | `R[RSP] ← R[RSP] - 2` |
| | `M₁₆[R[RSP]] ← PC_next` |
| | `PC ← V` |
| `RET` | `PC ← M₁₆[R[RSP]]` |
| | `R[RSP] ← R[RSP] + 2` |

The operations are listed in their logical execution order. The program must initialize `RSP` before using the stack.

### 8.5 Other Instructions

| Instruction | Execution Semantics |
|---|---|
| `NOP` | No changes to registers, memory, or flags. Execution continues at `PC_next`. |
| `HALT` | Stops execution and sets the CPU status to `CPU_HALT`. |

Unless explicitly changed by a control-flow instruction, the program counter advances to `PC_next` after successful execution.

# SIMPLE Assembler and Emulator

## 📌 Project Overview
This project implements a **two-pass assembler** and an **emulator** for the **extended SIMPLE instruction set** as required in the **CS210 MiniProject (2024)**.  

The assembler translates SIMPLE assembly code into machine code, producing both object files and human-readable listing files.  
The emulator executes the generated machine code, simulating the behavior of the SIMPLE machine.  

---

## ✨ Features
- **Two-pass assembler** (`asm.cpp`):
  - Resolves labels and mnemonics.
  - Uses a single routine for both passes.
  - Detects label errors (duplicate, missing, etc).
  - Produces:
    - **Object file** (`.o`) containing binary machine code.
    - **Listing file** (`.lst`) showing addresses, machine code, mnemonics, and labels.
    - **Log file** (`.log`) for warnings and errors.
  - Supports decimal, octal, and hexadecimal operands.
  - Skips blank lines and comments.
- **Emulator** (`emu.cpp`):
  - Loads object files and executes SIMPLE instructions.
  - Options:
    - `-trace` → Show execution trace.
    - `-before` → Show memory dump before execution.
    - `-after` → Show memory dump after execution.
    - `-isa` → Show instruction set architecture dump.
  - Produces output both on terminal and optional dump files.
  - Detects invalid mnemonics and errant programs.
- **Test Programs**:
  - Includes all provided test files (`test1.asm` … `test4.asm`).
  - Custom test programs (`mytest1.asm`, `mytest2.asm`).
  - Bubble sort implementation in SIMPLE assembly.

---

## ⚙️ Build Instructions
### Compile Assembler
```bash
g++ asm.cpp -o asm
````

### Compile Emulator

```bash
g++ emu.cpp -o emu
```

---

## 🚀 Usage

### Assemble a Program

```bash
./asm <filename>.asm
```

Generates:

* `<filename>.o`   → object file
* `<filename>.lst` → listing file
* `<filename>.log` → error/warning log

### Run on Emulator

```bash
./emu -<option> <filename>.o
```

Options:

* `trace`   → show execution trace
* `before`  → memory dump before execution
* `after`   → memory dump after execution
* `isa`     → instruction set dump

---

## 🧪 Example

```bash
./asm bubble.asm
./emu -trace bubble.o
```

Runs the **Bubble Sort program** in SIMPLE assembly and prints execution trace.

---

## ✅ Implemented Requirements

* [x] Two-pass assembler with error checking
* [x] Object, listing, and log file generation
* [x] Emulator with memory dump and trace support
* [x] Test programs (valid + error cases)
* [x] Custom test programs
* [x] Bubble sort program in SIMPLE assembly
* [ ] SET pseudo-instruction (not implemented)

---


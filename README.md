

# CHAS Assembler for CHASM

This VS Code project implements an assembler for the CHASM CPU. It is written in mostly-procedural C++, with only a little OOP.

The code is very basic and has a lot of avoidable copy-and-pasting in it; CHAS is meant to be adequate, not good.


## Usage

CHAS has no dependencies. It should build with a simple `cmake -S src build; cmake --build build` in the project's root directory.

The syntax for running CHAS is:

	`chas [--print-opcodes] [--base <address>] [--o <filename>] <filename>`

	`--print-opcodes`: Emits a complete space-separated list of the instruction mnemonics currently supported by this version of CHAS.
	`--base <address>`: Sets the base address. CHASM ROMs start at 0x3000 (the default). Userland programs in a multitasking OS start at 0x0000.
	`--o <filename>` or `--output <filename>`: Sets the output filename. Default is input filename with `.o` appended.
	`<filename>`: The last argument is the file to assemble.

Arguments are parsed from left to right, so `--base` and `--o` must precede `<filename>`.


## The CHAS assembly language

The opcodes and instructions are defined in https://github.com/rhetorica/chas/blob/master/doc/chasm-spec.txt

The file `doc/CHAS.xml` provides syntax highlighting for Notepad++ in UDL format.


## Lethe

This repository is a component of the LETHE project, which aims to build a complete software stack based on the Dhar programming language. Read more at http://lethe.rhetori.ca/

TARGET := hvexec

CC := clang
LD := clang -o
AS := toolchain/nasm -fbin

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=out/%.o)

out/$(TARGET): out $(OBJ) out/boot out/basic
	$(LD) $@ -framework Hypervisor $(OBJ)

$(OBJ): out/%.o : src/%.c
	$(CC) -Wall -Isrc -c $< -o $@

out/boot: src/boot.asm
	$(AS) src/boot.asm -o out/boot

out/basic: test/basic.c
	$(CC) -c test/basic.c -o out/basic.o
	$(CC) -o out/basic out/basic.o

out:
	mkdir -p out

.PHONEY: run
run: out/$(TARGET)
	out/hvexec -i -l DEBUG -k out/boot out/basic

.PHONEY: clean
clean:
	rm -r out

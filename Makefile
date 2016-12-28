TARGET := hvexec

CC := clang
LD := clang -o
AS := toolchain/nasm -fbin

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=out/%.o)

TEST_SRC := $(wildcard test/test_*.c)
TEST_TARGETS := $(TEST_SRC:test/%.c=out/%)

out/$(TARGET): out $(OBJ) out/boot
	$(LD) $@ -framework Hypervisor $(OBJ)

$(OBJ): out/%.o : src/%.c
	$(CC) -Wall -Isrc -c $< -o $@

out/boot: src/boot.asm
	$(AS) src/boot.asm -o out/boot

$(TEST_TARGETS): out/% : test/%.c
	$(CC) -Wall -Itest $< -o $@

out:
	mkdir -p out

.PHONEY: run
run: out/$(TARGET) $(TEST_TARGETS)
	echo $@
	out/hvexec -i -l DEBUG -k out/boot out/test_hello

.PHONEY: clean
clean:
	rm -r out

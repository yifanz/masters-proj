TARGET := hvexec

CC := clang
LD := clang -o
AS := toolchain/nasm -fbin

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=out/%.o)

TEST_SRC := $(wildcard test/test_*.c)
TEST_TARGETS := $(TEST_SRC:test/%.c=out/%)

out/$(TARGET): out $(OBJ) out/boot $(TEST_TARGETS) 
	$(LD) $@ -framework Hypervisor $(OBJ)

$(OBJ): out/%.o : src/%.c
	$(CC) -Wall -Isrc -Iinclude -c $< -o $@

out/boot: src/boot.asm
	$(AS) src/boot.asm -o out/boot

$(TEST_TARGETS): out/% : test/%.c
	$(CC) -Wall -Itest $< -o $@

out/plugin.dylib: test/plugin.c
	$(CC) -Wall -Iinclude -dynamiclib $< -o $@

out:
	mkdir -p out

.PHONEY: run-hello
run-hello: out/$(TARGET) out/plugin.dylib
	out/hvexec -i -l INFO -k out/boot -s out/plugin.dylib out/test_hello hello world

.PHONEY: run-cat
run-cat: out/$(TARGET) out/plugin.dylib
	out/hvexec -i -l INFO -k out/boot -s out/plugin.dylib out/test_cat ATTRIB

.PHONEY: clean
clean:
	rm -r out

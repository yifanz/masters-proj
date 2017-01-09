TARGET := hvexec

CC := clang
LD := clang -o
AS := toolchain/nasm -fbin

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=out/%.o)

TEST_SRC := $(wildcard test/test_*.c)
TEST_TARGETS := $(TEST_SRC:test/%.c=out/%)

out/$(TARGET): out $(OBJ) out/boot $(TEST_TARGETS) out/plugin.dylib
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

.PHONEY: run-echo
run-echo: out/$(TARGET)
	out/hvexec -i -l INFO -k out/boot out/test_echo $(ARGS)

.PHONEY: run-cat
run-cat: out/$(TARGET)
	out/hvexec -i -l INFO -k out/boot -s out/plugin.dylib out/test_cat ATTRIB

.PHONEY: bench-pidigits
bench-pidigits: out/$(TARGET)
	time out/hvexec -l SILENT -k out/boot out/test_pidigits 1000 > /dev/null
	time out/test_pidigits 1000 > /dev/null

.PHONEY: bench-syscall
bench-syscall: out/$(TARGET)
	time out/hvexec -l SILENT -k out/boot out/test_syscall $(ARGS) > /dev/null
	time out/test_syscall $(ARGS) > /dev/null

.PHONEY: bench-io
bench-io: out/$(TARGET)
	@echo Benchmark Write
	time out/hvexec -l SILENT -k out/boot out/test_io out/_tmp1 write 1000000 > /dev/null
	time out/test_io out/_tmp2 write 1000000 > /dev/null
	diff --brief out/_tmp1 out/_tmp2
	@echo Benchmark Read
	time out/hvexec -l SILENT -k out/boot out/test_io out/_tmp1 read > /dev/null
	time out/test_io out/_tmp2 read > /dev/null

.PHONEY: bench-startup
bench-startup: out/$(TARGET) 
	time for i in `seq 1 1000`; do out/hvexec -l SILENT -k out/boot out/test_startup; done
	time for i in `seq 1 1000`; do out/test_startup; done

.PHONEY: clean
clean:
	rm -r out

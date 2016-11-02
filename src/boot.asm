; Decimal to hexadecimal reminder.
; 2^10 (1024) = 0x400
; 2^12 (4096) = 0x1000
; 2^20 (mega) = 0x100000
; 2^30 (giga) = 0x40000000
; 2^32        = 0x100000000

; Hexadecimal to binary reminder.
; 0x1 = 0b0001  0x9 = 0b1001
; 0x2 = 0b0010  0xA = 0b1010
; 0x3 = 0b0011  0xB = 0b1011
; 0x4 = 0b0100  0xC = 0b1100
; 0x5 = 0b0101  0xD = 0b1101
; 0x6 = 0b0110  0xE = 0b1110
; 0x7 = 0b0111  0xF = 0b1111
; 0x8 = 0b1000

; Once assembled, the binary is loaded at this physical address.
; All relative jump calculations use this as the base address.
org 0x8000

; The VM is initialized in 32-bit protected mode. We skip 16-bit mode entirely.
; Although we only support 64-bit executables, having the 32-bit mode
; around might be useful for adding 32-bit support in the future.
[BITS 32]

; =======================
; |    Memory Layout    |
; =======================
;
; 0x0000 <- GDT for 32-bit mode (can be erased once we get to 64-bit mode)
; 0x1000 <- Start of 4KByte kernel stack (stack grows down)
; 0x1000 <- PML4E, root page structure
; 0x2000 <- PDPTE, 1st level page structure
; 0x3000 <- PDPTE2, (for dyld)
; 0x4000 <- PDE, 2nd level page structure (1st Gbyte for kernel)
; 0x5000 <- PDE2 (for dyld)
; 0x6000 <- PDE3 (5th Gbyte where we put the application)
; 0x7000 <- TSS segment
; 0x8000 <- The first instrucion is here

; Kernel stack
%define KERN_STACK_ADDR 0x20000

; Setup the kernel stack.
mov esp, KERN_STACK_ADDR - 16
mov ebp, esp

; IA-32e paging (aka long mode, 64-bit mode) translates 48-bit linear virtual
; addresses into 52 bit physical addresses. Intel uses the word "linear" to
; indicate the use of paging rather than segmentation to support virtual memory.
; In other words, if paging was disabled, the "linear" address is not translated
; any further and is the physical address.
; A total of 256 TBytes of virtual memory is addressable.
; Paging is required for long mode.
;
; x86 uses hierarchical paging and supports page sizes ranging from 4Kbytes
; to 1 GByte. Choosing larger page size improves TLB hits and reduces the
; metadata overhead at the cost of granularity and less optimal space allocation
; We use 2MBytes rather than the typical 4Kbytes page size in favor of
; reduced depth of the paging hierarchy.
; For long mode, the size of each paging structure (aka page table) is
; 4096 Bytes and contains 512 entries (each entry is 8 bytes).
;
; Here is the mapping for linear to physical address translation:
; bits  = description
; 0-20  = byte offset in 2Mbyte page (actual memory page)
; 21-29 = index into page directory table (2nd level)
; 30-38 = index into page directory pointer table (1st level)
; 39-47 = index into page mapping table (root level)
;
; Here is the breakdown of the coverage for each level
; A single PML4E covers 256 TBytes.
; A PML4E points to 512 PDPTEs. Each of which covers 512 GBytes.
; Each PDPTE points to 512 PDEs. A PDE covers 1 GByte.
; There are 512 entries in each PDE. Each entry points to a 2MByte page.

; Paging structures must be aligned to a 4KByte boundary.
; In other words, the lower 12 bits are always zero.
%define PML4E_ADDR 0x1000
%define PDPTE_ADDR 0x2000
%define PDPTE2_ADDR 0x3000
%define PDE_ADDR 0x4000
%define PDE2_ADDR 0x5000
%define PDE3_ADDR 0x6000

; PML4E entry format:
; bits  = description
; 0     = Present
; 1     = If 0, writes not allowed
; 2     = If 0, user mode access not allowed
; 3-11  = I don't use them
; 12-50 = PDPTE addr (4 KByte aligned)
; 51-62 = I don't use them
; 63    = If 1, execute disabled

mov dword eax, PDPTE_ADDR
; Set user, allow write and present bits.
or dword eax, 0b011 
mov dword [PML4E_ADDR], eax
mov dword [PML4E_ADDR+4], 0

; PDPTE entry format:
; similar to PML4E

mov dword eax, PDE_ADDR
or dword eax, 0b011
mov dword [PDPTE_ADDR], eax
mov dword [PDPTE_ADDR+4], 0

; PDE entry format:
; bits  = description
; 0     = Present
; 1     = If 0, writes not allowed
; 2     = If 0, user mode access not allowed
; 3-6   = I don't use them
; 7     = If 1, this entry points to 2Mbyte page
; 8-20  = I don't use them
; 21-50 = Address of 2Mbyte page
; 51-62 = I don't use them
; 63    = If 1, execute disabled

; The first 2MBytes are for the kernel
; Set present, allow write and page size bits.
mov dword [PDE_ADDR], 0b10000011
mov dword [PDE_ADDR+4], 0

; Point CR3 to the root page structure.
mov eax, PML4E_ADDR
mov cr3, eax

; Assert the PGE and PAE bits. This is a prerequisite for long mode.
mov eax, cr4
or eax, 0b10100000
or eax, 0b11000000000
mov cr4, eax

; Enable long mode by writing appropriate bits to the EFER MSR.
; Specify the MSR you want to read in ecx.
mov ecx, 0xC0000080
rdmsr 
; rdmsr puts the result value in eax.
or eax, 0b100000000
wrmsr ; exactly the reverse of rdmsr

; Assert PG bit of CR0 to turn on paging.
mov eax, cr0
or eax, 0x80000000
or eax, 0b10
mov cr0, eax

; load the GDT
lgdt [gdt_hdr]

; load the IDT
lidt [idt_hdr]

; load data segment selector into segment registers
mov ax, [ss_sel]
mov ss, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

; setup task segment
%define TSS_BASE 0x7000
%define TSS_LIMIT 0x67 ; 103 bytes, max legal offset inside a 104 byte segment
mov eax, TSS_BASE
mov dword [eax], 0x0 ; Reserved
add eax, 0x4
mov dword [eax], KERN_STACK_ADDR ; RSP0 lower
add eax, 0x4
mov dword [eax], 0x0 ; RSP0 upper
add eax, 0x4
mov dword [eax], 0x0 ; RSP1 lower
add eax, 0x4
mov dword [eax], 0x0 ; RSP1 upper
add eax, 0x4
mov dword [eax], 0x0 ; RSP2 lower
add eax, 0x4
mov dword [eax], 0x0 ; RSP2 upper
add eax, 0x4
mov dword [eax], 0x0 ; Reserved
add eax, 0x4
mov dword [eax], 0x0 ; Reserved
add eax, 0x4
mov dword [eax], 0x0 ; IST1 lower
add eax, 0x4
mov dword [eax], 0x0 ; IST1 upper
add eax, 0x4
mov dword [eax], 0x0 ; IST2 lower
add eax, 0x4
mov dword [eax], 0x0 ; IST2 upper
add eax, 0x4
mov dword [eax], 0x0 ; IST3 lower
add eax, 0x4
mov dword [eax], 0x0 ; IST3 upper
add eax, 0x4
mov dword [eax], 0x0 ; IST4 lower
add eax, 0x4
mov dword [eax], 0x0 ; IST4 upper
add eax, 0x4
mov dword [eax], 0x0 ; IST5 lower
add eax, 0x4
mov dword [eax], 0x0 ; IST5 upper
add eax, 0x4
mov dword [eax], 0x0 ; IST6 lower
add eax, 0x4
mov dword [eax], 0x0 ; IST6 upper
add eax, 0x4
mov dword [eax], 0x0 ; IST7 lower
add eax, 0x4
mov dword [eax], 0x0 ; IST7 upper
add eax, 0x4
mov dword [eax], 0x0 ; Reserved
add eax, 0x4
mov dword [eax], 0x0 ; Reserved
add eax, 0x4
mov word [eax], 0x0 ; Reserved
add eax, 0x2
mov word [eax], 0x0 ; IO Map Base Address

; load task segment selector into task register
ltr [tss_sel]

; load code segment selector into CS register using a far jump
jmp 0b00001000:_64_bits

BITS 64
_64_bits:
  ; TODO TESTING!!!
  ;mov rax, 0xfadefeedcafebeef
  ;vmcall

  ; TODO TESTING!!!
  ;mov rbx, rsp
  ;push rbx
  ;vmcall

  ; TODO jump to ring 3 not working
  ;mov rax, rsp
  ;push 0x101011 ;user data segment with bottom 2 bits set for ring 3
  ;push rax ;push our current stack just for the heck of it
  ;pushf
  ;push 0x100011; ;user code segment with bottom 2 bits set for ring 3
  ;push ring3 
  ;iretq

  ; Map virtual address 0x100000000 to physical address 0x400000
  %define EXE_ADDR 0x100000000

  mov rax, EXE_ADDR
  shr rax, 21
  and rax, 0b111111111
  shl rax, 3
  add rax, PDE3_ADDR

  mov rbx, 0x400000
  or rbx, 0b10000111
  mov [rax], rbx

  mov rax, EXE_ADDR
  shr rax, 30
  and rax, 0b111111111
  shl rax, 3
  add rax, PDPTE_ADDR

  mov rbx, PDE3_ADDR
  or rbx, 0b111
  mov [rax], rbx

  ; Map virtual address 0x7fff5fc00000 to physical address 0x200000
  %define DYLD_ADDR 0x7fff5fc00000

  mov rax, DYLD_ADDR
  shr rax, 21
  and rax, 0b111111111
  shl rax, 3
  add rax, PDE2_ADDR

  mov rbx, 0x200000
  or rbx, 0b10000111
  mov [rax], rbx

  mov rax, DYLD_ADDR
  shr rax, 30
  and rax, 0b111111111
  shl rax, 3
  add rax, PDPTE2_ADDR

  mov rbx, PDE2_ADDR
  or rbx, 0b111
  mov [rax], rbx

  mov rax, DYLD_ADDR
  shr rax, 39
  and rax, 0b111111111
  shl rax, 3
  add rax, PML4E_ADDR

  mov rbx, PDPTE2_ADDR
  or rbx, 0b111
  mov [rax], rbx 

  ; TODO TESTING!!!
  ;mov rax, 0x100000000 + 3744
  ;mov rbx, [rax]
  ;vmcall


  ; TODO Hardcoded for now; parse header to find the beginning of text segment.
  ;mov rax, 0x100000000
  ;pop rbx
  ;add rax, rbx
  ;mov rax, 0x100000000 + 0xc00

  pop rax
  pop rbx
  ;mov rbx, [rax]
  ;hlt

  ; Once the the app's main function returns, we halt.
  push halt

  ; Jump to the application.
  push rax
  push rbx
  
  ;mov rax, 0x7fff5fc22bee
  ;mov rbx, [rax]
  ;hlt

  mov rax, 0
  mov rbx, 0
  mov rcx, 0
  mov rdx, 0

  ret

halt:
  hlt

; TODO ring3 not working
;ring3:
;  mov rax, 0xfacebeefcafe
;  ret
;  ; jump to executable
;  ;push 0x1011d0
;  ;ret
;
;  hlt

; TODO whatever some test code
;  mov rdi, 0xb8000 ; This is the beginning of "video memory."
;  mov rdx, rdi     ; We'll save that value for later, too.
;  mov rcx, 80*25   ; This is how many characters are on the screen.
;  mov ax, 0x7400   ; Video memory uses 2 bytes per character. The high byte
;                   ; determines foreground and background colors. See also
;; http://en.wikipedia.org/wiki/List_of_8-bit_computer_hardware_palettes#CGA
;                   ; In this case, we're setting red-on-gray (MIT colors!)
;  rep stosw        ; Copies whatever is in ax to [rdi], rcx times.
;
;  mov rdi, rdx       ; Restore rdi to the beginning of video memory.
;  mov rsi, hello     ; Point rsi ("source" of string instructions) at string.
;  mov rbx, hello_end ; Put end of string in rbx for comparison purposes.
;hello_loop:
;  movsb              ; Moves a byte from [rsi] to [rdi], increments rsi and rdi.
;  inc rdi            ; Increment rdi again to skip over the color-control byte.
;  cmp rsi, rbx       ; Check if we've reached the end of the string.
;  jne hello_loop     ; If not, loop.
;  hlt                ; If so, halt.
;
;hello:
;  db "Hello, kernel!"
;hello_end:

; GDT global descriptor format (8 bytes)
; See Intel 64 Software Developers' Manual, Vol. 3A, Figure 3-8
; bits  = description
; First 4 bytes
; 0-15  = segment limit (0-15)
; 16-31 = base addr (0-15)
; Next 4 bytes
; 0-7   = base addr (16-23)
; 8-11  = type (determines READ,EXECUTE,WRITE access)
; 12    = descriptor type (1 is code/data segment, 0 is system segment)
; 13-14 = DPL descriptor privilege level (0-3)
; 15    = Present (1 if segment is present in memory)
; 16-19 = segment limit (16-19)
; 20    = reserved
; 21    = L 64 bit code segment (1 if 64-bit code segment and D-bit must be cleared, 0 otherwise)
; 22    = D (always 1 if 32-bit code/data segment, 0 otherwise)
; 23    = G granularity of segment limit (0 is byte, 1 is 4kb)
; 24-31 = base addr (24-31)

; Arguments: %1 base addr, %2 segment limit, %3 flags, %4 type
%macro GDT_ENTRY 4
  dw %2 & 0xffff
  dw %1 & 0xffff
  db (%1 >> 16) & 0xff
  db %4 | ((%3 << 4) & 0xf0)
  db (%3 & 0xf0) | ((%2 >> 16) & 0x0f)
  db %1 >> 24
%endmacro

; GDT type (see Intel Manual Vol. 3A, section 3.4.5.1)
%define EXECUTE_READ 0b1011
%define READ_WRITE 0b0011
; Flag bits: G, D, L, reserved, Present, Ring, descriptor type
%define RING0_64_CODE_DATA 0b10101001 
%define RING3_64_CODE_DATA 0b10101111 

; TSS Descriptor Entry
; See Intel 64 Software Developers' Manual, Vol. 3 Figure 7-4
%macro TSS_ENTRY 4
  ; %1 is base address, %2 is segment limit, %3 is flags, %4 is type.
  dw %2 & 0xffff
  dw %1 & 0xffff
  db (%1 >> 16) & 0xff
  db %4 | ((%3 << 4) & 0xf0)
  db (%3 & 0xf0) | ((%2 >> 16) & 0x0f)
  db %1 >> 24
  dd %1 >> 32 
  dd 0
%endmacro

; IDT Descriptor Entry
; See Intel Manual Vol. 3 Figure 6-7
; bits = description
; First 4 bytes
; 0-15   = Destination Offset (0-15)
; 16-31  = Destination segment selector
; Next 4 bytes
; 0-2    = IST
; 0-7    = 0
; 8-11   = type
; 12     = 0
; 13-14  = DPL
; 15     = Present
; 16-31  = Destination offset (16-31)
; Next 4 bytes
; 0-31   = Destination offset (32-63)
; Next 4 bytes
; Reserved

; Arguments: %1 base addr, %2 segment selector, %3 IST, %4 type
%macro IDT_ENTRY 4
  
%endmacro

%define TSS_TYPE 0b1001
; Flag bits: G, reserved, reserved, system use, P, Ring, descriptor type 
%define TSS_FLAGS 0b00001000 

; Global descriptor table (loaded by lgdt instruction)
gdt_hdr:
  ; Limit bits 0-15
  dw gdt_end - gdt - 1
  ; Base address bits 16-47
  dd gdt
gdt:
  GDT_ENTRY 0, 0, 0, 0                                    ; null descriptor in first position is required by Intel
  GDT_ENTRY 0, 0xffffff, RING0_64_CODE_DATA, EXECUTE_READ ; kernel code segment
  GDT_ENTRY 0, 0xffffff, RING0_64_CODE_DATA, READ_WRITE   ; kernel data segment
  TSS_ENTRY TSS_BASE, TSS_LIMIT, TSS_FLAGS, TSS_TYPE      ; task descriptor (unlike others, this is 16 bytes)
  GDT_ENTRY 0, 0xffffff, RING3_64_CODE_DATA, EXECUTE_READ ; user code segment
  GDT_ENTRY 0, 0xffffff, RING3_64_CODE_DATA, READ_WRITE   ; user data segment
gdt_end:

; Interrupt descriptor table (loaded by lidt instruction)
idt_hdr:
  ; Limit bits 0-15
  dw idt_end - gdt - 1
  ; Base address bits 16-47
  dd idt
idt:
  ; TODO
  IDT_ENTRY 0, 0, 0, 0
idt_end:

; Segment Selector Format
; bits = description
; 0-1  = RPL requested privilege level (0-3)
; 2    = TI table indicator (0 for GDT, 1 for all else)
; 3-15 = Index into GDT (multiplied by 8)

; Stack segment selector (stored in SS register)
ss_sel:
  dw 0b00010000
; Task segment selector (loaded by ltr instruction)
tss_sel:
  dw 0b00011000

#include "../kernel.hpp"
#include <stdint.h>
#include <stddef.h>

namespace abyss {

class Decompiler {
    const char* reg_names[32] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };

    void print_reg(int r) {
        volatile char* uart = (volatile char*)0x10000000;
        const char* name = reg_names[r];
        while (*name) *uart = *name++;
    }

    void print_str(const char* s) {
        volatile char* uart = (volatile char*)0x10000000;
        while (*s) *uart = *s++;
    }

    void print_hex(uint64_t n) {
        volatile char* uart = (volatile char*)0x10000000;
        const char* hex = "0123456789abcdef";
        print_str("0x");
        for (int i = 15; i >= 0; --i) { 
            *uart = hex[(n >> (i * 4)) & 0xF];
        }
    }

    void print_int(int64_t n) {
        // Simple decimal print
        if (n < 0) { print_str("-"); n = -n; }
        if (n == 0) { print_str("0"); return; }
        char buf[32];
        int i = 0;
        while (n > 0) { buf[i++] = (n % 10) + '0'; n /= 10; }
        while (i > 0) { volatile char* uart = (volatile char*)0x10000000; *uart = buf[--i]; }
    }

public:
    void syscall_decompile(uint64_t addr) {
        // Disassemble 16 bytes (4 instructions)
        uint32_t* ptr = (uint32_t*)addr;
        print_str("Decompiling at "); print_hex(addr); print_str(":\n");

        for (int i = 0; i < 4; ++i) {
            uint32_t inst = ptr[i];
            print_hex(inst); print_str("  ");

            uint32_t opcode = inst & 0x7F;
            uint32_t rd = (inst >> 7) & 0x1F;
            uint32_t funct3 = (inst >> 12) & 0x07;
            uint32_t rs1 = (inst >> 15) & 0x1F;
            uint32_t rs2 = (inst >> 20) & 0x1F;
            uint32_t funct7 = (inst >> 25) & 0x7F;
            int32_t imm_i = (int32_t)inst >> 20;
            int32_t imm_s = ((inst >> 25) << 5) | ((inst >> 7) & 0x1F);
            if (imm_s & 0x800) imm_s |= 0xFFFFF000; // sign extend

            if (opcode == 0x33) { // ADD, SUB, etc.
                if (funct3 == 0 && funct7 == 0) { print_str("add "); }
                else if (funct3 == 0 && funct7 == 0x20) { print_str("sub "); }
                else if (funct3 == 0x6) { print_str("or "); }
                else if (funct3 == 0x7) { print_str("and "); }
                else { print_str("unknown-R "); }
                print_reg(rd); print_str(", "); print_reg(rs1); print_str(", "); print_reg(rs2);
            } else if (opcode == 0x13) { // ADDI, etc.
                if (funct3 == 0) print_str("addi ");
                else if (funct3 == 6) print_str("ori ");
                else if (funct3 == 7) print_str("andi ");
                else print_str("unknown-I ");
                print_reg(rd); print_str(", "); print_reg(rs1); print_str(", "); print_int(imm_i);
            } else if (opcode == 0x37) { // LUI
                print_str("lui "); print_reg(rd); print_str(", "); print_hex((inst >> 12) & 0xFFFFF);
            } else if (opcode == 0x6F) { // JAL
                print_str("jal "); print_reg(rd); print_str(", <offset>"); 
            } else if (opcode == 0x67) { // JALR
                print_str("jalr "); print_reg(rd); print_str(", "); print_reg(rs1); print_str(", "); print_int(imm_i);
            } else if (opcode == 0x63) { // Branches
                 print_str("b<cond> "); print_reg(rs1); print_str(", "); print_reg(rs2); print_str(", <offset>");
            } else {
                print_str("???");
            }
            print_str("\n");
        }
    }
};

class HotPatcher {
public:
    void syscall_hotpatch(uint64_t addr, uint32_t new_inst) {
        volatile char* uart = (volatile char*)0x10000000;
        const char* msg = "Hotpatching...\n";
        while (*msg) *uart = *msg++;
        
        uint32_t* ptr = (uint32_t*)addr;
        *ptr = new_inst; // Overwrite instruction
        
        // Flush I-Cache (FENCE.I)
        asm volatile("fence.i");
    }
};

}

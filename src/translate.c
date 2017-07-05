#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tables.h"
#include "translate_utils.h"
#include "translate.h"

/* SOLUTION CODE BELOW */
const int TWO_POW_SEVENTEEN = 131072;    // 2^17

/* Writes instructions during the assembler's first pass to OUTPUT. The case
   for general instructions has already been completed, but you need to write
   code to translate the li and other pseudoinstructions. Your pseudoinstruction 
   expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in pass two. However, for
   pseudoinstructions, you must make sure that ARGS contains the correct number
   of arguments. You do NOT need to check whether the registers / label are 
   valid, since that will be checked in part two.

   Also for li:
    - make sure that the number is representable by 32 bits. (Hint: the number 
        can be both signed or unsigned).
    - if the immediate can fit in the imm field of an addiu instruction, then
        expand li into a single addiu instruction. Otherwise, expand it into 
        a lui-ori pair.

   If you are going to use the $zero or $0, use $0, not $zero.

   MARS has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if MARS behaves differently.

   Use fprintf() to write. If writing multiple instructions, make sure that 
   each instruction is on a different line.

   Returns the number of instructions written (so 0 if there were any errors).
 */
unsigned write_pass_one(FILE* output, const char* name, char** args, int num_args) {
    if (strcmp(name, "li") == 0) {
        /* YOUR CODE HERE */
        if(!output || !name || !args || num_args != 2){return 0;}
        long int imm;
        int temp = translate_num(&imm,args[1],INT32_MIN,UINT32_MAX);
        if(translate_reg(args[0]) || temp){
            return 0;
        }
        if(imm>=INT32_MIN && imm<=INT32_MAX){
            char* name1 = "addiu";
            char** args1 = malloc(3*sizeof(char*));
            args1[0] = args[0]; //  addiu rt rs imm
            args1[1] = "$0";
            args1[2] = args[1];
            write_inst_string(output, name1, args1, 3); // helper function to write instructions
            free(args1);
            return 1;
        }
        // use lui ori pair
        char* name2 ="lui";
        char** args2 = malloc(2*sizeof(char*));
        args2[0] = "$at";
        long int temp1 = imm>>16;
        temp1 = temp1 & 0xFFFF; // take the first 16 bits;
        char copy[100];
        sprintf(copy,"%ld",temp1); // transfer long int into string
        args2[1] = copy;
        
        char* name3 = "ori";
        char** args3 = malloc(2*sizeof(char*)); // ori $r1 $r1 imme
        args3[0] = args[0];
        args3[1] = "$at";
        long int temp2 = ((imm<<16)>>16) & 0xffff;
        char copy2[100];
        sprintf(copy2,"%ld",temp2);
        args[2] = copy2;
        
        write_inst_string(output,name2,args2,2);
        write_inst_string(output,name3,args3,3);
        
        return 2;
        
    } else if (strcmp(name, "push") == 0) {
        /* YOUR CODE HERE */
        if(!output || !name || !args || num_args != 1){return 0;}
        char* first_name = "addiu";
        char** first_args = malloc(3*sizeof(char*));
        first_args[0] = "$sp";
        first_args[1] = "$sp";
        long int temp = -4;
        char imm[100];
        sprintf(imm,"%ld",temp);
        first_args[2] = imm;
        
        char* second_name = "sw";
        char** second_args = malloc(3*sizeof(char*));
        second_args[0] = args[0];
        second_args[1] = "$0";
        second_args[2] = "$sp";
        
        write_inst_string(output,first_name,first_args,3);
        write_inst_string(output,second_name,second_args,3);
        
        free(first_args);
        free(second_args);
        return 2; 
        
    } else if (strcmp(name, "pop") == 0) {
        /* YOUR CODE HERE */
        if(!output || !name || !args || num_args != 1){return 0;}
        char* first_name = "lw";
        char** first_args = malloc(3*sizeof(char*));
        first_args[0] = args[0];
        first_args[1] = "$0";
        first_args[2] = "$sp";
        
        char* second_name = "addiu";
        char** second_args = malloc(3*sizeof(char*));
        second_args[0] = "$sp";
        second_args[1] = "$sp";
        long int temp = 4;
        char imm[100];
        sprintf(imm,"%ld",temp);
        second_args[2] = imm;
      
        write_inst_string(output,first_name,first_args,3);
        write_inst_string(output,second_name,second_args,3);
        
        free(first_args);
        free(second_args);
        
        return 2;  
        
    } else if (strcmp(name, "mod") == 0) {
        /* YOUR CODE HERE */
        if(!output || !name || !args || num_args != 3){return 0;}
        char* first_name = "div";
        char** first_args = malloc(2*sizeof(char*));
        first_args[0] = args[1];
        first_args[1] = args[2];
        
        char* second_name = "mfhi";
        char** second_args = malloc(sizeof(char*));
        second_args[0] = args[0];
        
        write_inst_string(output,first_name,first_args,2);
        write_inst_string(output,second_name,second_args,1);
        
        free(first_args);
        free(second_args);
        
        return 2;  
    } else if (strcmp(name, "subu") == 0) {
        /* YOUR CODE HERE */
        if(!output || !name || !args || num_args != 3){return 0;}
        char* first_name = "addiu";
        char** first_args = malloc(3*sizeof(char*));
        first_args[0] = "$at";
        first_args[1] = "$0";
        long int temp = -1;
        char imm[100];
        sprintf(imm,"%ld",temp);
        first_args[2] = imm;
        
        char* second_name = "xor";
        char** second_args = malloc(3*sizeof(char*));
        second_args[0] = "$at";
        second_args[1] = "$at";
        second_args[2] = args[2];
        
        char* third_name = "addiu";
        char** third_args = malloc(3*sizeof(char*));
        third_args[0] = "$at";
        third_args[1] = "$at";
        long int temp1 = 1;
        char imm1[100];
        sprintf(imm1,"%ld",temp1);
        third_args[2] = imm1;
        
        char* fouth_name = "addu";
        char** fouth_args = malloc(3*sizeof(char*));
        fouth_args[0] = args[0];
        fouth_args[1] = args[1];
        fouth_args[2] = args[2];
        
        write_inst_string(output,first_name,first_args,3);
        write_inst_string(output,second_name,second_args,3);
        write_inst_string(output,third_name,third_args,3);
        write_inst_string(output,fouth_name,fouth_args,3);
        
        free(first_args);
        free(second_args);
        free(third_args);
        free(fouth_args);
        
        return 4;
    }
    write_inst_string(output, name, args, num_args);
    return 1;

}

/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.
   
   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS. 

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step. If a symbol should be relocated, it should be added to the
   relocation table (RELTBL), and the fields for that symbol should be set to
   all zeros. 

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write 
   anything to OUTPUT but simply return -1. MARS may be a useful resource for
   this step.

   Some function declarations for the write_*() functions are provided in translate.h, and you MUST implement these
   and use these as function headers. You may use helper functions, but you still must implement
   the provided write_* functions declared in translate.h.

   Returns 0 on success and -1 on error. 
 */
int translate_inst(FILE* output, const char* name, char** args, size_t num_args, uint32_t addr,
    SymbolTable* symtbl, SymbolTable* reltbl) {
    if (strcmp(name, "addu") == 0)       return write_rtype      (0x21, output, args, num_args);
    else if (strcmp(name, "or") == 0)    return write_rtype      (0x25, output, args, num_args);
    else if (strcmp(name, "xor") == 0)   return write_rtype      (0x26, output, args, num_args);
    else if (strcmp(name, "slt") == 0)   return write_rtype      (0x2a, output, args, num_args);
    else if (strcmp(name, "sltu") == 0)  return write_rtype      (0x2b, output, args, num_args);
    else if (strcmp(name, "sll") == 0)   return write_shift      (0x00, output, args, num_args);
    /* YOUR CODE HERE */
    else if (strcmp(name,"lb") == 0)     return write_mem        (0x20, output, args, num_args);
    else if (strcmp(name,"lbu") == 0)    return write_mem        (0x24, output, args, num_args);
    else if (strcmp(name, "sb") == 0)    return write_mem        (0x28, output, args, num_args); 
    else if (strcmp(name, "lw") == 0)    return write_mem        (0x23, output, args, num_args);
    else if (strcmp(name, "sw") == 0)    return write_mem        (0x2b, output, args, num_args);
    else if (strcmp(name, "j") == 0)     return write_jump       (0x02, output, args, num_args, addr, reltbl);
    else if (strcmp(name, "jal") == 0)   return write_jump       (0x03, output, args, num_args, addr, reltbl);
    else if (strcmp(name, "beq") == 0)   return write_branch     (0x04, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "bne") == 0)   return write_branch     (0x05, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "mult") == 0)  return write_mult_div   (0x18, output, args, num_args);
    else if (strcmp(name, "div") == 0)   return write_mult_div   (0x1a, output, args, num_args);
    else if (strcmp(name, "mfhi") == 0)  return write_mfhi_mflo  (0x10, output, args, num_args);
    else if (strcmp(name, "mflo") == 0)  return write_mfhi_mflo  (0x12, output, args, num_args);
    else if (strcmp(name, "jr") == 0)    return write_jr         (0x08, output, args, num_args);
    else if (strcmp(name, "addiu") == 0) return write_addiu      (0x09, output, args, num_args);
    else if (strcmp(name, "ori") == 0)   return write_ori        (0x0d, output, args, num_args);
    else if (strcmp(name, "lui") == 0)   return write_lui        (0x0f, output, args, num_args);
    else                                 return -1;
}

/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_hex() to write to 
   OUTPUT. Both are defined in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_rtype(uint8_t funct, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if(!output || !args || num_args!=3) return -1;
    
    
    int rd = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int rt = translate_reg(args[2]);
    
    if(rd || rs || rt) return -1;
    uint32_t instruction = 0;
    instruction += (rs<<21);
    instruction += (rt<<16);
    instruction += (rd<<11);
    instruction += funct;
    

    write_inst_hex(output, instruction);
    return 0;
}

/* A helper function for writing shift instructions. You should use 
   translate_num() to parse numerical arguments. translate_num() is defined
   in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_shift(uint8_t funct, FILE* output, char** args, size_t num_args) {
	// Perhaps perform some error checking?
    if (!output || !args || num_args != 3) {
      return -1;
    }
    
    long int shamt;
    int rd = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    int err = translate_num(&shamt, args[2], 0, 31);
    if(rd||rt||err) return -1;  //(rd,rt,err are -1 if failed)

    uint32_t instruction = 0;
    instruction += (rd<<11);
    instruction += (rt<<16);
    instruction += (shamt<<6);
    instruction += funct;
    write_inst_hex(output, instruction);
    return 0;
}

/* The rest of your write_*() functions below */

int write_jr(uint8_t funct, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (!output || !args || num_args != 1) {   // jr should have only 1 argument
      return -1;
    }
    
    int rs = translate_reg(args[0]);
    if(rs) return -1;
    

    uint32_t instruction = 0;
    instruction += (rs<<21);
    instruction += funct;
    write_inst_hex(output, instruction);
    return 0;
}

int write_addiu(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (!output || !args || num_args != 3) {
      return -1;
    }
    
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], INT16_MIN, INT16_MAX);
    if(rt||rs||err) return -1;
    imm = imm & 0x0000ffff;  // take the leading 1s off

    

    uint32_t instruction = 0;
    instruction += (opcode<<26);
    instruction += (rs<<21);
    instruction += (rt<<16);
    instruction += imm;
    write_inst_hex(output, instruction);
    return 0;
}

int write_ori(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (!output || !args || num_args != 3) {
      return -1;
    }
    
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], 0, UINT16_MAX);
    if(rt||rs||err) return -1;
    
    imm = imm & 0x0000ffff;
    

    uint32_t instruction = 0;
    instruction += (opcode<<26);
    instruction += (rs<<21);
    instruction += (rt<<16);
    instruction += imm;
    write_inst_hex(output, instruction);
    return 0;
}

int write_mult_div(uint8_t funct, FILE* output, char** args, size_t num_args) {
    	// Perhaps perform some error checking?
    if(!output || !args || num_args!=2) return -1;
	int rs = translate_reg(args[0]);
	int rt = translate_reg(args[1]);
		        
	if (rs < 0 || rt < 0) {
		return -1;
	}
			    
	uint32_t instruction = 0;
    instruction += (rs<<21);
    instruction += (rt<<16);
    instruction += funct;
	write_inst_hex(output, instruction);
	return 0;
}

int write_mfhi_mflo(uint8_t funct, FILE* output, char** args, size_t num_args) {
    	// Perhaps perform some error checking?
    if(!output || !args || num_args!=1) return -1;
	int rd = translate_reg(args[0]);
	if (rd < 0) {
		return -1;
	}

	uint32_t instruction = 0;
    instruction += (rd<<11);
    instruction += funct;
	write_inst_hex(output, instruction);
	return 0;
}

int write_lui(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if (!output || !args || num_args != 2) {
      return -1;
    }
    
    long int imm;
    int rt = translate_reg(args[0]);
    int err = translate_num(&imm, args[1], 0, UINT16_MAX);
    if(rt||err) return -1;
    
    imm = imm & 0x0000ffff;

    uint32_t instruction = 0;
    instruction+=(opcode<<26);
    instruction+=(rt<<16);
    instruction+=imm;
    write_inst_hex(output, instruction);
    return 0;
}

int write_mem(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    if(!output || !args || num_args!=3) return -1;
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[2]);
    int err = translate_num(&imm, args[1], INT16_MIN, INT16_MAX);
    if(rt||rs||err) return -1;
    
    imm = imm & 0x0000ffff;

    uint32_t instruction =0;
    instruction += (rs<<21);
    instruction += (rt<<16);
    instruction += opcode;
    write_inst_hex(output, instruction);
    return 0;
}

/*  A helper function to determine if a destination address
    can be branched to
*/
static int can_branch_to(uint32_t src_addr, uint32_t dest_addr) {
    int32_t diff = dest_addr - src_addr;
    return (diff >= 0 && diff <= TWO_POW_SEVENTEEN) || (diff < 0 && diff >= -(TWO_POW_SEVENTEEN - 4));
}


int write_branch(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* symtbl) {
    // Perhaps perform some error checking?
    if(!output||!args||!symtbl|| num_args!=3) return -1;
    
    int rs = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    int label_addr = get_addr_for_symbol(symtbl, args[2]);
    
    if(rs||rt||label_addr|| !can_branch_to(addr,label_addr)) return -1;

    //Please compute the branch offset using the MIPS rules.
    int32_t offset = (int32_t) ((label_addr-(addr+4))>>2);
    offset = offset & 0x0000ffff;
    uint32_t instruction = 0;
    instruction += (opcode<<26);
    instruction += (rs<<21);
    instruction += (rt<<16);
    instruction += offset;
    write_inst_hex(output, instruction);        
    return 0;
}

int write_jump(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* reltbl) {
    /* YOUR CODE HERE */
    if(!output || !args || reltbl || num_args!=1) return -1;
    char* name = args[0];
    int temp = add_to_table(reltbl, name, addr);  // does it matter if we use args[0] directly?
    if(temp==-1) return -1;
    
    uint32_t instruction = 0;
    instruction+=(opcode<<26);
    write_inst_hex(output, instruction);
    return 0;
}

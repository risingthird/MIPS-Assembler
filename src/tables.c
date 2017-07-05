
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "tables.h"

const int SYMTBL_NON_UNIQUE = 0;
const int SYMTBL_UNIQUE_NAME = 1;

#define INITIAL_SIZE 5
#define SCALING_FACTOR 2

/*******************************
 * Helper Functions
 *******************************/

void allocation_failed() {
    write_to_log("Error: allocation failed\n");
    exit(1);
}

void addr_alignment_incorrect() {
    write_to_log("Error: address is not a multiple of 4.\n");
}

void name_already_exists(const char* name) {
    write_to_log("Error: name '%s' already exists in table.\n", name);
}

void write_symbol(FILE* output, uint32_t addr, const char* name) {
    fprintf(output, "%u\t%s\n", addr, name);
}

/*******************************
 * Symbol Table Functions
 *******************************/

/* Creates a new SymbolTable containg 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time. 
   If memory allocation fails, you should call allocation_failed(). 
   Mode will be either SYMTBL_NON_UNIQUE or SYMTBL_UNIQUE_NAME. You will need
   to store this value for use during add_to_table().
 */
SymbolTable* create_table(int mode) {
    /* YOUR CODE HERE */
    SymbolTable* newTable = malloc(sizeof(SymbolTable));
    if(!newTable) allocation_failed();
    newTable->tbl = malloc(sizeof(Symbol)*((int)INITIAL_SIZE));
    // set attributes in TABLE
    if(!(newTable->tbl)) {free(newTable);allocation_failed();}
    newTable->len=0;
    newTable->cap=(int) INITIAL_SIZE;
    newTable->mode=mode;
    
    return newTable;
    
}

/* Frees the given SymbolTable and all associated memory. */
void free_table(SymbolTable* table) {
    /* YOUR CODE HERE */
    for (int i = table->len; i > 0; i--) {  // names are allocated in the create_copy function, so also need to be freed
      free(table->tbl[i-1].name);
    }
    free(table->tbl);
    free(table);
}

/* A suggested helper function for copying the contents of a string. */
static char* create_copy_of_str(const char* str) {
    size_t len = strlen(str) + 1;
    char *buf = (char *) malloc(len);
    if (!buf) {
       allocation_failed();
    }
    strncpy(buf, str, len); 
    return buf;
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE. 
   ADDR is given as the byte offset from the first instruction. The SymbolTable
   must be able to resize itself as more elements are added. 

   Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.

   If ADDR is not word-aligned, you should call addr_alignment_incorrect() and
   return -1. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists 
   in the table, you should call name_already_exists() and return -1. If memory
   allocation fails, you should call allocation_failed(). 

   Otherwise, you should store the symbol name and address and return 0.
 */
int add_to_table(SymbolTable* table, const char* name, uint32_t addr) {
    /* YOUR CODE HERE */
    if(addr%4) {addr_alignment_incorrect();return -1;}  // test whether the addr is word-aligned
    if(table->mode && (get_addr_for_symbol(table, name)!=-1)){  // test whether name exists
        name_already_exists(name);
        return -1;
    }
    if(table->len == table->cap){
        table->tbl = realloc(table->tbl,(table->cap)*SCALING_FACTOR*sizeof(Symbol));
        if(!table->tbl) allocation_failed();
    }
    table->tbl[table->len].name = create_copy_of_str(name);  // table is 0-indexed
    table->tbl[table->len].addr = addr;
    table->len = table->len + 1;
    return 0;
}

/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable* table, const char* name) {
    /* YOUR CODE HERE */
    if(table){
        int i =0;
        int size = table->len;
        while(i<size){
            if(!strcmp(table->tbl[i].name, name)){    // table->tbl[i] is similar with *((table->tbl)+i)
                return table->tbl[i].addr;
            }
            i++;
        }
    }
    return -1;   
}

/* Writes the SymbolTable TABLE to OUTPUT. You should use write_symbol() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable* table, FILE* output) {
    /* YOUR CODE HERE */
    for(int i =0;i<table->len;i++){
        write_symbol(output, table->tbl[i].addr, table->tbl[i].name);
    }
}

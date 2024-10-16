#pragma once
#include <stdint.h>
#include <stdbool.h>

// Memoria fisica
//Tamaños (en bytes) y estructuras
#define SIZE_PCB_TABLE 8192                              // equivalente a 8KB     
#define SIZE_BITMAP_PAGE_TABLE 128                       // son 128B
#define SIZE_SECOND_PAGE_TABLE 131072                    // equivalente a 128 KB
#define SIZE_FRAME_BITMAP 8192                           // equivalente a 8KB 
#define SIZE_FRAME 32768                                 // equivalente a (32 KB)
#define NUM_FRAMES (2 * 1024 * 1024 * 1024 / SIZE_FRAME) // 2^16 frames (2 GB dividido por el tamanho de cada frame (32 KB))

// Memoria virtual
//Tamaños (en bytes) y estructuras
#define SIZE_VIRTUAL_MEM (128 * 1024 * 1024)             // 128 MB de memoria para procesos
#define PAGE_SIZE 32768                                  // paginas de (32 KB)
#define NUM_PCBS 32                                      // 32 entradas PCB
#define MAX_FILES 5                                      // Número archivos por proceso
#define FILE_ENTRY_SIZE 23                               // Tamaño de una entrada en la tabla de archivos 23B
#define PCB_ENTRY_SIZE 256                               // Tamaño de cada entrada de PCB
#define BITMAP_SIZE 128                                  // Tamaño del Bitmap (128 B o 1024 bits)

// Estructura Tabla de Archivos
typedef struct {
    uint8_t valid;                // 0x01 válido, 0x00 inválido
    char filename[14];            
    uint32_t filesize;           
    uint32_t virtual_address;    
} FileEntry;

// Estructura de una entrada de PCB
typedef struct {
    uint8_t estado;               // 0x01 ejecutando, 0x00 inactivo
    uint8_t pid;                 
    char process_name[11];        
    FileEntry file_table[MAX_FILES]; 
    uint16_t first_level_page_table[64]; 
} PCB;

// Estructura para la memoria
typedef struct {
    uint8_t *memory;;
    PCB pcb_table[NUM_PCBS];      
    uint8_t bitmap_page_tables[BITMAP_SIZE];
    uint8_t second_page_tables[SIZE_SECOND_PAGE_TABLE];
    uint8_t frame_bitmap[SIZE_FRAME_BITMAP]; 
    uint32_t num_frames;  
} OSRMSMemory;

void initialize_osrms_memory(OSRMSMemory *memory);
int allocate_process(OSRMSMemory *memory, uint8_t pid, const char *process_name);
void free_process(OSRMSMemory *memory, uint8_t pid);
uint32_t translate_virtual_address(OSRMSMemory *memory, uint8_t pid, uint32_t virtual_address);
void allocate_file(OSRMSMemory *memory, uint8_t pid, const char *filename, uint32_t size);
void deallocate_file(OSRMSMemory *memory, uint8_t pid, const char *filename);

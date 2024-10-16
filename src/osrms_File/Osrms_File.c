#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Osrms_File.h"

// Función para inicializar la memoria OSRMS
void initialize_osrms_memory(OSRMSMemory *memory) {
    memory->memory = (uint8_t *)malloc(SIZE_PCB_TABLE + SIZE_BITMAP_PAGE_TABLE + SIZE_SECOND_PAGE_TABLE + SIZE_FRAME_BITMAP + NUM_FRAMES * SIZE_FRAME);
    //Aqui considere que el dir es el numero de frames * el tamanho de cada frame (relativa a los ultimos 2 Gb)
    memset(memory->memory, 0, SIZE_VIRTUAL_MEM);
    memset(memory->pcb_table, 0, sizeof(memory->pcb_table));
    memset(memory->bitmap_page_tables, 0, BITMAP_SIZE);
    memset(memory->second_page_tables, 0, SIZE_SECOND_PAGE_TABLE);
    memset(memory->frame_bitmap, 0, SIZE_FRAME_BITMAP);
    memory->num_frames = NUM_FRAMES;
}

int allocate_process(OSRMSMemory *memory, uint8_t pid, const char *process_name) {
    // Buscar un PCB disponible
    for (int i = 0; i < NUM_PCBS; i++) {
        if (memory->pcb_table[i].estado == 0x00) { // 0x00: inactivo
            // Asignar el proceso
            memory->pcb_table[i].estado = 0x01;  // 0x01: en ejecución
            memory->pcb_table[i].pid = pid;
            strncpy(memory->pcb_table[i].process_name, process_name, 10);
            memory->pcb_table[i].process_name[10] = '\0'; 
            // Inicializando valores
            memset(memory->pcb_table[i].file_table, 0, sizeof(memory->pcb_table[i].file_table));
            memset(memory->pcb_table[i].first_level_page_table, 0, sizeof(memory->pcb_table[i].first_level_page_table));
            return 0; 
        }
    }
    return -1;
}


void free_process(OSRMSMemory *memory, uint8_t pid) {
    // Buscar el proceso correspondiente por PID
    PCB *pcb = NULL;
    for (int i = 0; i < NUM_PCBS; i++) {
        if (memory->pcb_table[i].pid == pid) {
            pcb = &memory->pcb_table[i];
            break;
        }
    }

    if (pcb == NULL) {
        return; 
    }

    pcb->estado =  0x00; // 0x00: inactivo

    // Liberar todos los archivos asignados al proceso
    for (int i = 0; i < MAX_FILES; i++) {
        if (pcb->file_table[i].valid ==  0x01) {
            printf("Liberando archivo '%s' del proceso PID %d.\n", pcb->file_table[i].filename, pid);
            pcb->file_table[i].valid =  0x00;  // Marcar archivo como inválido
        }
    }

    memset(pcb->first_level_page_table, 0, sizeof(pcb->first_level_page_table));
}


uint32_t translate_virtual_address(OSRMSMemory *memory, uint8_t pid, uint32_t virtual_address) {
    PCB *pcb = NULL;
    for (int i = 0; i < NUM_PCBS; i++) {
        if (memory->pcb_table[i].pid == pid) {
            pcb = &memory->pcb_table[i];
            break;
        }
    }

    if (pcb == NULL) return -1; // No se encontro el proceso, entonces retorna

    // Obtener los primeros 6 bits del VPN para acceder 
    uint16_t vpn_first_level = (virtual_address >> 21) & 0x3F;
    uint16_t second_page_table_index = pcb->first_level_page_table[vpn_first_level];

    if (second_page_table_index == 0) return -1;  // Entrada no válida

    // Obtener los siguientes 6 bits del VPN para acceder
    uint16_t vpn_second_level = (virtual_address >> 15) & 0x3F;
    uint16_t pfn = memory->second_page_tables[second_page_table_index * 64 + vpn_second_level];

    uint32_t offset = virtual_address & 0x7FFF;

    // Dirección física final
    return (pfn * SIZE_FRAME) + offset;
}



void allocate_file(OSRMSMemory *memory, uint8_t pid, const char *filename, uint32_t size) {
    // Buscar el proceso correspondiente por PID
    PCB *pcb = NULL;
    for (int i = 0; i < NUM_PCBS; i++) {
        if (memory->pcb_table[i].pid == pid) {
            pcb = &memory->pcb_table[i];
            break;
        }
    }

    if (pcb == NULL) {
        return;
    }

    for (int i = 0; i < MAX_FILES; i++) {
        if (pcb->file_table[i].valid == 0x00) { 
            pcb->file_table[i].valid = 0x01;   
            strncpy(pcb->file_table[i].filename, filename, 14);
            pcb->file_table[i].filename[14] = '\0';
            pcb->file_table[i].filesize = size;

            pcb->file_table[i].virtual_address = 0x00000 + (i * PAGE_SIZE); 

            printf("Archivo '%s' asignado a proceso PID %d.\n", filename, pid);
            return;
        }
    }
}

void deallocate_file(OSRMSMemory *memory, uint8_t pid, const char *filename) {
    // Buscar el proceso correspondiente por PID
    PCB *pcb = NULL;
    for (int i = 0; i < NUM_PCBS; i++) {
        if (memory->pcb_table[i].pid == pid) {
            pcb = &memory->pcb_table[i];
            break;
        }
    }

    if (pcb == NULL) {
        return;
    }

    for (int i = 0; i < MAX_FILES; i++) {
        if (pcb->file_table[i].valid == 0x01 && strcmp(pcb->file_table[i].filename, filename) == 0) {
            pcb->file_table[i].valid = 0x00;
            memset(pcb->file_table[i].filename, 0, sizeof(pcb->file_table[i].filename));
            pcb->file_table[i].filesize = 0;
            pcb->file_table[i].virtual_address = 0;

            printf("Archivo '%s' liberado del proceso PID %d.\n", filename, pid);
            return;
        }
    }
}
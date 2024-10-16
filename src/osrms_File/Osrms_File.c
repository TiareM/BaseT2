#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Osrms_File.h"

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
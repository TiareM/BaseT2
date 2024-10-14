#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"

FILE* m_path;

void os_mount(char* memory_path) {
    m_path = fopen(memory_path, "rb+");
}

void os_ls_processes() {
    //  Arreglo donde se guardaran los bytes de cada entrada
    unsigned char pcb_entry[256];

    // Se ubica en la posicion 0 del archivo
    fseek(m_path, 0, SEEK_SET);

    for (int i = 0; i < 32; i++) {
        // Se lee una entrada
        fread(pcb_entry, 1, 256, m_path);

        // Se lee el estado del proceso
        unsigned char state = pcb_entry[0];

        // Se revisa si el proceso esta en ejecucion
        if(state == 1) {
            // Se lee el id del proceso
            unsigned char id = pcb_entry[1];

            // Se lee el nombre del proceso
            char name[12];
            strncpy(name, (char*)&pcb_entry[2], 11);
            name[11] = '\0'; 


            printf("ID: %d\n", id);
            printf("Name: %s\n\n", name);
        }
    }
}

int os_exists(int process_id, char* file_name) {
    //  Arreglo donde se guardaran los bytes de cada entrada
    unsigned char pcb_entry[256];

    // Se ubica en la posicion 0 del archivo
    fseek(m_path, 0, SEEK_SET);

    for (int i = 0; i < 32; i++) {
        // Se lee una entrada de 256 bytes
        fread(pcb_entry, 1, 256, m_path);

        // Se lee el estado y id del proceso
        unsigned char state = pcb_entry[0];
        unsigned char id = pcb_entry[1];

        // Se revisa si el proceso contiene al archivo
        if(state == 1 && id == process_id) {
            // Se obtiene la direccion desde donde empieza la tabla de archivos
            unsigned char* file_table = &pcb_entry[13];

            for(int j = 0; j < 5; j++) {
                // Se lee la entrada de cada archivo
                unsigned char* file_entry = &file_table[j * 23];

                // Se lee la validez del archivo
                unsigned char file_validity = file_entry[0];

                if (file_validity == 1) {
                    // Se lee el nombre del archivo
                    char f_name[15];
                    strncpy(f_name, (char*)&file_entry[1], 14);
                    f_name[14] = '\0';

                    if (strncmp(f_name, file_name, 14) == 0) {
                        // Se encontro el archivo
                        return 1;
                    }
                }
            }
            return 0;
        }
    }
    return 0;
}

void os_ls_files(int process_id) {
    //  Arreglo donde se guardaran los bytes de cada entrada
    unsigned char pcb_entry[256];

    // Se ubica en la posicion 0 del archivo
    fseek(m_path, 0, SEEK_SET);

    for (int i = 0; i < 32; i++) {
        // Se lee una entrada de 256 bytes
        fread(pcb_entry, 1, 256, m_path);

        // Se lee el estado y id del proceso
        unsigned char state = pcb_entry[0];
        unsigned char id = pcb_entry[1];

        // Se revisa si el proceso esta en ejecucion
        if(state == 1 && id == process_id) {
            // Se obtiene la direccion desde donde empieza la tabla de archivos
            unsigned char* file_table = &pcb_entry[13];

            for(int j = 0; j < 5; j++) {
                // Se lee la entrada de cada archivo
                unsigned char* file_entry = &file_table[j * 23];

                // Se lee la validez del archivo
                unsigned char file_validity = file_entry[0];

                if (file_validity == 1) {
                    // Se lee el nombre del archivo
                    char file_name[15];
                    strncpy(file_name, (char*)&file_entry[1], 14);
                    file_name[14] = '\0';

                    // Convierte los siguientes 4 bytes en un int
                    unsigned int file_size = *(unsigned int*)&file_entry[15];

                    printf("Name: %s\n", file_name);
                    printf("Size: %d bytes\n\n", file_size);
                }
            }
        }
    }
}

void os_frame_bitmap() {
    unsigned char frame_bitmap_bytes[8192];  
    fseek(m_path, 139392, SEEK_SET);
    fread(frame_bitmap_bytes, 1, 8192, m_path);
    int occupied_frames = 0;
    int free_frames = 0;

    for(int i = 0; i < 8192; i++) {
        printf("byte %d: ", i);

        for(int j = 0; j < 8; j++) {
            unsigned char bit = (frame_bitmap_bytes[i] >> j) & 1;
            printf("bit %d: %d  -  ", j, bit);
            
            if (bit == 1) {
                occupied_frames++;
            } else {
                free_frames++;
            }
        }
        printf("\n");
    }
    printf("\nOccupied frames: %d\nFree frames: %d\n\n", occupied_frames, free_frames);
}

void os_tp_bitmap() {
    unsigned char tp_bitmap_bytes[128];  
    fseek(m_path, 8192, SEEK_SET);
    fread(tp_bitmap_bytes, 1, 128, m_path);
    int occupied_frames = 0;
    int free_frames = 0;

    for(int i = 0; i < 128; i++) {
        printf("byte %d: ", i);
        
        for(int j = 0; j < 8; j++) {
            unsigned char bit = (tp_bitmap_bytes[i] >> j) & 1;
            printf("bit %d: %d  -  ", j, bit);
            
            if (bit == 1) {
                occupied_frames++;
            } else {
                free_frames++;
            }
        }
        printf("\n");
    }
    printf("\nOccupied frames: %d\nFree frames: %d\n\n", occupied_frames, free_frames);
}

void os_start_process(int process_id, char* process_name) {
    
}
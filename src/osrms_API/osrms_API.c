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

    // Leer los 8192 bytes del frame bitmap
    fread(frame_bitmap_bytes, 1, 8192, m_path);
    int occupied_frames = 0;
    int free_frames = 0;

    for(int i = 0; i < 8192; i++) {
        printf("byte %d: ", i);

        for(int j = 0; j < 8; j++) {
            // Extrae el bit j del byte en frame_bitmap_bytes[i]
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

    // Leer los 128 bytes del bitmap de tabla de paginas
    fread(tp_bitmap_bytes, 1, 128, m_path);

    int occupied_frames = 0;
    int free_frames = 0;

    for(int i = 0; i < 128; i++) {
        printf("byte %d: ", i);
        
        for(int j = 0; j < 8; j++) {
            // Extrae el bit j del byte en frame_bitmap_bytes[i]
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
    unsigned char pcb_entry[256];
    fseek(m_path, 0, SEEK_SET);

    for (int i = 0; i < 32; i++) {
        fread(pcb_entry, 1, 256, m_path);
        unsigned char state = pcb_entry[0];

        if(state == 0) {
            // Se ingresan los datos del proceso nuevo
            pcb_entry[0] = 1;
            pcb_entry[1] = (unsigned char) process_id;
            strncpy((char*)&pcb_entry[2], process_name, 11);
            for (int j = strlen(process_name); j < 11; j++) {
                pcb_entry[2 + j] = 0x00;
            }

            // Se copia el numero 0 en todo el espacio de la tabla de archivos y la tabla de paginas de primer orden
            memset(&pcb_entry[13], 0, 115);
            memset(&pcb_entry[128], 0, 128);

            // Se ubica en la posicion (num de entrada * tamaño de entrada) y se reescribe la entrada con los datos nuevos
            fseek(m_path, i*256, SEEK_SET);
            fwrite(pcb_entry, 1, 256, m_path);
            break;
        }
    }
}

void free_memory_process(int entry, uint32_t  virtual_address) {
    unsigned char pcb_entry[256];
    fseek(m_path, 0, SEEK_SET);
    fseek(m_path, entry * 256, SEEK_SET);
    fread(pcb_entry, 1, 256, m_path);

    // Se calcula el vpn y el offset de la direccion virtual
    unsigned int vpn = (virtual_address >> 15) & 12;
    // unsigned int offset = virtual_address & 15;

    // Se obtienen los primeros 6 bits del vpn para la tabla de páginas de primer orden
    uint8_t vpn1 = (vpn >> 6) & 0x3F;
    // Se obtienen los ultimos 6 bits del vpn para la tabla de paginas de segundo Orden
    uint8_t vpn2 = vpn & 0x3F;

    unsigned int first_table_entry[128];      // Tabla de paginas de primer orded
    unsigned int second_table_page[128];    // Tabla de paginas de segundo orden
    unsigned char page_table_bitmap[128];   // Bitmap de tablas de paginas
    unsigned char frame_bitmap[8192];

    memcpy(first_table_entry, &pcb_entry[128], 128);

    // Se obtiene el frame bitmap
    fseek(m_path, 139392, SEEK_SET);
    fread(frame_bitmap, 1, 8192, m_path); 

    // Se obtiene el bitmap de tablas de paginas
    fseek(m_path, 8192, SEEK_SET);
    fread(page_table_bitmap, 1, 128, m_path); 

    // // Se obtiene la tabla de paginas de primer orden del proceso
    // fseek(m_path, entry * 256 + 128, SEEK_SET);
    // fread(first_table_entry, 1, 128, m_path);

    // Se forma el numero que entrega la tabla de primer orden
    uint16_t n_first_entry = (first_table_entry[vpn1 * 2] << 8) | first_table_entry[vpn1 * 2 + 1];

    // Cambia el bit n_first entry por un 0
    page_table_bitmap[n_first_entry / 8] &= ~(1 << (n_first_entry % 8));
    fseek(m_path, 8192, SEEK_SET);
    fwrite(page_table_bitmap, 1, 128, m_path);

    // Se obtiene la tabla de paginas de segundo orden a partir del numero obtenido antes
    fseek(m_path, 8320 + n_first_entry * 128, SEEK_SET);
    fread(second_table_page, 1, 128, m_path);

    // Se forma el numero que entrega la tabla de segundo orden
    uint16_t pfn = (second_table_page[vpn2* 2] << 8) | second_table_page[vpn2 * 2 + 1];

    // Cambia el bit igual al pfn por un 0 en el frame bitmap
    frame_bitmap[pfn / 8] &= ~(1 << (pfn % 8));
    fseek(m_path, 139392, SEEK_SET);
    fwrite(frame_bitmap, 1, 8192, m_path);

    // Se calcula la direccion fisica a partir de l pfn y el offset 
    // uint32_t direccion_fisica = (pfn << 15) | offset;   
}

void os_finish_process(int process_id) {
    unsigned char pcb_entry[256];
    fseek(m_path, 0, SEEK_SET);

    for (int i = 0; i < 32; i++) {
        // Se obtien el estado y el id del proceso
        fread(pcb_entry, 1, 256, m_path);
        unsigned char state = pcb_entry[0];
        unsigned char id = pcb_entry[1];

        if(state == 1 && id == process_id) {
            // Se invalida la entrada del proceso
            pcb_entry[0] = 0;

            // Se copian los cambios a la memoria
            fseek(m_path, i*256, SEEK_SET);
            fwrite(pcb_entry, 1, 256, m_path);

            // Se obtiene la direccion desde donde empieza la tabla de archivos
            unsigned char* file_table = &pcb_entry[13];

            for(int j = 0; j < 5; j++) {
                // Se obtiene la entrada de cada archivo y se lee su validez
                unsigned char* file_entry = &file_table[j * 23];
                unsigned char file_validity = file_entry[0];

                if (file_validity == 1) {
                    // Se invalida el archivo
                    file_entry[0] = 0;

                    uint32_t  virtual_address = (file_entry[19] << 24) | (file_entry[20] << 16) | (file_entry[21] << 8) | file_entry[22];

                    free_memory_process(i, virtual_address);

                    // Se copian los cambios a la memoria
                    fseek(m_path, i*256, SEEK_SET);
                    fwrite(pcb_entry, 1, 256, m_path);
                }
            }
        }
    }
}

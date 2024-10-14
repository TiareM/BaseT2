#pragma once
#include "../osrms_File/Osrms_File.h"
#include <stdio.h>

extern FILE* m_path;

// Funciones generales
void os_mount(char* memory_path);
void os_ls_processes();
int os_exists(int process_id, char* file_name);
void os_ls_files(int process_id);
void os_frame_bitmap();
void os_tp_bitmap();

// Funciones para procesos
void os_start_process(int process_id, char* process_name);

// Funciones para archivos
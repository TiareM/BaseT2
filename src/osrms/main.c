#include "../osrms_API/osrms_API.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    //montar la memoria
    os_mount((char *)argv[1]);

    //resto de instrucciones
    printf("os_ls_processes()\n");
    printf("-----------------\n");
    os_ls_processes();

    printf("os_exists(117, \"dino.jpg\")\n");
    printf("--------------------------\n");
    printf("Answer: %d\n\n", os_exists(117, "dino.jpg"));

    printf("os_exists(91, \"hello.jpg\")\n");
    printf("--------------------------\n");
    printf("Answer: %d\n\n", os_exists(91, "hello.jpg"));

    printf("os_ls_files(162)\n");
    printf("----------------\n");
    os_ls_files(162);

    printf("os_frame_bitmap()\n");
    printf("----------------\n");
    os_frame_bitmap();

    printf("os_tp_bitmap()\n");
    printf("--------------\n");
    os_tp_bitmap();

    printf("os_start_process(43, \"nuevo\")\n");
    printf("-----------------------------\n\n");
    os_start_process(43, "nuevo");
    printf("os_ls_processes()\n");
    printf("-----------------\n");
    os_ls_processes();

    printf("os_finish_process(162)\n");
    printf("----------------------\n\n");
    os_finish_process(162);

    fclose(m_path);
    return 0;

}
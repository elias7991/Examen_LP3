#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/wait.h>

void my_handler(int signum); //realizar cambio


int main(){

    for (int i= 1 ; i<= 64 ; i++){ //asignamos para escuchar todas las señales
    signal(i, my_handler); //se asigna la señal
    }
    
    pause(); //se espera una señal
    return 0;
}


void my_handler(int signum)
{
    
        printf("Received!\n"); //señal recibida
        
}

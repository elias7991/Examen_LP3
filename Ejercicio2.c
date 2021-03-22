#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>
typedef struct {int pid; int signal; short time;}lectura;
void sig_handler(int signum);

lectura regLect;

int main(){

    char archivoName [256] ;
    char a;
    int pid, pid_max;
    int max = 0;
    signal(SIGALRM,sig_handler);
    printf("Ingrese el nombre del documento a analizar\n");
    scanf("%s", archivoName) ;
    scanf("%c",&a); // captura el '\n'

    //abrir archivo
    FILE *fd=fopen(archivoName,"r");
	if(fd==NULL){
	    printf("ERROR, archivo no encontrado\n");
	    exit(1);
	}
	
	//leer archivo
    fscanf(fd,"%d %d %hd",&regLect.pid,&regLect.signal,&regLect.time);
    
    //crear hijo para la primera señal
    pid = fork();
    
    if( regLect.time > max) //guardar hijo con tiempo de espera mayor
        pid_max = pid;
       
    if(pid==0){ //el hijo crea la señal en x tiempo
        alarm(regLect.time);
    }
    if(pid >0) //solo el padre lee el archivo
    while(!feof(fd)){
        
        //procesarRegistros(regLect,&corte,dominios);
        //printf("%d %d %d\n",regLect.pid,regLect.signal,regLect.time);
        fscanf(fd,"%d %d %hd",&regLect.pid,&regLect.signal,&regLect.time);
        
        
        pid = fork(); //crear hijo para la n señal
        
        if( regLect.time > max) //guardar hijo con tiempo de espera mayor
            pid_max = pid;
            
        if(pid ==0){ //el hijo crea la señal en x tiempo
            alarm(regLect.time);
            break; //el hijo no sigue leyendo el archivo
        }
    }
    
    if(pid == 0) //el hijo se duerme esperando su tiempo para activar la alarma
        pause();
    else{
        waitpid(pid_max,NULL,0); //el padre espera al hijo que tardará más
        fclose(fp); //cierra el archivo
    }
    return 0;

}
void sig_handler(int signum)
{
    
        kill(regLect.pid, regLect.signal); //se envia la señal al proceso
        
    
    //printf("\nfasf\n");
}

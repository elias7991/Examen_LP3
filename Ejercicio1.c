#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <math.h>


typedef struct {
    int origen, destino, datoOrigen, datoDestino;
}cambio;

void * create_shared_memory(size_t size); //memoria compartida
void my_handler(int signum); //realizar cambio
void endChild(int signum); //fin de hijo

int * array;
int bucle = 1;
void* shmem;

int main(int argc, char *argv[])
{

    if (argc!=3) {
		printf("Debe pasar como argumento los nros a dividir\n");
		exit(0);
	}
	
    printf("--beginning of program\n");
    signal(SIGUSR1, my_handler);
    signal(SIGUSR2, endChild);
    
    //necesarios para memoria compartida
    cambio dat;
    cambio *datos = &dat;
    shmem = create_shared_memory(sizeof(cambio)); //crear memoria compartida
    memcpy(shmem, datos, sizeof(cambio) ); //cambiar datos de memoria compartida
    

    int counter = 0; //cantidad de elementos del array
    int *estado, *estado2;
    int l_inf ,l_sup , l_suph1 , l_infh2 = 0; //limites del array que puede tomar el padre y sus hijos
    char * elementos = argv[2]; //cadena que representa el array
    array = (int *)malloc( 0 ); //array vacio;
    int numero = 0; //numero a cargar en el array
    
    //Creacion de array
    
    for ( int i = 0 ; *(elementos + i)!= '\0' ; i++){ //hasta que no haya elementos por agregar
        
        
        if ( i == 0 ){ //si es el primer elemento
            counter++;
            numero=atoi ( (elementos + i) ); // convertir a numerico
            array = realloc(array, sizeof(int)*counter ); //agrandar array
            *(array + sizeof(int)*(counter -1)) = numero; //agregar elemento
        
        } else if( *(elementos + i - 1) == '\0' || *(elementos + i - 1) == ',' ){ //despues viene una ',' o fin de cadena
        
            counter++;
            numero=atoi ( (elementos + i) ); 
            array = realloc(array, sizeof(int)*counter );
            *(array + sizeof(int)*(counter -1)) = numero;
        
        }
    }
    printf("\n ");
    
    //imprimir array
    
    /*for(int i = 0; i<counter ; i++){
    
        printf("%d ", *(array + i*sizeof(int)) );
    
    }*/
    
    
    printf("\n");
    l_inf =0; //limite inferior
    l_sup = counter -1; //limite superior del array del proceso
    pid_t pid, pid_2; //hijo izquierdo, hijo derecho
    pid =0,pid_2=0;
    
    //Gestión de procesos
    for (double i=1, h=2; i<atof(argv[1]) ; ++h  ){
        i=pow(2,h);
        //División de array
        l_suph1 = (l_sup + l_inf)/2;
        l_infh2 = l_suph1 + 1;
        
        
        //printf("%.0lf %d \n",i, getpid());
        //sleep(1);
        
        // Crear un hijo
        pid = fork();
        
        //Si soy un padre, por crear el primer hijo, creo otro hijo
        if(pid >0){
            /*do{
                pause();
                printf("Desperté 1\n");
            }while(bucle);*/
            bucle =1;
            pid_2 = fork();
            //if( pid_2>0)
            /*do{
                pause();
                printf("Desperté 2\n");
            }while(bucle);*/
            
        }
            
        // Hijo 1
        if( pid == 0){
        
            l_sup = l_suph1;
            
            //printf("%d Actual1, %d Padre, %d inf, %d sup\n", getpid() , getppid(), l_inf, l_sup);
            
        }
        
        if (pid > 0) //No es hijo 1
        
            if (pid_2 ==0){ //Hijo 2
                
                l_inf = l_infh2;
                //printf("%d Actual2 %d Padre, %d inf, %d sup\n\n", getpid(), getppid(), l_inf, l_sup);
            
            }
        
        if (pid > 0 && pid_2 > 0){
            //printf("%d %d\n", pid, pid_2);
            break;
        }
	}
	
	
    
    if (pid == 0 || pid_2 ==0){
        // procesos finales, no esperan a nadie
        if( l_inf!=l_sup)
            for(int i = l_inf; i< l_sup ; i++){
            
                for(int j = i+1 ; j <= l_sup ; j++){
                
                    if( *(array +i*sizeof(int))>*(array +j*sizeof(int)) ){ //se realiza un cambio
                        //printf("%d ",*(array +i*sizeof(int) );
                        int aux = *(array +i*sizeof(int));
                        *(array +i*sizeof(int)) = *(array +j*sizeof(int));
                        *(array +j*sizeof(int)) = aux;
                        
                        
                        (((cambio*)shmem)->datoOrigen ) = *(array +j*sizeof(int)) ;
                        (((cambio*)shmem)->datoDestino ) = *(array +i*sizeof(int)) ;
                        (((cambio*)shmem)->origen ) = j ;
                        (((cambio*)shmem)->destino ) = i;
                        kill(getppid(), SIGUSR1); //informar al padre, cambio en el array
                        //sleep(1);
                
                    }
                    
                }
            
            }
        
        //sleep(3);
	    //printf("child process id: %d\n", getpid());
        //for(int i = l_inf; i<= l_sup ; i++)
	    //    printf("%d ", *(array+ i*sizeof(int)) );
	    //kill(getppid(), SIGUSR1);
	    kill(getppid(), SIGUSR2); //informar al padre fin de hijo
        
	    return 1;
    }
    else if (pid > 0)
    {
        // parent process
        //sleep(3);
	    //printf("parent process id: %d, wait to child: %d %d\n", getpid(), pid, pid_2);
        
        //waitpid(pid,estado,0);
        //waitpid(pid_2,estado2,0);
	    //printf("end parent, child status: %d\n", WEXITSTATUS(estado));
	    /*for(int i= l_inf; i<=l_suph1 ; i++){
	    
	        *(array+ i*sizeof(int)) = *(estado+ i*sizeof(int));
	    
	    }*/
	    //sleep(1);
	    if( l_inf!=l_sup)
            for(int i = l_inf; i< l_sup ; i++){
            
                for(int j = i+1 ; j <= l_sup ; j++){
                
                    if( *(array +i*sizeof(int))>*(array +j*sizeof(int)) ){ //se realiza un cambio
                        //printf("%d ",*(array +i*sizeof(int) );
                        int aux = *(array +i*sizeof(int));
                        *(array +i*sizeof(int)) = *(array +j*sizeof(int));
                        *(array +j*sizeof(int)) = aux;
                        
                        
                        (((cambio*)shmem)->datoOrigen ) = *(array +j*sizeof(int)) ;
                        (((cambio*)shmem)->datoDestino ) = *(array +i*sizeof(int)) ;
                        (((cambio*)shmem)->origen ) = j ;
                        (((cambio*)shmem)->destino ) = i;
                        if( l_inf!=0 || l_sup != counter -1)
                            kill(getppid(), SIGUSR1);
                        //sleep(1);
                
                    }
                    
                }
            
            }
        
        
	    //printf("child process id: %d\n", getpid());
        
	    if( l_inf!=0 || l_sup != counter -1){ //si no es de extremo a extremo, es decir, el array entero
	    
	        kill(getppid(), SIGUSR2); //informar a padre, fin de hijo
            //kill(getppid(), SIGUSR1);
        }else
            for(int i = l_inf; i<= l_sup ; i++)
	            printf("%d ", *(array+ i*sizeof(int)) );
	    return 1;
    }
    else{
    
        // fork failed
        printf("fork() failed!\n");
        return -1;
    }
}



void my_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        //printf("Received SIGUSR1!\n");
        //printf("\nfafasftsf\n");
        int aux = *(array +(((cambio*)shmem)->destino )*sizeof(int));
        *(array +(((cambio*)shmem)->destino )*sizeof(int)) = *(array +(((cambio*)shmem)->origen )*sizeof(int));
        *(array +(((cambio*)shmem)->origen )*sizeof(int)) = aux;
    }
    //printf("\nfasf\n");
}

void endChild(int signum)
{
    if (signum == SIGUSR2)
    {
        //printf("Received SIGUSR2!\n");
        bucle = 0;
        //kill(getpid(), SIGUSR1);
    }
}



void * create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}

/*
 --INFO IMPORTANTE--
 PARA COMPILAR CON MAC - clang 13012_laberinto.c -o laberinto -lpthread
 PARA ABRIR EL ARCHIVO LLAMARLO CON CUALQUIERA DE LOS DOS LABERINTOS QUE SOLUCIONA
 ./laberinto labyrinth.txt
 ./laberinto labyrinth_2.txt
 */

/* librerias usadas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

/* creamos estructura para posiciones */
struct posicion {
    int num_linea;
    int num_columna;
    int id_hilo;
    int id_hilo_padre;
    int pasos;
    char direccion;
};

/* posicion inicial y final y del recorrido*/
struct posicion *ini, *end;
struct posicion recorrido[1024];

/* inicializamos variable global de laberinto */
char laberinto[1024][1024];
/* filas y columnas totales del laberinto. ID del hilo creado, inicio y fin */
int filas, columnas, id = 0, casilla = 0, hilos_activos = 0, fin = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

/* funciones para moverse en horizontal y en vertical */
void *mov_horizontal(void *pos_rec);
void *mov_vertical(void *pos_rec);

/* funcion imprimir el laberinto */
void print_laberinto(){
    pthread_mutex_lock(&mutex1);
    printf("\n LABERINTO %d x %d\n",filas+1,columnas);
    for (int i=0; i<=filas;++i){
        printf("  ");
        for (int j=0; j<=columnas;++j){
            printf("%c", laberinto[i][j]);
        }
    }
    printf("\n");
    pthread_mutex_unlock(&mutex1);
}

/* moverse horizontalmente */
void *mov_horizontal(void *pos_rec)
{
    struct posicion *pos, *pos1, *pos2, *pos3;
    pthread_t hilo1, hilo2, hilo3;
    int i, j, x = 0, hilos_creados = 0;
    
    pos1 = malloc(sizeof(struct posicion));
    pos2 = malloc(sizeof(struct posicion));
    pos3 = malloc(sizeof(struct posicion));

    pos = pos_rec;
    pos->id_hilo_padre = pos->id_hilo;
    pos->id_hilo = ++id;
    pos->pasos = 0;
    
    if(pos->direccion == 'r'){x = 1;}
    else if(pos->direccion == 'l'){x = -1;}
    
    ++hilos_activos;
    
    /*printf ("Hilo %d creado por padre %d y empieza en (%d, %d)\n", pos->id_hilo, pos->id_hilo_padre, pos->num_linea, pos->num_columna);*/

    i = pos->num_linea;
    j = pos->num_columna;
    
    for (; j >= 0 && j < columnas; j+=x)
    {
        pos->num_linea = i;
        pos->num_columna = j;
        
        switch (laberinto[i][j])
        {
            case 'I' :
                break;
            case '#' :
                /*printf ("Hilo %d termina en (%d, %d) despues de %d pasos\n",pos->id_hilo, pos->num_linea, pos->num_columna, pos->pasos);*/
                --hilos_activos;
                return NULL;
                break;
            case 'O' :
                ++pos->pasos;
                *end = *pos;
                recorrido[casilla++] = *pos;
                fin = 1;
                /*printf ("Hilo %d termina en (%d, %d) despues de %d pasos\n",pos->id_hilo, pos->num_linea, pos->num_columna, pos->pasos);*/
                --hilos_activos;
                return NULL;
            case ' ' :
                /*laberinto[i][j] = '-';*/
                ++pos->pasos;
                recorrido[casilla++] = *pos;
                break;
            default :
                printf("formato de laberinto invalido\n");
                break;
        }
        
        switch (laberinto[i - 1][j])
        {
            case '#' :
                break;
            case 'O' :
                *end = *pos;
                end->num_linea = i - 1;
                recorrido[casilla++] = *pos;
                fin = 1;
            case ' ' :
                pos1->num_linea = i - 1;
                pos1->num_columna = j;
                pos1->id_hilo = pos->id_hilo;
                pos1->direccion = 'u';
                pthread_create (&hilo1, NULL, mov_vertical, (void*) pos1);
                ++hilos_creados;
                break;
            default :
                printf("formato de laberinto invalido\n");
                break;
        }
        
        switch (laberinto[i + 1][j])
        {
            case '#' :
                break;
            case 'O' :
                *end = *pos;
                end->num_linea = i + 1;
                recorrido[casilla++] = *pos;
                fin = 1;

            case ' ' :
                pos2->num_linea = i + 1;
                pos2->num_columna = j;
                pos2->id_hilo = pos->id_hilo;
                pos2->direccion = 'd';
                pthread_create (&hilo2, NULL, mov_vertical, (void*) pos2);
                ++hilos_creados;
                break;
            default :
                printf("formato de laberinto invalido\n");
                break;
        }
        
        if (hilos_creados == 2 && laberinto[i][j+x] == ' ')
        {
            pos3->num_linea = i;
            pos3->num_columna = j + x;
            pos3->id_hilo = pos->id_hilo;
            pos3->direccion = pos->direccion;
            pthread_create (&hilo3, NULL, mov_horizontal, (void*) pos3);
            /*printf ("Hilo %d termina en (%d, %d) despues de %d pasos\n",pos->id_hilo, pos->num_linea, pos->num_columna, pos->pasos);*/
            --hilos_activos;
            return NULL;
        }
        
    }
}

/*----------------------------------------------------------------------------------------------------------
        FUNCION PARA MOVERSE EN LA VERTICAL
-----------------------------------------------------------------------------------------------------------*/
void *mov_vertical(void *pos_rec){
    struct posicion *pos, *pos1, *pos2, *pos3;
    pthread_t hilo1, hilo2, hilo3;
    int i, j, x = 0, hilos_creados = 0;
    
    pos1 = malloc(sizeof(struct posicion));
    pos2 = malloc(sizeof(struct posicion));
    pos3 = malloc(sizeof(struct posicion));

    pos = pos_rec;
    
    pos->id_hilo_padre = pos->id_hilo;
    pos->id_hilo = ++id;
    pos->pasos = 0;
    
    if(pos->direccion == 'd'){x = 1;}
    else if(pos->direccion == 'u'){x = -1;}
    
    ++hilos_activos;
    
    /*printf ("Hilo %d creado por padre %d y empieza en (%d, %d)\n",pos->id_hilo, pos->id_hilo_padre, pos->num_linea, pos->num_columna);*/

    i = pos->num_linea;
    j = pos->num_columna;
    
    for (; i >= 0 && i <= filas; i+=x){
        
        pos->num_linea = i;
        pos->num_columna = j;
        
        switch (laberinto[i][j]){
            case 'I' :
                break;
            case '#' :
                /*printf ("Hilo %d termina en (%d, %d) despues de %d pasos\n",pos->id_hilo, pos->num_linea, pos->num_columna, pos->pasos);*/
                --hilos_activos;
                return NULL;
                break;
            case 'O' :
                ++pos->pasos;
                *end = *pos;
                recorrido[casilla++] = *pos;
                fin = 1;
                /*printf ("Hilo %d termina en (%d, %d) despues de %d pasos\n",pos->id_hilo, pos->num_linea, pos->num_columna, pos->pasos);*/
                --hilos_activos;
            case ' ' :
                /*laberinto[i][j] = '.';*/
                ++pos->pasos;
                recorrido[casilla++] = *pos;
                break;
            default :
                printf("formato de laberinto invalido\n");
                break;
        }
        
        switch (laberinto[i][j-1]){
            case '#' :
                break;
            case 'O' :
                *end = *pos;
                end->num_columna = i - 1;
                recorrido[casilla++] = *pos;
                fin = 1;
            case ' ' :
                pos1->num_linea = i;
                pos1->num_columna = pos->num_columna - 1;
                pos1->id_hilo = pos->id_hilo;
                pos1->direccion = 'l';
                pthread_create (&hilo1, NULL, mov_horizontal, (void*) pos1);
                ++hilos_creados;
                break;
            default :
                printf("formato de laberinto invalido\n");
                break;
        }
        
        switch (laberinto[i][j+1]){
            case '#' :
                break;
            case 'O' :
                *end = *pos;
                end->num_columna = i + 1;
                recorrido[casilla++] = *pos;
                fin = 1;
            case ' ' :
                pos2->num_linea = i;
                pos2->num_columna = pos->num_columna + 1;
                pos2->id_hilo = pos->id_hilo;
                pos2->direccion = 'r';
                pthread_create (&hilo2, NULL, mov_horizontal, (void*) pos2);
                ++hilos_creados;
                break;
            default :
                printf("formato de laberinto invalido\n");
                break;
        }
        
        if (hilos_creados == 2 && laberinto[i+x][j] == ' '){
            pos3->num_linea = i + x;
            pos3->num_columna = j;
            pos3->id_hilo = pos->id_hilo;
            pos3->direccion = pos->direccion;
            pthread_create (&hilo3, NULL, mov_vertical, (void*) pos3);
            /*printf ("Hilo %d termina en (%d, %d) despues de %d pasos\n",pos->id_hilo, pos->num_linea, pos->num_columna, pos->pasos);*/
            --hilos_activos;
            return NULL;
        }
    }
}

/*----------------------------------------------------------------------------------------------------------
            MAIN
-----------------------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int inicio=0, id_0, id_1, l_0, c_0, contador=0,total_pasos=0;
    FILE* fp;
    char line[1024];
    //char *name = "labyrinth.txt"; //argv[1];
    pthread_t hilo1;
        
    ini = malloc(sizeof(struct posicion));
    end = malloc(sizeof(struct posicion));
    
    fp = fopen(argv[1], "r");
    if (fp != NULL){
        while(fgets(line, 1024, fp) != NULL){
            strcpy(laberinto[inicio], line);
            ++inicio;
            contador = 0;
            for(int z = 0; z < sizeof(laberinto[inicio]); ++z )
            {
                if(laberinto[inicio-1][z]=='#' || laberinto[inicio-1][z]=='I' || laberinto[inicio-1][z]=='O')
                {
                    ++contador;
                }
            }
            if(contador > columnas)
            {
                columnas=contador;
            }
        }
        filas = inicio-1;
        inicio=0;
    }
    else {
        printf("Error al abrir archivo\n");
        return 2;
    }
    fclose(fp);
    
     for (int r=0; r<filas;++r){
        for (int k=0; k<columnas;++k){
            if(laberinto[r][k] == 'I'){
                ini->num_linea = r;
                ini->num_columna = k;
                ini->id_hilo = 0;
                r=filas+1;
                break;
            }
        }
    }
    printf("fila: %d columna: %d filas: %d columnas %d", ini->num_linea+1, ini->num_columna+1, filas, columnas);
    
    if(ini->num_columna == 0){
        ini->direccion = 'r';
        printf("columna: %d", ini->num_columna+1);
        
        *end = *ini;
        
        pthread_create (&hilo1, NULL, mov_horizontal, (void*) end);
        pthread_join( hilo1, NULL);
        
        while (hilos_activos != 0 || fin != 1){}

    }
    else if(ini->num_columna == columnas-1){
        ini->num_columna = ini->num_columna +1;
        ini->direccion = 'l';
        printf("columna: %d", ini->num_columna+1);
        *end = *ini;

        pthread_create (&hilo1, NULL, mov_horizontal, (void*) end);
        pthread_join( hilo1, NULL);
        while (hilos_activos != 0 || fin != 1){}
    }
    else if(ini->num_linea == 0){
        printf("fila: %d", ini->num_linea+1);
        ini->direccion = 'd';
        *end = *ini;

        pthread_create (&hilo1, NULL, mov_vertical, (void*) end);
        pthread_join( hilo1, NULL);
        while (hilos_activos != 0 || fin != 1){}
    }
    else if(ini->num_linea == filas){
        printf("fila: %d", ini->num_linea+1);
        ini->direccion = 'u';
        *end = *ini;

        pthread_create (&hilo1, NULL, mov_vertical, (void*) end);
        pthread_join( hilo1, NULL);
        while (hilos_activos != 0 || fin != 1){}
    }
    else{
        printf("Error en el inicio del laberinto, debería estar en los lados del rectangulo \n");
        return 1;
    }
    /*sleep(2);*/
    printf("\n\nESTADO INICIAL DEL LABERINTO \n");
    print_laberinto();
    
    id_0 = end->id_hilo;
    id_1 = end->id_hilo_padre;
    l_0 = end->num_linea;
    c_0 = end->num_columna;
    total_pasos = end->pasos;
    
    while(inicio<casilla)
    {
        for(int i = casilla+1; i >= 0; --i)
        {
            if((abs(recorrido[i].num_linea - l_0) + abs(recorrido[i].num_columna - c_0)) == 1)
            {
                if(recorrido[i].id_hilo == id_1){
                    total_pasos += recorrido[i].pasos;
                    laberinto[recorrido[i].num_linea][recorrido[i].num_columna] = '.';
                }
                
                if(recorrido[i].id_hilo == id_0){
                    laberinto[recorrido[i].num_linea][recorrido[i].num_columna] = '.';
                }
                id_0 = recorrido[i].id_hilo;
                id_1 = recorrido[i].id_hilo_padre;
                l_0 = recorrido[i].num_linea;
                c_0 = recorrido[i].num_columna;
                ++inicio;
                
            }
        }
    }
    printf("\nRESULETO \n");
    print_laberinto();
    /*sleep(1);*/
    
    printf ("El numero de hilos activos %d\n", hilos_activos);
    printf ("Se empieza en la fila: %d y en la columna: %d\n", ini->num_linea+1, ini->num_columna+1);
    printf ("Se sale por la fila: %d y en la columna: %d\n", end->num_linea+1, end->num_columna+1);
    printf ("Minimos pasos para salir del laberinto: %d\n", total_pasos);
    printf ("Pasos dados por el programa: %d\n", casilla);
    
    return 0;
}

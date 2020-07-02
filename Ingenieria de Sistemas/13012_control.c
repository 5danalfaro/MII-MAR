/*Para compilar en Mac clang 13012_contol.c -o control -lpthread */

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <time.h>
/* para hacer un grafico -  #include <koolplot.h> */


int stp = 0, freq=20000;
/*empezaremos a una temperatura de 10 grados y 1 bar para controlar en 50 y 10 bar*/
float T=10, P=100000, Tsen, Psen, Tref=50, Pref=1000000;
int var_T = 5, var_P = 20;
pthread_t thread_id[9];

/*--para sacar los datos en un gráfico*/
/*struct datos {
    float t_t[1024];
    float p_t[1024];
    float t[1024];
};
 struct datos *dat;*/

/*niveles de prioridad*/
struct sched_param thread_param1, thread_param2, thread_param3;
pthread_attr_t thread_attr1, thread_attr2, thread_attr3;
int status, min_prio, max_prio, med_prio;
char c,end = 'o';
FILE *datafile;


/*control de temperatura*/
void *controlT (void *arg)
{
    usleep(100000);
    if(Tref > Tsen) T = T + (Tref-Tsen)/5;
    if(Tref < Tsen  ) T = T - (Tsen-Tref)/5;
return NULL;
}

/*control de presion*/
void *controlP (void *arg)
{
    usleep(50000);
    if(Pref > Psen) P = P + (Pref-Psen)/8;
    if(Pref < Psen) P = P - (Psen-Pref)/8;
return NULL;
}

/*imprimir datos en un csv y guardar para plotear*/
void *logdata (void *arg)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(datafile, "%d/%02d/%02d %02d:%02d:%02d,%.1f,%.0f\n", tm.tm_mday, tm.tm_mon + 1,  tm.tm_year + 1900,tm.tm_hour, tm.tm_min, tm.tm_sec, Tsen,Psen);
    
    /* para el gráfico
    datos->t_t=Tsen;
    datos->p_t=Psen;
    datos->t_t=datos->t_t+1;*/
    return NULL;
}


void *sensor (void *arg)
{
//
while(!stp){
    srand(time(NULL));
    Tsen = T - var_T + 1.2*(rand()%(2*var_T+1));
    Psen = P - var_P + 1.2*(rand()%(2*var_P+1));
    /*logeamos los datos en un csv*/
    pthread_create(&thread_id[3], &thread_attr3, logdata, NULL);
    /*Controlamos temperatura*/
    pthread_create(&thread_id[5], &thread_attr2, controlT, NULL);
    /*Controlamos presión*/
    pthread_create(&thread_id[6], &thread_attr2, controlP, NULL);
    
    pthread_join(thread_id[5], NULL);
    pthread_join(thread_id[6], NULL);
}
return NULL;
}

void *imprimir (void *arg)
{
while(!stp){
    system("tput clear");
    printf("\n%s %.1f %s %s %.0f %s", "Temperatura: ", Tsen, "ºC. ", "Presion: ", Psen, "Pa.\n");
    printf("Punto fijado es: %.0f ºC and %.0f Pa.\n" ,Tref,Pref);
    printf("Presiona 'p' para parar or 'c' para cambiar el punto fijado ¡Y ENTER!.\n");
    usleep(freq);
}
return NULL;
}


/*inciar procesos principales imprimir y sensor*/
void *start (void *arg)
{

pthread_create(&thread_id[1], &thread_attr2, sensor, NULL);
usleep(10000);
pthread_create(&thread_id[2], &thread_attr3, imprimir, NULL);

return NULL;
}

void *change_setP (void *arg)
{
stp = 1;
/*para que se paren los hilos*/
sleep(1);
system("tput clear");
printf("\nNUEVO PUNTO DE REFERENCIA\nEscriba la temperatura en grados(ºC).\n");
scanf("%f",&Tref);
system("tput clear");
printf("NUEVO PUNTO DE REFERENCIA\nEscriba la presión en Pascales(Pa).\n");
scanf("%f",&Pref);
stp = 0;

/*volvemos a empezar*/
pthread_create(&thread_id[0], &thread_attr2, start, NULL);
        
return NULL;
}

/* MAIN */

int main (int argc, char *argv[])
{
/*Para sacar los datos en un gráfico
dat = malloc(sizeof(struct datos));
dat->t=0;*/


pthread_attr_init(&thread_attr1);
pthread_attr_init(&thread_attr2);
pthread_attr_init(&thread_attr3);
    
/*First in first out scheduling y prioridades*/
pthread_attr_setschedpolicy(&thread_attr1, SCHED_FIFO);
pthread_attr_setschedpolicy(&thread_attr2, SCHED_FIFO);
pthread_attr_setschedpolicy(&thread_attr3, SCHED_FIFO);

min_prio = sched_get_priority_min (SCHED_FIFO);
max_prio = sched_get_priority_max (SCHED_FIFO);

thread_param1.sched_priority = max_prio;
thread_param2.sched_priority = (min_prio + max_prio) / 2;
thread_param3.sched_priority = min_prio;

pthread_attr_setschedparam(&thread_attr1, &thread_param1);
pthread_attr_setschedparam(&thread_attr2, &thread_param2);
pthread_attr_setschedparam(&thread_attr3, &thread_param3);

/*archivo donde guardar los datos*/
datafile = fopen("DatosProceso.csv", "w+");

fprintf(datafile,"Datetime,Temperatura(ºC),Presion(Pa)\n");

printf("Empezamos el programa con un punto fijado de.\nTemperatura %.1f ºC y Presión %.0f Pa.\n" ,Tref,Pref);
printf("Si quieres empezar pulsa s y enter\n");
do{
    scanf("%s",&end);
} while (end != 's');


/*Empezamos*/
pthread_create(&thread_id[0], &thread_attr2, start, NULL);
while(end=='s'){
    c = 'X';
    c = getchar();

    /* Parar */
    if(c=='p'){
        break;
    }
    /* cambiar punto fijado*/
    else if(c=='c'){
        pthread_create(&thread_id[8], &thread_attr3, change_setP, NULL);
        pthread_join(thread_id[8], NULL);
    }
}
system("tput clear");
printf("Fin del programa.\n");
printf("%s %.1f %s %s %.0f %s", "Temperatura final: ", Tsen, "ºC. ", "Presion final: ", Psen, "Pa.\n\n");
    
    
/* aqui ploteariamos el gráfico plot(dat->t, dat->t_t);*/
    /* aqui ploteariamos el gráfico plot(dat->t, dat->p_t);*/

fclose(datafile);

return 0;

}

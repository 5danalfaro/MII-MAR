/*
--INFO IMPORTANTE--
NO SE PUEDE COMPILAR CON MAC [<mqueue.h> es una libreria de POSIX messaging queues y no esta disponible en OS X]
COMPILAR EN https://onlinegdb.com/rk42FboCL
AÑADIR EN "EXTRA COMPILER FLAGS" (sin las comillas)-> "-lpthread -lrt"
*/

/*-librerias-*/
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <time.h>

/*-para las colas-*/
#define QUEUE_NAME  "/rs.mq.01"
#define MAX_SIZE    1024
#define MSG_STOP    "CIERRE DE PUERTA"

/*-variable -*/
int bool_flag = 0;

/*-para contar las entradas-*/
struct aforo {
    int normal;
    int vips;
    int total;
};

struct aforo *af;


/*-TOTALIZADOR-*/
void *
queue_totalizador (void *args)
{

  mqd_t mq;
  ssize_t bytes_read;
  struct mq_attr attr;
  char buffer[MAX_SIZE + 1];
  char cmp[5];
  int stop_msg = 0;
  af->normal=0;
  af->vips=0;
  af->total=0;

  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = MAX_SIZE;
  attr.mq_curmsgs = 0;

  mq = mq_open (QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK, 0644, &attr);
  assert (mq > 0);

  /*-while hasta que se cierren ambas puertas-*/
  while (1)
    {
      memset (buffer, 0x00, sizeof (buffer));

      bytes_read = mq_receive (mq, buffer, MAX_SIZE, NULL);

      if (bytes_read >= 0)
    {
      printf ("SIGUIENTE\n[%s]\n", buffer);

      snprintf (cmp, sizeof (cmp), buffer);

      if (strcmp (cmp, "[VIP") == 0)
        af->vips=af->vips+1;
      else if (strcmp (cmp, "[NOR") == 0)
        af->normal=af->normal+1;

      af->total = af->normal + af->vips;
      printf ("VIPs: %d     Normal: %d    Total: %d\n", af->vips, af->normal,
          af->total);
      system("clear");

      if (!strcmp (buffer, MSG_STOP))
        {
          ++stop_msg;
          if (stop_msg == 2)
        {
          mq_close (mq);
          mq_unlink (QUEUE_NAME);
          return NULL;
        }
        }
    }

      bool_flag = 1;

      fflush (stdout);
      usleep (2000);
    }
}


/*--Camara VIP--*/
void *
vip_cam_queue (void *args)
{

  mqd_t mq;
  char buffer[MAX_SIZE];

  while (bool_flag != 1)
    {
    }

  mq = mq_open (QUEUE_NAME, O_WRONLY);

  assert (mq > 0);

  printf ("PUERTA VIP ABIERTA\n");

  /*-entraran 14 personas VIP al concierto -*/
  for (int contador = 0; contador < 15; contador++)
    {
      if (contador < 14)
    snprintf (buffer, sizeof (buffer),
          "[VIP %d] entrada de VIP en el concierto", contador);
      else
    snprintf (buffer, sizeof (buffer), MSG_STOP);

      mq_send (mq, buffer, MAX_SIZE, 1);

      fflush (stdout);
      usleep (3200);
    }

  mq_close (mq);
  return NULL;
}

/*--Camara Normal--*/
void *
normal_cam_queue (void *args)
{

  mqd_t mq;
  char buffer[MAX_SIZE];

  while (bool_flag != 1)
    {
    }

  mq = mq_open (QUEUE_NAME, O_WRONLY);

  assert (mq > 0);

  printf ("PUERTA NORMAL ABIERTA\n");

  /*-entraran 44 personas normales al concierto -*/
  for (int contador = 0; contador < 45; contador++)
    {
      if (contador < 44)
    snprintf (buffer, sizeof (buffer),
          "[NOR %d] entrada de una persona en el concierto", contador);
      else
    snprintf (buffer, sizeof (buffer), MSG_STOP);

      mq_send (mq, buffer, MAX_SIZE, 0);

      fflush (stdout);
      usleep (500);
    }

  mq_close (mq);
  return NULL;
}

/*----MAIN---*/
int
main (int argc, char **argv)
{
  af = malloc(sizeof(struct aforo));

  pthread_t normal_cam, vip_cam, totalizer;

  printf ("\nabriendo puertas del concierto..\n");

  pthread_create (&totalizer, NULL, &queue_totalizador, NULL);
  pthread_create (&normal_cam, NULL, &normal_cam_queue, NULL);
  pthread_create (&vip_cam, NULL, &vip_cam_queue, NULL);

  printf ("juntando informacion de camaras...\n");
  pthread_join (normal_cam, NULL);
  pthread_join (vip_cam, NULL);
  pthread_join (totalizer, NULL);

  system("clear");
  printf ("FIN\n");
  printf ("RECUENTO FINAL\nVIPs: %d     Normal: %d    Total: %d\n", af->vips, af->normal,
  af->total);

  return 0;
}

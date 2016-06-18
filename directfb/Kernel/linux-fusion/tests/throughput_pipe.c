/*
 *      Fusion Kernel Module
 *
 *      (c) Copyright 2002  Convergence GmbH
 *
 *      Written by Denis Oliver Kropp <dok@directfb.org>
 *
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sched.h>

#include <linux/fusion.h>

#include <pthread.h>

typedef struct {
  int nr;
} TestMessage;

static int       fd[2];    /* File descriptors of the pipe */
static pthread_t receiver; /* Thread reading messages from the pipe. */

static int       last_nr = 0;

static void
process_sent_message (int msg_id, int msg_size, void *msg_data)
{
  TestMessage *message = (TestMessage*) msg_data;

  last_nr = message->nr;
}

static void
process_reactor_message (int reactor_id, int msg_size, void *msg_data)
{
  /* This program doesn't make use of reactors. */
}

static void *
receiver_thread (void *arg)
{
  int  len;
  char buf[1024];

#if 0
  struct sched_param param;
  
  param.sched_priority = 1;

  if (sched_setscheduler (0, SCHED_FIFO, &param))
    perror ("sched_setscheduler");
#endif

  /* Read as many messages as possible at once. */
  while ((len = read (fd[0], buf, 1024)) > 0 || errno == EINTR)
    {
      /* Current position within the buffer. */
      char *buf_p = buf;

      /* Shutdown? */
      pthread_testcancel();

      /* Possibly interrupted during blocking read. */
      if (len <= 0)
        continue;

      /* While there are still messages in the buffer... */
      while (buf_p < buf + len)
        {
          /* Header of next message is at the current buffer location. */
          FusionReadMessage *header = (FusionReadMessage*) buf_p;

          /* Its data follows immediately. */
          void *data = buf_p + sizeof(FusionReadMessage);
          
          /* Process the message depending on its type (origin). */
          switch (header->msg_type)
            {
            case FMT_SEND:
              process_sent_message (header->msg_id,
                                    header->msg_size, data);
              break;
            case FMT_REACTOR:
              process_reactor_message (header->msg_id,
                                       header->msg_size, data);
              break;
            default:
              break;
            }

          /* Shutdown? */
          pthread_testcancel();

          /* Next message header follows immediately. */
          buf_p = data + header->msg_size;
        }
    }

  perror ("receiver thread failure");

  return NULL;
}

int
main (int argc, char *argv[])
{
  int  n;
  int  synchronous = 0;
  long d;
  struct timeval t1, t2;

  /* Open the pipe. */
  if (pipe (fd))
    {
      perror ("pipe() failed");
      return -1;
    }

  /* Start the receiver thread. */
  pthread_create (&receiver, NULL, receiver_thread, NULL);

  /* Wait for the receiver being up. */
  usleep (100000);

  /* Stop time before sending the messages. */
  gettimeofday (&t1, NULL);

  /* Send some messages. */
  for (n = 0; n < 4000000; n++)
    {
      struct {
        FusionReadMessage header;         /* Message header. */
        TestMessage       message;        /* Message data. */
      } data;

      /* Fill message header. */
      data.header.msg_type = FMT_SEND;
      data.header.msg_id   = 0;
      data.header.msg_size = sizeof(TestMessage);

      /* Set message number. */
      data.message.nr = n;

      /* Post the message. */
      if (write (fd[1], &data, sizeof(data)) < 0)
        {
          perror ("write() failed");
          return -1;
        }

      if (last_nr == n)
        synchronous++;
    }

  /* Wait for all messages to arrive. */
  while (last_nr < n-1)
    sched_yield();

  /* Stop time after having received all messages. */
  gettimeofday (&t2, NULL);

  /* Calculate time it took. */
  d = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000;

  /* Print message. */
  printf ("Sent/received %lu messages per second (%d%% synchronous).\n",
          4000000000U / d, synchronous * 100 / 4000000);

  /* Stop the receiver. */
  pthread_cancel (receiver);
  pthread_join (receiver, NULL);

  /* Close the pipe. */
  close (fd[0]);
  close (fd[1]);

  return 0;
}

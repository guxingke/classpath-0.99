/* gnu_java_nio_VMSelector.c - Native methods for SelectorImpl class
   Copyright (C) 2004, 2005 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.
 
GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

#include "config.h"

/* <sys/types.h> needs to be included on OSX before <sys/select.h> */
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_SYS_SELECT_H)
#include <sys/select.h>
#endif
#include <sys/time.h>

#include <string.h>

#include <errno.h>

#include <jni.h>
#include <jcl.h>

#include "gnu_java_nio_VMSelector.h"

/* Amount of characters in the error message buffer for strerror_r. */
#define BUF_SIZE 250

void helper_put_filedescriptors (JNIEnv *, jintArray, fd_set *, int *);

void helper_get_filedescriptors (JNIEnv *, jintArray *, fd_set *);

void helper_reset (JNIEnv *, jintArray *);

int
helper_select (JNIEnv *, jclass, jmethodID,
	       int, fd_set *, fd_set *, fd_set *, struct timeval *);

void
helper_put_filedescriptors (JNIEnv * env, jintArray fdArray, fd_set * fds,
			    int *max_fd)
{
}

void
helper_get_filedescriptors (JNIEnv * env, jintArray * fdArray, fd_set * fds)
{
}

void
helper_reset (JNIEnv * env, jintArray * fdArray)
{
  jint *tmpFDArray = (*env)->GetIntArrayElements (env, fdArray, 0);
  int size = (*env)->GetArrayLength (env, fdArray);
  int index;

  for (index = 0; index < size; index++)
    tmpFDArray[index] = 0;
}

/* A wrapper for select() which ignores EINTR.
 * Taken from gclib's posix.cc
 */
int
helper_select (JNIEnv * env, jclass thread_class,
	       jmethodID thread_interrupted, int n, fd_set * readfds,
	       fd_set * writefds, fd_set * exceptfds, struct timeval *timeout)
{
#ifdef HAVE_SYS_SELECT_H
  /* If we have a timeout, compute the absolute ending time. */
  struct timeval end, delay, after;
  int r;

  if (timeout)
    {
      gettimeofday (&end, NULL);

      end.tv_usec += timeout->tv_usec;

      if (end.tv_usec >= 1000000)
	{
	  ++end.tv_sec;
	  end.tv_usec -= 1000000;
	}

      end.tv_sec += timeout->tv_sec;
      delay = *timeout;
    }
  else
    {
      /* Placate compiler. */
      delay.tv_sec = delay.tv_usec = 0;
    }

  while (1)
    {
      r = select (n, readfds, writefds, exceptfds, timeout ? &delay : NULL);

      if (r < 0 && errno != EINTR)
	return -errno;
      else if (r >= 0)
	return r;

      /* Here we know we got EINTR. */
      if ((*env)->
	  CallStaticBooleanMethod (env, thread_class, thread_interrupted))
	{
	  return -EINTR;
	}

      if (timeout)
	{
	  gettimeofday (&after, NULL);

	  /* Now compute new timeout argument. */
	  delay.tv_usec = end.tv_usec - after.tv_usec;
	  delay.tv_sec = end.tv_sec - after.tv_sec;

	  if (delay.tv_usec < 0)
	    {
	      --delay.tv_sec;
	      delay.tv_usec += 1000000;
	    }

	  if (delay.tv_sec < 0)
	    {
	      /* We assume that the user wants a valid select() call
	       * more than precise timing.  So if we get a series of
	       * EINTR we just keep trying with delay 0 until we get a
	       * valid result.
	       */
	      delay.tv_sec = 0;
	    }
	}
    }
#else /* HAVE_SYS_SELECT_H */
  return 0;
#endif

}

JNIEXPORT jint JNICALL
Java_gnu_java_nio_VMSelector_select (JNIEnv * env,
				     jclass obj __attribute__ ((__unused__)),
				     jintArray read,
				     jintArray write,
				     jintArray except, jlong timeout)
{
  return 0;
}

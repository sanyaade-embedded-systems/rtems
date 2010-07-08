/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>
#include "test_support.h"
#include "gxx_wrappers.h"

void test_recursive_mutex(void)
{
  int                sc;
   __gthread_mutex_t mutex;

  mutex = 0;
  puts( "rtems_gxx_recursive_mutex_init() - OK" );
  rtems_gxx_recursive_mutex_init(&mutex);
  rtems_test_assert( mutex != 0 );

  puts( "rtems_gxx_recursive_mutex_trylock() - OK" );
  sc = rtems_gxx_recursive_mutex_trylock(&mutex);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_recursive_mutex_trylock() - Nest" );
  sc = rtems_gxx_recursive_mutex_trylock(&mutex);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_recursive_mutex_unlock() - Unnest" );
  sc = rtems_gxx_recursive_mutex_unlock(&mutex);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_recursive_mutex_unlock() - OK" );
  sc = rtems_gxx_recursive_mutex_unlock(&mutex);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_recursive_mutex_lock() - OK" );
  sc = rtems_gxx_recursive_mutex_lock(&mutex);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_recursive_mutex_unlock() - OK" );
  sc = rtems_gxx_recursive_mutex_unlock(&mutex);
  rtems_test_assert( sc == 0 );
}

void test_mutex(void)
{
  int                sc;
   __gthread_mutex_t mutex;

  mutex = 0;
  puts( "rtems_gxx_mutex_init() - OK" );
  rtems_gxx_mutex_init(&mutex);
  rtems_test_assert( mutex != 0 );

  puts( "rtems_gxx_mutex_trylock() - OK" );
  sc = rtems_gxx_mutex_trylock(&mutex);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_mutex_unlock() - OK" );
  sc = rtems_gxx_mutex_unlock(&mutex);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_mutex_lock() - OK" );
  sc = rtems_gxx_mutex_lock(&mutex);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_mutex_unlock() - OK" );
  sc = rtems_gxx_mutex_unlock(&mutex);
  rtems_test_assert( sc == 0 );
}

void once_function(void)
{
  puts( "Running once method" );
}

void test_once(void)
{
  __gthread_once_t once;
  int              sc;

  once = 0;

  puts( "Call once method the first time" );
  sc = rtems_gxx_once(&once, once_function);
  rtems_test_assert( sc == 0 );

  puts( "Call once method the second time" );
  sc = rtems_gxx_once(&once, once_function);
  rtems_test_assert( sc == 0 );
}

volatile bool key_dtor_ran;

void key_dtor(void *ptr)
{
  key_dtor_ran = true;
}

void test_key(void)
{
  int              sc;
  __gthread_key_t  key;
  void            *p;

  puts( "rtems_gxx_key_create(&key, NULL) - OK" );
  sc = rtems_gxx_key_create(&key, NULL);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_key_delete(key) - OK" );
  sc = rtems_gxx_key_delete(key);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_key_create(&key, key_dtor) - OK" );
  sc = rtems_gxx_key_create(&key, key_dtor);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_setspecific() - OK" );
  sc = rtems_gxx_setspecific(key, (void *)0x1234);
  rtems_test_assert( sc == 0 );

  puts( "rtems_gxx_petspecific() - OK" );
  p = rtems_gxx_getspecific(key);
  rtems_test_assert( p == (void *)0x1234 );

  puts( "rtems_gxx_key_delete(key) - OK" );
  sc = rtems_gxx_key_delete(key);
  rtems_test_assert( sc == 0 );
  rtems_test_assert( key_dtor_ran == true );

#if 0

void *rtems_gxx_getspecific(__gthread_key_t key);

int rtems_gxx_setspecific(__gthread_key_t key, const void *ptr);
#endif
}

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** TEST GXX 01 ***" );

  test_mutex();
  puts( "" );

  test_recursive_mutex();
  puts( "" );

  test_once();
  puts( "" );

  test_key();

  puts( "*** END OF TEST GXX 01 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS        1
#define CONFIGURE_MAXIMUM_SEMAPHORES   2
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
pthread_rwlock_t rwlock;

void* multi_write_fun(void* argv)
{
  printf("tid[%lx] begin\n",pthread_self());
  pthread_rwlock_wrlock(&rwlock);
  for(int i = 0; i < 5; i++)
    {
      printf("tid[%lx] write\n", pthread_self());
      sleep(1);
    }
  pthread_rwlock_unlock(&rwlock);
}

void* multi_read_fun(void* argv)
{
  printf("tid[%lx] begin\n",pthread_self());
  pthread_rwlock_rdlock(&rwlock);
  for(int i = 0; i < 5; i++)
    {
      printf("tid[%lx] read\n",pthread_self());
      sleep(1);
    }
  pthread_rwlock_unlock(&rwlock);
}

void* write_fun(void* argv)
{
  while(1)
    {
      pthread_rwlock_wrlock(&rwlock);
      printf("tid[%lx] write\n", pthread_self());
      sleep(1);
      pthread_rwlock_unlock(&rwlock);
      sleep(1);
    }
}

void* read_fun(void* argv)
{
  while(1)
    {
      pthread_rwlock_rdlock(&rwlock);
      printf("tid[%lx] read\n",pthread_self());
      sleep(1);
      pthread_rwlock_unlock(&rwlock);
    }
}

void test_read()
{
  const int read_thread_num = 2;
  pthread_t read_threads[read_thread_num];
    for(int i = 0; i < read_thread_num; i++)      
      {
	pthread_create(&read_threads[i],NULL, multi_read_fun, NULL);
      }
    for(int i = 0; i < read_thread_num; i++)
      {
	pthread_join(read_threads[i],NULL);
      }
}

void test_write()
{
  const int write_thread_num = 3;
  pthread_t write_threads[write_thread_num];
    for(int i = 0; i < write_thread_num; i++)
    {
      pthread_create(&write_threads[i],NULL,multi_write_fun,NULL);
    }
    for(int i = 0; i < write_thread_num; i++)
      {
	pthread_join(write_threads[i],NULL);
      }
}

int main()
{
  pthread_rwlock_init(&rwlock, NULL);
  test_read();
  test_write();
  //test_read_write();
}

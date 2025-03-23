#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock *rw)
{
  rw->numWorkers = 0;
  rw->numReaders = 0;
  rw->lock = PTHREAD_MUTEX_INITIALIZER;
  rw->lockR = PTHREAD_MUTEX_INITIALIZER;
  rw->lockW = PTHREAD_MUTEX_INITIALIZER;
  rw->reader = PTHREAD_COND_INITIALIZER;
  rw->writer = PTHREAD_COND_INITIALIZER;
}

void ReaderLock(struct read_write_lock *rw)
{
  pthread_mutex_lock(&rw->lockW);
  while (rw->numWorkers > 0)
  {
    pthread_cond_wait(&rw->writer, &rw->lock);
  }
  if (rw->numReaders == 0)
  {
    pthread_mutex_lock(&rw->lock);
  }
  rw->numReaders++;
  pthread_mutex_unlock(&rw->lockW);
}

void ReaderUnlock(struct read_write_lock *rw)
{
  pthread_mutex_lock(&rw->lockR);
  rw->numReaders--;
  if (rw->numReaders == 0)
  {
    pthread_mutex_unlock(&rw->lock);
  }
  pthread_mutex_unlock(&rw->lockR);
}

void WriterLock(struct read_write_lock *rw)
{
  pthread_mutex_lock(&rw->lockW);
  rw->numWorkers++;
  pthread_mutex_lock(&rw->lock);
  pthread_mutex_unlock(&rw->lockW);
}

void WriterUnlock(struct read_write_lock *rw)
{
  pthread_mutex_lock(&rw->lockW);
  rw->numWorkers--;
  if (rw->numWorkers == 0)
  {
    pthread_cond_broadcast(&rw->reader);
  }
  pthread_mutex_unlock(&rw->lock);
  pthread_mutex_unlock(&rw->lockW);
}

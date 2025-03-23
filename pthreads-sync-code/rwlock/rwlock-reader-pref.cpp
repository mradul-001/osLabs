#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock *rw)
{
  rw->numWorkers = 0;
  rw->numReaders = 0;
  rw->lock  = PTHREAD_MUTEX_INITIALIZER;
  rw->lockR = PTHREAD_MUTEX_INITIALIZER;
  rw->lockW = PTHREAD_MUTEX_INITIALIZER;
  rw->reader = PTHREAD_COND_INITIALIZER;
  rw->writer = PTHREAD_COND_INITIALIZER;
}

void ReaderLock(struct read_write_lock *rw)
{
  pthread_mutex_lock(&rw->lockR);
  rw->numReaders += 1;
  if (rw->numReaders == 1) {
    pthread_mutex_lock(&rw->lock);        // if you are first reader, acquire the lock
  }
  pthread_mutex_unlock(&rw->lockR);
  return;
}

void ReaderUnlock(struct read_write_lock *rw)
{
  pthread_mutex_lock(&rw->lockR);
  rw->numReaders -= 1;
  if (rw->numReaders == 0) {
    pthread_cond_broadcast(&rw->writer);  // signal the writer if you are last
    pthread_mutex_unlock(&rw->lock);      // if you are the last reader, release the lock
  }
  else {
    pthread_cond_broadcast(&rw->reader);  // if you are not the last one, signal other readers
  }
  pthread_mutex_unlock(&rw->lockR);
  return;
}

void WriterLock(struct read_write_lock *rw)
{
  pthread_mutex_lock(&rw->lock);
  while (rw->numReaders > 0) {
    pthread_cond_wait(&rw->writer, &rw->lock);
  }
  pthread_mutex_lock(&rw->lockW);
  rw->numWorkers += 1;
  pthread_mutex_unlock(&rw->lockW);
}

void WriterUnlock(struct read_write_lock *rw)
{
  pthread_mutex_lock(&rw->lockW);
  rw->numWorkers--;
  pthread_mutex_unlock(&rw->lockW);
  pthread_cond_broadcast(&rw->writer);
  pthread_mutex_unlock(&rw->lock);
  return;
}

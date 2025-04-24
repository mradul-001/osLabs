#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"

struct psm
{
	int numElements;   // tracks the last location where a message was written
	int topicPageSize; // size of each topic page
	// Besides these, you may need the following:
	// 1] A channel for the topic, so readers can sleep on it
	// 2] A lock for the topic, so that readers and writers can synchronize access to the topic
	// 3] A page for the topic, so that readers and writers can read/write to it
	void *chan;
	struct spinlock lock;
	char *buff;
};

void initpsm(struct psm *psmInstance)
{
	// Allocate memory for the topic page, 1 page should be sufficient
	// Also remember to initialize the lock and channel appropriately, if you are using pointers, remember to allocate memory for them as well
	psmInstance->numElements = 0;
	psmInstance->topicPageSize = 1024;
	initlock(&psmInstance->lock, "psmlock");
	psmInstance->buff = kalloc();
	return;
}

int psmRetrieve(struct psm *psmInstance, int *buff, int length)
{
	acquire(&psmInstance->lock);

	// if asked for more data, sleep
	while (length > psmInstance->numElements)
		sleep(psmInstance->chan, &psmInstance->lock);

	// copy data into buffer
	for (int i = myproc()->last; i < length + myproc()->last; i++)
	{
		buff[i - myproc()->last] = psmInstance->buff[i];
	}
	myproc()->last += length;

	release(&psmInstance->lock);
	return 0;
}

int psmPublish(struct psm *psmInstance, int *buff, int length)
{
	acquire(&psmInstance->lock);
	int start = psmInstance->numElements;
	
	// write the data into the buffer
	for (int i = start; i < start + length; i++)
	{
		psmInstance->buff[i] = buff[i - start];
	}

	// increase the element count
	psmInstance->numElements = psmInstance->numElements + length;
	wakeup(psmInstance->chan);
	
	release(&psmInstance->lock);
	return 0;
}

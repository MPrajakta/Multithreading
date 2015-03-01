#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

#define NUMTHREADS 10

// lock = 1 HELD
// lock = 0 FREE
struct spin_lock_t{
	int lock;
};

/*
* atomic_cmpxchg
*
* equivalent to atomic execution of this code:
*
* if (*ptr == old) {
* 	*ptr = new;
* 	return old;
* } 
* else {
* 	return *ptr;
* }
*
*/
static inline int atomic_cmpxchg (volatile int *ptr, int old, int new)
{
	int ret;
	asm volatile ("lock cmpxchgl %2,%1"
	: "=a" (ret), "+m" (*ptr)
	: "r" (new), "0" (old)
	: "memory");
	return ret;
}

void spin_lock (struct spin_lock_t *s)
{
	atomic_cmpxchg(&(s->lock), 0, 1);
	//s->lock = 1;
}
void spin_unlock (struct spin_lock_t *s)
{
	atomic_cmpxchg(&(s->lock), 1, 0);
	//s->lock = 0;
}

void *runner(void *tid)
{
	int t = (size_t)tid;
	printf("%d\n", t);
	pthread_exit(0);
}



int main(void)
{
	struct spin_lock_t *s= &(struct spin_lock_t){.lock = 0};
	
	spin_lock(s);
	printf("%d\n", s->lock);
	spin_unlock(s);
	printf("%d\n", s->lock);
	
	pthread_t threads[NUMTHREADS] = {0};
	int  t = 0 ;
	
	for(t =0; t < NUMTHREADS; t++)
	{
		printf("In 'main': creating thread %d\n", t);
		pthread_create(&threads[t], NULL, runner,(void *)(size_t)t);
	}
	long int duration = 10;
	
	// processor sleeps
	usleep(duration * 1000);
	/*
	for(t =0; t < NUMTHREADS; t++)
	{
		//printf("In Main: creating thread %d\n", t);
		pthread_join(threads[t], NULL);
	}
	*/
	
	return 0;
}

//http://stackoverflow.com/questions/11709929/how-to-initialize-a-pointer-to-a-struct-in-c


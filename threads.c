#include "common.h"
#include "battledotnet.h"
#include "threads.h"
#include <pthread.h>

/* pthread mutex and condition variable initializations */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t* cond = NULL;

#define MAX_THREADS 5
volatile int cnt = 0;
static int itrArr[MAX_THREADS] = {0};

/* Thread init function to start condition init */
void thread_init() {

	cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t) * MAX_THREADS);

    int i =0;
	for (i = 0; i < MAX_THREADS; i++) {
		if (pthread_cond_init(&cond[i], NULL) != 0) {
			perror("pthread_cond_init() error");
			exit(1);
		}
	}
	BDN_log("[%s:%10d] Pthread cont are created",__func__,__LINE__);

}

/* Thread start function to create threads and threads join */
int thread_start() {
	int i, *arr;
	pthread_t* tid;
	tid = (pthread_t*)malloc(sizeof(pthread_t) * MAX_THREADS);
	arr = malloc(sizeof(int) * MAX_THREADS);

	for (i = 0; i < MAX_THREADS; i++) {
		BDN_log("[%s:%10d] Thread creation %d",__func__,__LINE__,i);
		arr[i] = i;
		pthread_create(&tid[i], NULL, thread_func, (void*)&arr[i]);
	}

	for (i = 0; i < MAX_THREADS; i++) {
		pthread_join(tid[i], NULL);
	}

	return 0;
}

/* Thread function which activiely moves the gameplay among the
 * thread pool synchronosly
 */
void* thread_func(void* arg)
{
	int 		turn = *(int*)arg;
	char 		*pName;
	char		*oppName;
	bool		isNodeCreated = false;		 	
	int		i,j;
	int		rc;
	pthread_t	tid;
	bool		amI_Winner = false;
	while (1) {
		pthread_mutex_lock(&mutex);

		if (turn != cnt)
			pthread_cond_wait(&cond[turn], &mutex);
		/* This conditions holds the threads until nodes
		 * are created */
		if(!isNodeCreated) {
			isNodeCreated = true;
			tid = pthread_self();
			pName = include_player(tid); //add the player in the list
			BDN_log("[%s:%10d} Thread num %d Player Name :%s",
					__func__,__LINE__,turn,pName);
		}	
		else {
            		usleep(800);
			/* This makes sure that, only when the node is present
			 * thread shall proceed with boombing its opponent 
			 * This condition might be requried only for the current cycle */ 
			if(isNodePresent(pName) == true) {
				i = rand() % 10;
				j = rand() % 10;
				oppName = findopponent(pName);
				BDN_log("[%s:%10d] Player %s =====ATTACK====> Player Name %s",
						__func__,__LINE__, pName, oppName);
				if(strcmp(pName, oppName) == 0 ) {
					BDN_log(" WINNER WINNER CHICKERN DINNER");
					BDN_log(" Player %s is the Winner ",pName);
					amI_Winner = true;
					break;

				}
				rc = bombPlayer(oppName, i, j);
				if( rc == 1 ) {
					BDN_log("[%s:%10d] Target Player %s  Destoryed",
							__func__,__LINE__,oppName);
					rc=remove_player(oppName); 
				}
			}
		}
		/* this loop makes sure that thread that are exited
		 * are not given turns
		 */
        	while(1)
        	{
			cnt++;
			cnt = cnt%MAX_THREADS;
			if(!itrArr[cnt])
			break;
        	}

		if(( isNodeCreated == true) && (isNodePresent(pName) == false)) {
			BDN_log("[%s:%10d} Thread num %d Player Name :%s is existing cnt %d",
						__func__,__LINE__,turn,pName, turn);
			itrArr[turn] = 1;
			pthread_cond_signal(&cond[cnt]);
			pthread_mutex_unlock(&mutex);
			pthread_exit("DEAD");
		}

		pthread_cond_signal(&cond[cnt]);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}


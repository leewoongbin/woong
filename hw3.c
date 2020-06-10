#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <stdlib.h>

#define M 2048
#define MAX 128

typedef struct thpl{
	int row_s; //last row
	int row; //first row
	int _N;  ///thread count
	int i;   ///idx
	int b_bf;  ///bounded buffer
	float* a;
	float* b;
	float* c;	
}thpl;



sem_t f,e,m;
int fill=0,use=0;
int buf[MAX];

void multi(void* arg){

	thpl* th = (thpl*)arg;
	int i,j,k;
			
	printf("th->i : %d\n",th->i);
	
	for(j=0;j<M;j++)
	{
		for(k=0;k<M;k++)
		{
			th->c[(th->i*M)+j] += th->a[(th->i*M)+k] * th->b[(k*M)+j];
		}
	}

	
}

void put(void* arg){
	thpl* thdpl = (thpl*)arg;	
	buf[fill]=thdpl->i;
	fill=(fill+1)%thdpl->b_bf;
}

int get(void* arg){
	thpl* thdpl = (thpl*)arg;
	int tmp = buf[use];
	use = (use+1)%thdpl->b_bf;
	return tmp;
}

void producer(void* arg){
	int i;
	thpl* thdpl = (thpl*)arg;
	for( i=0 ; i<M ; i++)
	{
	printf("producer  %d\n",i);

		sem_wait(&e);
		sem_wait(&m);

		thdpl->i = i;
		put(thdpl);
				
		sem_post(&m);
		sem_post(&f);
	
	}
	
	
}

void* consumer(void* arg){
	thpl* thdpl = (thpl*)arg;
	int i;

	for( i=thdpl->row ; (i<M)&&(i < thdpl->row_s + 1); i++)
	{
		printf("th->row : %d\n",thdpl->row);
		printf("th->row_s : %d\n",thdpl->row_s);	
		printf("consumer  %d\n",i);
		sem_wait(&f);
		sem_wait(&m);

		thdpl->i = get(thdpl);

		multi(thdpl);
		
		sem_post(&m);
		sem_post(&e);
		

	}

	
}


int main(int argc, char *argv[]){
	int i,j,k;
	int afd, bfd, cfd; /* file descriptor */
  	float *aaddr, *baddr, *caddr;
  	int _M = 0;
  	int _N = 0;
	int count=0;
	
	clock_t ttt,t1=0,t2=0,t3=0,t4=0,tt;
	
	t1=clock();
	
	if (argc < 4){
	    fprintf(stderr, "Usage: %s <matrix1> <matrix2> 				<result_matrix> M N\n", argv[0]);
	    exit(1);
	  }
	  
	_M = atoi(argv[4]);
	if(argv[5] == NULL) _N=1;
	else _N = atoi(argv[5]);
	
	afd = open(argv[1], O_RDONLY);
	assert(afd);
	bfd = open(argv[2], O_RDONLY);
	assert(bfd);

	aaddr = mmap(0, sizeof(float) * M * M, PROT_READ, MAP_PRIVATE, afd,0);
	assert (aaddr != MAP_FAILED);
	baddr = mmap(0, sizeof(float) * M * M, PROT_READ, MAP_PRIVATE, bfd,0);
	assert (baddr != MAP_FAILED);

	cfd=open(argv[3], O_RDWR|O_CREAT|O_TRUNC );
	assert(cfd);
	  
	ftruncate(cfd,sizeof(float)*M*M);
	  
	caddr = mmap(0, sizeof(float) * M * M, PROT_WRITE, MAP_SHARED, cfd,0);
	assert (caddr != MAP_FAILED);  
	
	thpl* thl=(thpl*)malloc(sizeof(thpl));
		
	thl->row=0;  ///init
	thl->row_s=0;  //init
	thl->a = aaddr;
	thl->b = baddr;
	thl->c = caddr;
	thl->_N = _N;
	thl->b_bf = _M;
	thl->i = 0;	
	
	t2=clock();
	
	printf("Read time : %fs\n",(double)(t2-t1)/CLOCKS_PER_SEC);
	

	pthread_t* thrd = (pthread_t*)malloc(sizeof(pthread_t));	   		pthread_t* thrpl = (pthread_t*)malloc(sizeof(pthread_t)*_M);
	
	sem_init(&e,0,_M);
	sem_init(&f,0,0);
	sem_init(&m,0,1);
	

	pthread_create(thrd,NULL,producer,(void*)thl);
	

	for(i=0 ; i<_N ; i++){
		thl->row = M*i/_N;	
		thl->row_s = (i+1)*M/_N-1;
		pthread_create(&thrpl[i],NULL,consumer,(void*)thl);
	}
	
	printf(" created   end\n");


	pthread_join(&thrd,NULL);
	printf("produve  join\n");

	for(i=0 ; i<_N ; i++){
		printf("consumer  join %d\n",i);
		pthread_join(&thrpl[i],NULL);
	}	
		
	
	printf("end\n");

	close(afd);
  	close(bfd);
  	close(cfd);
  	t4=clock();
  	printf("Total time : %fs\n",(double)(t4-t1)/CLOCKS_PER_SEC);
	
	///free(thrd);
  	exit(0);

}

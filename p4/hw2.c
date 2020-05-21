#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/mman.h>
#include <time.h>

#define M 2048
#define MAX_C 10

int main(int argc, char *argv[])
{
  int afd, bfd, cfd; /* file descriptor */
  int i,j,k;
  float *aaddr, *baddr, *caddr;
  float *result=(float*)malloc(sizeof(float)*M*M);
  
  float r=0; 
  pid_t pids[MAX_C];
  int run=0;
  int tidx=0;
  int s=0;

  clock_t t1,t2,t3;

  t3=clock();

  if (argc < 4){
    fprintf(stderr, "Usage: %s <matrix1> <matrix2> <result_matrix>\n", argv[0]);
    exit(1);
  }
  
  

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
 
  if(argv[4]==NULL) run=1;
  else run=atoi(argv[4]);


  while(tidx < run){
  pids[tidx]=fork();
  t2=clock();
  if(pids[tidx]<0) return -1;
  else if(pids[tidx]==0){
 
  	  for( i= M/run*tidx; i<M/run*(1+tidx) ; i++)
	  {
	  	  for(j=0;j<M;j++)
	  	  {
	  		  for(k=0;k<M;k++)
			  {
		 		  *(caddr+(i*M)+j) += *(aaddr+(i*M)+k) * *(baddr+(k*M)+j);
		  	  }	
	  	  }

 	 	 
	  }
	 t1=clock();
  	 printf("%d child process time : %fs\n",tidx+1,(double)(t1-t2)/CLOCKS_PER_SEC);
	 

	 exit(0);

  }else{
	wait(&s);
  }
   tidx++;
  }
  t1=clock();
  printf("parent process time : %fs\n",(double)(t1-t3)/CLOCKS_PER_SEC);

  close(afd);
  close(bfd);
  close(cfd);
  exit(0);
}

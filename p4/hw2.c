#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

#define M 2048



int main(int argc, char* argv[])
{
  pid_t pids[4];
  int i,j,k;
  int tidx=0;
  int t=0;
  int afd,bfd,cfd;
  float *aaddr, *baddr, *caddr;
  
  int runProcess=atoi(argv[4]);

  afd = open(argv[1],O_RDONLY);
  assert(afd);
  bfd = open(argv[2],O_RDONLY);
  assert(bfd);
  cfd = open(argv[3],O_RDWR|O_CREAT|O_TRUNC);
  assert(cfd);

  ftruncate(cfd, sizeof(float)*M*M);

  aaddr = mmap(0, sizeof(float)*M*M, PROT_READ, MAP_PRIVATE, afd, 0);
  assert(aaddr!=MAP_FAILED);
  baddr = mmap(0, sizeof(float)*M*M, PROT_READ, MAP_PRIVATE, bfd,0);
  assert(baddr!=MAP_FAILED);
  caddr = mmap(0, sizeof(float)*M*M, PROT_WRITE, MAP_SHARED,cfd, 0);
  assert(caddr!=MAP_FAILED);

  while(tidx < runProcess){
    pids[tidx]=fork();

    if(pids[tidx]<0){
     return -1;
    }else if(pids[tidx]==0){
	printf("chiled process %d runProcess is %d \n", getpid(), runProcess);
	
	for(i=M/runProcess*tidx ; i < M/runProcess*(1+tidx) ; i++)
	{
	 for(j=0;j<M;j++)
	 {
	  for(k=0;k<M;k++)
	  {
	   *(caddr+(i*M)+j) += *(aaddr+(i*M)+k) * *(baddr+(k*M)+j);
	  }
	 }
	}


    }else{
	printf("parents process %d  \n",getpid());

    }


    tidx++;
  }

  close(afd);
  close(bfd);
  close(cfd);
  exit(0);

}

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

int main(int argc, char *argv[])
{
  int i,j,k;
  int afd, bfd, cfd; /* file descriptor */
  float *aaddr, *baddr, *caddr;
  float *result=(float*)malloc(sizeof(float)*M*M);
  
  clock_t t1, t2;


  if (argc < 4){
    fprintf(stderr, "Usage: %s <matrix1> <matrix2> <result_matrix>\n", argv[0]);
    exit(1);
  }
  
  t1 = clock();

  afd = open(argv[1], O_RDONLY);
  assert(afd);
  bfd = open(argv[2], O_RDONLY);
  assert(bfd);
  
  
  aaddr = mmap(0, sizeof(float) * M * M, PROT_READ, MAP_PRIVATE, afd,0);
  assert (aaddr != MAP_FAILED);
  baddr = mmap(0, sizeof(float) * M * M, PROT_READ, MAP_PRIVATE, bfd,0);
  assert (baddr != MAP_FAILED);
  
  t2 = clock();

  printf("Read two file time : %fs\n",(double)(t2-t1)/CLOCKS_PER_SEC);

  
  t1=clock();
  
  for( i=0; i<M ; i++)
  {
	for(j=0;j<M;j++)
	{
		for(k=0;k<M;k++)
		{
			*(result+(i*M)+j) += *(aaddr+(i*M)+k) * *(baddr+(k*M)+j);
		}	
	}
  }
  

  t2=clock();
 
  printf("Matrix Multi time : %fs\n",(double)(t2-t1)/CLOCKS_PER_SEC);

  t1=clock();
  cfd=open(argv[3], O_RDWR );
  assert(cfd);
  
  ftruncate(cfd,sizeof(float)*M*M);
  
  caddr = mmap(0, sizeof(float) * M * M, PROT_WRITE, MAP_SHARED, cfd,0);
  assert (caddr != MAP_FAILED);

  for(i=0;i<M;i++)
  {
	for(j=0;j<M;j++)
	{
		*(caddr+(i*M)+j)=*(result+(i*M)+j);
	}
  }


  t2=clock();

  printf("input matrix time : %fs\n",(double)(t2-t1)/CLOCKS_PER_SEC);
  
  close(afd);
  close(bfd);
  close(cfd);
  exit(0);
}

/* 두 파일이 2048*2048개의 float 실수 (행렬)들로 이루어져 있고, 두 파일의 각 실수들의 차이가 세 번째 argument보다 크면 error를 출력한다. 두 파일의 크기는 4*2048*2048byte이어야 한다.
   compile: gcc -g -Wall -o hwdiff hwdiff.c -lm
   실행예: ./hwdiff c1.dat c2.dat 0.0001
 */ 
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
  float diff;
  int afd, bfd, cfd; /* file descriptor */
  float *aaddr, *baddr, *caddr;
  float *result=(float*)malloc(sizeof(float)*M*M);
  
  clock_t t1, t2;


  if (argc < 4){
    fprintf(stderr, "Usage: %s <file1> <file2> <diff>\n", argv[0]);
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

  diff = atof(argv[3]);
   
  for (i = 0; i < M * M; i++){
    if (fabsf(aaddr[i] - baddr[i]) > diff){
       printf("Values %f and %f at location %d are different.\n", aaddr[i], baddr[i], i);
       break;
     }
   }
  

  
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
  cfd=open(argv[3], O_RDWR | O_CREAT | O_TRUNC);
  assert(cfd);
  
  ftruncate(cfd,sizeof(float)*M*M);
  
  caddr = mmap(0, sizeof(float) * M * M,PROT_READ | PROT_WRITE, MAP_SHARED, cfd,0);
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

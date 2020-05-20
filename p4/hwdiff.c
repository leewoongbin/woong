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
  FILE *afd, *bfd, *cfd; /* file descriptor */
  int i,j,k;
  float *aaddr, *baddr, *caddr;
  float *result=(float*)malloc(sizeof(float)*M*M);
  
  clock_t t1, t2;


  if (argc < 4){
    fprintf(stderr, "Usage: %s <matrix1> <matrix2> <result_matrix>\n", argv[0]);
    exit(1);
  }

  afd=fopen(argv[1],"rb");
  bfd=fopen(argv[2],"rb");
  cfd=fopen(argv[3],"wb");

  aaddr=(float*)malloc(sizeof(float)*M*M);
  baddr=(float*)malloc(sizeof(float)*M*M);
  caddr=(float*)malloc(sizeof(float)*M*M);

  for(i=0;i<M;i++)
  {
	for(j=0;j<M;j++)
	{
		fread((aaddr+(i*M)+j),sizeof(float),1,afd);
		fread((baddr+(i*M)+j),sizeof(float),1,bfd);
	}
  }
  
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
 for(i=0;i<M;i++)
  {
	for(j=0;j<M;j++)
	{
	///	*(caddr+(i*M)+j)=*(result+(i*M)+j);
	}
  }

  for( i=0; i<M ; i++)
  {
	for(j=0;j<M;j++)
	{
		for(k=0;k<M;k++)
		{
			*(caddr+(i*M)+j) += *(aaddr+(i*M)+k) * *(baddr+(k*M)+j);
		}	
	}
  }

  
  for(i=0;i<M;i++)
  {
	for(j=0;j<M;j++)
	{
		fwrite((caddr+(i*M)+j),sizeof(float),1,cfd);
	}
  }
 
  
  fclose(afd);
  fclose(bfd);
  fclose(cfd);
  exit(0);
}

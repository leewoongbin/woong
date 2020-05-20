#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#define M_C 10
#define M 6 
#define RUN 3

int gla_var=0;

void print(int* p)
{
	int i,j;

	for(i=0;i<M;i++)
	{
		for(j=0;j<M;j++)
		{
			printf("%d ",*(p+(M*i)+j));
		}printf("\n");
	}

}

void fpn(FILE* fp, int* q, int s, int n)
{
	int i;

	for(i=s;i<n+s;i++)
	{
		fprintf(fp,"%d ",q+i); 
	}

}


int main()
{
	pid_t pids[M_C], child;
	int tidx = 0;
	int run=5;
	int i,j,k;

	FILE* fp =fopen("text.txt","w");
	int* p =(int*)malloc(sizeof(int)*M*M);
	int* q =(int*)malloc(sizeof(int)*M*M);
	int* r =(int*)malloc(sizeof(int)*M*M);
	int* tmp = (int*)malloc(sizeof(int)*M*M);
	memset(tmp,0,sizeof(int)*M*M);
	memset(r,0,sizeof(int)*M*M);
	memset(p,0,sizeof(int)*M*M);
	memset(q,0,sizeof(int)*M*M);
	
	for(i=0;i<M*M;i++)
	{
		*(p+i)=i;
		*(q+i)=i;
	}



	while(tidx<RUN)
	{
		pids[tidx]=fork();
		if(pids[tidx]<0) return -1;
		else if(pids[tidx]==0){
			for(i=M/RUN*tidx;i<M/RUN*(1+tidx);i++)
			{
				for(j=0;j<M;j++)
				{
					for(k=0;k<M;k++)
					{
						*(r+(i*M)+j) += *(p+(i*M)+k) *  *(q+(k*M)+j); 
							
					}
				}
			}
				for(j=tidx*M*M/RUN;j<M*M/RUN*(1+tidx);j++)
				{
					*(tmp+j)=*(r+j);
				}
		

			for(j=tidx*M*M/RUN;j<M*M/RUN*(1+tidx);j++)
				{
					printf("%d ",j);
					fprintf(fp,"%d ",*(r+j));
				}
			printf("==============================\n");
				
			exit(0);	
		}else{
			
		}


		tidx++;
	}
	

	fclose(fp);
}

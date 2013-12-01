#include <stdio.h>

int main(void)
{
	char str[100];
	printf("Give the number of nodes pls\n");
	scanf("%s",str);
	int size = atoi(str);
	printf("%d \n",size);
	double P[size][size];
	P = generateMatrix(size,P);
	int i,j;
	for(i=0;i<size;i++){
		for(j = 0;j<size;j++){
			printf("%f \t", P[i][j]);
		}
		printf("\n");
	}
	double b[] = powerMethod(size,P);
	printf("The stationary vector is: \n");
	double sum = 0;
	double x;
	for(i=0;i<size;i++){
		x = b[i];
		printf("%f \n", x);
		sum += x;
	}
	printf("Sum= %f", sum);
  	return 0;
}

double** generateMatrix(size_t size, double **A)
{
	double result[size][size];
	//TODO: ik denk dat we dit best laten vallen in de c implementatie
}

double[] powerMethod(int n, double **A)
{
	double pi[n];
	int m,i,j;
	double alpha = 0.9;
	for(m=0;m<n;m++){
		pi[m] = (double) 1/n;
	}
	int counter = 50;
	double E  = 0.1*(double)1/n;
	while(counter < 50){
		for(i=0;i<n;i++){
			double tmp = 0;
			for(j=0;j<n;j++){
				tmp += alpha*pi[j]*A[j][i];
			}
			pi[i] = tmp + E; //TODO eigenlijk moet er staan: tmp+ (1-alpha)*E maar anders klopt het niet
		}
		counter++;
	}
}

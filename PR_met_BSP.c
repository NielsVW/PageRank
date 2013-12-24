#include <mcbsp.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define _POSIX_C_SOURCE >= 199309L

static int count;
static const double fudge_factor = 0.9;
static double vector[4];
static double vector_tmp[4];

//initialisation function for ip
void ip_init( double **ip_buffer ) {
	const size_t size = bsp_nprocs() * sizeof(double);
	*ip_buffer = malloc( size );
	bsp_push_reg( *ip_buffer, size );
}

//calculates the inner-product from local vectors
double ip( double *x, double *y, double *ip_buffer, size_t np ) {
	double alpha = 0.0;
	for( size_t i = 0; i < np; ++i ){
		//printf("In thread %d: value of x[%d] = %f and y=[%d] = %f \n",bsp_pid(),i,x[i],i,y[i]);
		alpha += fudge_factor * x[ i ] * y[ i ];
	}
	for( unsigned int k = 0; k < bsp_nprocs(); ++k ) {
		bsp_put( k, &alpha, ip_buffer,bsp_pid()*sizeof(double), sizeof(double) );
	}
	bsp_sync();
	for( unsigned int k = 1; k < bsp_nprocs(); ++k )
		ip_buffer[ 0 ] += ip_buffer[ k ];
	return ip_buffer[ 0 ];
}

//Vb: {{1,2,3}{4,5,6}{7,8,9}} --> {1,4,7,2,5,8,3,6,9}
//deze functie vult een array met een geflattende 2D array volgens rij
void fill_matrix(size_t M, double matrix[M*M]) {
	unsigned int x,y;
	for(x=0;x<M;x++){
		for(y=0;y<M;y++){
			matrix[x*M + y] = y*M + x;
		}
	}
}

void spmd() {
	const size_t M = 4;
	double matrix[] = {0.2,0.2,0.5,0.0,0.2,0.2,0.0,0.0,0.2,0.2,0.0,0.0,0.2,0.2,0.0,1}; //test matrix
	//double matrix[M*M];
	//fill_matrix(M, matrix);

	bsp_begin( bsp_nprocs() );
	double *ip_buffer, *x, *y;
	size_t np;
	ip_init( &ip_buffer );
	np = M/bsp_nprocs();
	//double x_array[] = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,0.10,0.11,0.12};
	x = vector+(bsp_pid()*np);
	y = matrix;
	y = y + count*M + (bsp_pid()*np);
	bsp_sync();

	//printf("1e element(k=%d) in thread %d van y= %f\n",k,bsp_pid(),*y);
	double alpha = ip( x, y, ip_buffer, np );
	alpha+= 0.1*((double) 1/M); // this is now the vector-matrix product of pi(stationary vector and P (stochastic matrix) + E
	//printf("alpha thread %d = %f met k=%d\n",bsp_pid(),alpha,count);
	
	//printf("alpha van thread %d = %f.(k= %d)\n",bsp_pid(),alpha,k);
	bsp_end();
	vector_tmp[count] = alpha;
}

int main( int argc, char **argv ) {
	
	#define BILLION  1E9

	const int size = 4;
	
	for(unsigned int l = 0;l < size;l++)
		vector[l] = (double) 1/size;

	struct timespec start, end;
	clock_gettime(clock_getcpuclockid() , &start);
	//start
	for(unsigned int w = 0;w<50;w++){ // Power method
		count = 0;
		for(int k = 0;k < size;k++){
			bsp_init( &spmd, argc, argv );
			spmd();
			count++;
		}
		memcpy(&vector,&vector_tmp,8*size);
	}
	//end
	clock_gettime(clock_getcpuclockid(), &end);

	// Calculate time it took
	double accum = (double) ( end.tv_sec - start.tv_sec ) + (double) ( end.tv_nsec - start.tv_nsec ) / BILLION;
	printf( "Time taken: %lf\n", accum );

	for(unsigned int o = 0;o < size;o++)
		printf("Stationary vector [%d] = %f\n",o,vector_tmp[o]);
}



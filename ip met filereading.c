#include <mcbsp.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define _POSIX_C_SOURCE >= 199309L
#define N 8

static int count;
static const double fudge_factor = 0.9;
static double vector[N];
static double vector_tmp[N];
static double matrix[N*N];

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

void fill_matrix(size_t M, double matrix[M*M]) {
	unsigned int x,y;
	for(x=0;x<M;x++){
		for(y=0;y<M;y++){
			matrix[x*M + y] = y*M + x;
		}
	}
}

//example usage
void spmd() {
	const size_t M = N;
	//double matrix[] = {0.0,0.125,0.0,0.0,0.0,0.25,0.0,0.125,0.5,0.125,0.5,0.0,0.0,0.25,0.3333333333333333,0.125,0.0,0.125,0.0,1.0,0.0,0.25,0.0,0.125,0.0,0.125,0.0,0.0,0.25,0.0,0.0,0.125,0.5,0.125,0.0,0.0,0.25,0.0,0.3333333333333333,0.125,0.0,0.125,0.0,0.0,0.25,0.25,0.0,0.125,0.0,0.125,0.0,0.0,0.0,0.0,0.0,0.125,0.0,0.125,0.5,0.0,0.25,0.0,0.3333333333333333,0.125};
	//double matrix[M*M];
	//fill_matrix(M, matrix);

	bsp_begin( bsp_nprocs() );
	double *ip_buffer, *x, *y;
	size_t np;
	ip_init( &ip_buffer );
	//more initialisation calls to set x, y, np, and others
	np = M/bsp_nprocs();

	x = vector+(bsp_pid()*np);
	y = matrix;
	y = y + count*M + (bsp_pid()*np);
	bsp_sync();

	//printf("1e element(k=%d) in thread %d van y= %f\n",k,bsp_pid(),*y);
	double alpha = ip( x, y, ip_buffer, np );
	alpha+= 0.1*((double) 1/M); // we do this + E
	//printf("alpha thread %d = %f met k=%d\n",bsp_pid(),alpha,count);
	
	//printf("alpha van thread %d = %f.(k= %d)\n",bsp_pid(),alpha,k);
	bsp_end();
	vector_tmp[count] = alpha;
}

int main( int argc, char **argv ) {
	
	#define BILLION  1E9

	const int size = N;
	/*struct timespec start, end;
	clock_gettime(clock_getcpuclockid() , &start);
	bsp_init( &spmd, argc, argv );	
	spmd();
	clock_gettime(clock_getcpuclockid(), &end);*/

	FILE *file = fopen("matrix.txt", "r");
    	int b=0;
   	double num;
	double doubles[N*N];
        while(fscanf(file, "%lf", &num) > 0) {
        	doubles[b] = num;
	        b++;
    	}
	
	memcpy(&matrix,&doubles,8*N*N);
	//printf("%lf %lf %lf %lf %lf\n",matrix[0],matrix[1],matrix[2],matrix[3],matrix[4]);

	for(unsigned int l = 0;l < size;l++)
		vector[l] = (double) 1/size;

	struct timespec start, end;
	clock_gettime(clock_getcpuclockid() , &start);
	//start
	for(unsigned int w = 0;w<50;w++){
		count = 0;
		for(int k = 0;k < size;k++){
			bsp_init( &spmd, argc, argv );
			spmd();
			count++;
		}
		memcpy(&vector,&vector_tmp,8*size);
		memcpy(&matrix,&doubles,8*N*N);
	}
	//end

	clock_gettime(clock_getcpuclockid(), &end);
	// Calculate time it took
	double accum = (double) ( end.tv_sec - start.tv_sec ) + (double) ( end.tv_nsec - start.tv_nsec ) / BILLION;
	printf( "Time taken: %lf\n", accum );
	for(unsigned int o = 0;o < size;o++)
		printf("vector_tmp[%d] = %f en vector[%d] = %f\n",o,vector_tmp[o],o,vector[o]);
	fclose(file);
}



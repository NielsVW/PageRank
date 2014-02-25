/*
 * Copyright (c) 2012 by Albert-Jan N. Yzelman
 *
 * This file is part of MulticoreBSP in C --
 *        a port of the original Java-based MulticoreBSP.
 *
 * MulticoreBSP for C is distributed as part of the original
 * MulticoreBSP and is free software: you can redistribute
 * it and/or modify it under the terms of the GNU Lesser 
 * General Public License as published by the Free Software 
 * Foundation, either version 3 of the License, or 
 * (at your option) any later version.
 * MulticoreBSP is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU Lesser General Public 
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General 
 * Public License along with MulticoreBSP. If not, see
 * <http://www.gnu.org/licenses/>.
 */

//for documentation see the .h file:
#include "mcbsp-internal.h"

//default affinity field
enum mcbsp_affinity_mode MCBSP_DEFAULT_AFFINITY;

//default number of threads per core field
size_t MCBSP_DEFAULT_THREADS_PER_CORE = 1;

//default thread numbering field
enum mcbsp_thread_numbering MCBSP_DEFAULT_THREAD_NUMBERING;

void mcbsp_set_maximum_threads( const size_t max ) {
	struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	machine->threads = max;
	machine->Tset = true;
}

void mcbsp_set_affinity_mode( const enum mcbsp_affinity_mode mode ) {
	struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	machine->affinity = mode;
	machine->Aset = true;
}

void mcbsp_set_available_cores( const size_t num_cores ) {
	struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	machine->cores = num_cores;
	machine->Cset = true;
}

void mcbsp_set_threads_per_core( const size_t TpC ) {
	struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	machine->threads_per_core = TpC;
	machine->TpCset = true;
}

void mcbsp_set_thread_numbering( const enum mcbsp_thread_numbering numbering ) {
	struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	machine->thread_numbering = numbering;
	machine->TNset = true;
}

void mcbsp_set_pinning( const size_t * const pinning, const size_t length ) {
	//get machine info
	struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	//set number of threads
	machine->threads = length;
	//check for NULL
	if( pinning == NULL ) {
		machine->manual_affinity = NULL;
		if( machine->affinity != MANUAL ) {
			machine->affinity = MCBSP_DEFAULT_AFFINITY;
			fprintf( stderr, "Warning: mcbsp_set_pinning called with NULL pinning. Updated supported threads as normal, but reset affinity strategy to defaults.\n" );
		}
	}
	//allocate pinning array
	machine->manual_affinity = malloc( length * sizeof( size_t ) );
	//check if successfully allocated
	if( machine->manual_affinity == NULL ) {
		fprintf( stderr, "Error: could not allocate manual affinity vector of size %lu (=#maximum threads)!\n", (unsigned long int)machine->threads );
		mcbsp_util_fatal();
	}
	//copy array
	for( size_t s = 0; s < machine->threads; ++s ) {
		machine->manual_affinity[ s ] = pinning[ s ];
	}
	//also set affinity
	machine->affinity = MANUAL;
	//set manual flags
	machine->Tset = true;
	machine->Pset = true;
	machine->Aset = true;
}

void mcbsp_set_reserved_cores( const size_t * const reserved, const size_t length ) {
	//get machine info
	struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	//check for NULL
	if( reserved == NULL ) {
		machine->num_reserved_cores = 0;
		machine->reserved_cores = NULL;
		//check for sane user input
		if( length != 0 ) {
			fprintf( stderr, "Error: mcbsp_set_reserved_cores called with NULL array while non-zero number of reserved cores was given!\n" );
			mcbsp_util_fatal();
		}
	}
	//set number of cores
	machine->num_reserved_cores = length;
	//allocate reserved array
	machine->reserved_cores = malloc( length * sizeof( size_t ) );
	//check
	if( machine->reserved_cores == NULL ) {
		fprintf( stderr, "Error: could not allocate reserved cores vector of size %lu!\n", (unsigned long int)length );
		mcbsp_util_fatal();
	}
	//copy
	for( size_t s = 0; s < length; ++s )
		machine->reserved_cores[ s ] = reserved[ s ];
	//set manual flag
	machine->Rset = true;
}

size_t mcbsp_get_maximum_threads() {
        const struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
        return machine->threads;
}

enum mcbsp_affinity_mode mcbsp_get_affinity_mode() {
	const struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	return machine->affinity;
}

size_t mcbsp_get_available_cores() {
	const struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	return machine->cores;
}
	
size_t mcbsp_get_threads_per_core() {
	const struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	return machine->threads_per_core;
}

enum mcbsp_thread_numbering mcbsp_get_thread_numbering() {
	const struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	return machine->thread_numbering;
}

size_t * mcbsp_get_pinning() {
	const struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	//check boundary condition
	if( machine->manual_affinity == NULL )
		return NULL;
	//not null, so allocate array copy
	const size_t size = machine->threads;
	size_t * ret = malloc( size * sizeof( size_t ) );
	if( ret == NULL ) {
		fprintf( stderr, "Error: could not allocate copy of manual affinity array (size %ld)!\n", (unsigned long int)size );
		mcbsp_util_fatal();
	}
	//do copy
	for( size_t s = 0; s < size; ++s )
		ret[ s ] = machine->manual_affinity[ s ];
	//do return
	return ret;
}

size_t mcbsp_get_reserved_cores_number() {
	const struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	return machine->num_reserved_cores;
}

size_t * mcbsp_get_reserved_cores() {
	const struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
	//check for NULL
	if( machine->reserved_cores == NULL )
		return NULL;
	//otherwise create copy
	const size_t size = machine->num_reserved_cores;
	size_t * ret = malloc( size * sizeof( size_t ) );
	if( ret == NULL ) {
		fprintf( stderr, "Error: could not allocate copy of reserved cores array (size %ld)!\n", (unsigned long int)size );
		mcbsp_util_fatal();
	}
	//do copy
	for( size_t s = 0; s < size; ++s )
		ret[ s ] = machine->reserved_cores[ s ];
	//do return
	return ret;
}

void bsp_begin( const MCBSP_PROCESSOR_INDEX_DATATYPE P ) {
	struct mcbsp_init_data * const init = bsp_begin_check();

	//if the check did not return an init struct, we are a
	//spawned thread and should just continue the SPMD
	//code.
	if( init == NULL ) {
#ifdef MCBSP_ENABLE_HP_DIRECTIVES
		//required, since HP directives assume allocated thread-local comm. queues
		const struct mcbsp_thread_data * const data = mcbsp_internal_const_prefunction();
#ifdef MCBSP_USE_SPINLOCK
		mcbsp_internal_syncWithCondition( data->init, data->init->mid_condition, (size_t)(data->bsp_id) );
#else
		mcbsp_internal_syncWithCondition( data->init, &(data->init->mid_condition) );
#endif
#endif
		return;
	}

	//otherwise we need to start the SPMD code 
	int *pinning = mcbsp_util_pinning( P, mcbsp_internal_getMachineInfo() );
	if( pinning == NULL ) {
		fprintf( stderr, "Could not get a valid pinning!\n" );
		mcbsp_util_fatal();
	}

#ifdef MCBSP_SHOW_PINNING
	//report pinning:
	fprintf( stdout, "Info: pinning used is" );
	for( size_t s=0; s<P; ++s )
		fprintf( stdout, " %d", pinning[ s ] );
	fprintf( stdout, "\n" );
#endif

	init->threads = malloc( P * sizeof( pthread_t ) );
	if( init->threads == NULL ) {
		fprintf( stderr, "Could not allocate new threads!\n" );
		mcbsp_util_fatal();
	}

	pthread_attr_t attr;

#ifdef __MACH__
	const struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
#elif _WIN32
	DWORD_PTR mask;
#else
	cpu_set_t mask;
#endif

	//further initialise init object
	init->P     = P;
	init->abort = false;
	init->ended = false;
	init->sync_entry_counter = 0;
	init->sync_exit_counter  = 0;
	pthread_mutex_init( &(init->mutex), NULL );
#ifdef MCBSP_USE_SPINLOCK
	init->condition     = malloc( P * sizeof( unsigned char ) );
	init->mid_condition = malloc( P * sizeof( unsigned char ) );
	if( init->condition == NULL || init->mid_condition == NULL ) {
		fprintf( stderr, "Could not allocate spinlock conditions!\n" );
		mcbsp_util_fatal();
	}
	for( size_t s = 0; s < P; ++s )
		init->condition[ s ] = init->mid_condition[ s ] = 0;
#else
	pthread_cond_init ( &(init->condition), NULL );
	pthread_cond_init ( &(init->mid_condition), NULL );
#endif
	mcbsp_util_address_table_initialise( &(init->global2local), P );
	init->threadData = malloc( P * sizeof( struct mcbsp_thread_data * ) );
	init->prev_data  = pthread_getspecific( mcbsp_internal_thread_data );
	init->tagSize = 0;

	//spawn P-1 threads.
	for( size_t s = (size_t)P - 1; s < (size_t)P; --s ) {
		//allocate new thread-local data
		struct mcbsp_thread_data *thread_data = malloc( sizeof( struct mcbsp_thread_data ) );
		if( thread_data == NULL ) {
			fprintf( stderr, "Could not allocate local thread data!\n" );
			mcbsp_util_fatal();
		}
		//provide a link to the SPMD program init struct
		thread_data->init   = init;
		//set local ID
		thread_data->bsp_id = s;
		//set the maximum number of registered globals at any time (0, since SPMD not started yet)
		thread_data->localC = 0;
		//initialise local to global map
		mcbsp_util_address_map_initialise( &(thread_data->local2global ) );
		//initialise stack used for efficient registration of globals after de-registrations
		mcbsp_util_stack_initialise( &(thread_data->removedGlobals), sizeof( unsigned long int ) );
		//initialise stack used for de-registration of globals
		mcbsp_util_stack_initialise( &(thread_data->localsToRemove), sizeof( void * ) );
		//initialise stacks used for communication
		thread_data->request_queues = malloc( P * sizeof( struct mcbsp_util_stack ) );
		thread_data->queues         = malloc( P * sizeof( struct mcbsp_util_stack ) );
		thread_data->hpsend_queues  = malloc( P * sizeof( struct mcbsp_util_stack ) );
		for( size_t i = 0; i < (size_t)P; ++i ) {
			mcbsp_util_stack_initialise( &(thread_data->request_queues[ i ]), sizeof( struct mcbsp_get_request ) );
			mcbsp_util_stack_initialise( &(thread_data->queues[ i ]),         sizeof( struct mcbsp_message) );
			mcbsp_util_stack_initialise( &(thread_data->hpsend_queues[ i ]),  sizeof( struct mcbsp_hpsend_request) );
		}
		//initialise default tag size
		thread_data->newTagSize = 0;
		//initialise BSMP queue
		mcbsp_util_stack_initialise( &(thread_data->bsmp), sizeof( size_t ) );
		//initialise push request queue
		mcbsp_util_stack_initialise( &(thread_data->localsToPush), sizeof( struct mcbsp_push_request ) );
		//initialise the map-search stack
		mcbsp_util_stack_initialise( &(thread_data->globalsToPush), sizeof( unsigned long int ) );
		//provide a link back to this thread-local data struct
		init->threadData[ s ] = thread_data;

		//spawn new threads if s>0
		if( s > 0 ) {
			//create POSIX threads attributes
			//(currently for pinning, if supported)
			pthread_attr_init( &attr );

#ifdef _WIN32
			mask = (DWORD_PTR)1;
			mask <<= pinning[ s ];
#elif !defined(__MACH__)
			CPU_ZERO( &mask );
			CPU_SET ( pinning[ s ], &mask );
			pthread_attr_setaffinity_np( &attr, sizeof( cpu_set_t ), &mask );
#endif

			//spawn the actual thread
			const int pthr_rval = pthread_create( &(init->threads[ s ]), &attr, mcbsp_internal_spmd, thread_data );
			if( pthr_rval != 0 ) {
				fprintf( stderr, "Could not spawn new thread (%s)!\n", strerror( pthr_rval ) );
				mcbsp_util_fatal();
			}

#ifdef _WIN32
			//do after-creation pinning
			SetThreadAffinityMask( pthread_getw32threadhandle_np( init->threads[ s ] ), mask );
#elif __MACH__
			//set after-creation affinities
			thread_port_t osx_thread = pthread_mach_thread_np( init->threads[ s ] );
			struct thread_affinity_policy ap;
			switch( machine->affinity ) {
				case SCATTER:
				{
					//Affinity API release notes do not specify whether 0 is a valid tag, or in fact equal to NULL; so 1-based to be sure
					ap.affinity_tag = s + 1;
					break;
				}
				case COMPACT:
				{
					ap.affinity_tag = 1;
					break;
				}
				case MANUAL:
				{
					ap.affinity_tag = machine->manual_affinity[ s ];
					break;
				}
				default:
				{
					fprintf( stderr, "Unhandled affinity type for Mac OS X!\n" );
					mcbsp_util_fatal();
				}
			}
			thread_policy_set( osx_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&ap, THREAD_AFFINITY_POLICY_COUNT );
#endif

			//destroy attributes object
			pthread_attr_destroy( &attr );
		} else {
			//continue ourselves as bsp_id 0. Do pinning
#ifdef __MACH__
			thread_port_t osx_thread = pthread_mach_thread_np( pthread_self() );
			struct thread_affinity_policy ap;
			if( machine->affinity == SCATTER || machine->affinity == COMPACT )
				ap.affinity_tag = 1;
			else if( machine->affinity == MANUAL )
				ap.affinity_tag = machine->manual_affinity[ s ];
			else {
				fprintf( stderr, "Unhandled affinity type for Mac OS X!\n" );
				mcbsp_util_fatal();
			}
			thread_policy_set( osx_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&ap, THREAD_AFFINITY_POLICY_COUNT );
#elif _WIN32
			DWORD_PTR mask = 1;
			mask <<= pinning[ s ];
			SetThreadAffinityMask( GetCurrentThread(), mask );
#else
			CPU_ZERO( &mask );
			CPU_SET ( pinning[ s ], &mask );
			if( pthread_setaffinity_np( pthread_self(), sizeof( cpu_set_t ), &mask ) != 0 ) {
				fprintf( stderr, "Could not pin master thread to requested hardware thread (%d)!\n", pinning[ s ] );
				mcbsp_util_fatal();
			}
#endif
			//record our own descriptor
			init->threads[ 0 ] = pthread_self();
			//copy part of mcbsp_internal_spmd.
			const int rc = pthread_setspecific( mcbsp_internal_thread_data, thread_data );
			if( rc != 0 ) {
				fprintf( stderr, "Could not store thread-local data in continuator thread!\n" );
				fprintf( stderr, "(%s)\n", strerror( rc ) );
				mcbsp_util_fatal();
			}

			//set start time
#ifdef __MACH__
			//get rights for accessing Mach's timers
			const kern_return_t rc1 = host_get_clock_service( mach_host_self(), SYSTEM_CLOCK, &(thread_data->clock) );
			if( rc1 != KERN_SUCCESS ) {
				fprintf( stderr, "Could not access the Mach system timer (%s)\n", mach_error_string( rc1 ) );
				mcbsp_util_fatal();
			}
			const kern_return_t rc2 = clock_get_time( thread_data->clock, &(thread_data->start) );
			if( rc2 != KERN_SUCCESS ) {
				fprintf( stderr, "Could not get starting time (%s)\n", mach_error_string( rc2 ) );
				mcbsp_util_fatal();
			}
#elif _WIN32
			QueryPerformanceCounter( &(thread_data->start) );
			QueryPerformanceFrequency( &(thread_data->frequency) );
#else
			clock_gettime( CLOCK_MONOTONIC, &(thread_data->start) );
#endif
			//this enables possible BSP-within-BSP execution.
			if( pthread_setspecific( mcbsp_internal_init_data, NULL ) != 0 ) {
				fprintf( stderr, "Could not reset initialisation data to NULL on SPMD start!\n" );
				mcbsp_util_fatal();
			}
		}
	}
	//free pinning
	free( pinning );
#ifdef MCBSP_ENABLE_HP_DIRECTIVES
	//required, since HP directives assume allocated thread-local comm. queues
#ifdef MCBSP_USE_SPINLOCK
	mcbsp_internal_syncWithCondition( init, init->mid_condition, (size_t)0 );
#else
	mcbsp_internal_syncWithCondition( init, &(init->mid_condition) );
#endif
#endif
}

void bsp_end() {
	//get thread-local data
	struct mcbsp_thread_data * const data = pthread_getspecific( mcbsp_internal_thread_data );
	if( data == NULL ) {
		fprintf( stderr, "Error: could not get thread-local data in call to bsp_abort( error_message )!\n" );
		mcbsp_util_fatal();
	}

	//record end
	data->init->ended = true;

#ifdef MCBSP_USE_SPINLOCK
	mcbsp_internal_sync( data->init, (size_t)(data->bsp_id) );
#else
	mcbsp_internal_sync( data->init );
#endif

	//set thread-local data to NULL
	if( pthread_setspecific( mcbsp_internal_thread_data, NULL ) != 0 ) {
		fprintf( stderr, "Could not set thread-local data to NULL on thread exit.\n" );
		mcbsp_util_fatal();
	}

	//free data and exit gracefully,
#ifdef __MACH__
	mach_port_deallocate( mach_task_self(), data->clock );
#endif
	mcbsp_util_address_map_destroy( &(data->local2global) );
	mcbsp_util_stack_destroy( &(data->removedGlobals) );
	mcbsp_util_stack_destroy( &(data->localsToRemove) );
	for( MCBSP_PROCESSOR_INDEX_DATATYPE s = 0; s < data->init->P; ++s ) {
		mcbsp_util_stack_destroy( &(data->request_queues[ s ]) );
		mcbsp_util_stack_destroy( &(data->queues[ s ]) );
		mcbsp_util_stack_destroy( &(data->hpsend_queues[ s ]) );
	}
	free( data->request_queues );
	free( data->queues );
	free( data->hpsend_queues );
	mcbsp_util_stack_destroy( &(data->bsmp) );
	mcbsp_util_stack_destroy( &(data->localsToPush) );
	mcbsp_util_stack_destroy( &(data->globalsToPush) );

	//exit if not master thread
	if( data->bsp_id != 0 ) {
		//free thread-local data
		free( data );
		pthread_exit( NULL );
	}

	//master thread cleans up init struct
	struct mcbsp_init_data *init = data->init;

	//that's everything we needed from the thread-local data struct
	free( data );

	//wait for other threads
	for( MCBSP_PROCESSOR_INDEX_DATATYPE s = 1; s < init->P; ++s )
		pthread_join( init->threads[ s ], NULL );

	//destroy mutex and conditions
	pthread_mutex_destroy( &(init->mutex) );
#ifdef MCBSP_USE_SPINLOCK
	free( init->condition );
	free( init->mid_condition );
#else
	pthread_cond_destroy( &(init->    condition) );
	pthread_cond_destroy( &(init->mid_condition) );
#endif

	//destroy global address table
	mcbsp_util_address_table_destroy( &(init->global2local) );

	//destroy pointers to thread-local data structs
	free( init->threadData );

	//free threads array
	free( init->threads );

	//reset thread-local data, if applicable (hierarchical execution)
	const int rc = pthread_setspecific( mcbsp_internal_thread_data, init->prev_data );
#ifndef MCBSP_NO_CHECKS
	if( rc != 0 ) {
		fprintf( stderr, "Could not restore old thread data, or could not erase local thread data!\n" );
		fprintf( stderr, "(%s)\n", strerror( rc ) );
		mcbsp_util_fatal();
	}
#endif
	
	//exit gracefully, free BSP program init data
	free( init );
}

void bsp_init( void (*spmd)(void), int argc, char **argv ) {
	//create a BSP-program specific initial data struct
	struct mcbsp_init_data *initialisationData = malloc( sizeof( struct mcbsp_init_data ) );
	if( initialisationData == NULL ) {
		fprintf( stderr, "Error: could not allocate MulticoreBSP initialisation struct!\n" );
		mcbsp_util_fatal();
	}
	//set values
	initialisationData->spmd 	= spmd;
	initialisationData->bsp_program = NULL;
	initialisationData->argc 	= argc;
	initialisationData->argv 	= argv;
	//continue initialisation
	bsp_init_internal( initialisationData );
}

MCBSP_PROCESSOR_INDEX_DATATYPE bsp_pid() {
	const struct mcbsp_thread_data * const data = mcbsp_internal_const_prefunction();
	return data->bsp_id;
}

MCBSP_PROCESSOR_INDEX_DATATYPE bsp_nprocs() {
	//since bsp_nprocs can be called from userspace without even a preceding bsp_init,
	//we must always check whether the BSP run-time is sufficiently-well initialised.
	mcbsp_internal_check_keys_allocated();
	const struct mcbsp_thread_data * const data = pthread_getspecific( mcbsp_internal_thread_data );
	if( data == NULL ) {
		//called outside from SPMD environment.
		//return machineInfo data.
		const struct mcbsp_util_machine_info * const machine = mcbsp_internal_getMachineInfo();
		if( machine->num_reserved_cores >= machine->cores )
			return 0;
		else
			return machine->threads - machine->num_reserved_cores * machine->threads_per_core;
	} else {
		//check if the BSP execution was aborted
		mcbsp_internal_check_aborted();
		//return nprocs involved in this SPMD run
		return data->init->P;
	}
}

void bsp_abort( char *error_message, ... ) {
	//get variable arguments struct
	va_list args;
	va_start( args, error_message );

	//pass to bsp_vabort
	bsp_vabort( error_message, args );

	//mark end of variable arguments
	va_end( args );
}

void bsp_vabort( char *error_message, va_list args ) {

	//print error message
	vfprintf( stderr, error_message, args );
	
	//get thread-local data and check for errors
	const struct mcbsp_thread_data * const data = mcbsp_internal_const_prefunction();

	//send signal to all sibling threads
	data->init->abort = true;

	//if there are threads in sync, wake them up
	//first get lock, otherwise threads may sync
	//while checking for synched threads.
	//Only regular condition are checked, as aborts
	//cannot occur within bsp_begin or bsp_sync.
#ifndef MCBSP_USE_SPINLOCK //in spinlock-loop abort-condition is busy-checked
	pthread_mutex_lock( &(data->init->mutex) );
	if( data->init->sync_entry_counter > 0 )
		pthread_cond_broadcast( &(data->init->condition) );
	pthread_mutex_unlock( &( data->init->mutex) );
#else
	//in spinlock-loop abort-condition is busy-checked; no code necessary
#endif
	
	//quit execution
	pthread_exit( NULL );
}

void bsp_sync() {
	//get local data
	struct mcbsp_thread_data * const data = pthread_getspecific( mcbsp_internal_thread_data );
	
	//clear local BSMP queue
	data->bsmp.top = 0;

	//bsp_hpput, bsp_hpget and bsp_hpsend could be handled here

	//see if synchronisation is complete (after this synch we perform non-hp communication)
#ifdef MCBSP_USE_SPINLOCK
	mcbsp_internal_sync( data->init, (size_t)(data->bsp_id) );
#else
	mcbsp_internal_sync( data->init );
#endif

	//check for mismatched sync/end
#ifndef MCBSP_NO_CHECKS
	if( data->init->ended ) {
		fprintf( stderr, "Mismatched bsp_sync and bsp_end detected!\n" );
		mcbsp_util_fatal();
	}
#endif

	//handle the various BSP requests
	
	//update tagSize, phase 1
	if( data->bsp_id == 0 && data->newTagSize != data->init->tagSize )
		data->init->tagSize = data->newTagSize;

	//look for requests with destination us, first cache get-requests
	for( MCBSP_PROCESSOR_INDEX_DATATYPE s = 0; s < data->init->P; ++s ) {
		struct mcbsp_util_stack * const queue = &(data->init->threadData[ s ]->request_queues[ data->bsp_id ]);
		//each request in queue is directed to us. Handle all of them.
		while( !mcbsp_util_stack_empty( queue ) ) {
			//get get-request
			const struct mcbsp_get_request * const request = (struct mcbsp_get_request *) mcbsp_util_stack_pop( queue );
			//the top part is actually the BSP message header
			const struct mcbsp_message * const message = (struct mcbsp_message *) ((char*)request + sizeof( void * ));
			//put data in bsp communication queue
			struct mcbsp_util_stack * const comm_queue     = &(data->init->threadData[ s ]->queues[ data->bsp_id ]);
			//no data race here since we are the only ones allowed to write here
			mcbsp_util_varstack_push( comm_queue, request->source, request->length );
			//put BSP message header
			mcbsp_util_varstack_regpush( comm_queue, message );
		}
	}

	//handle pop_regs: loop over all locals, without destroying the stacks
	for( size_t i = 0; i < data->localsToRemove.top; ++i ) {
		void * const * const array = (void **)(data->localsToRemove.array);
		void * const toRemove = array[ i ];
		void * search_address = toRemove;
		const struct mcbsp_util_address_map * search_map = &(data->local2global);
		if( toRemove == NULL ) {
			//use other process' logic to process this pop_reg
			MCBSP_PROCESSOR_INDEX_DATATYPE s;
			void * const * array2 = NULL;
			//find processor with non-NULL entry at this point
			for( s = 0; s < data->init->P; ++s ) {
				array2 = (void**)(data->init->threadData[ s ]->localsToRemove.array);
				if( array2[ i ] != NULL )
					break;
			}
			//check if we found an entry
#ifndef MCBSP_NO_CHECKS
			if( s == data->init->P ) {
				fprintf( stderr, "Warning: tried to de-register a NULL address at all processes!\n" );
				continue; //ignore
			}
#endif
			//use this processor's logic
			search_address = array2[ i ];
			search_map = &(data->init->threadData[ s ]->local2global);
		}

		//get global key. Note we may be searching in other processors' map, thus:
		// -this should happen after a barrier to ensure all processors have all pop_regs.
		// -this function cannot change the local local2global map
		const unsigned long int globalIndex =
			mcbsp_util_address_map_get( search_map, search_address );

#ifdef MCBSP_DEBUG_REGS
		fprintf( stderr, "Debug: %d removes address %p (global table entry %ld).\n", data->bsp_id, search_address, globalIndex );
#endif

		//delete entry from table
		if( !mcbsp_util_address_table_delete( &(data->init->global2local), globalIndex, data->bsp_id ) ) {
			//there are still other registrations active. Do not delete from map later on.
			((void **)(data->localsToRemove.array))[ i ] = NULL;
		} //otherwise we should delete, but after the next barrier

		//register globalIndex now is free
		if( data->localC == globalIndex + 1 )
			--(data->localC);
		else
			mcbsp_util_stack_push( &(data->removedGlobals), (void*)(&globalIndex) );

#ifdef MCBSP_DEBUG_REGS
		fprintf( stderr, "Debug: %d has local registration count %ld and a removedGlobals stack size of %ld.\n", data->bsp_id, data->localC, data->removedGlobals.top );
#endif

	}

	//coordinate exit using the same mutex (but not same condition!)
#ifdef MCBSP_USE_SPINLOCK
	mcbsp_internal_syncWithCondition( data->init, data->init->mid_condition, (size_t)(data->bsp_id) );
#else
	mcbsp_internal_syncWithCondition( data->init, &(data->init->mid_condition) );
#endif

	//handle pop_regs: loop over all locals, without destroying the stacks
	while( !mcbsp_util_stack_empty( &(data->localsToRemove) ) ) {
		void * const toRemove = *(void**)mcbsp_util_stack_pop( &(data->localsToRemove) );
		//NOTE: this is safe, since it is guaranteed that this address table entry
		//	will not change during synchronisation.

		//delete from map, if address was not NULL
		if( toRemove != NULL ) {

#ifdef MCBSP_DEBUG_REGS
		fprintf( stderr, "Debug: %d removes address %p from local2global map.\n", data->bsp_id, toRemove );
#endif

			mcbsp_util_address_map_remove( &(data->local2global), toRemove );
		}
	}

	//handle push_reg after sync to avoid races on the internal table when NULL pointers exist
	for( size_t i = 0; i < data->localsToPush.top; ++i ) {
		//get array
		const struct mcbsp_push_request * const array = 
			(struct mcbsp_push_request *)data->localsToPush.array;
		//get address
		const struct mcbsp_push_request request = array[ i ];
		void * const address = request.address;

		//get size
		const MCBSP_BYTESIZE_TYPE size = request.size;

		//set initial search targets
		void * search_address = address;
		const struct mcbsp_util_address_map * search_map = &(data->local2global);

		//check for NULL-pointers; if found, change search targets
		if( address == NULL ) {
			//search for a neighbour with a non-NULL entry
			MCBSP_PROCESSOR_INDEX_DATATYPE s;
			const struct mcbsp_push_request * array2 = NULL;
			for( s = 0; s < data->init->P; ++s ) {
				array2 = (struct mcbsp_push_request *)data->init->threadData[ s ]->localsToPush.array;
				if( array2[ i ].address != NULL ) //bingo!
					break;
			}
			//check if we did find a neighbour
#ifndef MCBSP_NO_CHECKS
			if( s == data->init->P ) {
				//this is an all-NULL registration. Ignore
				fprintf( stderr, "Warning: tried to register a NULL-address on all processes!\n" );
				continue;
			}
#endif
			//do logic using data from process s instead of local process
			search_address = array2[ i ].address;
			search_map     = &(data->init->threadData[ s ]->local2global);
		}

		//get global index of this registration. First check map if the key already existed
		const unsigned long int mapSearch = mcbsp_util_address_map_get( search_map, search_address);

		//if the key was not found, create a new global entry
		const unsigned long int global_number = mapSearch != ULONG_MAX ? mapSearch :
								mcbsp_util_stack_empty( &(data->removedGlobals) ) ?
								data->localC++ :
								*(unsigned long int*)mcbsp_util_stack_pop( &(data->removedGlobals) );

		//insert value, global2local map (false sharing is possible here, but effects should be negligable)
		mcbsp_util_address_table_set( &(data->init->global2local), global_number, data->bsp_id, address, size );

#ifdef MCBSP_DEBUG_REGS
		fprintf( stderr, "Debug: %d adds %p to global ID %ld in address table.\n", data->bsp_id, address, global_number );
#endif

		//insert value in local2global map (if this is a new global entry), but not if NULL pointer
		//the actual insertion happens after the next barrier, since the above code may consult
		//other processor's local2global map (see also handling of the bsp_pop_reg requests).
		if( mapSearch == ULONG_MAX && address != NULL ) {
			mcbsp_util_stack_push( &(data->globalsToPush), &global_number );
		} else {
			((struct mcbsp_push_request *)data->localsToPush.array)[ i ].address = NULL;
		}

	}

	//update tagsize, phase 2 (check)
#ifndef MCBSP_NO_CHECKS
	if( data->newTagSize != data->init->tagSize ) {
		fprintf( stderr, "Different tag sizes requested from different processes (%lu requested while process 0 requested %lu)!\n", (unsigned long int)(data->newTagSize), (unsigned long int)(data->init->tagSize) );
		mcbsp_util_fatal();
	}
#endif
	
	//now process put requests to local destination
	for( MCBSP_PROCESSOR_INDEX_DATATYPE s = 0; s < data->init->P; ++s ) {
		struct mcbsp_util_stack * const hpqueue =
			&(data->init->threadData[ s ]->hpsend_queues[ data->bsp_id ]);
		//each message in this queue is directed at us; handle them
		while( !mcbsp_util_stack_empty( hpqueue ) ) {
			//pop hpsend request from outgoing stack
			const struct mcbsp_hpsend_request * const request =
				(struct mcbsp_hpsend_request *) mcbsp_util_stack_pop( hpqueue );
			//add payload to bsmp queue
			mcbsp_util_varstack_push( &(data->bsmp), request->payload, request->payload_size );
			//add tag to bsmp queue
			mcbsp_util_varstack_push( &(data->bsmp), request->tag, data->init->tagSize );
			//record length of payload
			mcbsp_util_varstack_regpush( &(data->bsmp), &(request->payload_size) );
		} //go to next hpsend request

		struct mcbsp_util_stack * const queue = &(data->init->threadData[ s ]->queues[ data->bsp_id ]);
		//each request in queue is directed to us. Handle all of them.
		while( !mcbsp_util_stack_empty( queue ) ) {
			struct mcbsp_message * const request = (struct mcbsp_message*) mcbsp_util_varstack_regpop( queue );
			if( request->destination == NULL ) {
				//get tag and payload from queue
				const void * const tag     = mcbsp_util_varstack_pop( queue, data->init->tagSize );
				const void * const payload = mcbsp_util_varstack_pop( queue, request->length );
				//push payload
				mcbsp_util_varstack_push( &(data->bsmp), payload, request->length );
				//push tag
				mcbsp_util_varstack_push( &(data->bsmp), tag,     data->init->tagSize );
				//push payload size
				mcbsp_util_varstack_regpush( &(data->bsmp), &(request->length) );
			} else {
				//copy payload to destination
				memcpy( request->destination, mcbsp_util_varstack_pop( queue, request->length ), request->length );
			}
		}
	} //go to next processors' outgoing queues

	//final sync
#ifdef MCBSP_USE_SPINLOCK
	mcbsp_internal_sync( data->init, (size_t)(data->bsp_id) );
#else
	mcbsp_internal_sync( data->init );
#endif

	//final processing of push_regs
	while( !mcbsp_util_stack_empty( &(data->localsToPush) ) ) {

		//get push request
		const struct mcbsp_push_request request =
			*(struct mcbsp_push_request *) mcbsp_util_stack_pop( &(data->localsToPush ) );

		//get address
		void * const address = request.address;

		//ignore NULL addresses
		if( address == NULL )
			continue;

		//get global number as derived earlier
		assert( !mcbsp_util_stack_empty( &(data->globalsToPush ) ) );
		const unsigned long int global_number = *(unsigned long int*)mcbsp_util_stack_pop( &(data->globalsToPush) );

#ifdef MCBSP_DEBUG_REGS
		fprintf( stderr, "Debug: %d maps address %p to global ID %ld.\n", data->bsp_id, address, global_number );
#endif

		//add to address and corresponding global index to local2global map
		mcbsp_util_address_map_insert( &(data->local2global), address, global_number );
	} //go to next address to register
} //end synchronisation

double bsp_time() {

	//get init data
	struct mcbsp_thread_data * const data = mcbsp_internal_prefunction();

	//get stop time

#ifdef __MACH__
	//get rights for accessing Mach's timers
	const kern_return_t rc1 = host_get_clock_service( mach_host_self(), SYSTEM_CLOCK, &(data->clock) );
 #ifndef MCBSP_NO_CHECKS
	if( rc1 != KERN_SUCCESS ) {
		fprintf( stderr, "Could not access the Mach system timer (%s)\n", mach_error_string( rc1 ) );
		mcbsp_util_fatal();
	}
 #endif

	mach_timespec_t stop;
	const kern_return_t rc2 = clock_get_time( data->clock, &stop );
 #ifndef MCBSP_NO_CHECKS
	if( rc2 != KERN_SUCCESS ) {
		fprintf( stderr, "Could not get time at call to bsp_time (%s)\n", mach_error_string( rc2 ) );
		mcbsp_util_fatal();
	}
 #endif
#elif _WIN32
	LARGE_INTEGER stop;
 #ifndef MCBSP_NO_CHECKS
	if( !QueryPerformanceCounter( &stop ) ) {
		fprintf( stderr, "Could not get time at call to bsp_time!\n" );
		mcbsp_util_fatal();
	}
 #endif
#else
	struct timespec stop;
	clock_gettime( CLOCK_MONOTONIC, &stop);
#endif

	//return elapsed time
#ifdef _WIN32
	stop.QuadPart -= data->start.QuadPart;
	return stop.QuadPart / ((double)(data->frequency.QuadPart));
#else
	double time = (stop.tv_sec - data->start.tv_sec);
	time += (stop.tv_nsec-data->start.tv_nsec)/1000000000.0;
	return time;
#endif
}

void bsp_push_reg( void * const address, const MCBSP_BYTESIZE_TYPE size_in ) {
	//library internals work with size_t only; convert if necessary
	const size_t size = (size_t) size_in;

	//get init data
	struct mcbsp_thread_data * const data = mcbsp_internal_prefunction();

#ifdef MCBSP_DEBUG_REGS
	fprintf( stderr, "Debug: %d registers address %p with size %ld.\n", data->bsp_id, address, size_in );
#endif

	//construct the push request
	struct mcbsp_push_request toPush;
	toPush.address = address;
	toPush.size    = size;

	//push the request
	mcbsp_util_stack_push( &(data->localsToPush), (void*)(&toPush) );
}

void bsp_pop_reg( void * const address ) {
	//get init data
	struct mcbsp_thread_data * const data = mcbsp_internal_prefunction();

#ifdef MCBSP_DEBUG_REGS
	fprintf( stderr, "Debug: %d de-registers address %p.\n", data->bsp_id, address );
#endif

	//register for removal
	mcbsp_util_stack_push( &(data->localsToRemove), (void*)(&address) );
}

void bsp_put( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const source,
	void * const destination, const MCBSP_BYTESIZE_TYPE offset_in,
	const MCBSP_BYTESIZE_TYPE size_in ) {
	//library internals work with size_t only; convert if necessary
	const size_t offset = (size_t) offset_in;
	const size_t size   = (size_t) size_in;

	//get init data
	struct mcbsp_thread_data * const data = mcbsp_internal_prefunction();

	//build request
	struct mcbsp_message request;

	//record destination; get global index from local map
	const unsigned long int globalIndex = mcbsp_util_address_map_get( &(data->local2global), destination );

	//sanity checks
	assert( pid < data->init->P );
#ifndef MCBSP_NO_CHECKS
	if( globalIndex == ULONG_MAX ) {
		fprintf( stderr, "Error: bsp_put into unregistered memory area (%p) requested!\n", destination );
		bsp_abort( "Aborting due to BSP primitive call with invalid arguments." );
	}	
#endif

	//get entry from address table
	const struct mcbsp_util_address_table_entry * const entry = mcbsp_util_address_table_get( &(data->init->global2local), globalIndex, pid );

	//sanity checks
#ifndef MCBSP_NO_CHECKS
	if( entry == NULL ) {
		fprintf( stderr, "Error: bsp_put called with an erroneously registered destination variable!\n" );
		bsp_abort( "Aborting due to BSP primitive call with invalid arguments." );
	}
	if( offset + size > entry->size ) {
		fprintf( stderr, "Error: bsp_put would go out of bounds at destination processor (offset=%lu, size=%lu, while registered memory area is %lu bytes)!\n", (unsigned long int)offset, (unsigned long int)size, (unsigned long int)(entry->size) );
		bsp_abort( "Aborting due to BSP primitive call with invalid arguments." );
	}
#endif

	//record final destination address
	request.destination = ((char*)(entry->address)) + offset;

	//record length
	request.length = size;

	//record payload
	mcbsp_util_varstack_push( &(data->queues[ pid ]), source, size );

	//record request header
	mcbsp_util_varstack_regpush( &(data->queues[ pid ]), &request );

}

void bsp_get( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const source,
	const MCBSP_BYTESIZE_TYPE offset_in, void * const destination,
	const MCBSP_BYTESIZE_TYPE size_in ) {
	//library internals work with size_t only; convert if necessary
	const size_t offset = (size_t) offset_in;
	const size_t size   = (size_t) size_in;

	//get init data
	struct mcbsp_thread_data * const data = mcbsp_internal_prefunction();

	//build request
	struct mcbsp_get_request request;

	//record source address, plus sanity checks
	const unsigned long int globalIndex = mcbsp_util_address_map_get( &(data->local2global), source );
#ifndef MCBSP_NO_CHECKS
	if( globalIndex == ULONG_MAX ) {
		fprintf( stderr, "Error: bsp_put into unregistered memory area (%p) requested!\n", destination );
		bsp_abort( "Aborting due to BSP primitive call with invalid arguments." );
	}	
#endif
	const struct mcbsp_util_address_table_entry * entry = mcbsp_util_address_table_get( &(data->init->global2local), globalIndex, pid );
#ifndef MCBSP_NO_CHECKS
	if( entry == NULL ) {
		fprintf( stderr, "Error: bsp_get called with an erroneously registered source variable!\n" );
		bsp_abort( "Aborting due to BSP primitive call with invalid arguments." );
	}
	if( offset + size > entry->size ) {
		fprintf( stderr, "Error: bsp_get would go out of bounds at source processor (offset=%lu, size=%lu, while registered memory area is %lu bytes)!\n", (unsigned long int)offset, (unsigned long int)size, (unsigned long int)(entry->size) );
		bsp_abort( "Aborting due to BSP primitive call with invalid arguments." );
	}
#endif
	request.source = ((char*)(entry->address)) + offset;

	//record destination
	request.destination = destination;

	//record length
	request.length = size;

	//record request
	mcbsp_util_stack_push( &(data->request_queues[ data->bsp_id ]), &request );
}

void bsp_direct_get( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const source,
        const MCBSP_BYTESIZE_TYPE offset_in, void * const destination,
	const MCBSP_BYTESIZE_TYPE size_in ) {
	//library internals work with size_t only; convert if necessary
	const size_t offset = (size_t) offset_in;
	const size_t size   = (size_t) size_in;
	
	//get init data
	struct mcbsp_thread_data * const data = mcbsp_internal_prefunction();

	//get source address
	const unsigned long int globalIndex = mcbsp_util_address_map_get( &(data->local2global), source );
#ifndef MCBSP_NO_CHECKS
	if( globalIndex == ULONG_MAX ) {
		bsp_abort( "Error: bsp_direct_get called with unregistered remote address!\n" );
	}
#endif
	const struct mcbsp_util_address_table_entry * entry = mcbsp_util_address_table_get( &(data->init->global2local), globalIndex, pid );
#ifndef MCBSP_NO_CHECKS
	if( entry == NULL ) {
		fprintf( stderr, "Error: bsp_direct_get called with an erroneously registered source variable!\n" );
		bsp_abort( "Aborting due to BSP primitive call with invalid arguments." );
	}
	if( offset + size > entry->size ) {
		fprintf( stderr, "Error: bsp_direct_get would go out of bounds at source processor (offset=%lu, size=%lu, while registered memory area is %lu bytes)!\n", (unsigned long int)offset, (unsigned long int)size, (unsigned long int)(entry->size) );
		bsp_abort( "Aborting due to BSP primitive call with invalid arguments." );
	}
#endif

	//perform direct get
	if( destination != entry->address )
		memcpy( destination, ((char*)(entry->address)) + offset, size );
}

void bsp_set_tagsize( MCBSP_BYTESIZE_TYPE * const size_in ) {
	//library internals work with size_t only; convert if necessary
	const size_t size = (size_t) *size_in;

	//get init data
	struct mcbsp_thread_data * const data = mcbsp_internal_prefunction();

	//record new tagsize
	data->newTagSize = size;

	//return old tag size
	*size_in = (MCBSP_BYTESIZE_TYPE) (data->init->tagSize);
}

void bsp_send( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const tag,
	const void * const payload, const MCBSP_BYTESIZE_TYPE size_in ) {
	//library internals work with size_t only; convert if necessary
	const size_t size = (size_t) size_in;

	//get init data
	struct mcbsp_thread_data * const data = mcbsp_internal_prefunction();

	//build request
	struct mcbsp_message request;

	//record destination
	request.destination = NULL;

	//record payload length
	request.length = size;

	//record payload
	mcbsp_util_varstack_push( &(data->queues[ pid ]), payload, size );

	//record tag
	mcbsp_util_varstack_push( &(data->queues[ pid ]), tag, data->init->tagSize );

	//record message header
	mcbsp_util_varstack_regpush( &(data->queues[ pid ]), &request );	

}

#ifdef MCBSP_ENABLE_HP_DIRECTIVES
void bsp_hpsend( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const tag,
	const void * const payload, const MCBSP_BYTESIZE_TYPE size_in ) {
	//library internals work with size_t only; convert if necessary
	const size_t size = (size_t) size_in;

	//get init data
	struct mcbsp_thread_data * const data = mcbsp_internal_prefunction();

	//build request
	struct mcbsp_hpsend_request request;

	//record source address
	request.payload = payload;
	request.tag     = tag; //misuse destination field

	//record length
	request.payload_size = size;

	//record request
	mcbsp_util_stack_push( &(data->hpsend_queues[ pid ]), &request );
}
#endif

void bsp_qsize( MCBSP_NUMMSG_TYPE * const packets,
	MCBSP_BYTESIZE_TYPE * const accumulated_size ) {
	//get thread data
	const struct mcbsp_thread_data * const data = mcbsp_internal_const_prefunction();

	//check no-messages case
	if( mcbsp_util_stack_empty( &(data->bsmp) ) ) {
		*packets = (MCBSP_NUMMSG_TYPE) 0;
		if( accumulated_size != NULL )
			*accumulated_size = 0;
		return;
	}

	//set initial values
	*packets = 0;
	if( accumulated_size != NULL )
		*accumulated_size = 0;

	//loop over all messages
	const char * raw = (char*)(data->bsmp.array) + data->bsmp.top - sizeof( size_t );
	do {
		//if requested, count accumulated message sizes
		const size_t curmsg_size = *(size_t*)raw;
		if( accumulated_size != NULL )
			*accumulated_size += curmsg_size;
		//skip payload size, tag size, and payload size fields
		raw -= curmsg_size + data->init->tagSize + sizeof( size_t );
		//we skipped one packet
		++(*packets);
		//sanity check
		assert( raw + sizeof( size_t ) >= (char*)(data->bsmp.array) );
		//continue loop if we did not go past the start of the bsmp array
	} while( raw + sizeof( size_t ) != (char*)data->bsmp.array );
}

void bsp_move( void * const payload, const MCBSP_BYTESIZE_TYPE max_copy_size_in ) {
	//library internals work with size_t only; convert if necessary
	const size_t max_copy_size = (size_t) max_copy_size_in;

	//get thread data
	struct mcbsp_thread_data * const data = mcbsp_internal_prefunction();

	//if stack is empty, do not return anything
	if( mcbsp_util_stack_empty( &(data->bsmp) ) )
		return;

	//get payload size
	const size_t size = *(size_t*) mcbsp_util_varstack_regpop( &(data->bsmp) );

	//skip tag
	mcbsp_util_varstack_pop( &(data->bsmp), data->init->tagSize );

	//copy message
	memcpy( payload, mcbsp_util_varstack_pop( &(data->bsmp), size ), size > max_copy_size ? max_copy_size : size );
}

void bsp_get_tag( MCBSP_BYTESIZE_TYPE * const status, void * const tag ) {
	//get thread data
	const struct mcbsp_thread_data * const data = mcbsp_internal_const_prefunction();

	//if stack is empty, set status to -1
	if( mcbsp_util_stack_empty( &(data->bsmp) ) ) {

		//return failure
		*status = (MCBSP_BYTESIZE_TYPE) -1;

	} else {

		//set status to tag size and copy tag into target memory area
		*status = (MCBSP_BYTESIZE_TYPE) data->init->tagSize;

		//get tag; location is the top of the bsmp stack minus the payload size field
		memcpy( tag, mcbsp_util_varstack_peek( &(data->bsmp), data->init->tagSize + sizeof( size_t ) ), data->init->tagSize );

	}
}

MCBSP_BYTESIZE_TYPE bsp_hpmove( void* * const p_tag, void* * const p_payload ) {
	//get thread data
	struct mcbsp_thread_data * const data = mcbsp_internal_prefunction();

	//if empty, return -1
	if( mcbsp_util_stack_empty( &(data->bsmp) ) )
		return ((MCBSP_BYTESIZE_TYPE)-1);

	//get size
	const size_t size = *((size_t*) mcbsp_util_varstack_regpop( &(data->bsmp) ));

	//get tag 
	*p_tag     = mcbsp_util_varstack_pop( &(data->bsmp), data->init->tagSize );

	//get tag
	*p_payload = mcbsp_util_varstack_pop( &(data->bsmp), size );

	//return the payload length
	return (MCBSP_BYTESIZE_TYPE) size;
}

#ifdef ENABLE_FAKE_HP_DIRECTIVES
void bsp_hpput( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const source,
        void * const destination, const MCBSP_BYTESIZE_TYPE offset,
	const MCBSP_BYTESIZE_TYPE size ) {
	bsp_put( pid, source, destination, offset, size );
}

void bsp_hpget( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const source,
        const MCBSP_BYTESIZE_TYPE offset, void * const destination,
	const MCBSP_BYTESIZE_TYPE size ) {
	bsp_get( pid, source, offset, destination, size );
}
#endif


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

#ifndef _H_MCUTIL
#define _H_MCUTIL

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#ifdef _WIN32
 #include <windows.h>
#endif

#include "mcbsp.h"
#include "mcbsp-affinity.h"

/**
 * Structure representing the machine hardware information.
 */
struct mcbsp_util_machine_info {

	/** Whether the threads field has been set manually. */
	bool Tset;

	/** Whether the affinity field as been set manually. */
	bool Aset;

	/** Whether the cores field has been set manually. */
	bool Cset;

	/** Whether the threads_per_core field has been set manually. */
	bool TpCset;

	/** Whether the thread numbering field has been set manually. */
	bool TNset;

	/** Whether the manual pinning field has been set. */
	bool Pset;

	/** Whether the reserved cores field has been set. */
	bool Rset;

	/** The total number of threads available for computation. */
	size_t threads;

	/** Currently active affinity strategy. */
	enum mcbsp_affinity_mode affinity;

	/**
	 * The number of hardware cores available on the
	 * current system. MulticoreBSP can not (as of yet)
	 * determine this information reliably by itself.
	 * A user can set this value manually at run-time
	 * via mcbsp_set_available_cores, or supply a
	 * value via `machine.info'.
	 */
	size_t cores;

	/**
	 * The number of hardware threads per core on the system
	 * MulticoreBSP is executing on. 
	 */
	size_t threads_per_core;
	
	/** Currently active thread-numbering method. */
	enum mcbsp_thread_numbering thread_numbering;

	/**
	 * Pointer to the manually-defined affinity definition.
	 *
	 * Required when MCBSP_AFFINITY is set to MANUAL.
	 *
	 * The list should be of length threads, and consist out
	 * of unique numbers between 0 and threads-1 (inclusive),
	 * where C is the number of available hardware threads
	 * on the current machine.
	 */
	size_t *manual_affinity;

	/**
	 * Number of entries in the reserved_cores array.
	 */
	size_t num_reserved_cores;

	/**
	 * Pointer to the manually-defined reversed-cores list.
	 *
	 * This list is of length <= cores, and contains 
	 * unique entries in-between 0 and cores-1 (inclusive).
	 * It represents the core-IDs that are reserved by the
	 * system (or other software), thus forbidding 
	 * MulticoreBSP to pin threads to that core.
	 */
	size_t *reserved_cores;

};

/** A map from pointers to unsigned long ints. */
struct mcbsp_util_address_map {

	/** Capacity. */
	unsigned long int cap;

	/** Size. */
	unsigned long int size;

	/** Keys. */
	void * * keys;

	/** Values. */
	unsigned long int * values;

};

/** A single address table entry. */
struct mcbsp_util_address_table_entry {

	/** Local address. */
	void * address;

	/** Length of global area. */
	size_t size;

};

/** Self-growing stack. */
struct mcbsp_util_stack {

	/** Stack capacity. */
	size_t cap;

	/** Location of the top of the stack. */
	size_t top;

	/** Size of a single entry. */
	size_t size;

	/** Stack entries. */
	void * array;

};

/** A table of local address locations per SPMD variable. */
struct mcbsp_util_address_table {
	
	/** Mutex for locking-out write access to the global table. */
	pthread_mutex_t mutex;

	/** Capacity. */
	unsigned long int cap;

	/** Number of local versions. */
	unsigned long int P;

	/** Table entries. */
#ifdef MCBSP_ALLOW_MULTIPLE_REGS
	struct mcbsp_util_stack ** table;
#else
	struct mcbsp_util_address_table_entry ** table;
#endif
};

/**
 * Initialises the mcbsp_util_stack struct.
 *
 * @param stack 	The stack to initialise. 
 * @param elementSize 	The size of a single element in the stack.
 */
void mcbsp_util_stack_initialise( struct mcbsp_util_stack * const stack, const size_t elementSize );

/** Doubles the capacity of a given stack. */
void mcbsp_util_stack_grow( struct mcbsp_util_stack * const stack );

/** Checks whether a given stack is empty. */
bool mcbsp_util_stack_empty( const struct mcbsp_util_stack * const stack );

/**
 * Returns the newest item in the stack.
 * This function removes that item from the stack.
 *
 * Does not do boundary checking! Call mcbsp_util_stack_empty first when
 * unsure if the stack still contains items.
 *
 * @return A pointer to the next element in the stack.
 */

void * mcbsp_util_stack_pop( struct mcbsp_util_stack * const stack );

/**
 * Returns the top stack item. Does not remove it from the stack.
 *
 * Does not do boundary checking! Call mcbsp_util_stack_empty first when
 * unsure if the stack still contains items.
 *
 * @return A pointer to the top element in the stack.
 */
void * mcbsp_util_stack_peek( const struct mcbsp_util_stack * const stack );

/** Pushes a new item on the stack. */
void mcbsp_util_stack_push( struct mcbsp_util_stack * const stack, const void * const item );

/** Frees all memory related to a given stack. */
void mcbsp_util_stack_destroy( struct mcbsp_util_stack * const stack );

/** Doubles the capacity of a given stack. */
void mcbsp_util_varstack_grow( struct mcbsp_util_stack * const stack );

/** Returns the top variable stack item. */
void * mcbsp_util_varstack_pop( struct mcbsp_util_stack * const stack, const size_t size );

/** Returns the top fixed-size item of the stack. */
void * mcbsp_util_varstack_regpop( struct mcbsp_util_stack * const stack );

/** Peeks for a variably-sized item on the stack. */
void * mcbsp_util_varstack_peek( const struct mcbsp_util_stack * const stack, const size_t size );

/** Pushes a variably-sized item on the stack. */
void mcbsp_util_varstack_push( struct mcbsp_util_stack * const stack, const void * const item, const size_t size );

/** Pushes a fixed-size item on the stack. */
void mcbsp_util_varstack_regpush( struct mcbsp_util_stack * const stack, const void * const item );

/**
 * Initialises the mcbsp_util_address_table struct.
 * This function is not thread-safe.
 *
 * @param table Pointer to the struct to initialise.
 * @param P     The number of local versions to maintain for each entry.
 */
void mcbsp_util_address_table_initialise( struct mcbsp_util_address_table * const table, const unsigned long int P );

/**
 * Doubles the capacity of a given address table.
 * This function is not thread-safe.
 *
 * @param table Pointer to the struct of the table to increase storage of.
 */
void mcbsp_util_address_table_grow( struct mcbsp_util_address_table * const table );

/**
 * Ensures the address table is of at least the given size.
 * This function is thread-safe.
 *
 * @param table Pointer to the struct to initialise.
 * @param target_size The (new) minimum size of the table after function exit.
 */
void mcbsp_util_address_setsize( struct mcbsp_util_address_table * const table, const unsigned long int target_size );

/**
 * Frees the memory associated with a given address table.
 *
 * @param table Pointer to the struct to destroy.
 */
void mcbsp_util_address_table_destroy( struct mcbsp_util_address_table * const table );

/**
 * Sets an entry in a given address table.
 * Grows table capacity if required.
 *
 * @param table   Pointer to the address table.
 * @param key     Which entry to set.
 * @param version Which local version to set.
 * @param value   Pointer to the local memory region..
 * @param size    Size of the local memory region.
 */
void mcbsp_util_address_table_set( struct mcbsp_util_address_table * const table, const unsigned long int key, const unsigned long int version, void * const value, const size_t size );

/**
 * Gets an entry from a given address table.
 * Does not do boundary checks.
 *
 * @param table   Pointer to the address table.
 * @param key     Which entry to get.
 * @param version Which local version to obtain.
 * @return The requested address pointer.
 */
const struct mcbsp_util_address_table_entry * mcbsp_util_address_table_get( const struct mcbsp_util_address_table * const table, const unsigned long int key, const unsigned long int version );

/**
 * Removes an entry from a given address table.
 * Does not do boundary checks.
 *
 * @param table   Pointer to the address table.
 * @param key     Which entry to remove.
 * @param version Which local version to remove.
 * @return Whether the registration stack at (key,version) is empty.
 */
bool mcbsp_util_address_table_delete( struct mcbsp_util_address_table * const table, const unsigned long int key, const unsigned long int version );

/**
 * Initialises a mcbsp_util_address_map struct.
 *
 * @param address_map Pointer to the struct to initialise.
 */
void mcbsp_util_address_map_initialise( struct mcbsp_util_address_map * const address_map );

/**
 * Doubles the capacity of a given address map.
 *
 * @param address_map Pointer to the struct of the map to increase storage of.
 */
void mcbsp_util_address_map_grow( struct mcbsp_util_address_map * const address_map );

/**
 * Frees the memory related to a given address map.
 *
 * @param address_map Pointer to the struct to destroy.
 */
void mcbsp_util_address_map_destroy( struct mcbsp_util_address_map * const address_map );

/**
 * Address map accessor.
 *
 * @param address_map 	Pointer to the map to consult.
 * @param key		Which key to search for.
 * @return		The value associated with the key.
 */
unsigned long int mcbsp_util_address_map_get( const struct mcbsp_util_address_map * const address_map, const void * const key );

/**
 * Inserts a key-value pair in the map.
 *
 * @param address_map 	Pointer to the map to insert in.
 * @param key		Under which key to insert the value.
 * @param value		The value to insert.
 */
void mcbsp_util_address_map_insert( struct mcbsp_util_address_map * const address_map, void * const key, unsigned long int value );

/**
 * Removes a key-value pair from the map.
 * Does nothing when the key is not in the map.
 *
 * @param address_map Pointer to the map to remove from.
 * @param key         Key of the key-value pair to remove.
 */
void mcbsp_util_address_map_remove( struct mcbsp_util_address_map * const address_map, void * const key );

/**
 * Helper-function for
 *   mcbsp_util_address_map_insert, and
 *   mcbsp_util_address_map_remove.
 * Performs a binary search for key and returns the largest
 * index for which the map key is less or equal to the 
 * supplied key value.
 *
 * @param address_map Pointer to the map to look in.
 * @param key         The key value to look for.
 * @param lo          Lower bound on range of the map to look for.
 * @param hi          upper bound (inclusive) on range of the map to look for.
 */
size_t mcbsp_util_address_map_binsearch( const struct mcbsp_util_address_map * const address_map, const void * const key, size_t lo, size_t hi );

/**
 * Attempts to detect the number of hard-ware
 * threads supported on the current machine.
 *
 * @return Detected number of HW threads.
 */
size_t mcbsp_util_detect_hardware_threads();

/**
 * Creates a new machine info struct.
 *
 * @return A pointer to a new machine info struct.
 */
struct mcbsp_util_machine_info *mcbsp_util_createMachineInfo();

/**
 * If a current machine info instance exists,
 * destroys it.
 *
 * This function is not thread-safe.
 *
 * @param machine_info Pointer to the machine info to deallocate properly.
 */
void mcbsp_util_destroyMachineInfo( void * machine_info );

/**
 * Comparison function for use with qsort
 * on arrays of size_t's.
 */
int mcbsp_util_int_compare( const void * a, const void * b );

/** Binary logarithm, unsigned integer version,
 *  rounding up.
 *
 *  Note: returns 0 for x=0.
 */
size_t mcbsp_util_log2( size_t x );

/**
 * Sorts an array of integers.
 * The array is assumed to hold unique elements in a given
 * range of values. The algorithm runs linearly in the
 * amount of possible values.
 *
 * @param array       The array to sort.
 * @param length      Length of array.
 * @param lower_bound Lower bound (inclusive) on values in array.
 * @param upper_bound Upper bound (exclusive) on values in array.
 *
 * @return Size of the sorted array.
 */
size_t mcbsp_util_sort_unique_integers( size_t * const array, const size_t length, const size_t lower_bound, const size_t upper_bound );

/**
 * Returns whether an array contains a given value. Only
 * the range [lo,hi) of array is checked for value.
 *
 * @param array Where to search in.
 * @param value The value to search for.
 * @param lo    From which index on to search (inclusive).
 * @param hi    Up to which index to search (exclusive).
 * @return Whether array contains value in the given range.
 */
bool mcbsp_util_contains( const size_t * const array, const size_t value, const size_t lo, const size_t hi );

/**
 * Function that yields a pinning of P threads according
 * to the given machine info.
 *
 * @param  P       The number of threads to pin.
 * @param  machine The machine to pin for.
 * @return An array R where R[i] gives the core to
 * 	   which thread i should pin to.
 */
int* mcbsp_util_pinning( const MCBSP_PROCESSOR_INDEX_DATATYPE P, struct mcbsp_util_machine_info * const machine );

/**
 * Handles a fatal error in a uniform fashion.
 *
 * Only call this function if the error is fatal and
 * unrecoverable, and should thus stop the entire
 * parallel execution.
 */
void mcbsp_util_fatal();

#endif


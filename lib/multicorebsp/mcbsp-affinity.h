/*
 * Copyright (c) 2013 by Albert-Jan N. Yzelman
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

/*! \file mcbsp-affinity.h
 *
 * The MulticoreBSP for C affinity interface
 * 
 * The functions and data types defined in this class
 * allow a user to take control of the way MulticoreBSP
 * pins BSP processes to hardware threads, at run-time.
 *
 * Changes made to the machine layout using these
 * functions take effect only at the next call to
 * bsp_begin; calling these functions does not 
 * affect running SPMD sections.
 *
 * Values set using this interface supercede values
 * defined in
 *    1. the `machine.info' file (if it exists);
 *    2. the default values.
 *
 * The default values can be changed by directly
 * setting the MCBSP_DEFAULT_AFFINITY,
 * MCBSP_DEFAULT_THREADS_PER_CORE, or
 * MCBSP_DEFAULT_THREAD_NUMBERING globals.
 *
 * All fields are thread-local. Setting values
 * manually should occur on each thread separately.
 * This is only useful if multiple threads call
 * bsp_begin *simultaneously*. This is useful when
 * employing hierarchical BSP computations.
 */

#ifndef _H_MCBSP_AFFINITY
#define _H_MCBSP_AFFINITY

/**
 * Pre-defined strategies for pinning threads.
 */
enum mcbsp_affinity_mode {

	/**
	 * A scattered affinity will pin P consecutive threads
	 * so that the full range of cores is utilised. This
	 * assumes the cores are numbered consecutively by the
	 * OS. If this is not the case, please use MANUAL.
	 * This is the default strategy.
	 */
	SCATTER = 0,

	/**
	 * A compact affinity will pin P consecutive threads 
	 * to the first P available cores.
	 */
	COMPACT,

	/**
	 * A manual affinity performs pinning as per user-
	 * supplied definitions.
	 * See also MCBSP_DEFAULT_MANUAL_AFFINITY
	 */
	MANUAL
};

/**
 * Enumerates ways of hardware thread numbering.
 */
enum mcbsp_thread_numbering {

	/**
	 * If each core supports s threads, and t cores are available
	 * (for a total of p=s*t hardware threads), then CONSECUTIVE
	 * assigns the numbers i*t, i*t+1, ..., i*t+s-1 to hardware
	 * threads living on core i (with 0<=i<t).
	 */
	CONSECUTIVE = 0,

	/**
	 * If each core supports s threads, and t cores are available
	 * (for a total of p=s*t hardware threads), then WRAPPED
	 * assigns the numbers i, i+s, i+2*s, ..., i+(t-1)*s to
	 * hardware threads living on core i (with 0<=i<t).
	 */
	WRAPPED
};

/**
 * Changes the maxmimum amount of threads
 * MulticoreBSP can allocate. Setting this
 * higher than the (auto-detected) maximum
 * of your machine will cause your MulticoreBSP
 * applications to hang, unless combined
 * with a manually set affinity strategy
 * preventing this.
 */
void mcbsp_set_maximum_threads( const size_t max );

/**
 * Changes the currently active affinity strategy.
 *
 * Users should use this function to set the
 * affinity strategy to something other than
 * the default strategy (SCATTER).
 *
 * This will only affect new SPMD instances.
 *
 * This function is thread-safe in that upon
 * exit, valid machine info is guaranteed.
 * Concurrent calls with bsp_begin still
 * constitutes a programming error, however
 * (but will not result in crashes).
 *
 * Will override defaults, and will override
 * values given in `machine.info'.
 */
void mcbsp_set_affinity_mode( const enum mcbsp_affinity_mode mode );

/**
 * Changes the number of available cores.
 * Will override defaults, and will override
 * values given in `machine.info'.
 *
 * By default, MulticoreBSP will set this value
 * equal to the number of hardware threads
 * detected, divided by threads_per_core.
 *
 * This will only affect new SPMD instances.
 *
 * This function is thread-safe in that upon
 * exit, valid machine info is guaranteed.
 * Concurrent calls with bsp_begin still
 * constitutes a programming error, however
 * (but will not result in crashes).
 */
void mcbsp_set_available_cores( const size_t num_cores );

/**
 * Changes the number of threads per core.
 * Will override defaults, and will override
 * values given in `machine.info'.
 *
 * The default value is 1. The value affects
 * only the SCATTER and COMPACT strategies.
 *
 * This will only affect new SPMD instances.
 *
 * This function is thread-safe in that upon
 * exit, valid machine info is guaranteed.
 * Concurrent calls with bsp_begin still
 * constitutes a programming error, however
 * (but will not result in crashes).
 *
 * @param threads_per_core The number of
 * hardware threads that is supported by
 * each core on this machine.
 */
void mcbsp_set_threads_per_core( const size_t threads_per_core );

/**
 * Changes the thread numbering strategy this
 * machines adheres to.
 * Will override defaults, and will override
 * values given in `machine.info'.
 *
 * The default is CONSECUTIVE. This value
 * affects only the SCATTER and COMPACT
 * strategies.
 *
 * This will only affect new SPMD instances.
 *
 * This function is thread-safe in that upon
 * exit, valid machine info is guaranteed.
 * Concurrent calls with bsp_begin still
 * constitutes a programming error, however
 * (but will not result in crashes).
 */
void mcbsp_set_thread_numbering( const enum mcbsp_thread_numbering numbering );

/**
 * Supplies a manually defined pinning strategy
 * for MulticoreBSP to use. Implies
 *     `mcbsp_set_affinity_mode( MANUAL );'
 * Will override values given in `machine.info'.
 *
 * The supplied pinning array must be of size
 * equal to the maximum number of threads
 * supported by the current machine, and is
 * buffered (copied) internally.
 *
 * This will only affect new SPMD instances.
 *
 * This function is thread-safe in that upon
 * exit, valid machine info is guaranteed.
 * Concurrent calls with bsp_begin still
 * constitutes a programming error, however
 * (but will not result in crashes).
 *
 * Note: the user-supplied array must also be
 * freed by the user.
 */
void mcbsp_set_pinning( const size_t * const pinning, const size_t length );

/**
 * Supplies a list of core IDs that are NOT to
 * be used by MulticoreBSP SPMD runs.
 * The user should NOT update the number of threads
 * of the machine; MulticoreBSP will automatically
 * infer the number of threads available at run-
 * time, whenever necessary.
 *
 * Please do make sure to set the number of
 * threads_per_core and thread_numbering correctly
 * to ensure the pinning strategy works properly.
 *
 * This will only affect new SPMD instances.
 *
 * This function is thread-safe in that upon
 * exit, valid machine info is guaranteed.
 * Concurrent calls with bsp_begin still
 * constitutes a programming error, however
 * (but will not result in crashes).
 *
 * Note: the user-supplied array must also be
 * freed by the user.
 */
void mcbsp_set_reserved_cores( const size_t * const reserved, const size_t length );

/** 
 * @return The maximum number of hardware-i
 *         supported threads.
 * @see mcbsp_set_maximum_threads
 */
size_t mcbsp_get_maximum_threads();

/**
 * @return The currently active thread
 *         affinity strategy.
 * @see mcbsp_set_affinity_mode
 */
enum mcbsp_affinity_mode mcbsp_get_affinity_mode();

/**
 * @return The number of hardware cores
 *         on this system.
 * @see mcbsp_set_available_cores
 */
size_t mcbsp_get_available_cores();

/**
 * @return The number of hardware threads
 *         supported by each core on this
 *         machine.
 * @see mcbsp_set_threads_per_core
 */
size_t mcbsp_get_threads_per_core();

/**
 * @return The thread-numbering scheme
 *         employed on this machine.
 * @see mcbsp_set_thread_numbering
 */
enum mcbsp_thread_numbering mcbsp_get_thread_numbering();

/**
 * @return NULL if there is no manually-set
 *         pinning strategy, or a copy of the
 *         pinning array otherwise. The array
 *         length equals the maximum number
 *         of supported threads.
 * @see mcbsp_set_pinning
 * @see mcbsp_get_maximum_threads
 */
size_t * mcbsp_get_pinning();

/**
 * @return The number of reserved cores.
 * @see mcbsp_set_reserved_cores
 */
size_t mcbsp_get_reserved_cores_number();

/**
 * @return A copy of the array of 
 *         reserved cores, or NULL if
 *         no cores are reserved.
 * @see mcbsp_set_reserved_cores
 * @see mcbsp_get_reserved_cores_number
 */
size_t * mcbsp_get_reserved_cores();

//Default values

/**
 * Default affinity strategy (SCATTER).
 */
extern enum mcbsp_affinity_mode MCBSP_DEFAULT_AFFINITY;

/**
 * Default number of threads per core (1).
 */
extern size_t MCBSP_DEFAULT_THREADS_PER_CORE;

/**
 * Default thread numbering (CONSECUTIVE).
 */
extern enum mcbsp_thread_numbering MCBSP_DEFAULT_THREAD_NUMBERING;

#endif


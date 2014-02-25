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

#ifndef _HPP_MCBSP
#define _HPP_MCBSP

//#include <cstdarg>
//#include <cstddef>

extern "C" {
	#include "mcbsp.h"
}

#ifndef SIZE_MAX
 /**
  * Provide SIZE_MAX if this ANSI C99 standard macro 
  * does not make it into C++ space.
  */
 #define SIZE_MAX ((size_t)-1)
#endif

/**
 * Namespace in which the MulticoreBSP-for-C C++ wrapper lives.
 * @see mcbsp::BSP_program
 */
namespace mcbsp {

	/**
	 * Abstract class which a user can extend to write BSP programs.
	 *
	 * Class-based BSP programming implies that each SPMD thread executes
	 * the same BSP_program::spmd() program on different class instances;
	 * thus class-local variables are automatically also thread-local.
	 * This is somewhat more elegant than declaring everything locally,
	 * within functions, as is necessary in C.
	 *
	 * Furthermore, the functions bsp_init(), bsp_begin(), and bsp_end()
	 * are implied and no longer need to be called explicitly; users
	 * instead call BSP_program::begin().
	 * 
	 * BSP algorithms using this C++ wrapper extend the BSP_program
	 * class, and provide implementations for the parallel part of the
	 * program in BSP_program::spmd().
	 * The user furthermore supplies the C++ wrapper a means of
	 * constructing new instances of their BSP program by implementing
	 * BSP_program::newInstance(). This lets the wrapper provide a
	 * separate instance to each thread executing the parallel program.
	 * 
	 * @see BSP_program::spmd()
	 * @see BSP_program::newInstance()
	 * @see BSP_program::begin()
	 */
	class BSP_program {

		private:

			/** The number of threads that should be spawned. */
			MCBSP_PROCESSOR_INDEX_DATATYPE P;

		protected:

			/**
			 * Base constructor is only accessible by self,
			 * derived instances, and friends.
			 */ 
			BSP_program() {}

			/** The parallel SPMD code to be implemented by user. */
			virtual void spmd() = 0;

			/** 
			 * Creates a new instance of the implementing class,
			 * which will be used by new threads spawned by
			 * bsp_begin().
			 *
			 * Note that this need not be a copy if your BSP
			 * program does not require this! The recommended
			 * implementation is the following:
			 *
			 * virtual BSP_program * newInstance() {
			 * 	return new FinalType();
			 * }
			 *
			 * where FinalType is the name of the BSP program you
			 * are implementing (FinalType must be a non-abstract
			 * subclass of BSP_program).
			 */
			virtual BSP_program * newInstance() = 0;

		public:

			/**
			 * Initialises and starts the current BSP program.
			 * Automatically implies (correct) calls to bsp_init()
			 * and bsp_begin().
			 *
			 * The parallel SPMD section is the overloaded function
			 * spmd() of this class instance.
			 *
			 * The SPMD program makes use of the same globally
			 * defined BSP primitives as available in plain C;
			 * there are no special C++ wrappers for communication.
			 * 
			 * bsp_end() is automatically called after the spmd()
			 * function exits.
			 */
			void begin( const MCBSP_PROCESSOR_INDEX_DATATYPE P = bsp_nprocs() );

			/** Abstract classes should have virtual destructors. */
			virtual ~BSP_program() {};
	};

}

#endif


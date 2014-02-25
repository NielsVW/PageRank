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

#ifndef _HPP_MCBSP_INTERNAL
#define _HPP_MCBSP_INTERNAL

#include <iostream>

//for documentation of the public parts, see mcbsp.hpp

extern "C" {
	#include "mcinternal.h"
	#include "mcutil.h"
}

namespace mcbsp {

	/**
	 * This function enables C++ wrapping of MulticoreBSP
	 * for C. It is defined as a friend function of
	 * BSP_program, and used as the SPMD entry-point of
	 * the plain-C bsp_init() function, which requires a
	 * globally-defined function.
	 *
	 * This function is only of interest to the C++
	 * wrapper, and is therefore not exposed to the
	 * public (i.e., it is not included in the public
	 * mcbsp.hpp file).
	 */
	void mcbsp_cpp_callback();

	class BSP_program {

		private:

			MCBSP_PROCESSOR_INDEX_DATATYPE P;

		protected:

			BSP_program() {}

			virtual void spmd() = 0;

			virtual BSP_program * newInstance() = 0;

		public:

			void begin( const MCBSP_PROCESSOR_INDEX_DATATYPE P = bsp_nprocs() );

			virtual ~BSP_program() {};

			friend void mcbsp_cpp_callback();
	};

}

#endif


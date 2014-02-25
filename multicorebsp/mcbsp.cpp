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

#include "mcbsp-internal.hpp"

namespace mcbsp {

	void BSP_program::begin( const MCBSP_PROCESSOR_INDEX_DATATYPE P ) {
		//set number of threads to use
		this->P = P;
		//create a BSP-program specific initial data struct
		struct mcbsp_init_data *initialisationData = static_cast< struct mcbsp_init_data * >( malloc( sizeof( struct mcbsp_init_data ) ) );
		if( initialisationData == NULL ) {
			fprintf( stderr, "Error: could not allocate MulticoreBSP initialisation struct!\n" );
			mcbsp_util_fatal();
		}
		//set values
		initialisationData->spmd 	= &mcbsp_cpp_callback;
		initialisationData->bsp_program = static_cast< void * >( this );
		initialisationData->argc 	= 0;
		initialisationData->argv 	= NULL;

		//continue initialisation
		bsp_init_internal( initialisationData );

		//call SPMD part
		mcbsp_cpp_callback();
	}

	void mcbsp_cpp_callback() {
		const struct mcbsp_thread_data * const data = static_cast< struct mcbsp_thread_data * >( pthread_getspecific( mcbsp_internal_thread_data ) );
		BSP_program *bsp_program = NULL;
		if( data != NULL ) {
			bsp_program = static_cast< BSP_program * >( data->init->bsp_program );
		} else {
			const struct mcbsp_init_data * const init = static_cast< struct mcbsp_init_data * >( pthread_getspecific( mcbsp_internal_init_data ) );
			if( init == NULL ) {
				std::cerr << "Error during initialisation of the MulticoreBSP C++ wrapper; no initialisation data found.\n" << std::endl;
				mcbsp_util_fatal();
			}
			bsp_program = static_cast< BSP_program * >( init->bsp_program );
		}
		const MCBSP_PROCESSOR_INDEX_DATATYPE P = bsp_program->P;

		bsp_begin( P );
		if( bsp_pid() == 0 )
			bsp_program->spmd();
		else {
			BSP_program *myInstance = bsp_program->newInstance();
			myInstance->spmd();
			delete myInstance;
		}
		bsp_end();
	}

}


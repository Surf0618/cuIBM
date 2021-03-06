/**
 * \file generateM.inl
 * \brief Implementation of the methods to generate the mass matrix and its inverse.
 */


#include <solvers/kernels/generateM.h>


/**
 * \brief Generates the mass matrix and its inverse.
 *
 * The mass diagonal matrix is stored in a COO format. 
 * The time-increment value is included in the matrix.
 *
 */
template<>
void NavierStokesSolver<device_memory>::generateM()
{
	int nx = domInfo->nx,
	    ny = domInfo->ny;

	real dt = (*paramDB)["simulation"]["dt"].get<real>();
	
	real *dxD = thrust::raw_pointer_cast(&(domInfo->dxD[0])),
	     *dyD = thrust::raw_pointer_cast(&(domInfo->dyD[0]));
	
	int numU  = (nx-1)*ny;
	int numUV = numU + nx*(ny-1);
	M.resize(numUV, numUV, numUV);
	Minv.resize(numUV, numUV, numUV);
	
	int *MRows = thrust::raw_pointer_cast(&(M.row_indices[0])),
	    *MCols = thrust::raw_pointer_cast(&(M.column_indices[0])),
	    *MinvRows = thrust::raw_pointer_cast(&(Minv.row_indices[0])),
	    *MinvCols = thrust::raw_pointer_cast(&(Minv.column_indices[0]));
	
	real *MVals = thrust::raw_pointer_cast(&(M.values[0])),
	     *MinvVals = thrust::raw_pointer_cast(&(Minv.values[0]));
	
	const int blockSize = 256;
	dim3 dimGrid( int((nx*ny-0.5)/blockSize) + 1, 1);
	dim3 dimBlock(blockSize, 1);
	kernels::fillM_u <<<dimGrid, dimBlock>>> (MRows, MCols, MVals, MinvRows, MinvCols, MinvVals, nx, ny, dxD, dyD, dt);
	kernels::fillM_v <<<dimGrid, dimBlock>>> (MRows, MCols, MVals, MinvRows, MinvCols, MinvVals, nx, ny, dxD, dyD, dt);
} // generateM

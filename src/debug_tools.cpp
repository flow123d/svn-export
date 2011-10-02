#include <string>
#include <fstream>
#include <iostream>
#include "petscmat.h"
#include <mpi.h>

#include <vector>

// simple function for exporting a PETSc matrix into a given file
void print_matrix_with_export( Mat mat, std::string fileName ) {

    Mat matrixForPrint;
    PetscInt m, n;
    PetscErrorCode ierr;
    MatGetSize( mat, &m, &n );
    std::cout << "Size of the matrix: " << m << " " << n << std::flush << std::endl; 

    ierr = MatCreate( PETSC_COMM_WORLD, &matrixForPrint ); CHKERRV( ierr ); 
    ierr = MatSetType( matrixForPrint, MATMPIAIJ ); CHKERRV( ierr ); 
    ierr = MatSetSizes( matrixForPrint, PETSC_DECIDE, PETSC_DECIDE, m, n ); 

    Vec auxIn, auxOut;
    for ( int i = 0; i < n; i++ ) {
        // create auxiliary vector of unit matrix
        ierr = MatGetVecs( mat, &auxIn, &auxOut ); CHKERRV( ierr );

        VecSetValue( auxIn, i, 1., INSERT_VALUES );
        ierr = VecAssemblyBegin( auxIn ); CHKERRV( ierr ); 
        ierr = VecAssemblyEnd(   auxIn ); CHKERRV( ierr ); 
 
        ierr = MatMult( mat, auxIn, auxOut ); CHKERRV( ierr ); 

        PetscInt low, high;
        VecGetOwnershipRange( auxOut, &low, &high );
        PetscInt locSize = high - low;

        PetscScalar *values;
        VecGetArray( auxOut, &values );

        std::vector<PetscInt> rows;
        std::vector<PetscInt> columns;
        for ( int j = low; j < high; j++ ) {
            rows.push_back( j );
        }
        columns.push_back( i );

        MatSetValues( matrixForPrint, locSize, &(rows[0]), 1, &(columns[0]), values, INSERT_VALUES );

        VecRestoreArray( auxOut, &values );
        VecDestroy( auxIn );
        VecDestroy( auxOut );
    }
    ierr = MatAssemblyBegin( matrixForPrint, MAT_FINAL_ASSEMBLY ); CHKERRV( ierr ); 
    ierr = MatAssemblyEnd(   matrixForPrint, MAT_FINAL_ASSEMBLY ); CHKERRV( ierr ); 

    PetscViewer matViewer;
    PetscViewerASCIIOpen( PETSC_COMM_WORLD, fileName.c_str(), &matViewer );
    PetscViewerSetFormat(matViewer,PETSC_VIEWER_ASCII_MATLAB);
    MatView( matrixForPrint, matViewer );
    MatDestroy( matrixForPrint );
    PetscViewerDestroy(matViewer);
}

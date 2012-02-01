/*!
 *
 * Copyright (C) 2007 Technical University of Liberec.  All rights reserved.
 *
 * Please make a following refer to Flow123d on your project site if you use the program for any purpose,
 * especially for academic research:
 * Flow123d, Research Centre: Advanced Remedial Technologies, Technical University of Liberec, Czech Republic
 *
 * This program is free software; you can redistribute it and/or modify it under the terms
 * of the GNU General Public License version 3 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 021110-1307, USA.
 *
 *
 * $Id$
 * $Revision$
 * $LastChangedBy$
 * $LastChangedDate$
 *
 * @file
 * @brief   Wrappers for linear systems based on MPIAIJ and MATIS format.
 * @author  Jan Brezina
 *
 * Linear system only keep together matrix, RHS and the solution vector.
 *
 */


//=============================================================================
//
// LINER SYSTEM ROUTINES - linear system use wrapers of PETSc assembling routins
// in order to allow counting of allocation and filling of matrix to be done by the same code
//
// we want to allow allocations of nonlocal rows, to this end we count on- and off-processor
// members in an parallel Vector
//
//=============================================================================

#ifndef LA_LINSYS_HH_
#define LA_LINSYS_HH_

/**
 *  @brief  Abstract linear system class.
 *
 *  Linear system consists of Matrix, RHS and solution.
 *  It provides general methods for:
 *  - matrix preallocation
 *  - assembling matrix and RHS
 *  - application of linear constrains (Dirichlet conditions) either during assembly or
 *    on assembled system
 *  - solution of the system
 *  - output in Matlab format
 *
 *  Method operates on the system as single object. But some methods for matrix only manipulation
 *  can be provided until we have matrix as separate class.
 */

#include "system/par_distribution.hh"
#include "mesh/mesh.h"

#include <mpi.h>

class LinSys
{
public:
    typedef enum {
        INSERT=INSERT_VALUES,
        ADD=ADD_VALUES,
        ALLOCATE,
        DONE,
        NONE
    } SetValuesMode;

    typedef enum {
        PETSC_MPIAIJ_preallocate_by_assembly,
        PETSC_MPIAIJ_assembly_by_triples,
        BDDC,
        PETSC_schur_complement   // possibly we can implement Schur as another kind of lin solver
    } LinSysType;

private:
    typedef std::pair<unsigned,double>       Constraint_;
    typedef std::vector< Constraint_ >       ConstraintVec_;

public:
    /**
     * Constructor.
     * Constructor of abstract class should not be called directly, but is used for initialization of member common
     * to all particular solvers.
     *
     * @param lsize - local size of the solution vector
     * @param sol_array - optionally one can provide pointer to array allocated to size lsize, where
     *  the solution should be stored,
     */
    LinSys( MPI_Comm comm = MPI_COMM_WORLD )
      : comm_( comm )
    { };

    /// Particular type of the linear system.
    LinSysType  type;   ///< MAT_IS or MAT_MPIAIJ anyone can inquire my type

    virtual void load_mesh( Mesh *mesh,
                            Distribution *edge_ds,  
                            Distribution *el_ds,        
                            Distribution *side_ds,     
                            Distribution *rows_ds,    
                            int *el_4_loc,    
                            int *row_4_el,     
                            int *side_id_4_loc, 
                            int *side_row_4_id, 
                            int *edge_4_loc,   
                            int *row_4_edge )
    {
        ASSERT( false, "Function load_mesh is not implemented for linsys type %d \n.", this -> type );
    }

    /**
     *  Returns global system size.
     */
    inline unsigned int size()
    { 
        ASSERT ( rows_ds_ != NULL, "Empty distribution." );
        return rows_ds_-> size(); 
    }

    /**
     * Returns local system size. (for partitioning of solution vectors)
     * for PETSC_MPIAIJ it is also partitioning of the matrix
     */
    inline unsigned int vec_lsize()
    { 
        ASSERT ( rows_ds_ != NULL, "Empty distribution." );
        return rows_ds_ -> lsize(); 
    }

    /**
     * Returns whole Distribution class for distribution of the solution.
     */
    inline const Distribution* get_ds( )
    { return rows_ds_; }

    /**
     * Returns PETSC matrix (only for PETSC solvers)
     */
    virtual const Mat &get_matrix()
    {
        ASSERT( false, "Function get_matrix is not implemented for linsys type %d \n.", this -> type );
    }
    //{ return matrix; }

    /**
     * Returns RHS vector  (only for PETSC solvers)
     */
    virtual const Vec &get_rhs()
    {
        ASSERT( false, "Function get_rhs is not implemented for linsys type %d \n.", this -> type );
    }
    //{ return rhs; }
    

    /**
     *  Returns PETSC vector with solution. Underlying array can be provided on construction.
     */
    virtual const Vec &get_solution()
    {
        ASSERT( false, "Function get_solution is not implemented for linsys type %d \n.", this -> type );
    }

    /**
     *  Returns PETSC subarray with solution. Underlying array can be provided on construction.
     */
    virtual double *get_solution_array()
    {
        ASSERT( false, "Function get_solution_array is not implemented for linsys type %d \n.", this -> type );
    }
    
    /**
     * Returns whole solution vector.
     */
    virtual void get_whole_solution( std::vector<double> & globalSolution )
    {
        ASSERT( false, "Function get_whole_solution is not implemented for linsys type %d \n.", this -> type );
    }

    /**
     * Inserts solution vector.
     */
    virtual void set_whole_solution( std::vector<double> & globalSolution )
    {
        ASSERT( false, "Function set_whole_solution is not implemented for linsys type %d \n.", this -> type );
    }
    
    /**
     * Switch linear system into allocating assembly. (only for PETSC_MPIAIJ_preallocate_by_assembly)
     */
    virtual void start_allocation()
    {
        ASSERT( false, "Function start_allocation is not implemented for linsys type %d \n.", this -> type );
    }

    /**
     * Switch linear system into adding assembly. (the only one supported by triplets ??)
     */
    virtual void start_add_assembly()
    {
        ASSERT( false, "Function start_add_assembly is not implemented for linsys type %d \n.", this -> type );
    }

    /**
     * Switch linear system into insert assembly. (not currently used)
     */
    virtual void start_insert_assembly()
    {
        ASSERT( false, "Function start_insert_assembly is not implemented for linsys type %d \n.", this -> type );
    }

    /**
     * Finish assembly of the whole system. For PETSC this should call MatEndAssembly with MAT_FINAL_ASSEMBLY
     */
    virtual void finish_assembly()=0;

    /**
     *  Assembly full rectangular submatrix into the system matrix.
     *  Should be virtual, implemented differently in  particular solvers.
     */
    virtual void mat_set_values(int nrow,int *rows,int ncol,int *cols,double *vals)=0;

    /**
     * Shortcut for assembling just one element into the matrix.
     * Similarly we can provide method accepting armadillo matrices.
     */
    void mat_set_value(int row,int col,double val)
    { mat_set_values(1,&row,1,&col,&val); }

    /**
     *  Set values of the system right-hand side.
     *  Should be virtual, implemented differently in  particular solvers.
     */
    virtual void rhs_set_values(int nrow,int *rows,double *vals)=0;

    /**
     * Shorcut for assembling just one element into RHS vector.
     */
    void rhs_set_value(int row,double val)
    { rhs_set_values(1,&row,&val); }

    /**
     * Shortcut to assembly into matrix and RHS in one call.
     * This can also apply constrains at assembly time (only in add assembly regime).
     *
     * Constrains can either be set before through add_constrain. Or by additional parameters if we
     * have only per element knowledge about boundary conditions.
     *
     */
    void set_values( int nrow,int *rows,int ncol,int *cols,double *mat_vals, double *rhs_vals )
//                            std::vector<bool> &constrains_row_mask=std::vector<bool>(), double * constrain_values=NULL )
    {
        mat_set_values(nrow, rows, ncol, cols, mat_vals);
        rhs_set_values(nrow, rows, rhs_vals);
    }

    /**
     * Adds Dirichlet constrain.
     * @param row - global number of row that should be eliminated.
     * @param value - solution value at the given row
     */
    void add_constrain(int row, double value) {

        constraints_.push_back( Constraint_( static_cast<unsigned>( row ), value ) );
    }

    /**
     * Apply constrains to assembled matrix. Constrains are given by pairs: global row index, value.
     * i.e. typedef pair<unsigned int, double> Constrain;
     *
     * What is th meaning of ( const double factor ) form Cambridge code?
     */

    virtual void apply_constrains( )=0;

    /**
     * Solve the system.
     *
     * parameters should by provided in input file (currently INI file, but will be changed to JSON)
     *
     * If we realize that we need to set something, rather add some set_* function.
     *
     * double tol = 1.e-7,                        //!< tolerance on relative residual ||res||/||rhs||
       int  numLevels = 2,                        //!< number of levels
       std::vector<int> *  numSubAtLevels = NULL, //!< number of subdomains at levels
       int verboseLevel = 0,                      //!< level of verbosity of BDDCML library
                                                  //!< ( 0 - only fatal errors reported,
                                                  //!<   1 - mild output,
                                                  //!<   2 - detailed output )
       int  maxIt = 1000,                         //!< maximum number of iterations
       int  ndecrMax = 30 );                      //!< maximum number of iterations with non-decreasing residual
                                                  //!< ( used to stop diverging process )
     *
     *
     * Returns convergence reason
     */
    virtual int solve()=0;

    /**
     * Provides user knowledge about symmetry.
     */
    inline void set_symmetric(bool flag = true)
    {
        symmetric_ = flag;
        if (!flag) set_positive_definite(false);
    }

    inline bool is_symmetric()
    { return symmetric_; }

    /**
     * Provides user knowledge about positive definiteness.
     */
    inline void set_positive_definite(bool flag = true)
    {
        positive_definite_ = flag;
        if (flag) set_symmetric();
    }

    inline bool is_positive_definite()
    { return positive_definite_; }

    /**
     * Provides user knowledge about positive definiteness via symmetric general approach.
     * This is useful for solving Darcy flow by mixed hybrid method, where blocks on subdomains are saddle point but 
     * interface among subdomains is only at the block of Lagrange multipliers and is symmetric positive definite.
     * Problem condensed to interface can thus be solved by PCG method, although original problem is saddle point.
     */
    inline void set_spd_via_symmetric_general(bool flag = true)
    {
        spd_via_symmetric_general_ = flag;
        if (flag) set_symmetric();
    }

    inline bool is_spd_via_symmetric_general()
    { return spd_via_symmetric_general_; }


    /**
     *  Output the system in the Matlab format possibly with given ordering.
     *  Rather we shoud provide output operator <<, since it is more flexible.
     */
    virtual void view(std::ostream output_stream, int * output_mapping = NULL)
    {
        ASSERT( false, "Function view is not implemented for linsys type %d \n.", this -> type );
    }

    //~LinSys(){};

protected:
    MPI_Comm         comm_;

    bool             symmetric_;
    bool             positive_definite_;
    bool             spd_via_symmetric_general_;

    ConstraintVec_   constraints_;

    Distribution *   rows_ds_;        //!< final distribution of rows of MH matrix
};

#endif /* LA_LINSYS_HH_ */

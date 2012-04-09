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
 * @brief  Support classes for parallel programing.
 *
 */

#ifndef MESH_PARTITION_HH_
#define MESH_PARTITION_HH_

#include <petscvec.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>




/**
 * Continuous distribution of an 1D array of indexes.
 * Default is PETSC_COMM_WORLD communicator, but you can provide another.
 */

class Distribution {
public:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & num_of_procs;
        ar & my_proc;
        ar & starts;
        ar & lsizes;

        ar & communicator;
    }
    /**
     * Type of distribution automatically created only from global number of indices.
     */
    typedef enum  {
        /** Distribute indices evenly. */
        Block=-1,
        /** Put all on the first processor. */
        Localized=-2
    } SpecialDistribution;

    /**
     * Constructor. It makes distribution from given local number of indices on every processor.
     *
     * COLLECTIVE
     * @param size Local size on calling processor.
     * @param comm (optional) MPI Communicator. Default PETSC_COMM_WORLD.
     */
    Distribution(const unsigned int size);

    /**
     * Constructor. It makes distribution from given array of sizes of processors.
     *
     * NOT COLLECTIVE, but still use MPI to provide information about processors.
     *
     * @param sizes Int array with sizes.
     * @param comm (optional) MPI Communicator. Default PETSC_COMM_WORLD.
     */
    Distribution(const unsigned int * const sizes);

    /**
     * Constructor. It makes distribution from distribution of a PETSC vector.
     *
     * NOT COLLECTIVE, but still use MPI to provide information about processors.
     * @param petsc_vector
     * @param comm (optional) MPI Communicator. Default PETSC_COMM_WORLD.
     */
    Distribution(const Vec &petsc_vector);

    /**
     * Constructor. It makes distribution from global number of indices according to given scheme.
     *
     * NOT COLLECTIVE, but still use MPI to provide information about processors.
     * @param type Either Block or Localized distribution of indices.
     * @param global_size Total number of indices to distribute.
     * @param comm (optional) MPI Communicator. Default PETSC_COMM_WORLD.
     */
    Distribution(const SpecialDistribution type,unsigned int global_size);

    /**
     * Copy Constructor.
     */
    Distribution(const Distribution &distr);

    /// get num of processors
    inline int np() const {return num_of_procs;}
    /// get my processor
    inline int myp() const {return my_proc;}
    /// get starting local index
    inline int begin(int proc) const {return (starts[proc]);}
    inline int begin() const {return ( begin(myp()) );}
    /// get last local index +1
    inline int end(int proc) const {return (starts[proc+1]);}
    inline int end()  const {return ( end(myp()) );}
    /// get local size
    inline int lsize(int proc) const {return (end(proc)-begin(proc));}
    inline int lsize() const {return ( lsize(myp()) );}
    /// get global size
    inline int size() const {return (starts[np()]);}
    /// identify local index
    inline bool is_local(int idx) const {return ( begin()<=(idx) && (idx)<end() );}
    inline bool is_on_proc(int idx,int proc) const {return ( begin(proc)<=(idx) && (idx)<end(proc) );}
    /// get processor of the given index
    int get_proc(int idx) const;
    /// get local sizes array
    const int * get_lsizes_array();
    inline MPI_Comm get_comm() {return communicator;}
    /// distribution view
    void view();
    ~Distribution();
private:
    /// communicator
    const MPI_Comm communicator;
    /// number of procs
    int num_of_procs;
    /// my proc number
    int my_proc;
    /// starts[i] index of the first index on the proc i; starts[n_procs]=size of whole array
    int *starts;
    /// local sizes
    int *lsizes;
};
typedef class Distribution Distribution;



#endif // MESH_PARTITION_HH_

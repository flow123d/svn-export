/*
 * intersection.cc
 *
 *  Created on: May 24, 2011
 *      Author: jb
 */

#include "mesh/intersection.hh"


// inicializovat objekt, cist zbytek tokenu z tok a naplnit map a shift pro master a slave
// viz dokumentace k Armadillu
Intersection::Intersection(unsigned int dimension, ElementFullIter ele_master, ElementFullIter ele_slave, tokenizer<>::iterator &tok)
: master(ele_master),
  slave(ele_slave)
{

}

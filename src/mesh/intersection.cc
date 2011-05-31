/*
 * intersection.cc
 *
 *  Created on: May 24, 2011
 *      Author: jb
 */

#include "mesh/intersection.hh"
#include "xio.h"
#include <boost/tokenizer.hpp>
#include "boost/lexical_cast.hpp"
#include <armadillo>

// inicializovat objekt, cist zbytek tokenu z tok a naplnit map a shift pro master a slave
// viz dokumentace k Armadillu
Intersection::Intersection(unsigned int dimension, ElementFullIter ele_master,
		ElementFullIter ele_slave, tokenizer<boost::char_separator<char> >::iterator &tok) :
	dim(dimension),
	master(ele_master), slave(ele_slave),
	master_shift(master->dim), slave_shift(slave->dim),
	master_map(master->dim, dim), slave_map(slave->dim, dim)
{
	///otestuje se jestli dimenze masteru je mensi nez dimenze slave - chybova hlaska (vyjimka - throw)
	///pocet pointu=dim+1
	if (master->dim > slave->dim) {
		cout << "Exception: master->dim < slave->dim" << endl;
		//throw((char*) "master->dim < slave->dim");
	}
	//arma::vec vertex(master->dim);
	read_intersection_point(master_shift, slave_shift, tok);

	arma::vec master_tmp(master_shift), slave_tmp(slave_shift);
	// cyklus pres body pruniku
	for (unsigned int i = 1; i < (dim + 1); ++i) {
		read_intersection_point(master_tmp, slave_tmp, tok);
		master_tmp -= master_shift;
		slave_tmp -= slave_shift;

		master_map.col(i-1) = master_tmp;
		slave_map.col(i-1) = slave_tmp;
	}
	//TISK
	master_shift.print("master_shift: ");
	slave_shift.print("slave_shift: ");
	//master_map.print("master_map: ");
	//slave_map.print("slave_map: ");
	master_map.raw_print(cout, "master_map = ");
	slave_map.raw_print(cout, "slave_map = ");
}

void Intersection::read_intersection_point(arma::vec &vec1, arma::vec &vec2,
		tokenizer<boost::char_separator<char> >::iterator &tok) {

	// pocet lokalnich souradnic 1. elementu
    cout << "n_insec_points_el1: " << (*tok) << endl;
	int n_insec_points_el1 = lexical_cast<int> (*tok);
	++tok;
	INPUT_CHECK(n_insec_points_el1 == vec1.n_elem, "Exception: n_insec_points_el1 != vec1.n_elem");

	for (unsigned int j = 0; j < n_insec_points_el1; ++j) {
		double coords1 = lexical_cast<double> (*tok); ++tok;
		vec1[j] = coords1;
	}

	// pocet lokalnich souradnic 2. elementu
    cout << "n_insec_points_el2: " << (*tok) << endl;
	int n_insec_points_el2 = lexical_cast<int> (*tok);
	++tok;
	INPUT_CHECK(n_insec_points_el2 == vec2.n_elem, "Exception: n_insec_points_el2 != vec2.n_elem");

	for (unsigned int k = 0; k < n_insec_points_el2; ++k) {
		double coords2 = lexical_cast<double> (*tok); ++tok;
		vec2[k] = coords2;
	}

}

arma::vec Intersection::map_to_master(const arma::vec &point) const
{
	//dim = dimenze intersec elementu
	ASSERT(( point.n_elem == dim ),"Map to master: point.n_elem != dim \n");
	return (master_map * point + master_shift);
}

arma::vec Intersection::map_to_slave(const arma::vec &point) const
{
	ASSERT(( point.n_elem == dim ),"Map to slave: point.n_elem != dim \n");
	return (master_map * point + master_shift);
}

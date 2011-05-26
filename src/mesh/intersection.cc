/*
 * intersection.cc
 *
 *  Created on: May 24, 2011
 *      Author: jb
 */

#include "mesh/intersection.hh"
#include "xio.h"
#include<boost/tokenizer.hpp>
#include "boost/lexical_cast.hpp"
#include <armadillo>

// inicializovat objekt, cist zbytek tokenu z tok a naplnit map a shift pro master a slave
// viz dokumentace k Armadillu
Intersection::Intersection(unsigned int dimension, ElementFullIter ele_master,
		ElementFullIter ele_slave, tokenizer<>::iterator &tok) :
	dim(dimension),
	master(ele_master), slave(ele_slave),
	master_shift(master->dim), slave_shift(slave->dim),
	master_map(master->dim, dim), slave_map(slave->dim, dim)
{
	///otestuje se jestli dimenze masteru je mensi nez dimenze slave - chybova hlaska (vyjimka - throw)
	///pocet pointu=dim+1
	if (master->dim < slave->dim) {
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
}

void Intersection::read_intersection_point(arma::vec &vec1, arma::vec &vec2,
		tokenizer<>::iterator &tok) {

	// pocet lokalnich souradnic 1. elementu
	int n_insec_points_el1 = lexical_cast<int> (*tok);
	++tok;
	INPUT_CHECK(n_insec_points_el1 == vec1.n_elem, "Exception: n_insec_points_el1 != vec1.n_elem");

	for (unsigned int j = 0; j < n_insec_points_el1; ++j) {
		double coords1 = lexical_cast<double> (*tok);
		vec1[j] = coords1;
	}

	// pocet lokalnich souradnic 2. elementu
	int n_insec_points_el2 = lexical_cast<int> (*tok);
	++tok;
	INPUT_CHECK(n_insec_points_el2 == vec2.n_elem, "Exception: n_insec_points_el2 != vec2.n_elem");

	for (unsigned int k = 0; k < n_insec_points_el2; ++k) {
		double coords2 = lexical_cast<double> (*tok);
		vec2[k] = coords2;
	}

}

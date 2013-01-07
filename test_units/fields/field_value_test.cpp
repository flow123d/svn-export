/*
 * field_value_test.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: jb
 */


#include <gtest/gtest.h>
#include <fields/field_values.hh>

#include <iostream>
using namespace std;

TEST(FieldValue_, all) {

    {
        typedef FieldValue_<2,2, double> T;
        T::return_type x_val;
        T val(x_val);
        val(0,0)=3.0; val(0,1)=4.0;
        EXPECT_EQ(3.0,  val(0,0) );
        EXPECT_EQ(4.0,  val(0,1) );

        T::return_type r_val = val;
        EXPECT_EQ(3.0,  r_val(0,0) );
        EXPECT_EQ(4.0,  r_val(0,1) );

    }

}


/**
 * Speed results:
 * debug (-g -O0 -NODEBUG) (100 M steps):
 * interface: 1747ms
 * direct   :  361ms
 *
 * optimized -O3 (100 M steps):
 * interface: 123ms
 * direct   : 121ms
 */

TEST(FieldValue_, speed_test_interface) {

   typedef FieldValue_<1,1, double> T;
   double r_val;


   for(int step=0;step < 100000000; step++) {
       T val(r_val);

       for(int row=0;row< val.n_cols(); ++row)
           for(int col=0;col< val.n_rows(); ++col)
               val(row,col)+=step;
   }
   cout << r_val << endl;

}

TEST(FieldValue_, speed_test_direct) {

   double val;

   for(int step=0;step < 100000000; step++) {
       val+=step;
   }
   cout << val << endl;
}



string input = R"INPUT(
{   
double_scalar=1.3,
int_scalar=23,

double_fix_vector_full=[1.2, 3.4, 5.6],
int_fix_vector_full=[1,2,3],
double_fix_vector_const=1.3,
int_fix_vector_const=23,

double_vector_full=[1.2,3.4],
int_vector_full=[1,2,3,4],
double_vector_const=1.2,
int_vector_const=23,

double_fix_tensor_full=[ [1.1, 1.2, 1.3], [2.1, 2.2, 2.3] ],
double_fix_tensor_symm=[ 1, 2, 3],
double_fix_tensor_diag=[1,2],
double_fix_tensor_cdiag=1.3
}
)INPUT";

#include "input/input_type.hh"
#include "input/accessors.hh"
#include "input/json_to_storage.hh"

TEST(FieldValue_, init_from_input) {
    // setup FilePath directories
    FilePath::set_io_dirs(".",UNIT_TESTS_SRC_DIR,"",".");

    Input::Type::Record  rec_type("FieldValueTest","");
    rec_type.declare_key("double_scalar",FieldValue_<1,1,double>::get_input_type(), Input::Type::Default::obligatory(),"" );
    rec_type.declare_key("int_scalar",FieldValue_<1,1,int>::get_input_type(), Input::Type::Default::obligatory(),"" );

    rec_type.declare_key("double_fix_vector_full",FieldValue_<3,1,double>::get_input_type(), Input::Type::Default::obligatory(),"" );
    rec_type.declare_key("int_fix_vector_full",FieldValue_<3,1,int>::get_input_type(), Input::Type::Default::obligatory(),"" );
    rec_type.declare_key("double_fix_vector_const",FieldValue_<3,1,double>::get_input_type(), Input::Type::Default::obligatory(),"" );
    rec_type.declare_key("int_fix_vector_const",FieldValue_<3,1,int>::get_input_type(), Input::Type::Default::obligatory(),"" );

    rec_type.declare_key("double_vector_full",FieldValue_<0,1,double>::get_input_type(), Input::Type::Default::obligatory(),"" );
    rec_type.declare_key("int_vector_full",FieldValue_<0,1,int>::get_input_type(), Input::Type::Default::obligatory(),"" );
    rec_type.declare_key("double_vector_const",FieldValue_<0,1,double>::get_input_type(), Input::Type::Default::obligatory(),"" );
    rec_type.declare_key("int_vector_const",FieldValue_<0,1,int>::get_input_type(), Input::Type::Default::obligatory(),"" );

    rec_type.declare_key("double_fix_tensor_full",FieldValue_<2,3,double>::get_input_type(), Input::Type::Default::obligatory(),"" );
    rec_type.declare_key("double_fix_tensor_symm",FieldValue_<2,2,double>::get_input_type(), Input::Type::Default::obligatory(),"" );
    rec_type.declare_key("double_fix_tensor_diag",FieldValue_<2,2,double>::get_input_type(), Input::Type::Default::obligatory(),"" );
    rec_type.declare_key("double_fix_tensor_cdiag",FieldValue_<2,2,double>::get_input_type(), Input::Type::Default::obligatory(),"" );

    rec_type.finish();

    // read input string
    std::stringstream ss(input);
    Input::JSONToStorage reader;
    reader.read_stream( ss, rec_type );
    Input::Record in_rec=reader.get_root_interface<Input::Record>();


    {
        typedef FieldValue_<1,1,double> T; T::return_type x_val; T val(x_val);
        val.init_from_input(in_rec.val<double>("double_scalar"));
        EXPECT_EQ(T::return_type(val), 1.3);
    }
    {
        typedef FieldValue_<1,1,int> T; T::return_type x_val; T val(x_val);
        val.init_from_input(in_rec.val<int>("int_scalar"));
        EXPECT_EQ(T::return_type(val), 23);
    }


    {
        typedef FieldValue_<3,1,double> T; T::return_type x_val; T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("double_fix_vector_full"));
        T::return_type expected("1.2 3.4 5.6");
        EXPECT_TRUE( arma::max(expected == T::return_type(val)) );
    }
    {
        typedef FieldValue_<3,1,int> T; T::return_type x_val; T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("int_fix_vector_full"));
        EXPECT_TRUE( arma::max(T::return_type("1 2 3") == T::return_type(val)) );
    }
    {
        typedef FieldValue_<3,1,double> T; T::return_type x_val; T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("double_fix_vector_const"));
        EXPECT_TRUE( arma::max(T::return_type("1.3 1.3 1.3") == T::return_type(val)) );
    }
    {
        typedef FieldValue_<3,1,int> T; T::return_type x_val; T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("int_fix_vector_const"));
        EXPECT_TRUE( arma::max(T::return_type("23 23 23") == T::return_type(val)) );
    }


    {
        typedef FieldValue_<0,1,double> T; T::return_type x_val(2); T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("double_vector_full"));
        EXPECT_TRUE( arma::max(T::return_type("1.2 3.4") == T::return_type(val)) );
    }
    {
        typedef FieldValue_<0,1,int> T; T::return_type x_val(4); T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("int_vector_full"));
        EXPECT_TRUE( arma::max(T::return_type("1 2 3 4") == T::return_type(val)) );
    }
    {
        typedef FieldValue_<0,1,double> T; T::return_type x_val(3); T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("double_vector_const"));
        EXPECT_TRUE( arma::max(T::return_type("1.2 1.2 1.2") == T::return_type(val)) );
    }
    {
        typedef FieldValue_<0,1,int> T; T::return_type x_val(3); T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("int_vector_const"));
        EXPECT_TRUE( arma::max(T::return_type("23 23 23") == T::return_type(val)) );
    }


    {
        typedef FieldValue_<2,3,double> T; T::return_type x_val; T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("double_fix_tensor_full"));
        arma::umat match = (T::return_type("1.1 1.2 1.3; 2.1 2.2 2.3") == T::return_type(val));
        EXPECT_TRUE( match.max());
    }

    {
        typedef FieldValue_<2,2,double> T; T::return_type x_val; T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("double_fix_tensor_symm"));
        arma::umat match = (T::return_type("1 2; 2 3") == T::return_type(val));
        EXPECT_TRUE( match.max());
    }
    {
        typedef FieldValue_<2,2,double> T; T::return_type x_val; T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("double_fix_tensor_diag"));
        arma::umat match = (T::return_type("1 0; 0 2") == T::return_type(val));
        EXPECT_TRUE( match.max());
    }
    {
        typedef FieldValue_<2,2,double> T; T::return_type x_val; T val(x_val);
        val.init_from_input(in_rec.val<Input::Array>("double_fix_tensor_cdiag"));
        arma::umat match = (T::return_type("1.3 0; 0 1.3") == T::return_type(val));
        EXPECT_TRUE( match.max());
    }
}
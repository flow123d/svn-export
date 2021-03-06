/*
 * python_function_test.cpp
 *
 *  Created on: Aug 30, 2012
 *      Author: jb
 */



#include <gtest/gtest.h>
#include <string>
#include <cmath>


#ifdef HAVE_PYTHON

#include "system/python_loader.hh"
#include "fields/field_python.hh"
#include "input/input_type.hh"
#include "input/accessors.hh"
#include "input/json_to_storage.hh"

using namespace std;

string python_code = R"CODE(
def testFunc():
    print "spam!"

class testClass:
    def testMethod(self):
        print "eggs!"
)CODE";

string python_function = R"CODE(
import math

def func_xyz(x,y,z):
    return ( x*y*z , )     # one value tuple

def func_circle(r,phi):
    return ( r * math.cos(phi), r * math.sin(phi) )
)CODE";

string input = R"INPUT(
{   
   field_string={
       TYPE="FieldPython",
       function="func_circle",
       script_string="import math\ndef func_circle(r,phi): return ( r * math.cos(phi), r * math.sin(phi) )"
   },
   field_file={
       TYPE="FieldPython",
       function="func_xyz",
       script_file="fields/field_python_script.py"
   }
}
)INPUT";


TEST(PythonLoader, all) {
    PyObject * module = PythonLoader::load_module_from_string("my_module", python_code);
    PyObject * p_func = PyObject_GetAttrString(module, "testFunc" );
    PyObject * p_args = PyTuple_New( 0 );
    PyObject_CallObject(p_func, p_args); // this should print out 'spam!'
}

TEST(FieldPython, vector_2D) {

    double pi = 4.0 * atan(1);

    Point<2> point_1, point_2;
    point_1(0)=1.0; point_1(1)= pi / 2.0;
    point_2(0)= sqrt(2.0); point_2(1)= 3.0 * pi / 4.0;

    FieldPython<2, FieldValue<2>::VectorFixed > vec_func(0.0);
    vec_func.set_python_field_from_string(python_function, "func_circle");
    ElementAccessor<2> elm;

    arma::vec2 result;
    {
    result = vec_func.value( point_1, elm);
    EXPECT_DOUBLE_EQ( cos(pi /2.0 ) , result[0]); // should be 0.0
    EXPECT_DOUBLE_EQ( 1, result[1]);
    }

    {
    result = vec_func.value( point_2, elm);
    EXPECT_DOUBLE_EQ( -1, result[0]);
    EXPECT_DOUBLE_EQ( 1, result[1]);
    }
}


TEST(FieldPython, double_3D) {
    Point<3> point_1, point_2;
    point_1(0)=1; point_1(1)=0; point_1(2)=0;
    point_2(0)=1; point_2(1)=2; point_2(2)=3;

    ElementAccessor<3> elm;
    FieldPython<3, FieldValue<3>::Scalar> scalar_func;
    scalar_func.set_python_field_from_string(python_function, "func_xyz");

    EXPECT_EQ( 0, scalar_func.value(point_1, elm));
    EXPECT_EQ( 6, scalar_func.value(point_2, elm));


}


TEST(FieldPython, read_from_input) {
    typedef FieldBase<2, FieldValue<2>::VectorFixed > VectorField;
    typedef FieldBase<3, FieldValue<3>::Scalar > ScalarField;
    double pi = 4.0 * atan(1);

    // setup FilePath directories
    FilePath::set_io_dirs(".",UNIT_TESTS_SRC_DIR,"",".");

    Input::Type::Record  rec_type("FieldPythonTest","");
    rec_type.declare_key("field_string", VectorField::input_type, Input::Type::Default::obligatory(),"" );
    rec_type.declare_key("field_file", ScalarField::input_type, Input::Type::Default::obligatory(), "" );
    rec_type.finish();

    // read input string
    std::stringstream ss(input);
    Input::JSONToStorage reader;
    reader.read_stream( ss, rec_type );
    Input::Record in_rec=reader.get_root_interface<Input::Record>();

    auto flux=VectorField::function_factory(in_rec.val<Input::AbstractRecord>("field_string"), 0.0);
    {
        Point<2> point_1, point_2;
        point_1(0)=1.0; point_1(1)= pi / 2.0;
        point_2(0)= sqrt(2.0); point_2(1)= 3.0 * pi / 4.0;

        ElementAccessor<2> elm;
        arma::vec2 result;

        result = flux->value( point_1, elm);
        EXPECT_DOUBLE_EQ( cos(pi /2.0 ) , result[0]); // should be 0.0
        EXPECT_DOUBLE_EQ( 1, result[1]);

        result = flux->value( point_2, elm);
        EXPECT_DOUBLE_EQ( -1, result[0]);
        EXPECT_DOUBLE_EQ( 1, result[1]);
    }

    auto conc=ScalarField::function_factory(in_rec.val<Input::AbstractRecord>("field_file"), 0.0);
    {
        Point<3> point_1, point_2;
        point_1(0)=1; point_1(1)=0; point_1(2)=0;
        point_2(0)=1; point_2(1)=2; point_2(2)=3;
        ElementAccessor<3> elm;

        EXPECT_EQ( 0, conc->value(point_1, elm));
        EXPECT_EQ( 6, conc->value(point_2, elm));
    }


}

#endif // HAVE_PYTHON


#ifndef GENERIC_NODE_HPP_
#define GENERIC_NODE_HPP_

#include <map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace flow {

class Object_node;
class Vector_node;
class Value_node;

/*
Kam to smeruje:

  Object         = cca MAP,        "klic"= objekt O , vektor V nebo skalar S
  Vector         = cca VECTOR,     "klic"= vector[ (O nebo V nebo S), (O V S), (O V S), ... ]
  Value (skalar) = cca basic type  = string, number, bool, null

Pro vsechny tridy, ziskani podstromu:
  val_type get_type()
  node &   get_key("klic")
  node &   get_key("klic", & def_tree )
  node &   get_item( index )
  node &   get_item( index, & def_tree )

Moznosti ziskani skalarnich hodnot:
  get_string(), get_int(), get_double(), get_float(), get_bool()

  get_*( void ) - kdyz neni, pada

  get_*( & def_val ) - kdyz neni, vrati default

  get_*_check( & err_code ) - kdyz neni, vrati chybu a pokracuje

Moznosti vkladani hodnot:
  err_code insert_key( "klic", & node ) - vlozi; pokud existuje, prepise
  err_code insert_item( index, & node ) - vlozi; pokud existuje, prepise

Finalni pouziti:
  root.get_key("output").get_key("step").get_int();
  root.get_key("output").get_key("step").get_int(0);

  root.get_key("output").get_key("step").get_int_check( my_err );
  if ( my_err != EXIT_SUCCESS ) {
    //zde osetrim chybku
  }

Pri chybe:
  vytvorit ve stromu lehky objekt, ktery si uchova informace o chybe a vratit referenci na nej
  (ted to vraci referenci na jediny tridni prazdny objekt)

*/


/*!
 * Types of nodes in graph
 */
enum Value_type { type_generic, type_string, type_number, type_object, type_vector, type_bool, type_null };

string & Value_type_to_str( const Value_type vt );

/*!
 * @brief Generic node - represents any JSON construct
 */
class Generic_node {
protected:
    Value_type           value_type_;
    //class data
    static Generic_node * empty_node_generic_;  //prazdna instance
    static Object_node  * empty_node_object_;  //prazdna instance
    static Vector_node  * empty_node_vector_;  //prazdna instance
    static Value_node   * empty_node_value_;  //prazdna instance
    static string         value_type_to_string[10]; //pro preklad enum Value_type na string.
                                                    //Lazy inicializace = pri prvnim cteni.
    static bool value_type_to_string_filled;        //Uz je inicializovano?

public:
    Generic_node() {
        value_type_ = type_generic;
    }

    //Generic_node( Generic_node const & n ); //copy constructor - deep?

    Value_type get_type( void ) const { return value_type_; } //ziska typ nodu. To by mel umet kazdy
    const string & get_type_str( void ) const;                //ziska typ nodu jako string popis

    virtual Generic_node & get_key( const string & key );
    virtual Generic_node & get_key( const string & key, Generic_node & default_tree );
    virtual Generic_node & get_key_check( const string & key, int & err_code );
    virtual Generic_node & get_item( const size_t id );
    virtual Generic_node & get_item( const size_t id, Generic_node & default_tree );
    virtual Generic_node & get_item_check( const size_t id, int & err_code );

    /* Implementace as_* tady nefunguje, je potreba znat presnou deklaraci tridy.
     * Forward deklarace nestaci (nevi, jake ma k dispozici metody apod.)
     */
    virtual Object_node & as_object( void );
    virtual Vector_node & as_vector( void );
    virtual Value_node & as_value( void );

    //ziskani uz finalnich skalarnich hodnot
    //bez def: pri nemoznosti konverze spadne
    //  s def: uspeje vzdy, protoze pri nemoznosti konverze pouzije default
    //  s err: uspeje vzdy, pri neuspechu vyplni err, jako hodnotu vrati 0, null, nebo ekvivalent
    virtual bool get_bool( void );
    virtual bool get_bool( const bool & default_value );
    virtual bool get_bool_check( int & err_code );

    virtual int get_int( void );
    virtual int get_int( const int & default_value );
    virtual int get_int_check( int & err_code );

    virtual float get_float( void );
    virtual float get_float( const float & default_value );
    virtual float get_float_check( int & err_code );

    virtual double get_double( void );
    virtual double get_double( const double & default_value );
    virtual double get_double_check( int & err_code );

    virtual string get_string( void );
    virtual string get_string( const string & default_value );
    virtual string get_string_check( int & err_code );


    // hierarchicky projde cely podstrom do hloubky a vypise ho
    friend ostream & operator<<( ostream & stream, const Generic_node & node ); //TODO dodelat

    virtual ~Generic_node();
private:
    Generic_node & operator=( Generic_node const & n ) { return (*this); }
};


}
#endif /* GENERIC_NODE_HPP_ */

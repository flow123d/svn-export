#ifndef GENERIC_NODE_H_
#define GENERIC_NODE_H_

#include <map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace flow {

class Object_node;
class Vector_node;
class Value_node;

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
public:
    Generic_node() { value_type_ = type_generic; }

    //Generic_node( Generic_node const & n ); //copy constructor - deep?

    Value_type get_type( void ) const { return value_type_; } //ziska typ nodu. To by mel asi umet kazdy
    string & get_type_str( void ) const;

    virtual Generic_node & get_key( const string & key );
    virtual Generic_node & get_key( const string & key, Generic_node & default_tree ); //TODO do potomku
    virtual Generic_node & get_item( const size_t id );
    virtual Generic_node & get_item( const size_t id, Generic_node & default_tree ); //TODO do potomku

    /* Implementace as_* tady nefunguje, je potreba znat presnou deklaraci tridy.
     * Forward deklarace nestaci (nevi, jake ma k dispozici metody apod.)
     */
    virtual Object_node & as_object( void );
    virtual Vector_node & as_vector( void );
    virtual Value_node & as_value( void );

    // hierarchicky projde cely podstrom do hloubky a vypise ho
    friend ostream & operator<<( ostream & stream, const Generic_node & node );

    virtual ~Generic_node();
private:
    Generic_node & operator=( Generic_node const & n ) { return (*this); }
};


}
#endif /* GENERIC_NODE_H_ */

/*
 * type_record.hh
 *
 *  Created on: May 1, 2012
 *      Author: jb
 */

#ifndef TYPE_RECORD_HH_
#define TYPE_RECORD_HH_

#include "system/exceptions.hh"

#include "type_base.hh"
#include "type_selection.hh"

/**
 * Macro to create a key object. The reason for this is twofold:
 * 1) We may use it to implement compile time computed hashes for faster access to the data.
 * 2) We can store line, function, filename where the key where used in order to report more specific error messages.
 */
#define KEY(name) #name


namespace Input {
namespace Type {






/** *********************************************************************************************************************************
 * @brief Class \p Input::Type::Default specifies default value of keys of a \p Input::Type::Record.
 *
 * It contains type of default value and possibly the value itself stored as \p std::string. Currently we distinguish four
 * cases:
 * - \b Default value given \b at \b declaration time, i.e. the default value is part of the \p Input::Type specification.
 *   This should be preferred way to give the default value since it can by documented as part of
 *   Record type specification.
 * - \b Default value given \b at \b read time, i.e. when you ask for the value through the Input::Record accessor. It should be used only
 *   if the default value is not constant, i.e. is taken from a global setting. In this case you should provide textual description
 *   where the default value comes from. Of course it could be difficult if the value is read on several places with different default values.
 * - \b No \b default value is given and input value is \b obligatory. An exception is thrown when the key is missing on the input.
 * - \b No \b default value is given and input value is \b optional.
 *
 *
 *
 * @ingroup input_types
 */
class Default {
private:
    /**
     * Possible types of default values.
     */
    enum DefaultType {
        default_at_declaration,        ///< Default value given at declaration time.
        default_at_read_time,          ///< Some default value will be given when the key is read. The description of this value should be provided.
        no_default_optional_type,      ///< No default value, optional key. This is default type of the Default.
        no_default_obligatory_type     ///< No default value, obligatory key.
    };
public:


    /**
     * Constructor with given default value (at declaration time)
     */
    Default(const std::string & value);

    /**
     * Factory function to make an default value that will be specified at the time when a key will be read.
     * You have to provide a string with description of the default value used at the read time., e.g.
     * the key \p time_governer of an equation can specify default value as
     * @code
     *      Default::read_time("By default the global time governor is used.")
     * @endcode
     * To get the value of such key from the input you have to use non-throwing variant of the method
     * Input::Record::key, which returns the value through reference and allows checking presence of the key on the input.
     *
     * Example of usage:
     * @code
     *      some_record.declare_key("time_governor",TimeGovernor(),Default::optional(),"description");
     * @endcode
     */
    static Default read_time(const std::string & description)
    { return Default(default_at_read_time, description); }


    /**
     * Factory function to make an empty default value which is obligatory.
     * This and following factory functions should be used instead of private constructors.
     *
     * Example of usage:
     * @code
     *      some_record.declare_key("some_key",Integer(),Default::obligatory(),"description");
     * @endcode
     */
    static Default obligatory()
    { return Default(no_default_obligatory_type); }

    /**
     * Factory function to make an empty default value which is optional.
     * To get the value of such key from the input you have to use non-throwing variant of the method
     * Input::Record::key, which returns the value through reference and allows checking presence of the key on the input.
     *
     * Example of usage:
     * @code
     *      some_record.declare_key("some_key",Integer(),Default::optional(),"description");
     * @endcode
     */
    static Default optional()
    { return Default(no_default_optional_type); }


    /**
     * Returns true if the default value is or will be available when someone tries to read the value.
     */
    inline bool has_value_at_read_time() const
    { return (type_ == default_at_read_time); }

    /**
     * Returns true if the default value is or will be available when someone tries to read the value.
     */
    inline bool has_value_at_declaration() const
    { return (type_ == default_at_declaration); }


    /**
     * Returns true if the key is obligatory and thus must be specified on input. No default value is given.
     */
    inline bool is_obligatory() const
    { return (type_ == no_default_obligatory_type); }

    /**
     * Returns true if the key is optional.
     */
    inline bool is_optional() const
    { return (type_ == no_default_optional_type); }

    /**
     * Returns stored value. Possibly empty string.
     */
    inline const string & value() const
    { return (value_); }

private:
    string value_;              ///< Stored value.
    enum DefaultType type_;     ///< Type of the Default.

    /**
     * Forbids default constructor.
     */
    Default();

    /**
     * Constructor for other types then 'declaration'.
     */
    Default(enum DefaultType type, const string &value = "");

};


class AbstractRecord;


/** ******************************************************************************************************************************
 * @brief Record type proxy class.
 *
 * To keep consistency, we have to prevent copies of the actual Record data. Therefore this class is just a proxy  that
 * can be freely (and cheaply) copied.
 *
 * @ingroup input_types
 */
class Record : public TypeBase {
public:

    /*
     * Exceptions specific to this class.
     */
    TYPEDEF_ERR_INFO( EI_Record, Record );
    TYPEDEF_ERR_INFO( EI_RecordName, const string);
    DECLARE_EXCEPTION( ExcRecordKeyNotFound, << "Key " << EI_KeyName::qval <<" not found in Record:\n" <<  EI_Record::val );
    DECLARE_EXCEPTION( ExcDeriveNonEmpty, << "Can not derive from Record " << EI_RecordName::qval << " into "
            "non-empty Record:\n" << EI_Record::val );

    /**
     *  Structure for description of one key in record.
     *  The members dflt_type_ and default have reasonable meaning only for
     *  type_ == Scalar
     */
    struct Key {
        unsigned int key_index;                     ///< Position inside the record.
        string key_;                                ///< Key identifier.
        string description_;                        ///< Key description in context of particular Record type.
        boost::shared_ptr<const TypeBase> type_;    ///< Type of the key.
        Default default_;                      ///< Default, type and possibly value itself.
        bool derived;                               ///< Is true if the key was only derived from the parent Record, but not explicitly declared.
    };

    /**
     * Public typedef of constant iterator into array of keys.
     */
    typedef std::vector<struct Key>::const_iterator KeyIter;

    /**
     * Default constructor. Empty handle.
     */
    Record() {}

    /**
     * Basic constructor. You has to provide \p type_name of the new declared Record type and
     * its \p description.
     */
    Record(const string & type_name_in, const string & description);

    /**
     * Method to derive new Record from an AbstractRecord @p parent. This copy all keys from the @p parent and register the newly created Record
     * in the @p parent. You are free to overwrite copied keys, but you can not delete them.
     */
    void derive_from(AbstractRecord parent);

    /**
     * Allows shorter input of the Record providing only value of the \p from_key given as the parameter.
     * All other keys of the Record must have default values specified at declaration. This is checked when the
     * \p finish method is called.
     *
     * If the input reader come across the Record in declaration tree, but there is not 'record-like' input, it
     * save default values into storage tree and tries to match the input with the type of the \p from_key.
     */
    void allow_auto_conversion(const string &from_key);

    /**
     * Declares a key of the Record with name given by parameter @p key, the type given by parameter @p type, default value by parameter @p default_value, and with given
     * @p description. The parameter @p type has to be any of descendants of TypeBase.
     *
     */
    template <class KeyType>
    void declare_key(const string &key,
                            const KeyType &type,
                            const Default &default_value, const string &description);

    /**
     * Same as previous method but without given default value (same as Default() - optional key )
     */
    template <class KeyType>
    void declare_key(const string &key,
                            const KeyType &type,
                            const string &description);

    /**
     * Finish declaration of the Record type. Now further declarations can be added.
     */
    void finish();

    /**
     * Implements @p TypeBase::is_finished.
     */
    virtual bool is_finished() const;


    /**
     * @brief Implements @p Type:TypeBase::documentation.
     */
    virtual std::ostream& documentation(std::ostream& stream, bool extensive=false, unsigned int pad=0) const;

    /**
     * Set made_extensive_doc = false for this Record and all its descendants.
     */
    virtual void  reset_doc_flags() const;

    /// Record type name getter.
    virtual string type_name() const;

    /**
     * The default string can initialize an Record if the record is auto-convertible
     * and the string is valid default value for the auto conversion key.
     */
    virtual void valid_default(const string &str) const;

    /// Class comparison and Record type name comparision.
    virtual bool operator==(const TypeBase &other) const;

    /**
     * Interface to mapping key -> index in record. Returns index (in continuous array) for given key.
     *
     * Works also for unfinished Record.
     */
    inline unsigned int key_index(const string& key) const;

    /**
     * Returns iterator to the key struct for given key string.
     */
    inline KeyIter key_iterator(const string& key) const;

    /**
     * Returns iterator to auto-conversion key (see Record::allow_auto_conversion), or end() if the auto conversion
     * is not allowed.
     */
    KeyIter auto_conversion_key_iter() const;

    /**
     * Returns iterator to the key struct for given key string.
     *
     */
    inline bool has_key_iterator(const string& key, KeyIter &it) const;

    /**
     * Container-like access to the keys of the Record. Returns iterator to the first key.
     */
    inline KeyIter begin() const;

    /**
     * Container-like access to the keys of the Record. Returns iterator to the last key.
     */
    inline KeyIter end() const;

    /**
     * Returns true if the Record contains key with given name.
     */
    inline bool has_key(const string& key) const;

    /**
     * Returns number of keys in the Record.
     */
    inline unsigned int size() const;


protected:

    inline void empty_check() const {
        ASSERT( data_.use_count() != 0, "Empty Record handle.\n");
    }

    inline void finished_check() const {
        ASSERT( is_finished(), "Asking for information of unfinished Record type: %s\n", type_name().c_str());
    }

    /**
     * Internal data class.
     */
    class RecordData {
    public:
        RecordData(const string & type_name_in, const string & description);

        void declare_key(const string &key,
                         boost::shared_ptr<const TypeBase> type,
                         const Default &default_value, const string &description);

        std::ostream& documentation(std::ostream& stream, bool extensive=false, unsigned int pad=0) const;

        void  reset_doc_flags() const;

        /// Database of valid keys
        std::map<KeyHash, unsigned int> key_to_index;
        typedef std::map<KeyHash, unsigned int>::const_iterator key_to_index_const_iter;

        /// Keys in order as they where declared.
        std::vector<struct Key> keys;


        /// Description of the whole record type.
        const string description_;
        const string type_name_;

        /**
         * This flag is set to true when documentation of the Record was called with extensive==true
         * and full description of the Record was produced.
         *
         * This member is marked 'mutable' since it doesn't change structure or description of the type. It only influence the output.
         */
        mutable bool made_extensive_doc;

        bool finished;

        int auto_conversion_key;

    };


    /// Data handle.
    boost::shared_ptr<RecordData> data_;

};

/**
 * @brief Class for declaration of polymorphic Record.
 *
 * Like the Record class this is only proxy class. It derives all methods from Record, but
 * further  there is method \p no_more_descendants to close adding descendants. After this
 * you can not derive any Record from this AbstractRecord.
 *
 *
 *
 * A static method (typically part of an abstract class) for construction of an AbstractType can look like:
 *
 @code
    static Input::Type::AbstractRecord &SomeAbstractClass::get_input_type() {
        using namespace Input::Type;
        static AbstractRecord rec("Function",
            "Input for generic time-space function.");

        if (! rec.is_finished()) {
            // declaration of keys that should be derived
            rec.declare_key("domain",Domain::get_input_type(),Default::optional(),
                "Possibly restrict domain of the function.");
            // Finish adding keys.
            rec.finish();

            // call construction of derived Records
            FunctionLinear::get_input_type();
            FunctionPolynomial::get_input_type();
            FunctionInterpreted::get_input_type();

            // finish adding descendants.
            rec.no_more_descendants();
        }

        return rec;
    }
 @endcode
 *
 * @ingroup input_types
 */


class AbstractRecord : public Record {
protected:

    /**
     * Actual data of the abstract record.
     */
    class ChildData {
    public:
        ChildData(const string &name)
        : selection_of_childs( boost::make_shared<Selection> (name) )
        {}

        /**
         * Selection composed from names of derived Records. Indices are according to
         * the order of derivation (starting from zero).
         */
        boost::shared_ptr< Selection> selection_of_childs;

        /**
         * Vector of derived Records (proxies) in order of derivation.
         */
        vector< Record > list_of_childs;
    };

public:
    /**
     * Basic constructor. You has to provide \p type_name of the new declared Record type and
     * its \p description.
     */
    AbstractRecord(const string & type_name_in, const string & description);

    /**
     * This method close an AbstractRecord for any descendants (since they modify the parent). Maybe we should not use
     * a Selection for list of descendants, since current interface do not expose this Selection. Then this method
     * could be removed.
     */
    void no_more_descendants();


    /**
     * @brief Implements @p Type:TypeBase::documentation.
     */
    virtual std::ostream& documentation(std::ostream& stream, bool extensive=false, unsigned int pad=0) const;

    /**
     * Set made_extensive_doc = false for this Record and all its descendants.
     */
    virtual void  reset_doc_flags() const;

    /**
     * Returns reference to the inherited Record with given name.
     */
    const Record  &get_descendant(const string& name) const;

    /**
     * Returns reference to the inherited Record with given index (indexed in the same order
     * as they are derived).
     */
    const Record  &get_descendant(unsigned int idx) const;

    /**
     * Returns reference to Selection type of the implicit key TYPE.
     */
    const Selection &get_type_selection() const;

protected:
    /// Actual data of the AbstractRecord.
    boost::shared_ptr<ChildData> child_data_;

    /**
     * Add inherited Record.
     */
    void add_descendant(const Record &subrec);

    friend class Record;
};


/*********************************************************
 * Implementation
 */

template <class KeyType>
void Record::declare_key(const string &key,
                        const KeyType &type,
                        const Default &default_value, const string &description)
{
    // ASSERT MESSAGE: The type of declared keys has to be a class derived from TypeBase.
    BOOST_STATIC_ASSERT( (boost::is_base_of<TypeBase, KeyType>::value) );

    empty_check();
    if (is_finished() ) xprintf(PrgErr, "Declaration of key: %s in finished Record type: %s\n", key.c_str(), type_name().c_str());

    // If KeyType is not derived from Scalar, we check emptiness of the default value.
    //if (boost::is_same<Record, KeyType>::value && typedefault_value.has_value() )
    //    xprintf(Err, "Default value for non scalar type in declaration of key: %s in Record type: %s \n", key.c_str(), type_name().c_str() );

    if (! is_valid_identifier(key))
        xprintf(PrgErr, "Invalid key identifier %s in declaration of Record type: %s\n", key.c_str(), type_name().c_str());

    // We do not allow declaration with unfinished type. The only exception is internal "TYPE"
    // key defined by AbstractRecord.
    if ( ! type.is_finished() )
        xprintf(PrgErr, "Unfinished type of declaring key: %s in Record type: %s\n", key.c_str(), type_name().c_str() );

    // check validity of possibly given default value
    if ( default_value.has_value_at_declaration() ) {

        try {
            type.valid_default( default_value.value() );
        } catch (ExcWrongDefault & e) {
            e << EI_KeyName(key);
            throw;
        }
    }

    // make our own copy of type object allocated at heap (could be expensive, but we don't care)
    boost::shared_ptr<const KeyType> type_copy = boost::make_shared<KeyType>(type);

    data_->declare_key(key, type_copy, default_value, description);
}



template <class KeyType>
void Record::declare_key(const string &key,
                        const KeyType &type,
                        const string &description)
{
    declare_key(key,type, Default::optional(), description);
}



inline unsigned int Record::key_index(const string& key) const
{
    KeyHash key_h = key_hash(key);
    RecordData::key_to_index_const_iter it = data_->key_to_index.find(key_h);
    if (it != data_->key_to_index.end()) return it->second;
    else
        THROW( ExcRecordKeyNotFound() << EI_KeyName(key) << EI_Record(*this) );

    return size();
}



inline Record::KeyIter Record::key_iterator(const string& key) const
{
    finished_check();
    return begin() + key_index(key);
}



inline bool Record::has_key_iterator(const string& key, KeyIter &it) const
{
    finished_check();
    KeyHash key_h = key_hash(key);
    RecordData::key_to_index_const_iter data_it = data_->key_to_index.find(key_h);
    if (data_it == data_->key_to_index.end()) {
        return false;
    } else {
        it = begin()+data_it->second;
        return true;
    }
}



inline Record::KeyIter Record::begin() const
{
    finished_check();
    return data_->keys.begin();
}



inline Record::KeyIter Record::end() const
{
    finished_check();
    return data_->keys.end();
}



inline bool Record::has_key(const string& key) const
{
    return key_iterator(key) != end();
}



inline unsigned int Record::size() const {
    finished_check();
    ASSERT( data_->keys.size() == data_->key_to_index.size(), "Sizes of Type:Record doesn't match. (map: %d vec: %d)\n", data_->key_to_index.size(), data_->keys.size());
    return data_->keys.size();
}



} // closing namespace Type
} // closing namespace Input




#endif /* TYPE_RECORD_HH_ */
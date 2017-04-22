
#pragma once
namespace raven {
namespace sim {
namespace tern {
/**  The qualities of a planet

  e.g. volume, size, pressure, etc

*/
class cQuality
{
public:

    cQuality()
    : myValue( 5, 0 )
    {    }

    /** Set names of qualities

    @param[in] names  The names of qualities, in index order.

    Every planet has the same set of qualities,
    so this should be called just once at the start of the simulation
    */
    static void setNames( const std::vector< std::string >& names );

    /**  Get names of qualities */
    static void getNames( std::vector< std::string >& names );

    /** Number of qualities */
    static int size()
    {
        return myNameMap.size();
    }

    /** get index of quality by name

    @return index of name, -1 if name not defined

    This allows the fast set and get to be used.

    It should therefore be called once in the constructor of each flower
    and the indices stored in an attribute
    */
    static int getIndex( const std::string& name );

    /**  Set value by index */
    void setValue( int index, double value )
    { if( 0 > index || index >= (int)myValue.size() ) return; myValue[ index ] = value; }

    /** Get value by index */
    double getValue( int index ) const
    { if( 0 > index || index >= (int)myValue.size() ) return 0; return myValue[ index ]; }

    /** Set value by name ( slow ) */
    void setValue(  const std::string& name, double value )
    {
        setValue( getIndex( name ), value );
    }

    /** Get value by name ( slow ) */
    double getValue( const std::string& name ) const
    {
        return getValue( getIndex( name ) );
    }

//    std::string getName( int index ) const
//    {
//        if( 0 > index || index >= myValue.size() ) return ""; return myNameMap[index].first;
//    }

    typedef std::map< std::string, int >::const_iterator const_iter;
    const_iter begin()  { return myNameMap.begin(); }
    const_iter end()    { return myNameMap.end(); }

private:
    /** Every planet has the same set of qualities */
    static std::map< std::string, int > myNameMap;
    /** The values of qualities for the planet */
    std::vector< double > myValue;
};
}}}

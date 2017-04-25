#include <iostream>
#include <sstream>
#include <map>
#include <vector>


#ifdef tern_vase
#include "tern.h"
#else
#include "cQuality.h"
#endif // tern_vase

namespace raven {
namespace sim {
namespace tern {

std::map< std::string, int > cQuality::myNameMap;

void cQuality::setNames( const std::vector< std::string >& names )
{
    myNameMap.clear();
    int index = 0;
    for( const std::string n : names )
    {
        if( ! n.length() )
            continue;
        auto ret = myNameMap.insert( std::pair<std::string,int>( n, index ));
        if( ! ret.second ) {
            std::cout << "not inserted! \n";
        }
        index++;
    }

}
void cQuality::getNames( std::vector< std::string >& names )
{
    names.clear();
    for ( auto it : myNameMap )
    {
        names.push_back( it.first );
    }
}
int cQuality::getIndex( const std::string& name )
{
    auto it = myNameMap.find( name );
    if( it == myNameMap.end() )
    {
        #ifdef tern_vase
        std::stringstream msg;
        msg << "ERROR: Cannot find quality |" << name << "|\n"
            "Try adding it to list of qualities in gui menu option Simulate | Options";
        tern::theSimulationEngine.HandleFatalError( msg.str() );
        #else
        return -1;
        #endif
    }
    return it->second;
}
}}}

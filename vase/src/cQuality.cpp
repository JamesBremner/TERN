#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#include "cQuality.h"
#ifdef tern_build
#include "tern.h"
#endif // tern_build

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
        std::cout << "insert |" << n << "|\n";
        auto ret = myNameMap.insert( std::pair<std::string,int>( n, index ));
        if( ! ret.second ) {
            std::cout << "not inserted! \n";
        }
        index++;
    }

    std::cout << "cQuality::setNames " << names.size() <<" "<< myNameMap.size() << "\n";
    for( auto it : myNameMap )
    {
         std::cout << it.first << " " << it.second << "\n";
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
     std::cout << "cQuality::getIndex " << name << "\n";
    for( auto it : myNameMap )
    {
         std::cout << it.first << " " << it.second << "\n";
    }
    auto it = myNameMap.find( name );
    if( it == myNameMap.end() )
    {
        #ifdef tern_build
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

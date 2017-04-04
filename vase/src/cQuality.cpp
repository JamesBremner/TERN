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

std::map< std::wstring, int > cQuality::myNameMap;

void cQuality::setNames( const std::vector< std::wstring >& names )
{
    myNameMap.clear();
    int index = 0;
    for( const std::wstring n : names )
    {
        if( ! n.length() )
            continue;
        myNameMap.insert( std::pair<std::wstring,int>( n, index ));
        index++;
    }

}
void cQuality::getNames( std::vector< std::wstring >& names )
{
    names.clear();
    for ( auto it : myNameMap )
    {
        names.push_back( it.first );
    }
}
int cQuality::getIndex( const std::wstring& name )
{
    auto it = myNameMap.find( name );
    if( it == myNameMap.end() )
    {
        #ifdef tern_build
        std::wstringstream msg;
        msg << L"ERROR: Cannot find quality " << name << "\n"
            L"Try adding it to list of qualities in gui menu option Simulate | Options";
        tern::theSimulationEngine.HandleFatalError( msg.str() );
        #else
        return -1;
        #endif
    }
    return it->second;
}
}}}

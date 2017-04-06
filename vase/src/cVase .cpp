#include <iostream>
#include <cstdio>
#include <algorithm>
#include <utility>
#include <cstdarg>


#include "raven_sqlite.h"

#ifndef WXWIDGETS
#include "tern.h"
#endif
#include "cVase.h"
#include "cQuality.h"


#include "ModelFlowers.h"


namespace raven
{
namespace sim
{
namespace gui
{

cVase::cVase()
    : mySimType( task )
    , mySelected( 0 )
    , my2Selected( 0 )
    , myHandleSelected( -1 )
{
#ifdef tern_build
    // read values from database
    ReadDB();
#endif
}


void cVase::ReadDB()
{
    raven::sqlite::cDB db;
    db.Open(L"vase.dat");
    db.Query(L"SELECT type, time FROM params;");
    if( db.myError )
    {
        db.Query(L"DROP TABLE IF EXISTS params;");
        db.Query(L"CREATE TABLE params ( type, time );");
        db.Query(L"INSERT INTO params VALUES ( 1, 100 );" );
        db.Query(L"CREATE TABLE quality_names ( name );");
        db.Query(L"CREATE TABLE plot ( flower, plot, data );");
    }
    else
    {
        mySimType = ( e_type ) wcstol(db.myResult[0].c_str(),NULL,10);
        mySimTime = wcstol(db.myResult[1].c_str(),NULL,10);

        db.Query(L"SELECT * FROM quality_names;");
        raven::sim::tern::cQuality::setNames(  db.myResult );
    }

}


/**  Select flower under location

  @param[in] p location

*/
#ifdef WXWIDGETS


void cVase::Paint( wxPaintDC& dc )
{
    // loop over flowers
    for ( cFlower* flower : myVase )
    {

        // tell the flower if it is selected
        // so that it can paint itself highlighted
        if( mySelected == flower )
        {
            flower->Select();
        }

        // draw the flower
        flower->Set( dc );
        flower->Paint( dc );
        flower->PaintPipe( dc );
        flower->DrawText(dc);

        if( mySelected == flower )
        {
            flower->PaintResizeHandles( dc );
        }


        flower->Deselect();

    }
}

void cVase::Select( const wxPoint& p )
{
    my2Selected = mySelected;

    mySelected = find( p );

}
/**

  Popup a configuration dialog for selected flower

  @param[in] parent window

*/
void raven::sim::gui::cVase::Configure( wxWindow* parent )
{
    if( mySelected )
        mySelected->Configure( parent );
}
/**  Find flower under location

  @param[in] p location

  @return pointer to flower under location, or NULL if none

*/
cFlower * cVase::find( const wxPoint& p )
{
    // loop over flowers
    for( auto flower : myVase )
    {
        if( flower->IsUnder( p ) )
        {
            return flower;
        }
    }
    return NULL;
}
#endif
/**

  Find flower by index

  @param[in] idx the index

  @return iterator pointing to flower with index, end() if not found

*/
cVase::iterator cVase::find( int idx )
{
    for( iterator flower = myVase.begin();
            flower != myVase.end(); flower++ )
    {
        if( (*flower)->getIndex() == idx )
            return flower;
    }
    return myVase.end();
}
/**  Find flower by name

  @param[in] name

   @return  pointer to flower with name, NULL if not found

*/
cFlower * cVase::find( const std::wstring& name )
{
    for( auto flower : myVase )
    {
        if( flower->getName() == name )
            return flower;
    }
    return NULL;
}
void cVase::setName( const wstring& n )
{
    if( ! mySelected ) return;
    mySelected->setName( n );
}


/**

 Save to graphviz dot format file

 @param[in] filename  name of file to write

 The graphviz dot file format is used so that the graphviz utilities can
 be used to pretty print the layout.

 http://www.graphviz.org/content/dot-language

*/
bool cVase::Write(  const std::string& filename )
{
    if( ! AllNamesUnique() )
    {
#ifdef WXWIDGETS
        wxMessageBox("Not all flowers have unique names");
#endif
        return false;
    }

    FILE * fp = fopen(filename.c_str(),"w");
    if( ! fp )
        return false;
    fprintf(fp,"digraph V {\n");
    for (cFlower * flower : myVase )
    {
        flower->WritePipe( fp );
    }
    for ( cFlower * flower : myVase )
    {
        flower->Write( fp );
    }
    fprintf(fp,"}\n");
    fclose(fp);
    return true;
}
/**

  Read from grahviz dot formatted file

  @param[in[ filename

  */
bool cVase::Read( const std::string& filename )
{

    std::cout << "cVase::Read" << std::endl;

    Clear();

    FILE * fp = fopen(filename.c_str(),"r");
    if( ! fp )
        return false;

    wchar_t buf[1000];
    fgetws(buf,999,fp);
    while( fgetws(buf,999,fp) )
    {
        wstring line( buf );
        if( line.length() < 3 )
            continue;
        int p = line.find(L"->");
        if( p == -1 )
        {
            p = line.find(L"vase_type='")+11;
            int q = line.find(L"'",p);
            wstring type = line.substr(p,q-p);
            std::wcout << L"cVase::Read " << line <<L" t= " << type << std::endl;

            if( ! Add( type ))
            {
                wstring msg =  L"Unrecognized flower type of " + type;
#ifdef WXWIDGETS
                wxMessageBox(msg, "Vase");
#else
                tern::theSimulationEngine.HandleFatalError( msg );
#endif
                return false;
            }

            mySelected->Read( line.c_str() );

        }
    }
    fclose(fp);

    // Read connections

    fp = fopen(filename.c_str(),"r");
    if( ! fp )
        return false;

    fgetws(buf,999,fp);
    while( fgetws(buf,999,fp) )
    {
        wstring line( buf );
        if( line.length() < 3 )
            continue;
        int p = line.find(L"->");
        if( p != -1 )
        {
            p = line.find(L"start_idx=");
            int idx;
            idx = wcstol(line.substr(p+10).c_str(),NULL,10);
            cFlower * start = *find(idx);
            p = line.find(L"end_idx=");
            idx = wcstol(line.substr(p+8).c_str(),NULL,10);
            cFlower * end = *find(idx);
            if( ! start->getDestination() )
                start->Connect( end );
            else
                start->Connect2( end );
        }
    }

    fclose(fp);

#ifndef WXWIDGETS

    // remove pipebends

    for( auto f : myVase )
    {
        if( f->getType() == cFlowerFactory::Index(L"PipeBend") )
        {
            for( auto s : myVase )
            {
                if( s->getDestination() == f )
                {
                    s->Connect( f->getDestination() );
                }
                if( s->getDestination2() == f )
                {
                    s->Connect2( f->getDestination() );
                }
            }
        }
    }

#endif // WXWIDGETS

    return true;
}
/**

  Empty the vase


	 the flower pointers are stored in the vector
	 so we must delete the flowers they point to
*/
void cVase::Clear()
{
    for( cFlower* flower : myVase )
    {
        delete flower;
    }
    myVase.clear();

    mySelected = NULL;


}

bool flower_name_greater( cFlower* f1, cFlower * f2 )
{
    return f1->getName() > f2->getName();
}
bool flower_name_equal( cFlower* f1, cFlower * f2 )
{
    return f1->getName() == f2->getName();
}

/**

  Check for all unique names for flowers

  @return true if all flowers have unique names

*/
bool cVase::AllNamesUnique()
{
    std::sort(myVase.begin(),myVase.end(), flower_name_greater );
    if( myVase.end() == std::adjacent_find(myVase.begin(),myVase.end(), flower_name_equal ) )
        return true;
    return false;
}
/**

  Add flower

  @param[in] t type of flower

  The new flower is selected.

*/
bool cVase::Add( const std::wstring& flower_type_name )
{
    myHandleSelected = -1;
    mySelected = cFlowerFactory::Construct( flower_type_name );
    if( mySelected == nullptr )
        return false;
    myVase.push_back( mySelected );
    return true;
}

bool cVase::Add( int flower_type_index )
{
    myHandleSelected = -1;
    mySelected = cFlowerFactory::Construct( flower_type_index );
    if( mySelected == nullptr )
    {
#ifdef WXWIDGETS
        wxMessageBox("Unrecognized flower type index","cVase::Add ERROR");
#endif
        return false;
    }
    myVase.push_back( mySelected );
    return true;
}


/**

  Delete selected flower

  Also deletes any connected pipes

*/
void cVase::Delete()
{
    if( ! mySelected )
        return;

    // erase pipes to flower
    for ( cFlower* flower : myVase )
    {
        if( flower->getDestination() == mySelected )
        {
            flower->Connect( NULL );
        }
        if( flower->getDestination2() == mySelected )
        {
            flower->Connect2( NULL );
        }
    }
    // erase flower from container
    myVase.erase(
        std::find(
            myVase.begin(),
            myVase.end(),
            mySelected ) );

    // erase flower from heap
    delete mySelected;
}



/**

  Add pipe between flowers

  The pipe runs from the previously selected flower
  to the currently selected flower

*/
void cVase::Connect()
{
    if( ! mySelected ) return;
    if( ! my2Selected ) return;
    if( mySelected == my2Selected ) return;
    my2Selected->Connect( mySelected );

}
void cVase::Connect2()
{
    if( ! mySelected ) return;
    if( ! my2Selected ) return;
    if( mySelected == my2Selected ) return;
    my2Selected->Connect2( mySelected );

}
/**

  Drag selected flower to new location

  @param[in] p new location delta

*/
#ifdef WXWIDGETS
void cVase::setLocation( const wxPoint& p )
{
    if( ! mySelected ) return;
    if( myHandleSelected == -1 )
    {
        // No handle selected, so drag flower to new location

        mySelected->setLocationTopLeft(
            p.x,
            p.y );
    }
    else
    {
        // user dragging handle, so change size
        mySelected->DragHandle( myHandleSelected, p );
    }
}
#endif
}
}
}

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include "flower.h"
#include "flower_types.h"

#ifdef WXWIDGETS
#include <wx/propgrid/propgrid.h>
#endif // WXWIDGETS


namespace raven
{
namespace sim
{
namespace gui
{

/*

  Keep track of last index assigned

*/
int cFlower::lastIndex = 0;

const int handlesize = 10;

/**

  Construct a flower

  Never construct a flower directly.
  Instead call the method cVase::Add( cFlowerFactory::type ).
  This will look after constructing a flower of the correct type
  and adding it to the vase.

  Re-implement this for each specialised flower type

*/
cFlower::cFlower( )
    : myName( "unnamed" )
    , myTypeName( "flower" )
    , myType( 1 )
    , myShape( "box" )
    , myWidth( 50 )
    , myHeight( -1 )
    , IsSelected( false )
    , myDestination( NULL )
    , myDestination2( NULL )
    , myOutputPortCount( 1 )
{
    // Assign unique index
    myIndex = lastIndex++;

    setName();

}
/**

  Set name

  @param[in] n name

  If n is blank, set to typename_index

*/
void cFlower::setName( const string& n )
{
    if( n.length() > 0 )
        myName = n;
    else
    {
#ifdef WXWIDGETS
        myName = myTypeName + wxString::Format(L"_%d",myIndex);
#endif
    }

    myType = cFlowerFactory::Index( myTypeName );

}
void cFlower::setIndex( int id )
{
    myIndex = id;
    if( id > lastIndex )
        lastIndex = id;
}

/**

  Move flower top left to new location

  @param[in] x new left location
  @param[in] y new top location

*/
void cFlower::setLocationTopLeft( int x, int y )
{
    myX = x;
    myY = y;
}
/**

  Move flower center to new location

  @param[in] x new center location
  @param[in] y new center location

*/
void cFlower::setLocationCenter( int x, int y )
{
    myX = x - myWidth / 2;
    if( myHeight < 0 )
        myY = y - myWidth / 2;
    else
        myY = y - myHeight / 2;
}

#ifdef WXWIDGETS
/**

  Draw flower name and type

*/
void cFlower::DrawText( wxPaintDC& dc )
{
    // User does not care about name and type of pipebends
    if( myType == cFlowerFactory::Index( "PipeBend") )
        return;

    wxString name = myName;
    name.Replace("\\n","\n");
    dc.DrawText(name,myX+5,myY+20);
}

/**

  Popup a configuration dialog

  @param[in] parent window

*/
void cFlower::Configure( wxWindow* parent )
{
    cFlowerConfigDlg dlg( parent, this );
    if( dlg.ShowModal() == wxID_OK )
    {
        // extract new values
        myName = dlg.pg->GetPropertyValueAsString( "Name" );
        for( auto& prm : myParam )
        {
            prm.second.value = dlg.pg->GetPropertyValueAsDouble( prm.second.name.c_str() );
        }
    }
}
/**

  Is flower under a location

  @param[in] p location

  @return true if x,y is a location somewhere on the flower

*/
bool cFlower::IsUnder( const wxPoint& p )
{
    if( myHeight < 0 )
        return( myX <= p.x && myY <= p.y &&
                myX+myWidth >= p.x && myY+myWidth >= p.y );
    else
        return( myX <= p.x && myY <= p.y &&
                myX+myWidth >= p.x && myY+myHeight >= p.y );

}
/**
  Is one of the resize handles under a location

  @param[in] p location
  @return -1 no
  @return 1 top left
  @return 2 top right
  @return 3 bottom right
  @return 4 bottm left

*/
int  cFlower::HandleIsUnder( const wxPoint& p )
{
    std::vector< wxRect > vhandle;
    getHandles( vhandle );
    int count = 1;
    for( auto& h : vhandle )
    {
        if( h.Contains( p ) )
        {
            return count;
        }
        count++;
    }
    return -1;

}
/**  Resize flower, using dragged handle

  @param[in] handle index of handle being dragged
  @param[in] p delta point for handle

*/
void cFlower::DragHandle( int handle, wxPoint p )
{
    switch( handle )
    {
    case 1:
        myX += p.x;
        myWidth -= p.x;
        myY += p.y;
        if( myHeight > 0 )
            myHeight -= p.y;
        break;
    case 2:

        myWidth += p.x;
        myY += p.y;
        if( myHeight > 0 )
            myHeight -= p.y;

        break;
    case 3:
        myX += p.x;
        myWidth -= p.x;
        if( myHeight > 0 )
            myHeight += p.y;

        break;
    case 4:
        myWidth += p.x;
        if( myHeight > 0 )
        {
            myHeight += p.y;
        }
        break;
    }
}

/**

  Entry port location

*/
wxPoint cFlower::getEntryPort()
{
    wxPoint p( myX, myY );
    if( myHeight < 0 )
        p.y += myWidth / 2;
    else
        p.y += myHeight / 2;
    return p;
}
/**

 Exit port location

 */
wxPoint cFlower::getExitPort()
{
    wxPoint p( myX, myY );
    p.x += myWidth;
    if( myHeight < 0 )
        p.y += myWidth / 2;
    else
        p.y += myHeight/2;
    return p;
}
/**

  Set device context ready to draw flower

  @param[in] dc the device context

  Looks after highlighting the selected flower

*/
void cFlower::Set( wxPaintDC& dc )
{
    if( IsSelected )
    {
        dc.SetPen( *wxRED_PEN );
    }
    else
    {
        dc.SetPen( *wxBLACK_PEN );
    }
}
/**

  Draw the flower

*/
void cFlower::Paint( wxPaintDC& dc )
{
    dc.DrawRectangle( myX, myY, myWidth, myWidth );
}
void cFlower::PaintResizeHandles( wxPaintDC& dc )
{

    dc.SetPen( *wxBLUE_PEN );
    std::vector< wxRect > vhandle;
    getHandles( vhandle );
    for( auto& h : vhandle )
    {
        dc.DrawRectangle( h );
    }

}
void cFlower::getHandles( std::vector< wxRect >& vhandle )
{
    int y_bottom_handles = myY+myWidth+handlesize;
    if( myHeight > 0 )
        y_bottom_handles = myY+myHeight+handlesize;
    vhandle.clear();
    vhandle.push_back( wxRect(myX-handlesize,myY-handlesize,handlesize,handlesize ));
    vhandle.push_back( wxRect(myX+myWidth+handlesize,myY-handlesize,handlesize,handlesize ));
    vhandle.push_back( wxRect(myX-handlesize,y_bottom_handles,handlesize,handlesize ));
    vhandle.push_back( wxRect(myX+myWidth+handlesize,y_bottom_handles,handlesize,handlesize ));
}
/**

  Draw pipes that begin on this flower

*/
void cFlower::PaintPipe( wxPaintDC& dc )
{
    dc.SetPen( *wxBLUE_PEN );

    cFlower * dst = myDestination;
    if( dst )
    {
        int xportsrc, yportsrc;
        getPortLocation( xportsrc, yportsrc, *dst );
        int xportdst, yportdst;
        dst->getPortLocation( xportdst, yportdst, *this );
        PaintArrow( dc, wxPoint( xportsrc, yportsrc ), wxPoint( xportdst, yportdst ) );
    }
    dst = myDestination2;
    if( dst )
    {
        int xport, yport;
        getPortLocation( xport, yport, *dst );
        int xportdst, yportdst;
        dst->getPortLocation( xportdst, yportdst, *this );
        PaintArrow( dc, wxPoint( xport, yport ), wxPoint( xportdst, yportdst ) );
    }


}
/** Get port location on side towards other flower

@param[out] x port location
@param[out] y port location
@param[in] other flower

*/
void cFlower::getPortLocation( int& x, int& y, const cFlower& other)
{
    int h = myHeight;
    if( h < 0 )
        h = myWidth;

    double dx = other.getLocationX() - myX;
    if( fabs(dx) < 1.0 )
        dx = 0.5;
    double dy = other.getLocationY() - myY;
    double slope_from_dst = dy / dx;
    if( -0.5 < slope_from_dst && slope_from_dst < 0.5 && dx > 0 )
    {
        x =  myX+myWidth;
        y = myY+h / 2;
    }
    else if ( -0.5 < slope_from_dst && slope_from_dst < 0.5 && dx < 0 )
    {
        x = myX;
        y = myY + h / 2;
    }
    else if ( dy <= 0 )
    {
        x = myX + myWidth / 2;
        y = myY;
    }
    else
    {
        x = myX + myWidth / 2;
        y = myY + h;
    }

}
void cFlower::PaintArrow( wxPaintDC& dc, wxPoint exit_port, wxPoint entry_port )
{
    const int nWidth = 7;		// width (in pixels) of the full base of the arrowhead
    const float fTheta = 0.4;	// angle (in radians) at the arrow tip between the two
    //  sides of the arrowhead

    wxPoint arrow[5];
    float vecLine[2];
    float vecLeft[2];
    float fLength;
    float th;
    float ta;


    // set base and tip
    arrow[4] = exit_port;
    arrow[0] = entry_port;

    // build the line vector
    vecLine[0] = (float) arrow[0].x - arrow[4].x;
    vecLine[1] = (float) arrow[0].y - arrow[4].y;

    // build the arrow base vector - normal to the line
    vecLeft[0] = -vecLine[1];
    vecLeft[1] = vecLine[0];

    // setup length parameters
    fLength = (float) sqrt(vecLine[0] * vecLine[0] + vecLine[1] * vecLine[1]);
    th = nWidth / (2.0f * fLength);
    ta = nWidth / (2.0f * (tanf(fTheta) / 2.0f) * fLength);

    // find the base of the arrow
    arrow[3].x = (int) (arrow[0].x + -ta * vecLine[0]);
    arrow[3].y = (int) (arrow[0].y + -ta * vecLine[1]);

    // build the points on the sides of the arrow
    arrow[1].x = (int) (arrow[3].x + th * vecLeft[0]);
    arrow[1].y = (int) (arrow[3].y + th * vecLeft[1]);
    arrow[2].x = (int) (arrow[3].x + -th * vecLeft[0]);
    arrow[2].y = (int) (arrow[3].y + -th * vecLeft[1]);

    dc.DrawLine( arrow[4], arrow[3] );
    dc.DrawLine( arrow[3], arrow[1] );
    dc.DrawLine( arrow[1], arrow[0] );
    dc.DrawLine( arrow[0], arrow[2] );
    dc.DrawLine( arrow[2], arrow[3] );


}
void cFlower::AppendProperties( wxPropertyGrid * pg )
{
    // loop over parameters
    for ( auto& param : myParam )
    {
        wxFloatProperty * prop = new wxFloatProperty(
                       param.second.name, wxPG_LABEL, param.second.value );
        prop->SetHelpString( param.second.description );
        pg->Append( prop );
    }

}
#endif
/**

  Write data to graphviz dot format file

  @param[in] fp file pointer

  name [shape=circle,pos="25,25"] // [ vase_type='flower' vase_idx=1 ]

*/
void cFlower::Write( FILE * fp )
{
    fprintf(fp,
            "%s [shape=%s,pos=\"%d,%d\"] // [ vase_type='%s' vase_idx=%d ",
            myName.c_str(),
            myShape.c_str(),
            myX,myY,
            myTypeName.c_str(),
            myIndex );

    for( auto& pp : myParam )
    {
        fprintf(fp,
                "p_%s %f ",
                pp.second.name.c_str(), pp.second.value );
    }
    fprintf(fp,"]\n");
}

/**

  Write data to graphviz dot format file

  @param[in] fp file pointer

  name1 -> name2 // start_idx=0 end_idx=1

*/
void cFlower::WritePipe( FILE * fp )
{
    if( myDestination )
    {
        fprintf(fp,"%S -> %S // start_idx=%d end_idx=%d\n",
                (wchar_t*)myName.c_str (),
                (wchar_t*)myDestination->getName().c_str (),
                myIndex, myDestination->getIndex());
    }
    if( myDestination2 )
    {
        fprintf(fp,"%S -> %S // start_idx=%d end_idx=%d\n",
                (wchar_t*)myName.c_str (),
                (wchar_t*)myDestination2->getName().c_str (),
                myIndex, myDestination2->getIndex());
    }
}
/**

  Read from line graphviz dot file

*/
void cFlower::Read( const string& line )
{
    int p;
#ifdef WXWIDGETS
    p = line.find("pos=")+5;
    int x = atoi(line.substr(p).c_str());
    p = line.find(",",p)+1;
    int y = atoi(line.substr(p).c_str());
    setLocationTopLeft( x,y );

#endif
    setName(line.substr(0,line.find(" ")));

    p = line.find("vase_idx=");
    int idx;
    idx = atoi(line.substr(p+9).c_str());
    setIndex( idx );

    // loop over parameters
    for( auto& param : myParam )
    {
        // find specified value of parameter in input file
        string target = "p_" + param.second.name;
        p = line.find( target );
        if( p != -1 )
        {
            p += target.length() + 1;

            // set the value of the paramete
            param.second.value = atoi( line.substr(p).c_str());
            //std::wcout << myName << " " << param.second.name << " = " << param.second.value << std::endl;
        }
    }

}

/**

  Add parameter

  @param[in] name Name of parameter
  @param[in] description
  @param[in] defaultvalue  Initial value for parameter, default 0.0

  When user selects configure a dialog window pops up
  This shows a list of name value pairs and allows the values to be edited.

  In the constructor for a specialized flower,
  call this method to add a row to the parameter configuration dialog
  for the specialized flower type.

*/
void cFlower::AddParam( const string& name, const string& description, double defaultvalue )
{
    sparam param;
    param.name = name;
    param.value = defaultvalue;
    param.description = description;
    myParam.insert( std::pair<string,sparam>(name,param ));
}

double cFlower::getValue(
    const string& name    ) const
{

    auto pit = myParam.find( name );
    if( pit == myParam.end() )
    {
        return -1.0;
    }
    return pit->second.value;
}

raven::sim::cFlowerDict * cFlowerFactory::myFlowerDict;

/**

  Construct flower factory

*/
cFlowerFactory::cFlowerFactory()
{
    myFlowerDict = new  raven::sim::cFlowerDict();
}
/** Construct popup menu of flower types that can be created

  @param[in] IDM_CreateGeneric menu ID for the base flower creation popup menu option

  This creates a menu option for the creation each flower type.
  There must be enough room above IDM_CreateGeneric for all the flower types.
*/
void cFlowerFactory::setMenu( int IDM_CreateGeneric )
{
#ifdef WXWIDGETS

    for( auto& t : *myFlowerDict )
    {
        myMenuTask.Append( IDM_CreateGeneric+t.myIndex, t.myName );
    }
#endif

}

int cFlowerFactory::Index(
    const std::string& flower_type_name )
{
    return myFlowerDict->Find( flower_type_name );
}
int cFlowerFactory::TypeCount()
{
    return (int)myFlowerDict->Size();
}

/**

  Construct a new instance of a specified flower type

  @param[in] flower_type_name the flower type to create

*/
cFlower* cFlowerFactory::Construct( const std::string& flower_type_name )
{
    int index = myFlowerDict->Find( flower_type_name );
    if( index < 0 )
    {
        return nullptr;
    }

    return Construct( index );

}
cFlower * cFlowerFactory::Construct(
    int flower_type_index )
{
    switch( flower_type_index )
    {
    case 1:
        return new cFlower();
    case 2:
        return new cPump();
    case 3:
        return new cVessel();
    case 4:
        return new cFunnel();
    case 5:
        return new cSource();
    case 6:
        return new cSourceFlow();
    case 7:
        return new cSink();
    case 8:
        return new cDelay();
    case 9:
        return new cBusy();
    case 10:
        return new cQueue();
    case 11:
        return new cPipeBend();

    default:
        return ConstructModelFlower( flower_type_index );
    }
}
cPipeBend::cPipeBend()
{
    myTypeName = "PipeBend";
    setName();
    //myWidth = 5;
}
/**  Draw pipebend -  a small cirle
*/
#ifdef WXWIDGETS
void cPipeBend::Paint( wxPaintDC& dc )
{
    int r = 3;
    dc.DrawCircle( myX+r, myY+r, r );
}

wxPoint cPipeBend::getEntryPort()
{
    wxPoint p( myX, myY );
    return p;
}
wxPoint cPipeBend::getExitPort()
{
    wxPoint p( myX, myY );
    return p;

}
void cPipeBend::getPortLocation( int& x, int& y, const cFlower& other)
{
    x = myX;
    y = myY;
}
void cPipeBend::setLocation( wxPoint p )
{
    myX = p.x;
    myY = p.y;
}
wxPoint cVessel::getExitPort()
{
    wxPoint p( myX, myY );
    p.x += 100;
    p.y += 25;
    return p;
}

/**

  Flower Dialog Configuration dialog

  @param[in] parent window
  @param[in] flower to be configured
*/
cFlowerConfigDlg::cFlowerConfigDlg( wxWindow* parent, cFlower* flower )
    : wxDialog(parent,-1,"",wxDefaultPosition,wxSize(300,-1))
{
    SetTitle(flower->getTypeName() + " '" + flower->getName() + "' Configuration");
    int paramCount = flower->myParam.size();
    if( ! paramCount )
    {
        new wxStaticText(this,-1,
                         "Generic flower has no configuration parameters",
                         wxPoint(25,25));
        return;
    }

    wxBoxSizer * szr = new wxBoxSizer( wxVERTICAL );
    pg = new wxPropertyGrid( this, -1,
                             wxPoint(-1,-1), wxSize(300,-1),
                             wxPG_SPLITTER_AUTO_CENTER  );
    pg->SetExtraStyle( wxPG_EX_HELP_AS_TOOLTIPS );
    szr->Add( pg ,0,wxALL,20);
    szr->Add( CreateSeparatedButtonSizer( wxOK | wxCANCEL ) );

    pg->Append( new wxStringProperty("Name",wxPG_LABEL,flower->getName() ));

    flower->AppendProperties( pg );

    SetSizerAndFit( szr );

}
#endif
}

    void cFlowerDict::Insert(  const std::string& flower_type_name )
    {
        std::cout << "Flower Dictionary Insert " << myDict.size()+1 <<" "<< flower_type_name << std::endl;
        myDict.insert(
            cFlowerType(
                myDict.size()+1,
                flower_type_name ) );
    }


}
}

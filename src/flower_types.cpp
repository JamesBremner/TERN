/*
		* Copyright (c) 2012 by James Bremner
		* All rights reserved.
		*
		* Use license: Modified from standard BSD license.
		*
		* Redistribution and use in source and binary forms are permitted
		* provided that the above copyright notice and this paragraph are
		* duplicated in all such forms and that any documentation, advertising
		* materials, Web server pages, and other materials related to such
		* distribution and use acknowledge that the software was developed
		* by James Bremner. The name "James Bremner" may not be used to
		* endorse or promote products derived from this software without
		* specific prior written permission.
		*
		* THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
		* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
		* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/
#include <iostream>

#include "flower.h"
#include "flower_types.h"


#ifdef WXWIDGETS
#include <wx/propgrid/propgrid.h>
#include "ProcessFlowSimGUI.h"
#endif // WXWIDGETS


namespace raven
{
namespace sim
{
namespace gui
{

cPump::cPump( )
{
    myTypeName = "Pump";
    myType     = cFlowerFactory::Index( myTypeName );
	myShape = "circle";
	setName();

}
cVessel::cVessel()
{
	myTypeName = "Vessel";
	setName();
	AddParam( "Volume", " (liters) " );
}
cFunnel::cFunnel()
{
	myTypeName = "Funnel";
	setName();
	AddParam( "Capacity", " (liters) " );
	AddParam( "Output", " rate, volume/time" );
	AddParam( "StartVolume", "Starting Volume");

    std::vector< string > vQualityNames;
    raven::sim::tern::cQuality::getNames( vQualityNames );
    for ( auto& q : vQualityNames )
    {
        if( q.empty() )
            continue;
        if( q == "Volume" )
            continue;
        AddParam( q, "" );
    }

	myOutputPortCount = 2;
}
cSource::cSource()
{
    myTypeName = "Source";
    setName();
    AddParam("Steady","1 for constant rate, 0 for exponential");
    AddParam( "Mean", "Mean time between arrivals", 1 );
    AddParam( "Volume", "Volume of each arrival", 1);

    std::vector< string > vQualityNames;
    raven::sim::tern::cQuality::getNames( vQualityNames );
    for ( auto& q : vQualityNames )
    {
        if( ! q.empty() )
            AddParam( q, "" );
    }

}
cSourceFlow::cSourceFlow()
{
    myTypeName = "SourceFlow";
    setName();

    std::vector< string > vQualityNames;
    raven::sim::tern::cQuality::getNames( vQualityNames );
    for ( auto& q : vQualityNames )
    {
        if( ! q.empty() )
            AddParam( q, "" );
    }
}
cSink::cSink()
{
    myTypeName = "Sink";
    setName();
}
cDelay::cDelay()
{
    myTypeName = "Delay";
    setName();
    AddParam( "Mean", "secs");
}
cBusy::cBusy()
{
    myTypeName = "Busy";
    setName();
    AddParam( "Mean", "secs");
}
cQueue::cQueue()
{
    myTypeName = "Queue";
    setName();
}

#ifdef WXWIDGETS
/**

  Draw pump - a circle of diameter mySize

*/
void cPump::Paint( wxPaintDC& dc )
{
	int r = myWidth/2;
	dc.DrawCircle( myX, myY, r );
}
/**
  Draw vessel - a rectangle of height mySize and double the width

*/
void cVessel::Paint( wxPaintDC& dc )
{
	dc.DrawRectangle( myX, myY, 2*myWidth, myWidth );
}

void cFunnel::Paint( wxPaintDC& dc )
{
	dc.DrawRectangle( myX, myY, 2*myWidth, myWidth );
}
wxPoint cFunnel::getExitPort()
{
	wxPoint p( myX, myY );
	p.x += 100;
	p.y += 20;
	return p;
}
wxPoint cFunnel::getExitPort2()
{
	wxPoint p( myX, myY );
	p.x += 100;
	p.y += 30;
	return p;
}
#endif

}}}

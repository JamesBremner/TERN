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
#include "ModelFlowers.h"
#include "cQuality.h"

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
    myTypeName = L"Pump";
    myType     = cFlowerFactory::Index( myTypeName );
	myShape = L"circle";
	setName();

}
cVessel::cVessel()
{
	myTypeName = L"Vessel";
	setName();
	AddParam( L"Volume", L" (liters) " );
}
cFunnel::cFunnel()
{
	myTypeName = L"Funnel";
	setName();
	AddParam( L"Capacity", L" (liters) " );
	AddParam( L"Output", L" rate, volume/time" );
	AddParam( L"StartVolume", L"Starting Volume");

    std::vector< wstring > vQualityNames;
    raven::sim::tern::cQuality::getNames( vQualityNames );
    for ( auto& q : vQualityNames )
    {
        if( q.empty() )
            continue;
        if( q == L"Volume" )
            continue;
        AddParam( q, L"" );
    }

	myOutputPortCount = 2;
}
cSource::cSource()
{
    myTypeName = L"Source";
    setName();
    AddParam( L"Mean", L"Mean time between arrivals" );

    std::vector< wstring > vQualityNames;
    raven::sim::tern::cQuality::getNames( vQualityNames );
    for ( auto& q : vQualityNames )
    {
        if( ! q.empty() )
            AddParam( q, L"" );
    }

}
cSourceFlow::cSourceFlow()
{
    myTypeName = L"SourceFlow";
    setName();

    std::vector< wstring > vQualityNames;
    raven::sim::tern::cQuality::getNames( vQualityNames );
    for ( auto& q : vQualityNames )
    {
        if( ! q.empty() )
            AddParam( q, L"" );
    }
}
cSink::cSink()
{
    myTypeName = L"Sink";
    setName();
}
cDelay::cDelay()
{
    myTypeName = L"Delay";
    setName();
    AddParam( L"Mean", L"secs");
}
cBusy::cBusy()
{
    myTypeName = L"Busy";
    setName();
    AddParam( L"Mean", L"secs");
}
cQueue::cQueue()
{
    myTypeName = L"Queue";
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

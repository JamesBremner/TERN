#pragma once
#include "flower.h"
//#include "cVase.h"
#include "cQuality.h"
#ifdef WXWIDGETS
class wxPropertyGrid;
#endif // WXWIDGETS

namespace raven
{
namespace sim
{
namespace gui
{
class cPump : public cFlower
{
public:
    cPump();
#ifdef WXWIDGETS
    void Paint( wxPaintDC& dc );
#endif
};
class cVessel : public cFlower
{
public:
    cVessel();
#ifdef WXWIDGETS
    void Paint( wxPaintDC& dc );
    wxPoint getExitPort();
#endif
};
class cFunnel : public cFlower
{
public:
    cFunnel();
#ifdef WXWIDGETS
    void Paint( wxPaintDC& dc );
    wxPoint getExitPort();
    wxPoint getExitPort2();
#endif
};
class cSource : public cFlower
{
public:
    cSource();
};
class cSourceFlow : public cFlower
{
public:
    cSourceFlow();
};
class cSink : public cFlower
{
public:
    cSink();
};
class cDelay : public cFlower
{
public:
    cDelay();
};
class cBusy : public cFlower
{
public:
    cBusy();
};
class cQueue : public cFlower
{
public:
    cQueue();
};

}
}
}

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

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------


#include "wx/wx.h"

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#if !defined(__WXMSW__) && !defined(__WXPM__)
#include "../sample.xpm"
#endif
#include "ProcessFlowSimGUI.h"



#include <cstdio>
#include <algorithm>
#include <utility>

#include "raven_sqlite.h"

#include "flower.h"
//#include "ModelFlowers.h"
#include "cVase.h"
#include "table.h"
#include "cQuality.h"


// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:

    raven::sim::gui::cTable * theTable;

    virtual bool OnInit();
};

namespace raven
{
namespace sim
{
namespace gui
{

// Define a new frame type: this is going to be our main frame
class cVaseMainFrame : public wxFrame
{
public:

    // ctor(s)
    cVaseMainFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnFileNew( wxCommandEvent& event );
    void OnSave( wxCommandEvent& event );
    void OnLoad( wxCommandEvent& event );
    void OnSimRun( wxCommandEvent& event );
    void OnSimOpts( wxCommandEvent& event );
    void OnReportSave( wxCommandEvent& event );
    void OnReportSaveNew( wxCommandEvent& event );
    void OnReportLoad( wxCommandEvent& event );


private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

}
}
}

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(raven::sim::gui::cVaseMainFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  raven::sim::gui::cVaseMainFrame::OnQuit)
    EVT_MENU(Minimal_About, raven::sim::gui::cVaseMainFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    raven::sim::gui::cVaseMainFrame *frame = new raven::sim::gui::cVaseMainFrame("Vase");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

namespace raven
{
namespace sim
{
namespace gui
{

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
cVaseMainFrame::cVaseMainFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(MY_ICON));

    // create a menu bar
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(IDM_file_new,"New");
    fileMenu->Append(IDM_rep_Save,"Save");
    fileMenu->Append(IDM_rep_SaveNew,"Save As New Report");
    fileMenu->Append(IDM_rep_Load,"Load");
    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *simMenu  = new wxMenu;
    simMenu->Append(IDM_sim_run,"Run!");
    simMenu->Append(IDM_sim_opts,"Options","Change simulation parameters");

//    wxMenu *reportMenu = new wxMenu;
//    reportMenu->Append(IDM_rep_Save,"Save");
//    reportMenu->Append(IDM_rep_Load,"Load");

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About...\tF1", "Show about dialog");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(simMenu,  "&Simulate");
    //menuBar->Append(reportMenu,  "&Report");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);


    Bind(wxEVT_MENU,&cVaseMainFrame::OnFileNew,this,IDM_file_new);
    Bind(wxEVT_MENU,&cVaseMainFrame::OnSave,this,IDM_Save);
    Bind(wxEVT_MENU,&cVaseMainFrame::OnLoad,this,IDM_Load);

    Bind(wxEVT_MENU,&cVaseMainFrame::OnSimRun,this,IDM_sim_run);
    Bind(wxEVT_MENU,&cVaseMainFrame::OnSimOpts,this,IDM_sim_opts);

    Bind(wxEVT_MENU,&cVaseMainFrame::OnReportSaveNew,this,IDM_rep_SaveNew);
    Bind(wxEVT_MENU,&cVaseMainFrame::OnReportSave,this,IDM_rep_Save);
    Bind(wxEVT_MENU,&cVaseMainFrame::OnReportLoad,this,IDM_rep_Load);

    // create a status bar
    CreateStatusBar(1);
    SetStatusText("Welcome to Vase!");


    // construct a table on which to display the flowers
    wxGetApp().theTable = new raven::sim::gui::cTable( this );

    // construct the flowers
    wxGetApp().theTable->Read("");

}


// event handlers

void cVaseMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void cVaseMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                     "Welcome to Vase!\n"
                 ),
                 "About Vase",
                 wxOK | wxICON_INFORMATION,
                 this);
}
void cVaseMainFrame::OnFileNew( wxCommandEvent& event )
{
    wxGetApp().theTable->Clear();
}
void cVaseMainFrame::OnSave( wxCommandEvent&  )
{
    wxGetApp().theTable->Write("vase.dot");
}
void cVaseMainFrame::OnLoad( wxCommandEvent&  )
{
    wxGetApp().theTable->Read("vase.dot");
}
void cVaseMainFrame::OnSimRun( wxCommandEvent& event )
{
    wxGetApp().theTable->SimRun();
}
/**  User wants to edit the simulation options
*/
void cVaseMainFrame::OnSimOpts( wxCommandEvent& event )
{
    raven::sim::gui::cSimOptionDlg dlg( this );
    dlg.ShowModal();
}
void cVaseMainFrame::OnReportSaveNew( wxCommandEvent& event )
{
    wxGetApp().theTable->ReportSaveNew();
}
void cVaseMainFrame::OnReportSave( wxCommandEvent& event )
{
    wxGetApp().theTable->ReportSave();
}
void cVaseMainFrame::OnReportLoad( wxCommandEvent& event )
{
    wxGetApp().theTable->ReportLoad();
    SetTitle( "Vase " + wxGetApp().theTable->ReportName() );
}




/**

  User has clicked somewhere, select flower under click

  */
void cTable::MouseLeftDown( wxMouseEvent& event )
{
    // select flower under click
    myVase.Select( event.GetPosition() );

    // refresh - this should show the selected flower highlighted in some way
    Refresh();

}
/**

  User has right clicked

  If he right clicks on a flower
  pop-up menue of things to do with flower

  if he right clicks on empty background
  create a new flower

*/
void cTable::MouseRightDown( wxMouseEvent& event )
{
    myPosition_clicked = event.GetPosition();

    // find flower under click
    myVase.Select( myPosition_clicked );

    // check there was a flower under click
    if( myVase.IsSelected() )
    {

        wxMenu * menu = new wxMenu();
        menu->Append( IDM_Rename,"Rename");
        menu->Append( IDM_Configure,"Configure");

        // If user has right clicked on a flower
        // when he has a different flower selected ( hilighted in red )
        // offer to connect the two flowers with a pipe

        if( myVase.getSelected2() )
        {
            menu->Append( IDM_Pipe,"Pipe");
            if( myVase.getSelected2()->getOutPortCount() > 1 )
            {
                menu->Append( IDM_Pipe2, "Pipe from 2nd output");
            }
        }
        menu->Append( IDM_Plot,"Plot");
        menu->Append( IDM_Delete,"Delete");
        PopupMenu( menu );
        delete menu;

    }
    else
    {

        // no flower under click
        // offer menu of new flowers
        PopupMenu( &myFlowerFactory.myMenuTask );

    }
    // redraw everything
    Refresh();
}
/**

  User has selected type of new flower to create

  @param[in] event The wxWidgets event generated by the selection

  The event.GetId() method gives the ID of the selection.
  The menu IDs should be arranged in the same order
  as the corresponding types in cFlowerFactory::type enum

*/
void cTable::OnCreateFlower( wxCommandEvent& event  )
{
    myVase.Add( ( event.GetId() - IDM_CreateGeneric ) );
    myVase.setLocation( myPosition_clicked );
}


/**

  Connect pipe from previously selected flower to current flower

*/
void cTable::OnPipe( wxCommandEvent& )
{
    myVase.Connect();
}
void cTable::OnPipe2( wxCommandEvent& )
{
    myVase.Connect2();
}

void cTable::MouseDoubleClick( wxMouseEvent& event )
{
    // find flower under click
    myVase.Select( event.GetPosition() );

    // check there was a flower under click
    if( myVase.IsSelected() )
    {
        wxTextEntryDialog dlg(this,"","Name the flower");
        dlg.ShowModal();
        myVase.setName(  dlg.GetValue().ToStdString() );
        Refresh();
    }

}

void cTable::OnRename( wxCommandEvent&  )
{
    wxTextEntryDialog dlg(this,"","Name the flower");
    dlg.ShowModal();
    myVase.setName( dlg.GetValue().ToStdString() );
    Refresh();

}
void cTable::OnConfigure( wxCommandEvent& )
{
    myVase.Configure( this );
}

void cTable::OnDelete( wxCommandEvent& )
{
    myVase.Delete();
}






cSimOptionDlg::cSimOptionDlg( wxWindow* parent )
    : wxDialog(parent,-1,"Simulation Options",
               wxDefaultPosition,wxSize(250,300))
{
    // read values from database
    int time = 100;
    int type = 1;
    int plot_points = 50;
    raven::sqlite::cDB db;
    db.Open("vase.dat");
    db.Query("SELECT type, time, plot_points FROM params;");
    if( db.myError )
    {
    }
    else
    {
        type = strtol(db.myResultA[0].c_str(),NULL,10);
        time = strtol( db.myResultA[1].c_str(),NULL,10);
        plot_points = strtol( db.myResultA[2].c_str(),NULL,10);
    }


    wxBoxSizer * szr = new wxBoxSizer( wxVERTICAL );
    pg = new wxPropertyGrid( this, -1, wxPoint(-1,-1), wxSize(200,150) );
    szr->Add( pg ,0,wxALL,20);
    szr->Add( CreateSeparatedButtonSizer( wxOK | wxCANCEL ) );
    SetSizer( szr );

    pg->Append( prop_time = new wxIntProperty("Time", wxPG_LABEL, time ) );
    pg->Append( prop_plot_points = new wxIntProperty("Plot Points", wxPG_LABEL, plot_points ) );

    pg->Append( new wxPropertyCategory("Task/Flow Qualities") );
    db.Query(L"SELECT * FROM quality_names;" );
    int QCount = 1;
    for( auto& name : db.myResult )
    {
        prop_quality[ QCount - 1 ] = new wxStringProperty(
            wxString::Format("Q%d",QCount), wxPG_LABEL, name );
        pg->Append( prop_quality[ QCount - 1 ]);
        QCount++;
    }
    for( ; QCount <= 5; QCount++ )
    {
        prop_quality[ QCount - 1 ] = new wxStringProperty(
            wxString::Format("Q%d",QCount), wxPG_LABEL );
        pg->Append( prop_quality[ QCount - 1 ] );
    }

    Bind( wxEVT_BUTTON, &cSimOptionDlg::OnOK, this, wxID_OK );

}
/**  User has clicked the OK button - save the paramater values to the database
*/
void cSimOptionDlg::OnOK( wxCommandEvent& event )
{
    // extract values from property grid
    wxAny value = prop_time->GetValue();
    int time = value.As<int>();
    value = prop_plot_points->GetValue();
    int plot_points = value.As<int>();

    // save values to database
    raven::sqlite::cDB db;
    db.Open("vase.dat");
    int ret = db.Query("UPDATE params SET type = 1, time = %d, plot_points = %d;",
             time, plot_points );
    if( ret == -1 )
    {
        wxMessageBox("Error writing params to db","VASE");
        return;
    }

    wxGetApp().theTable->myVase.setSimTime( time );
    wxGetApp().theTable->myVase.mySimType = ( cVase::e_type ) 1;
    wxGetApp().theTable->myVase.setPlotPoints(  plot_points );

    db.Query("DELETE FROM quality_names;");
    std::vector<string> vQNames;
    for( int QCount = 1; QCount <= 5; QCount++ )
    {
        string qname( prop_quality[ QCount - 1 ]->GetValueAsString().ToStdString() );
        db.Query("INSERT INTO quality_names VALUES ( '%s' );",
                qname.c_str());
        vQNames.push_back( qname );
    }
    raven::sim::tern::cQuality::setNames( vQNames );

    // Close the dialog
    EndModal( wxOK );
}
}
}
}

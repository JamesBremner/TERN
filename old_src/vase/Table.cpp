#include <fstream>
#include "wx/wx.h"
#include "wx/filename.h"
#include "wx/dirdlg.h"
#include "wx/filedlg.h"
#include "ProcessFlowSimGUI.h"
//#include "ModelFlowers.h"
#include "Table.h"
#include "raven_sqlite.h"


// freechart

#include <wx/xy/xyplot.h>
#include <wx/xy/xylinerenderer.h>
#include <wx/axis/numberaxis.h>
#include <wx/axis/dateaxis.h>
#include <wx/chart.h>
#include <wx/xy/vectordataset.h>
#include <wx/xy/xysimpledataset.h>

namespace raven
{
namespace sim
{
namespace gui
{
cTable::cTable( wxWindow * parent )
    : wxPanel(parent,-1,wxPoint(0,0),wxSize(600,600))
    , myChart( 0 )
{
    myFlowerFactory.setMenu( IDM_CreateGeneric );
    SetBackgroundColour(RGB(255,255,255));

    // bind events to handlers
    Bind( wxEVT_MOTION, &cTable::MouseMove, this );
    Bind( wxEVT_PAINT, &cTable::Paint, this );
    Bind( wxEVT_LEFT_DOWN, &cTable::MouseLeftDown, this );
    Bind( wxEVT_RIGHT_DOWN, &cTable::MouseRightDown, this );
    Bind( wxEVT_LEFT_DCLICK, &cTable::MouseDoubleClick, this );
    Bind( wxEVT_MENU, &cTable::OnRename, this, IDM_Rename );
    Bind( wxEVT_MENU, &cTable::OnDelete, this, IDM_Delete );
    Bind( wxEVT_MENU, &cTable::OnPipe, this, IDM_Pipe );
    Bind( wxEVT_MENU, &cTable::OnPipe2, this, IDM_Pipe2 );
    Bind( wxEVT_MENU, &cTable::OnCreateFlower, this,
          IDM_CreateGeneric,
          IDM_CreateGeneric + cFlowerFactory::TypeCount() );
    Bind( wxEVT_MENU, &cTable::OnConfigure, this, IDM_Configure );
    Bind( wxEVT_MENU, &cTable::OnPlot, this, IDM_Plot );
}

void cTable::Clear()
{
    myVase.Clear();
    myVase.DBClear();
    delete myChart;
    myChart = 0;
    Refresh();
}

bool cTable::Read( const std::string& filename )
{
    bool ret = myVase.Read( filename );
    Refresh();
    return ret;
}


void cTable::SimRun()
{
    Write( "vase.dot");

    myVase.DBEnsureSanity();

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    wchar_t cmd[100];
#define ENGINE_EXECUTABLE_FILENAME L"tern.exe"
    snwprintf(cmd,99,ENGINE_EXECUTABLE_FILENAME);

    // Retain keyboard focus
    si.wShowWindow = SW_SHOWNOACTIVATE;
    si.dwFlags = STARTF_USESHOWWINDOW;


    if( !CreateProcess( NULL,   // No module name (use command line)
                        cmd,			// Command line
                        NULL,           // Process handle not inheritable
                        NULL,           // Thread handle not inheritable
                        FALSE,          // Set handle inheritance to FALSE
                        CREATE_NEW_CONSOLE,              //  creation flags
                        NULL,           // Use parent's environment block
                        NULL,           // Use parent's starting directory
                        &si,            // Pointer to STARTUPINFO structure
                        &pi )           // Pointer to PROCESS_INFORMATION structure
      )
    {
        printf("Cannot start engine\n");
        int syserrno = GetLastError();
        if( syserrno == 2 )
        {
            printf("Cannot find executable file %S in working directory\n",
                   ENGINE_EXECUTABLE_FILENAME );
            return;
        }
        printf( "Sysem error no (%d)\n", (int)GetLastError() );
        wchar_t * lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            (DWORD) syserrno,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR) &lpMsgBuf,
            0, NULL );

        char errorbuf[200];
        snprintf(errorbuf,199,
                 "Error is %S",
                 lpMsgBuf);
        LocalFree(lpMsgBuf);

        return;
    }

    // Wait until child process exits.
    ((wxFrame*)GetParent())->SetStatusText("Simulation Running...");
    WaitForSingleObject( pi.hProcess, INFINITE );
    ((wxFrame*)GetParent())->SetStatusText("Simulation Completed");

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    ReadFinalReport();

    // Display final report

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Retain keyboard focus
    si.wShowWindow = SW_SHOWNOACTIVATE;
    si.dwFlags = STARTF_USESHOWWINDOW;

    wchar_t cmd2[100];
    snwprintf(cmd2,99,L"notepad.exe tern_final_report.txt" );


    CreateProcess( NULL,   // No module name (use command line)
                   cmd2,			// Command line
                   NULL,           // Process handle not inheritable
                   NULL,           // Thread handle not inheritable
                   FALSE,          // Set handle inheritance to FALSE
                   0,              //  creation flags
                   NULL,           // Use parent's environment block
                   NULL,           // Use parent's starting directory
                   &si,            // Pointer to STARTUPINFO structure
                   &pi );           // Pointer to PROCESS_INFORMATION structure


    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

}
/**

  Redraw the vase and all the flowers

*/
void cTable::Paint( wxEvent& )
{
    // construct device context for display
    wxPaintDC dc( this );

    if( myChart)
    {
        wxRect r = GetClientRect();
        myChart->Draw( dc, r );
    }

    myVase.Paint( dc );

}
/**

  User has moved the mouse

*/
void cTable::MouseMove( wxMouseEvent& event )
{
    // See if he is dragging something
    if( event.Dragging() )
    {

        // drag the selected flower
        myVase.setLocation( event.GetPosition() );

        Refresh();
    }
    else
    {
        cFlower * flower = myVase.find(  event.GetPosition() );
        if( ! flower )
            return;
        string target = flower->getName();
        for( auto& l : myFinalReport )
        {
            if( l.find( target ) == -1 )
                continue;
            ((wxFrame*)GetParent())->SetStatusText( l );

        }
    }
}

void cTable::ReadFinalReport()
{
    myFinalReport.clear();
    std::ifstream inf;
    inf.open("tern_final_report.txt");
    if( ! inf.is_open() )
    {
        printf("ERROR: Could not open %s\n",
               "tern_final_report.txt");
        return;
    }

    // loop over lines
    std::string line;
    while( 1 )
    {
        std::getline( inf, line );
        if( !inf.good() )
            break;
        if( line.length() < 2 )
            continue;
        myFinalReport.push_back( line );
    }
}
void cTable::ReportSaveNew()
{
    // Prompt user for report name
    wxTextEntryDialog dlg(this,"","Name the report");
    dlg.ShowModal();
    myReportName = dlg.GetValue();
    ReportSave();
}
void cTable::ReportSave()
{
    if( ! myReportName.Length())
    {
        ReportSaveNew();
        return;
    }

    wxFileName dir("./reports/" + myReportName, "");

    // Create folder for report files
    dir.Mkdir(wxS_DIR_DEFAULT,wxPATH_MKDIR_FULL );

    // Copy files from working direcotry to report folder
    wxCopyFile( "tern_final_report.txt",
                dir.GetPath() + "/tern_final_report.txt" );
    wxCopyFile( "vase.dot",
                dir.GetPath() + "/vase.dot");
    wxCopyFile( "vase.dat",
                dir.GetPath() + "/vase.dat");

    // Create a little file to simplify selection of the report
    wxFileName fn("./reports/" + myReportName + ".tbl" );
    FILE *fp = fopen( fn.GetFullPath(),"w");
    fprintf(fp,"%s\n",myReportName.mb_str().data());
    fclose(fp);

}
/**  Load a simulation report
*/
void cTable::ReportLoad()
{

    // query user for report required
    wxString filename = wxFileSelector(
                            "Choose simulation model/report",
                            "./reports/",
                            "",
                            ".tbl");
    if ( filename.empty() )
        return;
    wxFileName fn( filename );

    myReportName = fn.GetName();

    // copy report into working directory
    wxCopyFile( fn.GetPath() + "/" + fn.GetName() + "/tern_final_report.txt",
                "tern_final_report.txt" );
    wxCopyFile( fn.GetPath() + "/" + fn.GetName()  + "/vase.dot",
                "vase.dot" );
    wxCopyFile( fn.GetPath() + "/" + fn.GetName()  + "/vase.dat",
                "vase.dat");

    // read report into vase
    myVase.Clear();
    myVase.DBRead();
    myVase.Read( "vase.dot" );


    // display
    ReadFinalReport();
    Refresh();
}
void cTable::OnPlot( wxCommandEvent& event )
{

    //  create plot
    XYPlot *plot = new XYPlot();
    XYSimpleDataset * dataset = new XYSimpleDataset();

    try {

    // extract selected flower plots from report
    std::vector< std::string > vPlot;
    FindPlotInReport(
        vPlot,
        myVase.getSelected()->getName() );
    if( ! vPlot.size() )
        throw std::runtime_error{"not in db"};


    dataset->SetRenderer(new XYLineStepRenderer());

    bool fAllEmpty = true;
    for( auto& splt : vPlot )
    {

        // check there is something in this plot
        if( splt.empty() ) {
            // plot is empty, maybe something in other plots
            continue;
        }
        fAllEmpty = false;

        // Extract dataset

        std::vector<double> x;
        std::vector<double> y;
        int index = 0;
        int index_step = 1;
        if( myVase.getPlotPoints() )
            index_step = myVase.getSimTime() / myVase.getPlotPoints();
        const char * ps = splt.data();
        char * next = (char *)ps;
        while( 1 )
        {
            ps = next;
            double d = strtod( ps, &next );
            if( ps == next )
                break;
            y.push_back( d );
            x.push_back( (double) index * index_step );
            index++;
        }

        dataset->AddSerie( new XYSerie( x, y ));

    }
    if( fAllEmpty )
        throw std::runtime_error{"all empty"};
    }
    catch( std::runtime_error &e )
    {
        wxMessageBox(wxString::Format("No plots for %s",
                                     myVase.getSelected()->getName()),
                     "VASE plot error");
        delete myChart;
        return;
    }

    plot->AddDataset(dataset);

    // add left number and bottom axes
    NumberAxis *leftAxis = new NumberAxis(AXIS_LEFT);
    NumberAxis *bottomAxis = new NumberAxis(AXIS_BOTTOM);
    bottomAxis->IntegerValues();
    bottomAxis->SetVerticalLabelText(true);
    bottomAxis->SetTitle( "Time" );

    // add axes to first plot
    plot->AddAxis(leftAxis);
    plot->AddAxis(bottomAxis);

    // link axes and dataset
    plot->LinkDataVerticalAxis(0, 0);
    plot->LinkDataHorizontalAxis(0, 0);

    // and finally create chart
    delete myChart;
    myChart = new Chart(plot, myVase.getSelected()->getName() );

}
void cTable::FindPlotInReport(
    std::vector< std::string >& vplot,
    const std::string& name )
{
    vplot.clear();
    raven::sqlite::cDB db;
    db.Open("vase.dat");
    int rows = db.Query("SELECT * FROM plot WHERE flower = '%s';",
                        name.c_str() );
    for( int k = 0; k < rows; k++ )
    {
        vplot.push_back( db.myResultA[ k * 3 + 2] );
    }
}
//{
//    vplot.clear();
//    bool found = false;
//    int linecount = -1;
//    for( auto& l : myFinalReport )
//    {
//        linecount++;
//        if( ! linecount )
//        {
//            // skip headline
//            continue;
//        }
//
//        if( l.find(L"plot:") == -1 )
//        {
//            if( found )
//            {
//                // obtained all plots for this flower
//                return;
//            }
//
//            //get name of flower
//            int p = l.find(L" ");
//            int q = l.find(L" ",p+1);
//            if( name == l.substr( p+1, q-p-1 ) )
//            {
//                found = true;
//            }
//        }
//        else
//        {
//            // get plot;
//            if( found )
//            {
//                vplot.push_back( l );
//            }
//        }
//    }
//    return;
//}
}
}
}



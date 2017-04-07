#include <vector>

class Chart;

namespace raven
{
namespace sim
{
namespace gui
{
/**

A panel on which to display the flowers
*/

class cTable : public wxPanel
{
public:

    raven::sim::gui::cVase myVase;

    cTable( wxWindow * parent );

    // event handlers

    void Paint( wxEvent& event );
    void MouseLeftDown( wxMouseEvent& event );
    void MouseRightDown( wxMouseEvent& event );
    void MouseMove( wxMouseEvent& event );
    void MouseDoubleClick( wxMouseEvent& event );
    void OnRename( wxCommandEvent& event );
    void OnConfigure( wxCommandEvent& event );
    void OnDelete( wxCommandEvent& event );
    void OnPipe( wxCommandEvent& event );
    void OnPipe2( wxCommandEvent& event );
    void OnCreateFlower( wxCommandEvent& event );
    void OnCreateGeneric( wxCommandEvent& event );
    void OnCreatePump( wxCommandEvent& event );
    void OnCreateVessel( wxCommandEvent& event );
    void OnPlot( wxCommandEvent& event );

    void Clear();

    bool Read( const std::string& filename );
    bool Write( const std::string& filename )
    {
        return myVase.Write( filename );
    }
    void SimRun();
    void ReportSave();
    void ReportSaveNew();
    void ReportLoad();

    wxString& ReportName()
    {
        return myReportName;
    }

private:

    cFlowerFactory myFlowerFactory;
    wxPoint myPosition_clicked;
    wxString myReportName;

    std::vector < std::string > myFinalReport;
    Chart * myChart;

    void    ReadFinalReport();
    void    FindPlotInReport(
        std::vector< std::string >& vplot,
        const std::string& name );
};

}
}
}


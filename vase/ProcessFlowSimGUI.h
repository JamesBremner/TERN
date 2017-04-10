/*
  Interfaces for proccess flow simulation GUI classes

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
#include <wx/propgrid/propgrid.h>

#include "flower.h"
#include "cVase.h"

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    Minimal_About = wxID_ABOUT,
    IDM_Delete,
    IDM_Rename,
    IDM_Pipe,
    IDM_Pipe2,
    IDM_Plot,
    IDM_CreateGeneric,
    room_for_100_flower_types = IDM_CreateGeneric + 100,
    IDM_file_new,
    IDM_Save,
    IDM_Load,
    IDM_Configure,
    IDM_sim_run,
    IDM_sim_opts,
    IDM_rep_Save,
    IDM_rep_SaveNew,
    IDM_rep_Load
};


namespace raven
{
namespace sim
{
namespace gui
{







class cSimOptionDlg : public wxDialog
{
    public:

    wxPropertyGrid* pg;
    wxIntProperty*  prop_time;
    wxIntProperty*  prop_plot_points;
    wxEnumProperty* prop_type;
    wxStringProperty * prop_quality[5];

    cSimOptionDlg( wxWindow* parent );
    void OnOK( wxCommandEvent& event );
};

}}}

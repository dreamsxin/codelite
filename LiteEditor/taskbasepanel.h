//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : taskbasepanel.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __taskbasepanel__
#define __taskbasepanel__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TaskBasePanel
///////////////////////////////////////////////////////////////////////////////
class TaskBasePanel : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText3;
		wxChoice* m_choiceFilter;
		wxStaticText* m_staticText4;
		wxChoice* m_choiceScope;
		wxButton* m_buttonSearch;
		wxButton* m_buttonStop;
		wxButton* m_buttonClear;
		wxListCtrl* m_listCtrlTasks;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFilter( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSearch( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnStop( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClear( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClearUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		
	
	public:
		TaskBasePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 635,300 ), long style = wxTAB_TRAVERSAL );
		~TaskBasePanel();
	
};

#endif //__taskbasepanel__

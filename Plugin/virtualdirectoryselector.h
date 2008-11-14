//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : virtualdirectoryselector.h              
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

#ifndef __virtualdirectoryselector__
#define __virtualdirectoryselector__

/**
@file
Subclass of VirtualDirectorySelectorBase, which is generated by wxFormBuilder.
*/

#include "virtualdirectoryselectorbase.h"
class Workspace;

/** Implementing VirtualDirectorySelectorBase */
class VirtualDirectorySelector : public VirtualDirectorySelectorBase
{
	Workspace *m_workspace;
	wxString m_initialPath;
	
public:
	static wxString DoGetPath(wxTreeCtrl* tree, const wxTreeItemId &item, bool validateFolder);
	
protected:
	void DoBuildTree();
	
protected:
	// Handlers for VirtualDirectorySelectorBase events.
	void OnItemSelected( wxTreeEvent& event );
	void OnButtonOK( wxCommandEvent& event );
	void OnButtonCancel( wxCommandEvent& event );
	void OnButtonOkUI(wxUpdateUIEvent &event);
	
public:
	/** Constructor */
	VirtualDirectorySelector( wxWindow* parent, Workspace *wsp, const wxString &initialPath = wxEmptyString );
	wxString GetVirtualDirectoryPath() const {return m_staticTextPreview->GetLabel();}
	bool SelectPath(const wxString &path);
};

#endif // __virtualdirectoryselector__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : acceltabledlg.h
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
#ifndef __acceltabledlg__
#define __acceltabledlg__

/**
@file
Subclass of AccelTableBaseDlg, which is generated by wxFormBuilder.
*/

#include "acceltablebasedlg.h"
#include <map>
#include <vector>
#include "ikeyboard.h"


/** Implementing AccelTableBaseDlg */
class AccelTableDlg : public AccelTableBaseDlg
{
	enum ListctrlCols { LC_Start=0, LC_Menu=LC_Start, LC_Action, LC_Accel, /*LC_ID,*/ LC_End };
	bool m_direction;
	long                      m_selectedItem;
	std::vector<MenuItemData> m_itemsVec;
	wxArrayString 			  m_IDarray;
	ListctrlCols			  m_SortCol;

protected:
	// Handlers for AccelTableBaseDlg events.
	void OnItemActivated( wxListEvent& event );
	void OnItemSelected( wxListEvent& event );
	void OnColClicked(wxListEvent &event);
	void PopulateTable(MenuItemDataMap *accelMap = NULL);
	void OnButtonOk(wxCommandEvent &e);
	void OnButtonDefaults(wxCommandEvent &e);
	void OnEditButton(wxCommandEvent &e);
	void OnText(wxCommandEvent& event);


	void DoItemActivated();
	bool Compare(const wxString &accel1, const wxString &accel2);
	void DisplayCorrectColumnImage() const;

public:
	/** Constructor */
	AccelTableDlg( wxWindow* parent );
	virtual ~AccelTableDlg();
};

#endif // __acceltabledlg__

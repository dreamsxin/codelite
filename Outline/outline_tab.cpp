//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : outline_tab.cpp
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

#include "ColoursAndFontsManager.h"
#include "bitmap_loader.h"
#include "clToolBar.h"
#include "cl_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "outline_symbol_tree.h"
#include "outline_tab.h"
#include "plugin.h"
#include <imanager.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/textdlg.h>
#include <wx/wupdlock.h>

const wxEventType wxEVT_SV_GOTO_DEFINITION = wxNewEventType();
const wxEventType wxEVT_SV_GOTO_DECLARATION = wxNewEventType();
const wxEventType wxEVT_SV_FIND_REFERENCES = wxNewEventType();
const wxEventType wxEVT_SV_RENAME_SYMBOL = wxNewEventType();
const wxEventType wxEVT_SV_OPEN_FILE = wxNewEventType();

#define OUTLINE_TAB_CXX 0
#define OUTLINE_TAB_PHP 1
#define OUTLINE_PLACE_HOLDER_PAGE 2

#define CHECK_ENABLED()                                      \
    if(!m_isEnabled) {                                       \
        clDEBUG1() << "Outline: view is disabled" << clEndl; \
        return;                                              \
    }

OutlineTab::OutlineTab(wxWindow* parent, IManager* mgr)
    : OutlineTabBaseClass(parent)
    , m_mgr(mgr)
    , m_sortCxxTreeAlphabetically(false)
    , m_isEnabled(false)
{
    m_tree = new svSymbolTree(m_panelCxx, m_mgr, wxID_ANY);
    m_simpleBook->SetBackgroundColour(clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

    // Bind the wxID_FIND from the top level window (our main frame)
    // to this class
    wxTheApp->GetTopWindow()->GetEventHandler()->Bind(wxEVT_MENU, &OutlineTab::OnSearchSymbol, this, wxID_FIND);

    m_sortCxxTreeAlphabetically =
        clConfig::Get().Read("OutlineView/SortCxxAlphabetically", m_sortCxxTreeAlphabetically);
    m_tree->SetSortByLineNumber(!m_sortCxxTreeAlphabetically);

    m_panelCxx->GetSizer()->Add(m_tree, 1, wxEXPAND);
    m_tree->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(OutlineTab::OnMenu), NULL, this);

    m_tree->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    m_treeCtrlPhp->SetManager(m_mgr);

    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(OutlineTab::OnActiveEditorChanged),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(OutlineTab::OnEditorClosed), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OutlineTab::OnAllEditorsClosed), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(OutlineTab::OnWorkspaceClosed), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_CMD_RETAG_COMPLETED, wxCommandEventHandler(OutlineTab::OnFilesTagged), NULL,
                                  this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &OutlineTab::OnEditorSaved, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_PAGE_CHANGED, &OutlineTab::OnActiveEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &OutlineTab::OnThemeChanged, this);

    Connect(wxEVT_SV_GOTO_DEFINITION, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
            this);
    Connect(wxEVT_SV_GOTO_DECLARATION, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
            this);
    Connect(wxEVT_SV_FIND_REFERENCES, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
            this);
    Connect(wxEVT_SV_RENAME_SYMBOL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL, this);
    m_toolbar = new clToolBar(m_panelCxx);

    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(wxID_SORT_ASCENDING, _("Sort"), images->Add("sort"), "", wxITEM_CHECK);
    m_toolbar->AddTool(wxID_FIND, _("Find"), images->Add("find"), "", wxITEM_NORMAL);
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL, &OutlineTab::OnSortAlpha, this, wxID_SORT_ASCENDING);
    m_toolbar->Bind(wxEVT_UPDATE_UI, &OutlineTab::OnSortAlphaUI, this, wxID_SORT_ASCENDING);
    m_panelCxx->GetSizer()->Insert(0, m_toolbar, 0, wxEXPAND);
}

OutlineTab::~OutlineTab()
{
    wxTheApp->GetTopWindow()->GetEventHandler()->Unbind(wxEVT_MENU, &OutlineTab::OnSearchSymbol, this, wxID_FIND);
    m_toolbar->Unbind(wxEVT_TOOL, &OutlineTab::OnSortAlpha, this, wxID_SORT_ASCENDING);
    m_toolbar->Unbind(wxEVT_UPDATE_UI, &OutlineTab::OnSortAlphaUI, this, wxID_SORT_ASCENDING);
    m_tree->Disconnect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(OutlineTab::OnMenu), NULL, this);

    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                     wxCommandEventHandler(OutlineTab::OnActiveEditorChanged), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_PAGE_CHANGED, &OutlineTab::OnActiveEditorChanged, this);
    EventNotifier::Get()->Disconnect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(OutlineTab::OnEditorClosed), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OutlineTab::OnAllEditorsClosed),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(OutlineTab::OnWorkspaceClosed), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_RETAG_COMPLETED, wxCommandEventHandler(OutlineTab::OnFilesTagged), NULL,
                                     this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &OutlineTab::OnEditorSaved, this);
    Disconnect(wxEVT_SV_GOTO_DEFINITION, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
               this);
    Disconnect(wxEVT_SV_GOTO_DECLARATION, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
               this);
    Disconnect(wxEVT_SV_FIND_REFERENCES, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
               this);
    Disconnect(wxEVT_SV_RENAME_SYMBOL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutlineTab::OnItemSelectedUI), NULL,
               this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &OutlineTab::OnThemeChanged, this);
}

void OutlineTab::OnSearchSymbol(wxCommandEvent& event)
{
    event.Skip();

    int sel = m_simpleBook->GetSelection();
    if(sel == wxNOT_FOUND) {
        return;
    }
    wxWindow* win = m_simpleBook->GetPage(sel);
    if(!win) {
        return;
    }

    if(!win->GetScreenRect().Contains(::wxGetMousePosition())) {
        return;
    }
    event.Skip(false);

    wxString text = ::wxGetTextFromUser("Find Symbol:", "Outline");
    if(text.empty()) {
        return;
    }

    if(m_simpleBook->GetSelection() == OUTLINE_TAB_PHP) {
        // PHP
        m_treeCtrlPhp->Select(text);

    } else {
        // C++
        wxString name = text;
        name.Trim().Trim(false);
        m_tree->SelectItemByName(name);
    }
}

void OutlineTab::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    EditorChanged();
}

void OutlineTab::OnAllEditorsClosed(wxCommandEvent& e)
{
    e.Skip();
    m_tree->Clear();
    m_tree->ClearCache();
    m_treeCtrlPhp->Clear();
}

void OutlineTab::OnEditorClosed(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = reinterpret_cast<IEditor*>(e.GetClientData());
    if(editor) {
        if(m_tree->GetFilename() == editor->GetFileName()) {
            m_tree->Clear();
            m_tree->ClearCache();

        } else if(m_treeCtrlPhp->GetFilename() == editor->GetFileName()) {
            m_treeCtrlPhp->Clear();
        }
    }
}

void OutlineTab::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    wxWindowUpdateLocker locker(this);
    m_tree->Clear();
    m_treeCtrlPhp->DeleteAllItems();
}

void OutlineTab::OnFilesTagged(wxCommandEvent& e)
{
    e.Skip();
    CHECK_ENABLED();
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor) {

        wxWindow* oldFocusedWindow = wxWindow::FindFocus();
        m_tree->BuildTree(editor->GetFileName(), false);
        wxWindow* focusedWindow = wxWindow::FindFocus();
        if(oldFocusedWindow != focusedWindow && oldFocusedWindow) {
            // restore the focus back the old window
            oldFocusedWindow->SetFocus();
        }

    } else {
        m_tree->Clear();
    }
}
void OutlineTab::OnMenu(wxContextMenuEvent& e)
{
    wxMenu menu;

    if(IsIncludeFileNode()) {
        menu.Append(wxEVT_SV_OPEN_FILE, _("Open..."));
        menu.Connect(wxEVT_SV_OPEN_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutlineTab::OnOpenFile),
                     NULL, this);

    } else {
        menu.Append(wxEVT_SV_GOTO_DECLARATION, _("Goto Declaration"));
        menu.Append(wxEVT_SV_GOTO_DEFINITION, _("Goto Implementation"));
        menu.AppendSeparator();
        menu.Append(wxEVT_SV_FIND_REFERENCES, _("Find References..."));
        menu.AppendSeparator();
        menu.Append(wxEVT_SV_RENAME_SYMBOL, _("Rename Symbol..."));

        menu.Connect(wxEVT_SV_GOTO_DEFINITION, wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(OutlineTab::OnGotoImpl), NULL, this);
        menu.Connect(wxEVT_SV_GOTO_DECLARATION, wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(OutlineTab::OnGotoDecl), NULL, this);
        menu.Connect(wxEVT_SV_FIND_REFERENCES, wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(OutlineTab::OnFindReferenes), NULL, this);
        menu.Connect(wxEVT_SV_RENAME_SYMBOL, wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(OutlineTab::OnRenameSymbol), NULL, this);
    }

    m_tree->PopupMenu(&menu);
}

void OutlineTab::OnGotoDecl(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_decl"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnGotoImpl(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_impl"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnFindReferenes(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_references"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnRenameSymbol(wxCommandEvent& e)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("rename_symbol"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnItemSelectedUI(wxUpdateUIEvent& e)
{
    IEditor* editor = m_mgr->GetActiveEditor();
    e.Enable(editor && editor->GetSelection().IsEmpty() == false);
}

bool OutlineTab::IsIncludeFileNode() { return m_tree->IsSelectedItemIncludeFile(); }

void OutlineTab::OnOpenFile(wxCommandEvent& e)
{
    wxString includedFile = m_tree->GetSelectedIncludeFile();
    if(includedFile.IsEmpty())
        return;

    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("open_include_file"));
    evt.SetString(includedFile);
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evt);
}

void OutlineTab::OnPhpItemSelected(wxTreeEvent& event)
{
    event.Skip();
    m_treeCtrlPhp->ItemSelected(event.GetItem(), false);
}

void OutlineTab::OnEditorSaved(clCommandEvent& event)
{
    event.Skip();
    CHECK_ENABLED();
    wxFileName filename(event.GetFileName());
    if(FileExtManager::IsPHPFile(filename)) {
        m_treeCtrlPhp->BuildTree(filename);
    } else if(FileExtManager::IsCxxFile(filename)) {
        clDEBUG() << "Outline: editor saved";
        m_tree->BuildTree(filename, true);
    }
}

void OutlineTab::OnPhpItemActivated(wxTreeEvent& event)
{
    event.Skip();
    m_treeCtrlPhp->ItemSelected(event.GetItem(), true);
}

void OutlineTab::OnSortAlpha(wxCommandEvent& event)
{
    m_sortCxxTreeAlphabetically = event.IsChecked();
    clConfig::Get().Write("OutlineView/SortCxxAlphabetically", m_sortCxxTreeAlphabetically);
    m_tree->SetSortByLineNumber(!m_sortCxxTreeAlphabetically);
    CallAfter(&OutlineTab::DoRefreshCxxView);
}

void OutlineTab::OnSortAlphaUI(wxUpdateUIEvent& event) { event.Check(m_sortCxxTreeAlphabetically); }

void OutlineTab::DoRefreshCxxView()
{
    wxFileName fn = m_tree->GetFilename();
    m_tree->Clear();
    m_tree->BuildTree(fn, true);
}

void OutlineTab::EditorChanged()
{
    CHECK_ENABLED();
    IEditor* editor = m_mgr->GetActiveEditor();
    LexerConf::Ptr_t phpLexer = ColoursAndFontsManager::Get().GetLexer("php");
    LexerConf::Ptr_t cxxLexer = ColoursAndFontsManager::Get().GetLexer("c++");

    clDEBUG() << "Outline: editor changed event";

    // Use the lexer to determine if we can show outline
    if(editor && cxxLexer && FileExtManager::IsCxxFile(editor->GetFileName())) {
        m_tree->BuildTree(editor->GetFileName(), true);
        m_simpleBook->SetSelection(OUTLINE_TAB_CXX);

    } else if(editor && phpLexer && FileExtManager::IsPHPFile(editor->GetFileName())) {
        m_tree->Clear();
        m_treeCtrlPhp->BuildTree(editor->GetFileName());
        m_simpleBook->SetSelection(OUTLINE_TAB_PHP);

    } else {
        if(editor) {
            clDEBUG() << "Could not match an Outline to file:" << editor->GetFileName();
        }
        m_simpleBook->SetSelection(OUTLINE_PLACE_HOLDER_PAGE);
    }
}

void OutlineTab::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    m_simpleBook->SetBackgroundColour(clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
}

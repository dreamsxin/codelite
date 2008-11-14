//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : custombuildrequest.h              
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

#ifndef __custombuildrequest__
#define __custombuildrequest__

#include "compiler_action.h"
#include "build_config.h"

class CustomBuildRequest : public ShellCommand
{
	wxString m_fileName;
	
protected:
	void DoUpdateCommand(wxString &cmd, ProjectPtr proj, BuildConfigPtr bldConf);
	
public:
	CustomBuildRequest(wxEvtHandler *owner, const QueueCommand &buildInfo, const wxString &fileName);
	virtual ~CustomBuildRequest();
	
public:
	virtual void Process();
};
#endif // __custombuildrequest__

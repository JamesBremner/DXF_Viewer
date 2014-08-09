/***************************************************************
 * Name:      dxfv_wxWidgetsApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2014-06-09
 * Copyright:  ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif


#include "dxfv_wxWidgetsApp.h"
#include "dxfv_wxWidgetsMain.h"

IMPLEMENT_APP(dxfv_wxWidgetsApp);

bool dxfv_wxWidgetsApp::OnInit()
{
    dxfv_wxWidgetsFrame* frame = new dxfv_wxWidgetsFrame(0L, _("DXF File Viewer"));
    frame->SetIcon(wxICON(aaaa)); // To Set App Icon
    frame->Show();

    return true;
}

/***************************************************************
 * Name:      dxfv_wxWidgetsMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2014-06-09
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef DXFV_WXWIDGETSMAIN_H
#define DXFV_WXWIDGETSMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "dxfv_wxWidgetsApp.h"


class CDxf;

class dxfv_wxWidgetsFrame: public wxFrame
{
    public:
        dxfv_wxWidgetsFrame(wxFrame *frame, const wxString& title);
        ~dxfv_wxWidgetsFrame();
    private:
        enum
        {
            idMenuQuit = 1000,
            idMenuAbout,
            idMenuOpen,
        };
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnOpen(wxCommandEvent& event);
        void OnPaint(wxPaintEvent& event);
        void OnSize(wxSizeEvent& event);
        DECLARE_EVENT_TABLE()


        CDxf * dxf;
};


#endif // DXFV_WXWIDGETSMAIN_H

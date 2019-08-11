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


#include "src/Dxf.h"

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
            idMenuFit
        };
        wxPoint old_pos;

        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnOpen(wxCommandEvent& event);
        void OnFit(wxCommandEvent& event);
        void OnPaint(wxPaintEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnWheel(wxMouseEvent& event);
        void OnMouseMove(wxMouseEvent& event);
        void OnLeftDown(wxMouseEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        DECLARE_EVENT_TABLE()


        dxfv::CDxf * dxf;

        /// Model co-ordinates of window center
        wxPoint ModelAtWindowCenter();

        /// Pan so model point is at window center
        void PanToWindowCenter( wxPoint& P );
};


#endif // DXFV_WXWIDGETSMAIN_H

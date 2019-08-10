/***************************************************************
 * Name:      dxfv_wxWidgetsMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2014-06-09
 * Copyright:  ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#include "dxfv_wxWidgetsMain.h"

#include <string>
#include <vector>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <cmath>


#include "src/dxf.h"

//helper functions
enum wxbuildinfoformat
{
    short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

BEGIN_EVENT_TABLE(dxfv_wxWidgetsFrame, wxFrame)
    EVT_CLOSE(dxfv_wxWidgetsFrame::OnClose)
    EVT_MENU(idMenuQuit, dxfv_wxWidgetsFrame::OnQuit)
    EVT_MENU(idMenuAbout, dxfv_wxWidgetsFrame::OnAbout)
    EVT_MENU(idMenuOpen, dxfv_wxWidgetsFrame::OnOpen)
    EVT_MENU(idMenuFit, dxfv_wxWidgetsFrame::OnFit)
    EVT_PAINT(dxfv_wxWidgetsFrame::OnPaint )
    EVT_SIZE(dxfv_wxWidgetsFrame::OnSize )
    EVT_MOUSEWHEEL( dxfv_wxWidgetsFrame::OnWheel)
    EVT_MOTION( dxfv_wxWidgetsFrame::OnMouseMove )
    EVT_LEFT_DOWN( dxfv_wxWidgetsFrame::OnLeftDown )
END_EVENT_TABLE()

dxfv_wxWidgetsFrame::dxfv_wxWidgetsFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{

    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuOpen,"Open");
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* viewMenu = new wxMenu(_T(""));
    viewMenu->Append(idMenuFit,"Fit Contents");

    mbar->Append(viewMenu, _("&View"));


    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);


#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
//    CreateStatusBar(2);
//    SetStatusText(_("OK"),0);
    //SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR

    dxf = new dxfv::CDxf();

    SetBackgroundColour( *wxBLACK );

}


dxfv_wxWidgetsFrame::~dxfv_wxWidgetsFrame()
{
}

void dxfv_wxWidgetsFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void dxfv_wxWidgetsFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void dxfv_wxWidgetsFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = "Version 1.0";
    wxMessageBox(msg, _("wxWidgets DXF File Viewer "));
}

void dxfv_wxWidgetsFrame::OnOpen(wxCommandEvent& event)
{

    wxFileDialog         openFileDialog(this, _("Open DXF file"), "", "",                       "DXF files (*.dxf)|*.dxf",
                                        wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...

    dxf->LoadFile( std::string(openFileDialog.GetPath().utf8_str()));

#ifdef DEMO
    if( dxf->myLoadStatus == dxfv::CDxf::demo )
    {
        wxMessageBox( "Demo limit exceeded!", "Sorry");
        exit(1);
    }
#endif // DEMO
    SetTitle( openFileDialog.GetPath() );

    dxf->myBoundingRectangle.Fit();

    // restart the pan feature
    // this stores the current mouse position
    // if this is not done here, then a false pan operation
    // sometime occurrs on the first display of a newly opened file
    old_pos.x = wxGetMousePosition().x - GetScreenPosition().x;
    old_pos.y = wxGetMousePosition().y - GetScreenPosition().y - 55;

    Refresh();
}
void dxfv_wxWidgetsFrame::OnFit(wxCommandEvent& event)
{
    dxf->myBoundingRectangle.Fit();

    Refresh();
}
void dxfv_wxWidgetsFrame::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetTextForeground( *wxYELLOW );

    // Get window dimensions
    wxSize sz = GetClientSize();

    // scale to window
    dxf->myBoundingRectangle.CalcScale( sz.GetWidth(), sz.GetHeight() );

    dxfv::s_dxf_draw draw;


    for( dxfv::CLine& line : dxf->m_Line )
    {
        dxf->Init( draw );
        line.getDraw( draw );
        dc.DrawLine( draw.x1, draw.y1, draw.x2, draw.y2 );
    }


    for( dxfv::CPolyLine& polyline : dxf->m_PolyLine )
    {
        dxf->Init( draw );
        while( polyline.getDraw( draw ) )
        {
            dc.DrawLine( draw.x1, draw.y1, draw.x2, draw.y2 );
        }
    }

    for( dxfv::CCircle& circle : dxf->m_Circle )
    {
        dxf->Init( draw );
        circle.getDraw( draw );
        dc.DrawCircle( draw.x1, draw.y1, draw.r );
    }

    for ( dxfv::CArc& arc : dxf->m_Arc )
    {
        dxf->Init( draw );
        arc.getDraw( draw );
        dc.DrawEllipticArc (
            draw.x1, draw.y1,
            draw.r, draw.r,
            draw.sa, draw.ea );
    }

    for( dxfv::CSpline& spline : dxf->m_Spline )
    {
        dxf->Init( draw );
        if( dxf->wxwidgets() & spline.m_ControlPointCount>0  )
        {
            spline.wxwidgets();
            wxPoint pa[spline.m_ControlPointCount];
            while( spline.getDraw( draw ) )
            {
                wxPoint p( draw.x1, draw.y1 );
                pa[draw.index-1] = p;
            }
            dc.DrawSpline( spline.m_ControlPointCount, pa );
        }
        else
        {
            while( spline.getDraw( draw ) )
            {
                dc.DrawLine( draw.x1, draw.y1, draw.x2, draw.y2 );
            }
        }

    }

    for ( dxfv::CText& text : dxf->myText )
    {
        dxf->Init( draw );
        text.getDraw( draw );
        //dc.DrawLabel( draw.text, wxRect( draw.x1, draw.y1,draw.x1+100, draw.y1+100 ) );
        dc.DrawText( draw.text, draw.x1, draw.y1 );
    }

    dc.SetTextForeground( *wxBLUE );
    for ( dxfv::CDimension& dim : dxf->myDimension )
    {
        dxf->Init( draw );
        dim.getDraw( draw );
        dc.DrawText( draw.text, draw.x1, draw.y1 );
    }
}
void dxfv_wxWidgetsFrame::OnSize(wxSizeEvent& event)
{
    Refresh();
}

void dxfv_wxWidgetsFrame::OnWheel(wxMouseEvent& event)
{
    if( event.GetWheelRotation() > 0 )
        dxf->myBoundingRectangle.ZoomIn();
    else
        dxf->myBoundingRectangle.ZoomOut();
    Refresh();
}
void dxfv_wxWidgetsFrame::OnLeftDown(wxMouseEvent& event)
{
    old_pos = event.GetPosition();
}
void dxfv_wxWidgetsFrame::OnMouseMove(wxMouseEvent& event)
{
    if( ! event.Dragging() )
    {
        return;
    }

    wxPoint now = event.GetPosition();
    dxf->myBoundingRectangle.Pan( old_pos.x,old_pos.y,now.x,now.y);
    Refresh();
    old_pos = now;
}

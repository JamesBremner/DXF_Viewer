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
    EVT_KEY_DOWN(dxfv_wxWidgetsFrame::OnKeyDown)
END_EVENT_TABLE()

dxfv_wxWidgetsFrame::dxfv_wxWidgetsFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title,
              wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxWANTS_CHARS )
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

    try
    {
        dxf->LoadFile( std::string(openFileDialog.GetPath().utf8_str()));
    }
    catch( std::runtime_error& e )
    {
        wxMessageBox(e.what(),"Error reading file");
        exit(1);
    }

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

    for( auto po : dxf->Objects() )
    {
        dxf->Init( draw );
        //while( po->getDraw( draw ) ) {
        switch( po->myType )
        {
        case dxfv::cDXFGraphObject::eType::line:
        case dxfv::cDXFGraphObject::eType::lwpolyline:
        case dxfv::cDXFGraphObject::eType::polyline:
            while( po->getDraw( draw ) )
                dc.DrawLine( draw.x1, draw.y1, draw.x2, draw.y2 );
            break;
        case dxfv::cDXFGraphObject::eType::circle:
            po->getDraw( draw );
            dc.DrawCircle( draw.x1, draw.y1, draw.r );
            break;
        case dxfv::cDXFGraphObject::eType::arc:
            po->getDraw( draw );
            dc.DrawEllipticArc (
                draw.x1, draw.y1,
                draw.r, draw.r,
                draw.sa, draw.ea );
            break;
        case dxfv::cDXFGraphObject::eType::spline:
        {
            dxfv::CSpline* spline = (dxfv::CSpline*)po;

            if( dxf->wxwidgets() & spline->m_ControlPointCount>0  )
            {
                spline->wxwidgets();
                wxPoint pa[spline->m_ControlPointCount];
                while( spline->getDraw( draw ) )
                {
                    wxPoint p( draw.x1, draw.y1 );
                    pa[draw.index-1] = p;
                }
                dc.DrawSpline( spline->m_ControlPointCount, pa );
            }
            else
            {
                while( po->getDraw( draw ) )
                {
                    dc.DrawLine( draw.x1, draw.y1, draw.x2, draw.y2 );
                }
            }
        }
        break;
        case dxfv::cDXFGraphObject::eType::text:
            po->getDraw( draw );
            //dc.DrawLabel( draw.text, wxRect( draw.x1, draw.y1,draw.x1+100, draw.y1+100 ) );
            dc.DrawText( draw.text, draw.x1, draw.y1 );
            break;
        }
        //}
    }





//
//
//    dc.SetTextForeground( *wxBLUE );
//    for ( dxfv::CDimension& dim : dxf->myDimension )
//    {
//        dxf->Init( draw );
//        dim.getDraw( draw );
//        dc.DrawText( draw.text, draw.x1, draw.y1 );
//    }
}
void dxfv_wxWidgetsFrame::OnSize(wxSizeEvent& event)
{
    Refresh();
}

void dxfv_wxWidgetsFrame::OnWheel(wxMouseEvent& event)
{
    // remember where the window center is in the model
    wxPoint model_at_center = ModelAtWindowCenter();

    if( event.GetWheelRotation() > 0 )
        dxf->myBoundingRectangle.ZoomIn();
    else
        dxf->myBoundingRectangle.ZoomOut();

    // pan so window center remains at same location in model
    PanToWindowCenter( model_at_center );

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

    if( ! event.ButtonIsDown(wxMOUSE_BTN_LEFT) )
        return;

    wxPoint now = event.GetPosition();
    dxf->myBoundingRectangle.Pan( old_pos.x,old_pos.y,now.x,now.y);
    Refresh();
    old_pos = now;
}
wxPoint dxfv_wxWidgetsFrame::ModelAtWindowCenter()
{
    wxSize sz = GetClientSize();
    double x = sz.GetWidth() / 2;
    double y = sz.GetHeight() / 2;
    dxf->myBoundingRectangle.RemoveScale( x, y );
    return wxPoint( x, y )    ;
}
void dxfv_wxWidgetsFrame::PanToWindowCenter( wxPoint& P )
{
    wxSize sz = GetClientSize();
    dxf->myBoundingRectangle.CalcScale( sz.GetWidth(), sz.GetHeight() );
    double x = P.x;
    double y = P.y;
    dxf->myBoundingRectangle.ApplyScale( x, y );
    dxf->myBoundingRectangle.Pan( x, y, sz.GetWidth() / 2, sz.GetHeight() / 2 );
}
void dxfv_wxWidgetsFrame::OnKeyDown(wxKeyEvent& event)
{
    // Ensure we want a zoom
    int key = event.GetKeyCode();
    if( key != WXK_UP && key != WXK_DOWN )
        return;

    // remember where the window center is in the model
    wxPoint model_at_center = ModelAtWindowCenter();

    // zoom
    if (key==WXK_UP)
        dxf->myBoundingRectangle.ZoomOut();
    if (key==WXK_DOWN)
        dxf->myBoundingRectangle.ZoomIn();

    // pan so window center remains at same location in model
    PanToWindowCenter( model_at_center );

    // display
    Refresh();

//    double xd = x_model_at_center;
//    double yd = y_model_at_center;
//    dxf->myBoundingRectangle.ApplyScale( xd, yd );

//    wxPoint m = event.GetPosition();
//    double xold = m.x;
//    double yold = m.y;
//    dxf->myBoundingRectangle.RemoveScale( xold, yold );
//    if (event.GetKeyCode()==WXK_UP)
//         dxf->myBoundingRectangle.ZoomOut();
//    if (event.GetKeyCode()==WXK_DOWN)
//         dxf->myBoundingRectangle.ZoomIn();
//        // Get window dimensions
//    wxSize sz = GetClientSize();
//
//    // scale to window
//    dxf->myBoundingRectangle.CalcScale( sz.GetWidth(), sz.GetHeight() );
//    double xnew = m.x;
//    double ynew = m.y;
//    dxf->myBoundingRectangle.RemoveScale( xnew, ynew );
//    double xt = xnew - xold;
//    double yt = ynew - yold;
//    //dxf->myBoundingRectangle.ApplyScale( xt, yt );
//    xt = dxf->myBoundingRectangle.myScale * xt;
//    yt = dxf->myBoundingRectangle.myScale * yt;
//    xt *= -1;
//    xt *= 3;
//    yt *= 3;
//    std::cout << xnew <<" "<< ynew <<" "<< xold <<" "<< yold
//        <<" "<< xt <<" "<< yt << "\n";
//    dxf->myBoundingRectangle.Pan( m.x+xt, m.y+yt, m.x, m.y );
//
//    double dx = m.x;
//    double dy = m.y;
//    dxf->myBoundingRectangle.RemoveScale( dx, dy );

//    std::cout << "dxdy " << dx <<" "<< dy << "\n";
//    std::cout << "<zoom\n";



}

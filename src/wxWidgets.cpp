#include <wx/wx.h>
#include <string>
#include <vector>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <cmath>

#include "Dxf.h"

// Global pointer to instance of class that uses the wxWidgets library to draw on the window

wxDC * theDC = nullptr;


#include <wx/wx.h>
#include <wx/spinctrl.h>

class PenDialog : public wxDialog {
public:
    PenDialog(wxWindow* parent, int& penThickness, bool& enableOverride)
        : wxDialog(parent, wxID_ANY, "Override Pen Thickness", wxDefaultPosition, wxSize(300, 200)),
          m_penThickness(penThickness), m_enableOverride(enableOverride) {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        sizer->Add(new wxStaticText(this, wxID_ANY, "Thickness (pixels):"), 0, wxALL, 10);
        m_spinCtrl = new wxSpinCtrl(this, wxID_ANY);
        m_spinCtrl->SetRange(1, 10);
        m_spinCtrl->SetValue(penThickness);
        sizer->Add(m_spinCtrl, 0, wxALL | wxEXPAND, 10);

        m_checkbox = new wxCheckBox(this, wxID_ANY, "Enable");
        m_checkbox->SetValue(enableOverride);
        sizer->Add(m_checkbox, 0, wxALL, 10);

        wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
        btnSizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxALL, 10);
        btnSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 0, wxALL, 10);
        sizer->Add(btnSizer, 0, wxALIGN_CENTER);

        SetSizerAndFit(sizer);

        Bind(wxEVT_BUTTON, &PenDialog::OnOk, this, wxID_OK);
    }

private:
    void OnOk(wxCommandEvent&) {
        m_enableOverride = m_checkbox->GetValue();
        if (m_enableOverride == true)
            m_penThickness = m_spinCtrl->GetValue();
        else
            m_penThickness = 0;

        EndModal(wxID_OK);
    }

    wxSpinCtrl* m_spinCtrl;
    wxCheckBox* m_checkbox;
    int& m_penThickness;
    bool& m_enableOverride;
};

class dxfv_wxWidgetsApp : public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(dxfv_wxWidgetsApp);

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
        idMenuDXFFileVersion,
        idMenuFit,
        idMenuPen,
        idMenuSOLID2pointParser,
        idMenuSOLID3pointParser,
        idMenuSOLID4pointParser
    };
    wxPoint old_pos;

    void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnDXFFileVersion(wxCommandEvent& event);
    void OnFit(wxCommandEvent& event);
    void OnPen(wxCommandEvent& event);
    void OnSelectSolidParser(wxCommandEvent& event);
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

bool dxfv_wxWidgetsApp::OnInit()
{
    dxfv_wxWidgetsFrame* frame = new dxfv_wxWidgetsFrame(0L, _("DXF File Viewer"));
    frame->SetIcon(wxICON(aaaa)); // To Set App Icon
    frame->Show();

    return true;
}

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
    EVT_MENU(idMenuDXFFileVersion, dxfv_wxWidgetsFrame::OnDXFFileVersion)
    EVT_MENU(idMenuFit, dxfv_wxWidgetsFrame::OnFit)
    EVT_MENU(idMenuPen, dxfv_wxWidgetsFrame::OnPen)
    EVT_MENU_RANGE(idMenuSOLID2pointParser, idMenuSOLID4pointParser, dxfv_wxWidgetsFrame::OnSelectSolidParser)
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
    fileMenu->Append(idMenuDXFFileVersion, "DXF file version");
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* viewMenu = new wxMenu(_T(""));
    viewMenu->Append(idMenuFit,"Fit Contents");
    viewMenu->Append(idMenuPen,"Pen");

    mbar->Append(viewMenu, _("&View"));

    wxMenu* parserMenu = new wxMenu(_T(""));
    parserMenu->AppendRadioItem(idMenuSOLID2pointParser,"SOLID 2 point parser");
    parserMenu->AppendRadioItem(idMenuSOLID3pointParser,"SOLID 3 point parser");
    parserMenu->AppendRadioItem(idMenuSOLID4pointParser,"SOLID 4 point parser");

    mbar->Append(parserMenu, _("&Parser"));

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

    wxFileDialog openFileDialog(this, _("Open DXF file"), "", "", "DXF files (*.dxf)|*.dxf",
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
    // this does nothing, myLoadStatus is never set to demo
    // TODO: remove
    // if( dxf->myLoadStatus == dxfv::CDxf::demo )
    // {
    //     wxMessageBox( "Demo limit exceeded!", "Sorry");
    //     exit(1);
    // }
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

void dxfv_wxWidgetsFrame::OnDXFFileVersion(wxCommandEvent& event)
{
    std::string version = dxf->getFileVersion();
    wxMessageBox(version, "DXF file version", wxOK);
}

void dxfv_wxWidgetsFrame::OnFit(wxCommandEvent& event)
{
    dxf->myBoundingRectangle.Fit();

    Refresh();
}

void dxfv_wxWidgetsFrame::OnPen(wxCommandEvent& event)
{
    int penThickness;
    bool enableOverride;
    PenDialog dlg(this, penThickness, enableOverride);
    if (dlg.ShowModal() == wxID_OK) {
            // penThickness == 0 means we don't need thickness override
            dxf->penThickOverride(penThickness);
            std::string filePath = GetTitle().ToStdString();
            if( filePath != "DXF Viewer")
                dxf->LoadFile( filePath );
            Refresh();  // Trigger a redraw to apply new pen thickness settings
    }
}

void dxfv_wxWidgetsFrame::OnSelectSolidParser(wxCommandEvent& event)
{
    int id = event.GetId();
    if (id == idMenuSOLID2pointParser)
        dxfv::CSolid::parser( dxfv::CSolid::eParser::solid_2point );
    else if (id == idMenuSOLID3pointParser)
        dxfv::CSolid::parser( dxfv::CSolid::eParser::solid_3point );
    else if (id == idMenuSOLID4pointParser)
        dxfv::CSolid::parser( dxfv::CSolid::eParser::solid_4point );
}

void dxfv_wxWidgetsFrame::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    theDC = &dc; // set the global wxDC pointer
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetTextForeground( *wxYELLOW );

    // draw graphical entities
    wxSize sz = GetClientSize();
    dxf->Draw( sz.GetWidth(), sz.GetHeight() );

//
//
//    dc.SetTextForeground( *wxBLUE );
//    for ( dxfv::CDimension& dim : dxf->myDimension )
//    {
//        dxf->Init( draw );
//        dim.getDraw( draw );
//        dc.DrawText( draw.text, draw.x1, draw.y1 );
//    }

    theDC = nullptr;
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

namespace dxfv
{
void CLine::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        wxColour c(draw.color);
        theDC->SetBrush(wxBrush(c));
        theDC->SetPen(wxPen(c, draw.thick));
        theDC->DrawLine( draw.x1, draw.y1, draw.x2, draw.y2 );
    }
}
void CArc::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        double xTopLeft = draw.x1 - draw.r;
        double yTopLeft = draw.y1 - draw.r;
        double diameter = 2 * draw.r;
        wxColour c(draw.color);
        // Set the brush to be transparent
        theDC->SetBrush(*wxTRANSPARENT_BRUSH);
        theDC->SetPen(wxPen(c, draw.thick));
        theDC->DrawEllipticArc (
            xTopLeft, yTopLeft,
            diameter, diameter,
            draw.sa, draw.ea );
    }
}
void CCircle::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        wxColour c(draw.color);
        // Set the brush to be transparent
        theDC->SetBrush(*wxTRANSPARENT_BRUSH);
        theDC->SetPen(wxPen(c, draw.thick));
        theDC->DrawCircle( draw.x1, draw.y1, draw.r );
    }
}
void cLWPolyLine::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        wxColour c(draw.color);
        theDC->SetBrush(wxBrush(c));
        theDC->SetPen(wxPen(c, draw.thick));
        theDC->DrawLine( draw.x1, draw.y1, draw.x2, draw.y2 );
    }
}
void CText::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        wxColour c(draw.color);
        theDC->SetBrush(wxBrush(c));
        theDC->SetPen(wxPen(c, draw.thick));
        theDC->DrawText( draw.text, draw.x1, draw.y1 );
    }
}
void CSpline::Draw( CDxf* dxf )
{
    wxPoint spline_points[MAXPOINTS];
    cDrawPrimitiveData draw( dxf );

    wxColour c(draw.color);
    theDC->SetBrush(wxBrush(c));
    theDC->SetPen(wxPen(c, draw.thick));

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        if( dxf->wxwidgets() && m_ControlPointCount>0  )
        {
            // use wxwidgets spline method for control point spline TID6

            if( ! draw.index_curve )
            {
                // collect control point
                wxPoint p( draw.x1, draw.y1 );
                spline_points[ draw.index-1] = p;
            }
            else
            {
                // all contol points
                // use wxwidget spline method
                theDC->DrawSpline(
                    m_ControlPointCount,
                    spline_points );
            }
        }
        else
        {
            // use wxwidgets drawing primitive for spline

            theDC->DrawLine( draw.x1, draw.y1, draw.x2, draw.y2 );
        }
    }
}
void CSolid::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        std::cout << "CSolid::Draw " << draw.color << "\n";
        std::cout << draw.x1 <<" "<< draw.y1 <<" "
                  << draw.x2 <<" "<< draw.y2 <<" "
                  << draw.x3 <<" "<< draw.y3 <<"\n";

        const wxPoint points[] = {
            {draw.x1,  draw.y1},
            {draw.x2,  draw.y2},
            {draw.x3,  draw.y3},
        };

        wxColour c(draw.color);
        theDC->SetBrush(wxBrush(c));
        theDC->SetPen(wxPen(c, draw.thick));

        theDC->DrawPolygon(3, points);
    }
}
}

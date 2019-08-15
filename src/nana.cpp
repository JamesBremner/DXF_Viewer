#include <cmath>
#include <nana/gui.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/menubar.hpp>

#include <windows.h>

#include "dxf.h"

int main()
{

    nana::form fm;
    fm.bgcolor( nana::colors::black );

    dxfv::CDxf* dxf = new dxfv::CDxf();

    nana::menubar mb( fm );
    nana::menu& mf = mb.push_back("File");
    mf.append("Open",[&](nana::menu::item_proxy& ip)
    {
        // prompt for file to open
        nana::filebox fb( fm, true );
        auto paths = fb();
        if( paths.empty() )
            return;
        try
        {
            // read the file
            dxf->LoadFile( paths[0].string());
        }
        catch( std::runtime_error& e )
        {
            nana::msgbox mb(fm,"Error reading file");
            mb << e.what();
            mb.show();
            exit(1);
        }

        // refresh display with contents of opened file
        nana::API::refresh_window( fm );
    });

    // register drawing function
    nana::drawing dw{fm};
    dw.draw([&dxf](nana::paint::graphics& graph)
    {
        // store context so entity draw methods can use it
        dxf->graph( &graph );

        // scale to window
        dxf->myBoundingRectangle.CalcScale(
            graph.width(),
            graph.height() );

        // loop over graphical entities
        for( auto po : dxf->Objects() )
        {
            po->Draw( dxf );
        }
    });

    // previous mouse position when dragged with left button pressed
    nana::point old_pos;

    // handle left mouse button down
    fm.events().mouse_down([&old_pos](const nana::arg_mouse&arg)
    {
        if( arg.left_button )
            old_pos = arg.pos;  // store mouse position
    });

    // handle mouse movement
    fm.events().mouse_move([&](const nana::arg_mouse&arg)
    {
        if( ! arg.left_button )
            return;

        // left button is down, pan the display so it moves with the mouse
        auto now = arg.pos;
        dxf->myBoundingRectangle.Pan( old_pos.x,old_pos.y,now.x,now.y);
        old_pos = now;

        // refresh display
        nana::API::refresh_window( fm );
    });

    fm.show();

    nana::exec();
}

namespace dxfv
{
void CLine::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );
    getDraw( draw );
    dxf->graph()->line(
    {draw.x1, draw.y1},
    {draw.x2, draw.y2},
    nana::colors::white);
}
void CArc::Draw( CDxf* dxf )
{
    int xl, yt, xr, yb, sx, sy, ex, ey;
    WAPData( xl, yt, xr, yb, sx, sy, ex, ey, dxf );
    HDC hdc = reinterpret_cast<HDC>(const_cast<void*>(dxf->graph()->context()));
    SelectObject(hdc, GetStockObject(DC_PEN));
    SetDCPenColor(hdc, RGB(255,255,255));
    ::Arc(
        hdc,
        xl, yt, xr, yb, sx, sy, ex, ey );
}
void CCircle::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );
    getDraw( draw );
    ::Ellipse(
        reinterpret_cast<HDC>(const_cast<void*>(dxf->graph()->context())),
        draw.x1-draw.r, draw.y1-draw.r,
        draw.x1+draw.r, draw.y1+draw.r );
}
void cLWPolyLine::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        dxf->graph()->line(
        {draw.x1, draw.y1},
        {draw.x2, draw.y2},
        nana::colors::white);
    }
}
void CText::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        dxf->graph()->string( { draw.x1, draw.y1 }, draw.text, nana::colors::yellow );
    }
}
void CSpline::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        dxf->graph()->line(
        {draw.x1, draw.y1},
        {draw.x2, draw.y2},
        nana::colors::white);
    }
}
}

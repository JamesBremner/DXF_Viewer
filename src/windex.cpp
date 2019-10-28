#include <cmath>

#include "wex.h"

#include "dxf.h"

int main()
{
    dxfv::CDxf dxf;

    // previous mouse position when dragged with left button pressed
//    nana::point old_pos {-1, -1 };

    wex::gui& fm = wex::topWindow();
    fm.bgcolor( 0 );

    // register drawing function
    // call update() on this whenever refresh is needed
//    nana::drawing drawing{fm};
//    drawing.draw([&dxf](nana::paint::graphics& graph)
//    {
//        // store context so entity draw methods can use it
//        dxf.graph( &graph );
//
//        // draw graphical entities
//        dxf.Draw(
//            graph.width(),
//            graph.height());
//    });


    fm.events().draw([&dxf]( PAINTSTRUCT& ps )
    {
         wex::shapes S( ps );
         dxf.set( S );
    });

    wex::menubar mb( fm );

    wex::menu mf( fm );
    mf.append("Open",[&]
    {
        // prompt for file to open
        wex::filebox fb( fm );
        auto paths = fb.path();
        if( paths.empty() )
            return;
        try
        {
            // read the file
            dxf.LoadFile( paths);

            // refresh display with contents of opened file
//            drawing.update();

            fm.text( paths);
        }
        catch( std::runtime_error& e )
        {
            wex::msgbox mb(fm,"Error reading file");
            exit(1);
        }
    });
    mb.append( "Open", mf );

    wex::menu mv( fm );
    mv.append("Fit",[&]
    {
        // rescale and pan so all entities fit in the window
        dxf.myBoundingRectangle.Fit();

        // refresh
        //drawing.update();
    });
    mb.append("View", mv );


//    // handle left mouse button down
//    fm.events().mouse_down([&old_pos](const nana::arg_mouse&arg)
//    {
//        if( arg.left_button )
//            old_pos = arg.pos;  // store mouse position
//    });
//
//    // handle mouse movement
//    fm.events().mouse_move([&](const nana::arg_mouse&arg)
//    {
//        if( ! arg.left_button )
//            return;
//
//        /* Ensure that dragging has been properly started
//        Without this strange things happen on startup
//        */
//        auto now = arg.pos;
//        if( old_pos.x < 0 )
//        {
//            old_pos = now;
//            return;
//        }
//
//        // left button is down, pan the display so it moves with the mouse
//
//        dxf.myBoundingRectangle.Pan( old_pos.x,old_pos.y,now.x,now.y);
//        old_pos = now;
//
//        // refresh display
//        drawing.update();
//    });
//
//    // handle mouse wheel
//    fm.events().mouse_wheel([&](const nana::arg_wheel& wheel)
//    {
//        // point in model located at center of window
//        nana::size sz = fm.size();
//        dxf.myBoundingRectangle.CalcScale( sz.width, sz.height );
//        double x = sz.width / 2;
//        double y = sz.height / 2;
//        dxf.myBoundingRectangle.RemoveScale( x, y );
//        nana::size modelAtWindowCenter { x, y };
//
//        // zoom
//        if( wheel.upwards )
//            dxf.myBoundingRectangle.ZoomIn();
//        else
//            dxf.myBoundingRectangle.ZoomOut();
//
//        // pan so that the same model point is returned to window center
//        dxf.myBoundingRectangle.CalcScale( sz.width, sz.height );
//        x = modelAtWindowCenter.width;
//        y = modelAtWindowCenter.height;
//        dxf.myBoundingRectangle.ApplyScale( x, y );
//        dxf.myBoundingRectangle.Pan( x, y, sz.width / 2, sz.height / 2 );
//
//        // refresh display
//        drawing.update();
//    });

    fm.show();

    wex::run();
}

namespace dxfv
{

void CLine::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );
    getDraw( draw );
    dxf->shapes()->color( 255,255,255 );
    dxf->shapes()->line(
    {draw.x1, draw.y1, draw.x2, draw.y2});
}
void CArc::Draw( CDxf* dxf )
{
//    int xl, yt, xr, yb, sx, sy, ex, ey;
//    WAPData( xl, yt, xr, yb, sx, sy, ex, ey, dxf );
//    HDC hdc = reinterpret_cast<HDC>(const_cast<void*>(dxf->graph()->context()));
//    SelectObject(hdc, GetStockObject(DC_PEN));
//    SetDCPenColor(hdc, RGB(255,255,255));
//    ::Arc(
//        hdc,
//        xl, yt, xr, yb, sx, sy, ex, ey );
}
void CCircle::Draw( CDxf* dxf )
{
//    cDrawPrimitiveData draw( dxf );
//    getDraw( draw );
//    ::Ellipse(
//        reinterpret_cast<HDC>(const_cast<void*>(dxf->graph()->context())),
//        draw.x1-draw.r, draw.y1-draw.r,
//        draw.x1+draw.r, draw.y1+draw.r );
}
void cLWPolyLine::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    dxf->shapes()->color( 255,255,255 );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        dxf->shapes()->line(
        {draw.x1, draw.y1,
        draw.x2, draw.y2});
    }
}
void CText::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    dxf->shapes()->color( 255,255,0 );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        RECT r( (int)draw.x1, (int)draw.y1,(int)draw.x1+100, (int)draw.y1+2 );
        dxf->shapes()->text( draw.text,
                             { (int)draw.x1, (int)draw.y1,(int)draw.x1+100, (int)draw.y1+25 } );
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

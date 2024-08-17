#include <cmath>

#include "wex.h"
#include "window2file.h"

#include "dxf.h"

#ifndef UNIT_TEST
int main()
{
    dxfv::CDxf dxf;

    // previous mouse position when dragged with left button pressed
    wex::sMouse old_pos;
    old_pos.x = -1;

    // construct application window
    wex::gui& fm = wex::maker::make();
    fm.text("DXF Viewer");
    fm.bgcolor( 0 );

    // register drawing function
    fm.events().draw([&dxf]( PAINTSTRUCT& ps )
    {
        wex::shapes S( ps );
        S.color( 255,255,255);
        dxf.set( S );
        dxf.Draw(
            ps.rcPaint.right,
            ps.rcPaint.bottom);
    });

    wex::menubar mb( fm );

    wex::menu mf( fm );
    mf.append("Open DXF file",[&](const std::string &title)
    {
        // prompt for file to open
        wex::filebox fb( fm );
        auto paths = fb.open();
        if( paths.empty() )
            return;
        try
        {
            // read the file
            dxf.LoadFile( paths );

            // refresh display with contents of opened file
            fm.update();

            fm.text( paths);
        }
        catch( std::runtime_error& e )
        {
            wex::msgbox mb(
                           std::string("Error reading file\n")+e.what());
            exit(1);
        }
    });
    mf.append("Save to PNG", [&](const std::string &title)
    {
        wex::filebox fb( fm );
        auto path = fb.save();
        if( path.empty() )
            return;
        wex::window2file w2f;
        w2f.save( fm, path );
    });
    mb.append( "File", mf );


    wex::menu mv( fm );
    mv.append("Fit",[&](const std::string &title)
    {
        // rescale and pan so all entities fit in the window
        dxf.myBoundingRectangle.Fit();

        // refresh
        fm.update();
    });
    mb.append("View", mv );

    wex::menu mp( fm );
    mp.append("SOLID 2 point parser",[&](const std::string &title)
    {
        mp.check(0,true);
        mp.check(1,false);
        mp.check(2,false);
        dxf.SolidParser( dxfv::eParser::solid_2point );
    });
    mp.append("SOLID 3 point parser",[&](const std::string &title)
    {
        mp.check(0,false);
        mp.check(1,true);
        mp.check(2,false);
        dxf.SolidParser( dxfv::eParser::solid_3point );
    });
    mp.append("SOLID 4 point parser",[&](const std::string &title)
    {
        mp.check(0,false);
        mp.check(1,false);
        mp.check(2,true);
        dxf.SolidParser( dxfv::eParser::solid_4point );
    });
    mp.check(0);
    mb.append("Parsers", mp );


//    // handle left mouse button down
    fm.events().click([&]
    {
        auto p = fm.getMouseStatus();
        old_pos.x = p.x;
        old_pos.y = p.y;
    });
//    fm.events().mouse_down([&old_pos](const nana::arg_mouse&arg)
//    {
//        if( arg.left_button )
//            old_pos = arg.pos;  // store mouse position
//    });
//
//    // handle mouse movement
    fm.events().mouseMove([&](wex::sMouse& m)
    {
        if( ! m.left )
            return;

        /* Ensure that dragging has been properly started
        Without this strange things happen on startup
        */
        if( old_pos.x < 0 )
        {
            old_pos = m;
            return;
        }

        // left button is down, pan the display so it moves with the mouse

        dxf.myBoundingRectangle.Pan( old_pos.x,old_pos.y,m.x,m.y);
        old_pos = m;

//        // refresh display
        fm.update();
    });

    // handle mouse wheel
    fm.events().mouseWheel([&](int dist)
    {
        // point in model located at center of window
        RECT r;
        GetClientRect( fm.handle(), &r );
        int ww = r.right - r.left;
        int wh = r.bottom - r.top;
        dxf.myBoundingRectangle.CalcScale( ww, wh );
        double x = ww / 2;
        double y = wh / 2;
        dxf.myBoundingRectangle.RemoveScale( x, y );

        // zoom
        if( dist > 0 )
            dxf.myBoundingRectangle.ZoomIn();
        else
            dxf.myBoundingRectangle.ZoomOut();

        // pan so that the same model point is returned to window center
        dxf.myBoundingRectangle.CalcScale( ww, wh );
        dxf.myBoundingRectangle.ApplyScale( x, y );
        dxf.myBoundingRectangle.Pan( x, y, ww / 2, wh / 2 );

        // refresh display
        fm.update();
    });

    fm.events().resize([&]( int w, int h )
    {
        fm.update();
    });

    fm.show();

    fm.run();
}

#endif // UNIT_TEST

namespace dxfv
{

void CLine::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );
    getDraw( draw );
    dxf->shapes()->color( draw.color );
    dxf->shapes()->line(
    {(int)draw.x1, (int)draw.y1, (int)draw.x2, (int)draw.y2});
}
void CArc::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );
    getDraw( draw );
    dxf->shapes()->color( draw.color );
    dxf->shapes()->arc(
        (int)draw.x1, (int)draw.y1, draw.r,
        draw.sa, draw.ea );
}
void CCircle::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );
    getDraw( draw );
    dxf->shapes()->color( draw.color );
    dxf->shapes()->circle(
        draw.x1, draw.y1, draw.r );
}
void cLWPolyLine::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
        dxf->shapes()->color( draw.color );
        dxf->shapes()->line(
        {
            (int)draw.x1, (int)draw.y1,
            (int)draw.x2, (int)draw.y2
        });
    }
}
void CText::Draw( CDxf* dxf )
{
    cDrawPrimitiveData draw( dxf );

    dxf->shapes()->color( draw.color );

    // loop over drawing primitives
    while( getDraw( draw ) )
    {
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

        dxf->shapes()->color( draw.color );
        
        // std::cout << "CSpline::Draw "
        //     << draw.x1 <<" "
        //     << draw.y1 <<" "
        //     << draw.x2 <<" "
        //     << draw.y2 <<"\n";
        dxf->shapes()->line(
        {
            (int)draw.x1, (int)draw.y1,
            (int)draw.x2, (int)draw.y2
        });
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

        dxf->shapes()->fill();
        dxf->shapes()->polygon(
        {
            (int)draw.x1, (int)draw.y1,
            (int)draw.x2, (int)draw.y2,
            (int)draw.x3, (int)draw.y3,
        });
    }
}
}

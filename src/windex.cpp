/*

Windex specific code, including the main() function

Only compile and link this file if using the windex library

Do not link with any other GUI framework library or specific code

*/
#include <cmath>

#include "wex.h"
#include "window2file.h"

#include "Dxf.h"

// Global pointer to instance of class that uses the windex library to draw on the window
wex::shapes *theShaper;

#ifndef UNIT_TEST

void penDlg(wex::gui &fm, dxfv::CDxf &dxf)
{
    wex::gui &dlg = wex::maker::make();
    dlg.move(200, 200, 400, 300);
    dlg.text("Over-ride pen thickness");
    wex::label &lbThick = wex::maker::make<wex::label>(dlg);
    lbThick.move(50, 50, 100, 30);
    lbThick.text("Thickness pixels");
    wex::editbox &ebThick = wex::maker::make<wex::editbox>(dlg);
    ebThick.move(200, 50, 100, 30);
    ebThick.text("1");
    wex::checkbox &chk = wex::maker::make<wex::checkbox>(dlg);
    chk.move(100, 100, 100, 30);
    chk.text("  Enable");
    wex::button &bOK = wex::maker::make<wex::button>(dlg);
    bOK.move(200, 150, 50, 30);
    bOK.text("OK");
    bOK.events().click(
        [&]
        {
            int thick = 0;
            if (chk.isChecked())
                thick = atoi(ebThick.text().c_str());
            dxf.penThickOverride(thick);
            dlg.endModal();
        });

    dlg.showModal(fm);
}

int main()
{
    dxfv::CDxf dxf;

    // previous mouse position when dragged with left button pressed
    wex::sMouse old_pos;
    old_pos.x = -1;

    // construct application window
    wex::gui &fm = wex::maker::make();
    fm.text("DXF Viewer");
    fm.bgcolor(0);

    // register drawing function
    fm.events().draw(
        [&dxf](PAINTSTRUCT &ps)
        {
            // construct shaper to draw on window
            theShaper = new wex::shapes(ps);
            theShaper->color(255, 255, 255);

            // display every entity defined in the dxf file

            try
            {
                dxf.Draw(
                    ps.rcPaint.right,
                    ps.rcPaint.bottom);
            }
            catch (std::runtime_error &e)
            {
                wex::msgbox mb(
                    std::string("Error \n") + e.what());
            }
            // done with the shaper
            delete theShaper;
        });

    wex::menubar mb(fm);

    wex::menu mf(fm);
    mf.append("Open DXF file", [&](const std::string &title)
              {
                  // prompt for file to open
                  wex::filebox fb(fm);
                  auto paths = fb.open();
                  if (paths.empty())
                      return;
                  try
                  {
                      // read the file
                      dxf.LoadFile(paths);

                      // refresh display with contents of opened file
                      fm.update();

                      fm.text(paths);
                  }
                  catch (std::runtime_error &e)
                  {
                      wex::msgbox mb(
                          std::string("Error reading file\n") + e.what());
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
        w2f.save( fm, path ); });
    mf.append("DXF file version",
              [&](const std::string &title)
              {
                  wex::msgbox(
                      fm,
                      dxf.getFileVersion(),
                      "DXF file version",
                      MB_OK);
              });
    mb.append("File", mf);

    // View menu

    wex::menu mv(fm);
    mv.append("Fit",
              [&](const std::string &title)
              {
                  // rescale and pan so all entities fit in the window
                  dxf.myBoundingRectangle.Fit();

                  // refresh
                  fm.update();
              });
    mv.append("Pen",
              [&](const std::string &title)
              {
                  penDlg(fm, dxf);
                  auto fname = fm.text();
                  if (fname != "DXF Viewer")
                      dxf.LoadFile(fname);
                  fm.update();
              });
    mb.append("View", mv);

    wex::menu mp(fm);
    mp.append("SOLID 2 point parser", [&](const std::string &title)
              {
        mp.check(0,true);
        mp.check(1,false);
        mp.check(2,false);
        dxfv::CSolid::parser( dxfv::CSolid::eParser::solid_2point ); });
    mp.append("SOLID 3 point parser", [&](const std::string &title)
              {
        mp.check(0,false);
        mp.check(1,true);
        mp.check(2,false);
        dxfv::CSolid::parser( dxfv::CSolid::eParser::solid_3point ); });
    mp.append("SOLID 4 point parser", [&](const std::string &title)
              {
        mp.check(0,false);
        mp.check(1,false);
        mp.check(2,true);
        dxfv::CSolid::parser( dxfv::CSolid::eParser::solid_4point ); });
    mp.check(0);
    mb.append("Parsers", mp);

    // handle left mouse button down
    fm.events().click([&]
                      {
        auto p = fm.getMouseStatus();
        old_pos.x = p.x;
        old_pos.y = p.y; });

    // handle mouse movement
    fm.events().mouseMove([&](wex::sMouse &m)
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
        fm.update(); });

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
        fm.update(); });

    fm.events().resize([&](int w, int h)
                       { fm.update(); });

    fm.show();

    fm.run();
}

#endif // UNIT_TEST

namespace dxfv
{

    void CLine::Draw(CDxf *dxf)
    {
        cDrawPrimitiveData draw(dxf);
        getDraw(draw);
        theShaper->penThick(draw.thick);
        theShaper->color(draw.color);
        theShaper->line(
            {(int)draw.x1, (int)draw.y1, (int)draw.x2, (int)draw.y2});
    }
    void CArc::Draw(CDxf *dxf)
    {
        cDrawPrimitiveData draw(dxf);
        getDraw(draw);
        theShaper->color(draw.color);
        theShaper->penThick(draw.thick);
        theShaper->arc(
            (int)draw.x1, (int)draw.y1, draw.r,
            draw.sa, draw.ea);
    }
    void CCircle::Draw(CDxf *dxf)
    {
        cDrawPrimitiveData draw(dxf);
        getDraw(draw);
        theShaper->fill(false);
        theShaper->penThick(draw.thick);
        theShaper->color(draw.color);
        theShaper->circle(
            draw.x1, draw.y1, draw.r);
    }
    void cLWPolyLine::Draw(CDxf *dxf)
    {
        cDrawPrimitiveData draw(dxf);

        // loop over drawing primitives
        while (getDraw(draw))
        {
            theShaper->penThick(draw.thick);
            theShaper->color(draw.color);
            theShaper->line(
                {(int)draw.x1, (int)draw.y1,
                 (int)draw.x2, (int)draw.y2});
        }
    }
    void CText::Draw(CDxf *dxf)
    {
        cDrawPrimitiveData draw(dxf);

        theShaper->color(draw.color);

        // loop over drawing primitives
        while (getDraw(draw))
        {
            theShaper->text(draw.text,
                            {(int)draw.x1, (int)draw.y1, (int)draw.x1 + 100, (int)draw.y1 + 25});
        }
    }
    void CSpline::Draw(CDxf *dxf)
    {
        cDrawPrimitiveData draw(dxf);
        std::vector<std::pair<long, long>> vpp;

        // loop over drawing primitives
        while (getDraw(draw))
        {
            vpp.emplace_back(
                (long)draw.x1,
                (long)draw.y1);
        }
        theShaper->color(draw.color);
        theShaper->penThick(draw.thick);
        theShaper->polyLine(
            (POINT *)vpp.data(),
            vpp.size());
    }
    void CSolid::Draw(CDxf *dxf)
    {
        cDrawPrimitiveData draw(dxf);

        // loop over drawing primitives
        while (getDraw(draw))
        {
            // std::cout << "CSolid::Draw " << draw.color << "\n";
            // std::cout << draw.x1 << " " << draw.y1 << " "
            //           << draw.x2 << " " << draw.y2 << " "
            //           << draw.x3 << " " << draw.y3 << "\n";

            theShaper->color(draw.color);
            theShaper->fill();
            theShaper->polygon(
                {
                    (int)draw.x1,
                    (int)draw.y1,
                    (int)draw.x2,
                    (int)draw.y2,
                    (int)draw.x3,
                    (int)draw.y3,
                });
        }
    }
}

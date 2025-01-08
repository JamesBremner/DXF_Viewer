// Dxf.cpp: implementation of the CDxf class.
//
//////////////////////////////////////////////////////////////////////

#include "Dxf.h"

#ifdef TIMING_PROFILE
#include "cRunWatch.h"
#endif


namespace dxfv
{

    cDXFGraphObject *cDXFGraphObject::Factory(cCodeValue &cv)
    {
        if (cv.Code())
            throw std::runtime_error("cDXFGraphObject::Factory bad code");
        // std::cout << cv.myValue << "\n";
        cDXFGraphObject *po = (cDXFGraphObject *)new CLine(cv);
        if (po->myfValid)
            return po;
        delete po;
        po = (cDXFGraphObject *)new cLWPolyLine(cv);
        if (po->myfValid)
            return po;
        delete po;
        po = (cDXFGraphObject *)new cPolyLine(cv);
        if (po->myfValid)
            return po;
        delete po;
        po = (cDXFGraphObject *)new CSpline(cv);
        if (po->myfValid)
            return po;
        delete po;
        po = (cDXFGraphObject *)new CCircle(cv);
        if (po->myfValid)
            return po;
        delete po;
        po = (cDXFGraphObject *)new CArc(cv);
        if (po->myfValid)
            return po;
        delete po;
        po = (cDXFGraphObject *)new CText(cv);
        if (po->myfValid)
            return po;
        delete po;
        po = (cDXFGraphObject *)new CSolid(cv);
        if (po->myfValid)
            return po;
        delete po;
        return nullptr;
    }

    void cDXFGraphObject::AutocadColor2RGB(int ai)
    {
        // http://gohtx.com/acadcolors.php
        static const std::vector<int> colorMap{
            16777215, 255, 65535, 65280, 16776960, 16711680, 16711935, 16777215, 8421504, 12632256, 255, 8355839, 165,
            5395109, 127, 4145023, 76, 2500172, 38, 1250086, 16383, 8364031, 10661, 5400485, 8063, 4149119, 4940,
            2502476, 2342, 1252390, 32767, 8372223, 21157, 5405861, 16255, 4153215, 9804, 2505036, 4902, 1252390, 49151,
            8380415, 31909, 5411237, 24447, 4157311, 14668, 2507340, 7206, 1253670, 65535, 8388607, 42405, 5416357, 32639,
            4161407, 19532, 2509900, 9766, 1254950, 65471, 8388575, 42364, 5416337, 32607, 4161391, 19513, 2509890, 9756,
            1254945, 65407, 8388543, 42322, 5416316, 32575, 4161375, 19494, 2509881, 9747, 1254940, 65343, 8388511, 42281,
            5416295, 32543, 4161359, 19475, 2509871, 9737, 1254935, 65280, 8388477, 42240, 5416274, 32512, 4161343,
            19456, 2509862, 9728, 1254931, 4194048, 10485631, 2729216, 6792530, 2064128, 5209919, 1264640, 3099686, 599552,
            1529875, 8388352, 12582783, 5416192, 8168786, 4161280, 6258495, 2509824, 3755046, 1254912, 1857555, 12582656,
            14679935, 8168704, 9545042, 6258432, 7307071, 3755008, 4344870, 1844736, 5789715, 16776960, 16777087, 10855680,
            10855762, 8355584, 8355647, 5000192, 5000230, 2500096, 5789715, 16760576, 16768895, 10845184, 10850642,
            8347392, 14249791, 4995328, 8274470, 2497536, 5789715, 16744192, 16760703, 10834432, 10845266, 8339200,
            8347455, 4990464, 8272166, 2495232, 5774355, 16727808, 16752511, 10823936, 10839890, 8331008, 8343359,
            4985600, 8269606, 2492672, 5773075, 16711680, 16744319, 10813440, 10834514, 8323072, 8339263, 4980736,
            8267302, 2490368, 5772051, 16711743, 16744351, 10813481, 10834535, 8323103, 8339279, 4980755, 8267311,
            2490377, 5772055, 16711807, 16744383, 5374117, 10834556, 8323135, 8339295, 4980774, 8267321, 2490387,
            5772060, 16711871, 16744415, 10813564, 10834574, 8323167, 8339311, 4980793, 4990530, 2490396, 5772120,
            16711935, 16744447, 10813605, 10834597, 8323199, 8339327, 4980812, 4990540, 2490406, 5772120, 12517631,
            14647295, 8126629, 9523877, 6226047, 7290751, 3735628, 4335180, 1835046, 5772120, 8323327, 12550143, 5374117,
            8147621, 4128895, 6242175, 2490444, 3745356, 1245222, 1839960, 4129023, 10452991, 2687141, 6771365, 2031743,
            5193599, 1245260, 3089996, 589862, 1512280, 0, 6645093, 6710886, 10066329, 13421772, 16777215};

        myColor = colorMap[ai];
    }

    void cBoundingRectangle::CalcScale(int w, int h)
    {
        myWindowHeight = h;

        double dx = x2 - x1;
        double dy = y2 - y1;

        double sx = dx / w;
        double sy = dy / h;

        sx *= myZoom;
        sy *= myZoom;

        if (sx > sy)
        {
            myScale = sx;
            x_offset = int(-x1 / myScale);
            y_offset = int((h / 2) + (dy / (2 * myScale)) - (y2 / myScale));
        }
        else
        {
            myScale = sy;
            x_offset = int((w / 2) - (dx / (2 * myScale)) - (x1 / myScale));
            y_offset = int(-y1 / myScale);
        }

        //    std::cout << "CalcScale " << myScale
        //        <<" "<< x1<<" "<<x2
        //        <<" "<<w<<" "<<h<< "\n";
    }

    void cBoundingRectangle::ApplyScale(double &x, double &y)
    {
        // std::cout << "ApplyScale " << myScale <<" "<< x <<" " << y << " -> ";

        x = (x / myScale) + x_offset;
        y = myWindowHeight - ((y / myScale) + y_offset);

        x += xpan;
        y += ypan;

        // std::cout << x <<" " << y << "\n";
        //
        //    double xt = x;
        //    double yt = y;
        //    RemoveScale( xt, yt );
        //    std::cout <<"test " << xt <<" " << yt << "\n";
    }
    void cBoundingRectangle::RemoveScale(double &x, double &y)
    {
        // std::cout << "RemoveScale " << myScale <<" "<< x <<" " << y << " -> ";
        x -= xpan;
        y -= ypan;
        x = (x - x_offset) * myScale;
        y = myWindowHeight - y;
        y = (y - y_offset) * myScale;
        //     std::cout << x <<" " << y << "\n";
        //     double xt = x;
        //     double yt = y;
        //     ApplyScale( xt, yt );
        //     std::cout << "test " << xt <<" "<< yt << "\n";
    }

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    CDxf::CDxf()
        : myfwxwidgets(false) // do not use wxwidgets for contol point splines
          ,
          myfSplineControlPointsPreferred(false) // if true and choice available, splines prefer control points
          ,
          myFileVersion("n/a"),
          myPenThickOverride( 0 )
    {
        CSolid::parser(CSolid::eParser::solid_2point);
    }

    CDxf::~CDxf()
    {
    }
    /**  Read DXF file

    @param[in] str path and filename

    */
    void CDxf::LoadFile(const std::string &filepath)
    {
#ifdef TIMING_PROFILE
        raven::set::cRunWatch aWatcher("CDxf::LoadFile");
#endif

        Init();

        myCodeValue.clear();
        std::ifstream f(filepath);
        if (!f.is_open())
        {
            throw std::runtime_error(
                "Cannot open " + filepath);
        }
        //std::cout << filepath << "  ";

        std::istream ss(f.rdbuf());

        bool entities = false;
        bool version = false;
        cCodeValue cv;
        while (cv.Read(ss))
        {

            // ignore everything until the ENTITIES section is reached
            if (!entities)
            {
                if (cv.myValue == "ENTITIES") {
                    entities = true;
                    continue;
                }
                if( version ) {
                    {
                        std::cout << " version " << cv.myCode << "\n";
                        myFileVersion = cv.myCode;
                        version = false;
                    }
                    continue;
                }
                if (cv.myCode == "$ACADVER")
                {
                    version = true;
                }
                continue;
            }
            // std::cout << cv.myCode <<" " << cv.myValue << "\n";
            myCodeValue.push_back(cv);
        }
        if (!entities)
            throw std::runtime_error("DXF file has no ENTITIES section");

        for (
            cvit_t cvit = myCodeValue.begin();
            cvit != myCodeValue.end();
            cvit++)
        {
            if (cvit->Code() != 0)
                continue;

            // std::cout << cvit->myCode << ", "<< cvit->myValue << "\n";

            cDXFGraphObject *po = cDXFGraphObject::Factory(*cvit);
            if (po)
            {
                po->Options(this);
                po->Append(cvit);
                myGraphObject.push_back(po);
            }
        }
        UpdateBoundingRectangle();
    }

    void CDxf::UpdateBoundingRectangle()
    {
        myBoundingRectangle.init = false;
        for (auto po : myGraphObject)
            po->Update(myBoundingRectangle);

        // if( myBoundingRectangle.y1 < 0 )
        // {
        //     // entities with negative y co-ords present
        //     // zoom code doesn't handle -ve y
        //     // so adjust co-ords so that all y's are +ve

        //     for( auto po : myGraphObject )
        //     {
        //         po->Adjust( 0, -myBoundingRectangle.y1 );
        //     }

        //     // recalculate bounding rectangle
        //     myBoundingRectangle.init = false;
        //     for( auto po : myGraphObject )
        //     {
        //         po->Update( myBoundingRectangle );
        //     }
        // }
    }

    void CDxf::Init()
    {
        myGraphObject.clear();
        myFileVersion = "n/a";
    }

    void CDxf::Draw(int width, int height)
    {
        if( ! myGraphObject.size() )
            return;

#ifdef TIMING_PROFILE
        raven::set::cRunWatch aWatcher(" CDxf::Draw");
#endif

        // scale to window
        myBoundingRectangle.CalcScale(
            width,
            height);

        for (auto po : myGraphObject)
            po->Draw(this);
    }

    cDrawPrimitiveData::cDrawPrimitiveData(CDxf *dxf)
    {
        index = 0;
        index_curve = 0;
        rect = &(dxf->myBoundingRectangle);
    }

}

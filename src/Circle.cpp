// Circle.cpp: implementation of the CCircle class.
//
//////////////////////////////////////////////////////////////////////

#include "Dxf.h"
#include "Circle.h"

namespace dxfv
{

    CCircle::CCircle(cCodeValue &cv)
        : cDXFGraphObject()
    {
        myfValid = (cv.myValue == "CIRCLE");
        myType = cDXFGraphObject::eType::circle;
    }

    CCircle::~CCircle()
    {
    }

    void CCircle::Options(CDxf *dxf)
    {
        myPenThickOverride = dxf->penThickOverride();
        if (myPenThickOverride)
            myThick = myPenThickOverride;
    }

    void CCircle::Update(cBoundingRectangle &rect)
    {
        rect.Update(x, y + r);
        rect.Update(x, y - r);
        rect.Update(x + r, y);
        rect.Update(x - r, y);
    }
    bool CCircle::Append(cvit_t &cvit)
    {
        while (true)
        {
            cvit++;
            switch (cvit->Code())
            {
            case 0:
                // a new object
                cvit--;
                return false;
            case 8:
                myLayer = cvit->myValue;
                break;
            case 10:
                x = atof(cvit->myValue.c_str());
                break;
            case 20:
                y = atof(cvit->myValue.c_str());
                break;
            case 39:
                if( ! myPenThickOverride )
                    myThick = atoi(cvit->myValue.c_str());
                break;
            case 40:
                r = atof(cvit->myValue.c_str());
                break;
            case 62:
                AutocadColor2RGB(atof(cvit->myValue.c_str()));
                break;
            }
        }
        return true;
    }

    bool CCircle::getDraw(cDrawPrimitiveData &draw)
    {
        if (draw.index)
            return false;
        draw.index++;
        draw.x1 = x;
        draw.y1 = y;
        draw.r = r;
        draw.rect->ApplyScale(draw.x1, draw.y1);
        draw.r /= draw.rect->myScale;
        draw.color = myColor;
        draw.thick = myThick;
        return true;
    }
    void CCircle::Adjust(double ax, double ay)
    {
        x += ax;
        y += ay;
    }
}

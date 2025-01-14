// Line.cpp: implementation of the CLine class.
//
//////////////////////////////////////////////////////////////////////

#include "Dxf.h"
#include "Line.h"

namespace dxfv
{

    CLine::CLine(cCodeValue &cv)
        : cDXFGraphObject()
    {
        myfValid = (cv.myValue == "LINE");
        myType = cDXFGraphObject::eType::line;
    }

    CLine::~CLine()
    {
    }

    void CLine::Options(CDxf *dxf)
    {
        myPenThickOverride = dxf->penThickOverride();
        if( myPenThickOverride )
            myThick = myPenThickOverride;
    }

    bool CLine::Append(cvit_t &cvit)
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
                x1 = atof(cvit->myValue.c_str());
                break;
            case 20:
                y1 = atof(cvit->myValue.c_str());
                break;
            case 11:
                x2 = atof(cvit->myValue.c_str());
                break;
            case 21:
                y2 = atof(cvit->myValue.c_str());
                break;
            case 39:
                if( ! myPenThickOverride )
                    myThick = atoi(cvit->myValue.c_str());
                break;
            case 62:
                AutocadColor2RGB(atoi(cvit->myValue.c_str()));
                break;
            }
        }
        throw std::runtime_error("DXF file incorrectly terminated");
    }

    bool CLine::getDraw(cDrawPrimitiveData &draw)
    {
        if (draw.index)
            return false;
        draw.x1 = x1;
        draw.y1 = y1;
        draw.x2 = x2;
        draw.y2 = y2;
        draw.rect->ApplyScale(draw.x1, draw.y1);
        draw.rect->ApplyScale(draw.x2, draw.y2);
        draw.color = myColor;
        draw.thick = myThick;
        draw.index++;
        return true;
    }

    void CLine::Adjust(double ax, double ay)
    {
        x1 += ax;
        y1 += ay;
        x2 += ax;
        y2 += ay;
    }
}

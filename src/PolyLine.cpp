// PolyLine.cpp: implementation of the cLWPolyLine class.
//
//////////////////////////////////////////////////////////////////////

#include "Dxf.h"
#include "PolyLine.h"

namespace dxfv
{

    cLWPolyLine::cLWPolyLine(cCodeValue &cv)
        : cDXFGraphObject()
    {
        myfValid = (cv.myValue == "LWPOLYLINE");
        myType = cDXFGraphObject::eType::lwpolyline;
        m_VertexCount = 0;
        myfClosed = false;
    }

    cLWPolyLine::~cLWPolyLine()
    {
    }

    cPolyLine::cPolyLine(cCodeValue &cv)
        : cLWPolyLine(cv)
    {
        myfValid = (cv.myValue == "POLYLINE");
        myType = cDXFGraphObject::eType::polyline;
    }

    cPolyLine::~cPolyLine()
    {
    }

    void cLWPolyLine::Options(CDxf *dxf)
    {
        myThick = 1;
        myPenThickOverride = dxf->penThickOverride();
        if (myPenThickOverride)
            myThick = myPenThickOverride;
    }

    bool cLWPolyLine::Append(cvit_t &cvit)
    {
        int point_index = 0;
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
                myLayer = cvit->myCode;
                break;
            case 90:
                m_VertexCount = atoi(cvit->myValue.c_str());
                break;
            case 70:
                myfClosed = atoi(cvit->myValue.c_str());
                break;
            case 10:
                x[point_index] = atof(cvit->myValue.c_str());
                break;
            case 20:
                y[point_index++] = atof(cvit->myValue.c_str());
                if (point_index >= MAXPOINTS)
                    throw std::runtime_error("Too many LWPOLYLINE points");
                break;
            case 39:
                if (!myPenThickOverride)
                    myThick = atoi(cvit->myValue.c_str());
                break;
            case 62:
                AutocadColor2RGB(atoi(cvit->myValue.c_str()));
                break;
            }
        }
        throw std::runtime_error("DXF file incorrectly terminated");
    }

    bool cPolyLine::Append(cvit_t &cvit)
    {
        // https://help.autodesk.com/view/OARX/2023/ENU/?guid=GUID-748FC305-F3F2-4F74-825A-61F04D757A50

        int point_index = 0;
        bool fVertex = false;
        while (true)
        {
            cvit++;
            switch (cvit->Code())
            {
            case 0:
                // a new object
                if (cvit->myValue == "VERTEX")
                {
                    fVertex = true;
                    break;
                }
                m_VertexCount = point_index;
                cvit--;
                return false;
            case 8:
                myLayer = cvit->myCode;
                break;
            case 10:
                if (fVertex)
                    x[point_index] = atof(cvit->myValue.c_str());
                break;
            case 20:
                if (fVertex)
                    y[point_index++] = atof(cvit->myValue.c_str());
                if (point_index >= MAXPOINTS)
                    throw std::runtime_error("Too many POLYLINE points");
                break;
            case 39:
                myThick = atoi(cvit->myValue.c_str());
                break;
            case 70:
                myfClosed = atoi(cvit->myValue.c_str());
                break;
            }
        }

        throw std::runtime_error("DXF file incorrectly terminated");
    }

    void cLWPolyLine::Update(cBoundingRectangle &rect)
    {
        for (int k = 0; k < (int)m_VertexCount; k++)
        {
            rect.Update(x[k], y[k]);
        }
    }

    bool cLWPolyLine::getDraw(cDrawPrimitiveData &draw)
    {
        if (0 > draw.index || draw.index == (int)m_VertexCount)
            return false;
        if (draw.index == (int)m_VertexCount)
            return false;
        if (draw.index == (int)m_VertexCount - 1)
        {
            if (!myfClosed)
            {
                return false;
            }
            else
            {
                draw.x1 = x[draw.index];
                draw.y1 = y[draw.index];
                draw.x2 = x[0];
                draw.y2 = y[0];
                draw.index++;
            }
        }
        else
        {
            draw.x1 = x[draw.index];
            draw.y1 = y[draw.index];
            draw.index++;
            draw.x2 = x[draw.index];
            draw.y2 = y[draw.index];
        }
        draw.rect->ApplyScale(draw.x1, draw.y1);
        draw.rect->ApplyScale(draw.x2, draw.y2);
        draw.color = myColor;
        draw.thick = myThick;

        return true;
    }
    void cLWPolyLine::Adjust(double ax, double ay)
    {
        for (int k = 0; k < (int)m_VertexCount; k++)
        {
            x[k] += ax;
            y[k] += ay;
        }
    }
}

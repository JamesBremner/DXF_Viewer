// Dxf.cpp: implementation of the CDxf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "DXFNester.h"
#include "Dxf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace dxfv
{

cDXFGraphObject* cDXFGraphObject::Factory( cCodeValue& cv )
{
    if( cv.Code() )
        throw std::runtime_error( "cDXFGraphObject::Factory bad code");
    cDXFGraphObject* po = (cDXFGraphObject*)new CLine( cv );
    if( po->myfValid )
        return po;
    delete po;
    po = (cDXFGraphObject*)new CPolyLine( cv );
    if( po->myfValid )
        return po;
    delete po;
    po = (cDXFGraphObject*)new CSpline( cv );
    if( po->myfValid )
        return po;
    delete po;
        po = (cDXFGraphObject*)new CCircle( cv );
    if( po->myfValid )
        return po;
    delete po;
        po = (cDXFGraphObject*)new CArc( cv );
    if( po->myfValid )
        return po;
    delete po;
        po = (cDXFGraphObject*)new CText( cv );
    if( po->myfValid )
        return po;
    delete po;
    return nullptr;
}

void cBoundingRectangle::CalcScale( int w, int h )
{
    myWindowHeight = h;

    double dx = x2 - x1;
    double dy = y2 - y1;

    double sx = dx / w;
    double sy = dy / h;

    sx *= myZoom;
    sy *= myZoom;

    if(sx > sy)
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

void cBoundingRectangle::ApplyScale( double& x, double& y )
{
    //std::cout << "ApplyScale " << myScale <<" "<< x <<" " << y << " -> ";

    x = (x / myScale ) + x_offset;
    y = myWindowHeight - (( y / myScale ) + y_offset );

    x += xpan;
    y += ypan;

    //std::cout << x <<" " << y << "\n";
//
//    double xt = x;
//    double yt = y;
//    RemoveScale( xt, yt );
//    std::cout <<"test " << xt <<" " << yt << "\n";
}
void cBoundingRectangle::RemoveScale( double& x, double& y )
{
    //std::cout << "RemoveScale " << myScale <<" "<< x <<" " << y << " -> ";
    x -= xpan;
    y -= ypan;
    x = ( x - x_offset ) * myScale;
    y = myWindowHeight - y;
    y = ( y - y_offset ) * myScale;
    // std::cout << x <<" " << y << "\n";
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDxf::CDxf()
    : m_InitialDraw( false )
    , myLoadStatus( none )
    , m_Nesting( false )
    , myfwxwidgets( true )
{
}

CDxf::~CDxf()
{

}
/**  Read DXF file

@param[in] str path and filename

*/
void CDxf::LoadFile(const std::string& filepath)
{
    Init();

    myCodeValue.clear();
    std::ifstream f( filepath );
    if( ! f.is_open() )
    {
        myLoadStatus = none;
        return;
    }

    bool entities = false;
    cCodeValue cv;
    while( cv.Read( f ) )
    {
        // ignore everything until the ENTITIES section is reached
        if( ! entities )
        {
            if( cv.myValue == "ENTITIES")
                entities = true;
            continue;
        }
        //std::cout << cv.myCode <<" " << cv.myValue << "\n";
        myCodeValue.push_back( cv );
    }
    if( ! entities )
        throw std::runtime_error("DXF file has no ENTITIES section");

    for(
        cvit_t cvit = myCodeValue.begin();
        cvit != myCodeValue.end();
        cvit++ )
    {
        if( cvit->Code() != 0 )
            continue;

        //std::cout << cvit->myValue << "\n";

        cDXFGraphObject * po = cDXFGraphObject::Factory( *cvit );
        if( po )
        {
            po->Append( cvit );
            myGraphObject.push_back( po );
        }

    }
    UpdateBoundingRectangle();
    myLoadStatus = OK;

}

void CDxf::UpdateBoundingRectangle()
{
    myBoundingRectangle.init = false;
    for( auto po : myGraphObject )
        po->Update( myBoundingRectangle );
}

//UINT CDxf::GetLineCount()
//{
//#ifdef DEMO
//    if( m_Line.size() > 1 )
//    {
//        printf("Demo Version limit - sorry\n");
//        exit(1);
//    }
//#endif // DEMO
//    return m_Line.size();
//}
//
//UINT CDxf::GetCircleCount()
//{
//    return m_Circle.size();
//}
//
//UINT CDxf::GetArcCount()
//{
//    return m_Arc.size();
//}
//
//UINT CDxf::GetLwPolyLineCount()
//{
//#ifdef DEMO
//    if( m_PolyLine.size() > 1 )
//    {
//        printf("Demo Version limit - sorry\n");
//        exit(1);
//    }
//#endif // DEMO
//    return m_PolyLine.size();
//}
//
//UINT CDxf::GetSplineCount()
//{
//    return m_Spline.size();
//}

void CDxf::Init()
{
    myLoadStatus = none;
    m_InitialDraw = FALSE;
    myGraphObject.clear();
    m_Nesting = FALSE;
}

void CDxf::Init( s_dxf_draw& draw )
{
    draw.index = 0;
    draw.index_curve = 0;
    draw.rect = &myBoundingRectangle;
}
}

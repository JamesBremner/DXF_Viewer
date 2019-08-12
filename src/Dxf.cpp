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

/**  Read next two lines from a DXF file - a group code and a value

@param[in]  fp     the open DXF file
@param[out]  iCode  the group code, as an integer
@param[out]  lpValue  a 255 character array to hold the value string ( may contain spaces )

*/
void cDXFGraphObject::ReadTwoLines( FILE * fp, int& iCode, char* lpValue )
{
    // read next two lines in their entirety
    char lpCode[256];
    fgets(lpCode,255,fp);
    fgets(lpValue,255,fp);

    // delete the newlines
    lpCode[strlen(lpCode)-1] = '\0';
    lpValue[strlen(lpValue)-1] = '\0';

    // remove leading whitespace from the group code
    string sCode( lpCode );
    size_t p = sCode.find_first_not_of(" ");
    sCode.erase(0,p-1);

    // decode the group code
    iCode = atoi( lpCode );
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
        vector<cCodeValue>::iterator cvit = myCodeValue.begin();
        cvit != myCodeValue.end();
        cvit++ )
    {
        if( cvit->Code() != 0 )
            continue;

        //std::cout << cvit->myValue << "\n";

        CLine line( *cvit );
        if( line.myfValid )
        {
            line.Append( cvit );
            m_Line.push_back( line );
            continue;
        }

        CPolyLine PolyLine( *cvit );
        if( PolyLine.myfValid )
        {
            PolyLine.Append( cvit );
            m_PolyLine.push_back( PolyLine );
            continue;
        }

        CSpline spline( *cvit );
        if( spline.myfValid )
        {
            spline.Append( cvit );
            m_Spline.push_back( spline );
            continue;
        }

        CCircle circle( *cvit );
        if( circle.myfValid )
        {
            circle.Append( cvit );
            m_Circle.push_back( circle );
            continue;
        }

        CArc arc( *cvit );
        if( arc.myfValid )
        {
            arc.Append( cvit );
            m_Arc.push_back( arc );
            continue;
        }

        CText text( *cvit );
        if( text.myfValid )
        {
            text.Append( cvit );
            myText.push_back( text );
            continue;
        }
    }
    UpdateBoundingRectangle();
    myLoadStatus = OK;

}

void CDxf::UpdateBoundingRectangle()
{
#ifdef __GNUWIN32__
    myBoundingRectangle.init = false;
    for( CLine& line : m_Line )
    {
        line.Update( myBoundingRectangle );
    }
    for( CCircle& circle : m_Circle )
    {
        circle.Update( myBoundingRectangle );
    }
    for( CArc& arc : m_Arc )
    {
        arc.Update( myBoundingRectangle );
    }
    for( CPolyLine& polyline : m_PolyLine)
    {
        polyline.Update( myBoundingRectangle );
    }
    for( CSpline& spline: m_Spline )
    {
        spline.Update( myBoundingRectangle );
    }
#endif
}

UINT CDxf::GetLineCount()
{
#ifdef DEMO
    if( m_Line.size() > 1 )
    {
        printf("Demo Version limit - sorry\n");
        exit(1);
    }
#endif // DEMO
    return m_Line.size();
}

UINT CDxf::GetCircleCount()
{
    return m_Circle.size();
}

UINT CDxf::GetArcCount()
{
    return m_Arc.size();
}

UINT CDxf::GetLwPolyLineCount()
{
#ifdef DEMO
    if( m_PolyLine.size() > 1 )
    {
        printf("Demo Version limit - sorry\n");
        exit(1);
    }
#endif // DEMO
    return m_PolyLine.size();
}

UINT CDxf::GetSplineCount()
{
    return m_Spline.size();
}

void CDxf::Init()
{
    myLoadStatus = none;
    m_InitialDraw = FALSE;
    m_Line.clear();
    m_Arc.clear();
    m_Circle.clear();
    m_PolyLine.clear();
    m_Spline.clear();
    myText.clear();
    myDimension.clear();
    m_Nesting = FALSE;
}

void CDxf::Init( s_dxf_draw& draw )
{
    draw.index = 0;
    draw.index_curve = 0;
    draw.rect = &myBoundingRectangle;
}
}

// Spline.cpp: implementation of the CSpline class.
//
//////////////////////////////////////////////////////////////////////

#include <vector>
#include <cmath>
#include "dxf.h"
#include "Spline.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dxfv
{

CSpline::CSpline()
    : cDXFGraphObject("SPLINE",cDXFGraphObject::eType::spline)
    , m_Layer("0")
    , m_FitPointCount( 0 )
    , m_ControlPointCount( 0 )
    , m_KnotCount( 0 )
    , m_Select( false )
    , m_Nest( false )
    , myfwxwidgets( false )
{
}

CSpline::CSpline( cCodeValue& cv )
    : CSpline()
{
    myfValid =( cv.myValue == myCode );
}

CSpline::~CSpline()
{

}

bool CSpline::Append(  cvit_t& cvit )
{
    // https://help.autodesk.com/view/OARX/2023/ENU/?guid=GUID-E1F884F8-AA90-4864-A215-3182D47A9C74
    
    int point_index = 0;
    while( true )
    {
        cvit++;
        //std::cout << "spline " << cvit->myCode <<", " << cvit->myValue << "\n";
        switch( cvit->Code() )
        {
        case 0:
            // a new object
            //std::cout << "spline points " << m_FitPointCount  <<" "<< m_ControlPointCount<<" "<< point_index << "\n";
            if( m_FitPointCount == 0 && m_ControlPointCount == 0 )
                throw std::runtime_error("Spline has no points");
            if( point_index != m_FitPointCount && point_index != m_ControlPointCount )
                throw std::runtime_error("Spline has unexpected number of points");
            Generate();
            cvit--;
            return false;

        case 8:
            m_Layer = cvit->myValue;
            break;

        case 74:
            m_FitPointCount = atoi(cvit->myValue.c_str());
            if( ! m_FitPointCount )
                break;
            if( m_FitPointCount >= MAXPOINTS )
                throw std::runtime_error("Too many spline points");
            if( m_ControlPointCount )
            {
                // both control and fit points
                if( myfControlPointsPreferred ) {
                    m_FitPointCount = 0;
                }
                else
                {
                    m_ControlPointCount = 0;
                }
            }
            break;

        case 11:
            if(m_FitPointCount )
                x[point_index] = atof(cvit->myValue.c_str());
            break;
        case 21:
            if( m_FitPointCount )
                y[point_index++] = atof(cvit->myValue.c_str());
            break;


        case 73:
            m_ControlPointCount = atoi(cvit->myValue.c_str());
            if( ! m_ControlPointCount)
                break;
            if( m_ControlPointCount >= MAXPOINTS )
                throw std::runtime_error("Too many spline points");
            if( m_FitPointCount )
            {
                // both control and fit points
                if( myfControlPointsPreferred ) {
                    m_FitPointCount = 0;
                }
                else
                {
                    m_ControlPointCount = 0;
                }
            }
            break;
        case 10:
            if( m_ControlPointCount )
                x[point_index] = atof(cvit->myValue.c_str());
            break;
        case 20:
            if( m_ControlPointCount )
                y[point_index++] = atof(cvit->myValue.c_str());
            break;

        case 72:
            m_KnotCount =  atoi(cvit->myValue.c_str());
            if( m_KnotCount )
            {
                throw std::runtime_error(
                    "B-Spline not supported"                );
            }
            break;
            
        }
    }
    return true;
}

void CSpline::Options( CDxf * dxf )
{
    myfwxwidgets = dxf->wxwidgets();
    myfControlPointsPreferred = dxf->SplineControlPointsPreferred();
}

void CSpline::Update( cBoundingRectangle& rect )
{
    int count = m_FitPointCount;
    if( ! count )
        count = m_ControlPointCount;
    for( int k = 0; k < count; k++ )
    {
        rect.Update( x[k], y[k] );
    }
}
void CSpline::Generate()
{
    if( ! m_FitPointCount )
        return;

    float AMag, AMagOld;
    std::vector<float> k;
    std::vector< std::vector<float> > Mat( 3, std::vector<float>(m_FitPointCount) );

    // vector A
    for(int i= 0 ; i<=(int)m_FitPointCount-2 ; i++ )
    {
        Ax.push_back( x[i+1] - x[i] );
        Ay.push_back( y[i+1] - y[i] );
    }
    // k
    AMagOld = (float)sqrt(Ax[0]*Ax[0] + Ay[0]*Ay[0]);
    for( int i=0 ; i<=(int)m_FitPointCount-3 ; i++)
    {
        AMag = (float)sqrt(Ax[i+1]*Ax[i+1] + Ay[i+1]*Ay[i+1]);
        k.push_back( AMagOld / AMag );
        AMagOld = AMag;
    }
    k.push_back( 1.0f );

    // Matrix
    for( int i=1; i<=(int)m_FitPointCount-2; i++)
    {
        Mat[0][i] = 1.0f;
        Mat[1][i] = 2.0f*k[i-1]*(1.0f + k[i-1]);
        Mat[2][i] = k[i-1]*k[i-1]*k[i];
    }
    Mat[1][0] = 2.0f;
    Mat[2][0] = k[0];
    Mat[0][m_FitPointCount-1] = 1.0f;
    Mat[1][m_FitPointCount-1] = 2.0f*k[m_FitPointCount-2];

    //
    Bx.push_back( 3.0f*Ax[0] );
    By.push_back( 3.0f*Ay[0] );
    for(int i=1; i<=(int)m_FitPointCount-2; i++)
    {
        Bx.push_back( 3.0f*(Ax[i-1] + k[i-1]*k[i-1]*Ax[i]) );
        By.push_back( 3.0f*(Ay[i-1] + k[i-1]*k[i-1]*Ay[i]) );
    }
    Bx.push_back( 3.0f*Ax[m_FitPointCount-2] );
    By.push_back( 3.0f*Ay[m_FitPointCount-2] );

    //
    MatrixSolve(Bx, Mat );
    MatrixSolve(By, Mat );

    for( int i=0 ; i<=(int)m_FitPointCount-2 ; i++ )
    {
        Cx.push_back( k[i]*Bx[i+1] );
        Cy.push_back( k[i]*By[i+1] );
    }

}

void CSpline::MatrixSolve(
    std::vector<float>& B,
    std::vector< std::vector<float> >& Mat )
{
    float* Work = new float[m_FitPointCount];
    float* WorkB = new float[m_FitPointCount];
    for(int i=0; i<=m_FitPointCount-1; i++)
    {
        Work[i] = B[i] / Mat[1][i];
        WorkB[i] = Work[i];
    }

    for(int j=0 ; j<10 ; j++)
    {
        ///  need convergence judge
        Work[0] = (B[0] - Mat[2][0]*WorkB[1])/Mat[1][0];
        for(int i=1; i<m_FitPointCount-1 ; i++ )
        {
            Work[i] = (B[i]-Mat[0][i]*WorkB[i-1]-Mat[2][i]*WorkB[i+1])
                      /Mat[1][i];
        }
        Work[m_FitPointCount-1] = (B[m_FitPointCount-1] - Mat[0][m_FitPointCount-1]*WorkB[m_FitPointCount-2])/Mat[1][m_FitPointCount-1];

        for( int i=0 ; i<=m_FitPointCount-1 ; i++ )
        {
            WorkB[i] = Work[i];
        }
    }
    for(int i=0 ; i<=m_FitPointCount-1 ; i++ )
    {
        B[i] = Work[i];
    }
    delete[] Work;
    delete[] WorkB;
}

bool CSpline::getDraw( cDrawPrimitiveData& draw )
{
    if( ! m_FitPointCount)
    {
        return getDrawControlPoint( draw );
    }

    /* determine number of straight line segments to draw between each pait of fit points
    For good smooth looking curves ensure that no segment changes the x or y pixel by more than 2
    */

    int Ndiv = (int)(std::max(abs(x[draw.index]-x[draw.index+1]),abs(y[draw.index]-y[draw.index+1]))
                     / ( 2 * draw.rect->myScale ));

    if( draw.index_curve == 0 && draw.index == 0 )
    {
        //The first point in the spline
        draw.x1 = x[draw.index];
        draw.y1 = y[draw.index];
        draw.rect->ApplyScale(draw.x1,draw.y1);
    }
    else if( draw.index_curve == Ndiv )
    {
        // No more points in current curve
        if( draw.index == m_FitPointCount-2 )
        {
            // No more fittine points
            return false;
        }

        // Draw the line between the last point of the previous curve
        // and the next fitting point
        draw.index++;
        draw.index_curve = 0;
        draw.x1 = draw.x2;
        draw.y1 = draw.y2;
        draw.x2 = x[draw.index];
        draw.y2 = y[draw.index];
        draw.rect->ApplyScale(draw.x2,draw.y2);
        return true;

    }
    else
    {
        // In the middle of drawing the curve between two fitting points
        // Start from the end of the previous line
        draw.x1 = draw.x2;
        draw.y1 = draw.y2;
    }

    // Calulate the next point in the curve between two fitting points
    float  t,f,g,h;
    t = 1.0f / (float)Ndiv * (float)draw.index_curve;
    f = t*t*(3.0f-2.0f*t);
    g = t*(t-1.0f)*(t-1.0f);
    h = t*t*(t-1.0f);
    double x2 = (x[draw.index] + Ax[draw.index]*f + Bx[draw.index]*g + Cx[draw.index]*h);
    double y2 = (y[draw.index] + Ay[draw.index]*f + By[draw.index]*g + Cy[draw.index]*h);

    draw.rect->ApplyScale(x2,y2);
    draw.x2 = round(x2);
    draw.y2 = round(y2);

    //std::cout << t <<" "<< draw.x2 <<" "<< draw.y2 << "\n";

    draw.index_curve++;

    return true;
}

bool CSpline::getDrawControlPoint( cDrawPrimitiveData& draw )
{
    // check if using wxwidgets spline method
    if( myfwxwidgets )
    {
        // check if more points are available
        if( draw.index == m_ControlPointCount )
        {
            if( draw.index_curve )
                return false;
            draw.index_curve = 1;
            return true;
        }

        draw.x1 = x[draw.index];
        draw.y1 = y[draw.index];
        draw.rect->ApplyScale(draw.x1,draw.y1);
        draw.x2 = x[draw.index+1];
        draw.y2 = y[draw.index+1];
        draw.rect->ApplyScale(draw.x2,draw.y2);
        draw.index++;
        draw.index_curve = 0;
        return true;
    }

    int Ndiv = 100;

    // check if more points are available
    if( draw.index == Ndiv - 1 )
        return false;

    getBezierPoint( draw.x1, draw.y1, ((float)draw.index)/Ndiv );
    getBezierPoint( draw.x2, draw.y2, ((float)(draw.index+1))/Ndiv );
    draw.rect->ApplyScale(draw.x1,draw.y1);
    draw.rect->ApplyScale(draw.x2,draw.y2);
    draw.index++;
    draw.index_curve = 0;
    return true;

}


cP operator * ( double s, cP b)
{
    return cP(s * b.x, s * b.y);
}

void CSpline::getBezierPoint( double& ox, double& oy, double t )
{
    // https://stackoverflow.com/a/21642962/16582

    std::vector<cP> tmp;
    for( int k=0; k < m_ControlPointCount; k++ )
        tmp.push_back( cP( x[k], y[k] ) );
    int i = m_ControlPointCount - 1;
    while (i > 0)
    {
        for (int k = 0; k < i; k++)
            tmp[k] = tmp[k] + t * ( tmp[k+1] - tmp[k] );
        i--;
    }
    ox = tmp[0].x;
    oy = tmp[0].y;

    std::cout << "getBezierPoint "
        << ox <<" "<< oy << " | ";
}

void CSpline::Adjust( double ax, double ay )
{
    int count = m_FitPointCount;
    if( ! count )
        count = m_ControlPointCount;
    for( int k = 0; k < count; k++ )
    {
        x[k] += ax;
        y[k] += ay;
    }
}

}



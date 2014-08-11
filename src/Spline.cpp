// Spline.cpp: implementation of the CSpline class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
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

namespace dxfv {

CSpline::CSpline()
{
    m_FitPointCount = 0;
    m_Layer = "0";
    m_Select = FALSE;
    m_Nest = FALSE;
}

CSpline::~CSpline()
{

}
bool CSpline::Read( FILE * fp, int& code, char* lpValue )
{
    if( strcmp(lpValue,"SPLINE") != 0 )
    {
        // not a line
        return false;
    }
    int point_index = 0;
    while( fp != NULL )
    {
        ReadTwoLines(fp, code, lpValue );
        switch ( code )
        {

        case 0:
            // a new object
            Generate();
             if( ! m_FitPointCount )
                return false;
            return true;

        case 8:
            m_Layer = lpValue;
            break;
        case 74:
            m_FitPointCount = atoi(lpValue);
            break;
        case 11:
            x[point_index] = atof(lpValue);
            break;
        case 21:
            y[point_index++] = atof(lpValue);
            break;

        }
    }
    return true;
}
	void CSpline::Update( cBoundingRectangle& rect )
	{
	    for( int k = 0; k < m_FitPointCount; k++ )
        {
            rect.Update( x[k], y[k] );
        }
	}
void CSpline::Generate()
{
    if( ! m_FitPointCount )
        return;

    float AMag , AMagOld;
    vector<float> k;
    vector< vector<float> > Mat( 3, vector<float>(m_FitPointCount) );

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
    vector<float>& B,
    vector< vector<float> >& Mat )
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

bool CSpline::getDraw( s_dxf_draw& draw )
{
    //adjust this factor to adjust the curve smoothness
    // a smaller value will result in a smoother display
    const float DIV_FACTOR = 10.0;

    int Ndiv = (int)(max(abs((int)Ax[draw.index]),abs((int)Ay[draw.index]))/DIV_FACTOR);
    if( ! Ndiv )
        return false;

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
    draw.x2 = (int)(x[draw.index] + Ax[draw.index]*f + Bx[draw.index]*g + Cx[draw.index]*h);
    draw.y2 = (int)(y[draw.index] + Ay[draw.index]*f + By[draw.index]*g + Cy[draw.index]*h);

    draw.rect->ApplyScale(draw.x2,draw.y2);

    draw.index_curve++;

    return true;
}


}



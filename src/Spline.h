// Spline.h: interface for the CSpline class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>

namespace dxfv
{

class CSpline  : public cDXFGraphObject
{
public:
    std::string m_Layer;
    int m_FitPointCount;
    int m_ControlPointCount;
    double x[MAXPOINTS],y[MAXPOINTS];
    unsigned char m_Select;
    unsigned char m_Nest;

    CSpline();
    CSpline( cCodeValue& cv );
    virtual ~CSpline();
    bool Append( cvit_t& cvit );
    bool Read( FILE * fp, int& code, char* value );
    void Update( cBoundingRectangle& rect );

    /**  get wxWidgets drawing parameters, scaled to display window, for the 'next' line

    @param[in/out] draw  structure holding parameters, intialize index to zero on first call
    @return true if valid line has been returned

    */
    bool getDraw( cDrawPrimitiveData& draw );

    void Draw( CDxf * dxf );

    void wxwidgets( bool f = true )
    {
        myfwxwidgets = f;
    }

    // Set configuration options
    void Options( CDxf * dxf );

    /** Translate co-ords
        @param[in] ax addition to x
        @param[in] ay addition to y
    */
    void Adjust( double ax, double ay );

private:
    bool myfwxwidgets;          ///< true if using wxwidgets method for control points
    bool myfControlPointsPreferred;
    std::vector<float> Ax;
    std::vector<float> Ay;
    std::vector<float> Bx;
    std::vector<float> By;
    std::vector<float> Cx;
    std::vector<float> Cy;

    void Generate();
    void MatrixSolve(std::vector<float>& B,
                     std::vector< std::vector<float> >& Mat );
    bool getDrawControlPoint( cDrawPrimitiveData& draw );

    /** Get point on Bezier curve defined by control points
        @param[out] x
        @param[out] y
        @param[in]  t position along curve 0 to 1
    */
    void getBezierPoint( double& x, double& y, double t );
};

}



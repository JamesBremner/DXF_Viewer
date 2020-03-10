// Arc.h: interface for the CArc class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

namespace dxfv
{

class CArc  : public cDXFGraphObject
{
public:
    double x;   ///< center, +ve to left of window
    double y;   ///< venter, +ve to bottom of window
    double r;   ///< radius
    double sa;  ///< start angle degrees anti-clockwise from 3 o'clock
    double ea;  ///< start angle degrees anti-clockwise from 3 o'clock
    std::string m_Layer;
    unsigned char m_Select;
    unsigned char m_Nest;

    CArc();
    CArc( cCodeValue& cv );
    virtual ~CArc();


    /**  Update a bounding rectangle to include this

    @param[in/out] rect  The rectangle to be updates

    This is a safe approximation.  The bounding rectangle
    is updated to include the full circle,
    so sometimes it will be larger than neccessary.

    */
    void Update( cBoundingRectangle& rect );

    /**  get  drawing parameters, scaled to display window

    @param[in/out] draw  structure holding parameters

    draw.x1, draw.y1  Upper left corner of bounding rectangle of entire circle
    draw.r            Diameter of circle
    draw.sa           Starting angle
    draw.ea           Ending angle

    The starting/ending angles are adjusted to the peculiarities of wxWidgets on windows v3.0.0
    To run on other platforms or frameworks this code will need to be adjusted

    */
    bool getDraw( cDrawPrimitiveData& draw );

    /// get scaled parameters for windows API arc drawing primitive
    void WAPData(
        int& xl, int& yt, int& xr, int& yb,
        int& sx, int& sy, int& ex, int& ey,
        CDxf* dxf );

    bool Append( cvit_t& cvit );

    void Draw( CDxf * dxf );

    void Adjust( double x, double y );
};
}


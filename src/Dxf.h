// Dxf.h: interface for the CDxf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXF_H__148A5C6F_8B80_49A1_A007_2259EB5655F5__INCLUDED_)
#define AFX_DXF_H__148A5C6F_8B80_49A1_A007_2259EB5655F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <fstream>

namespace dxfv
{
/// DXF code value pair
class cCodeValue
{
public:
    std::string myCode;
    std::string myValue;

    bool Read( std::ifstream& f )
    {
        if( ! std::getline(f, myCode) )
            return false;
        if( ! std::getline(f, myValue) )
            return false;
        // remove leading whitespace from the group code
        size_t p = myCode.find_first_not_of(" ");
        if( p > 0 )
            myCode.erase(0,p-1);
        return true;
    }
    int Code()
    {
        return atoi( myCode.c_str() );
    }
};
typedef std::vector< cCodeValue >::iterator cvit_t;

/**  Manage zooming and scaling
*/
class cBoundingRectangle
{
public:
    double x1,y1,x2,y2;
    bool init;
    double myScale;
    double myZoom;
    int xpan, ypan;
    double x_offset, y_offset;
    int myWindowHeight;

    cBoundingRectangle()
        : init( false )
        , myZoom( 1 )
        , xpan( 0 ), ypan( 0 )
    {}

    /**  Fit all ontent so they show in the display window */
    void Fit()
    {
        myZoom = 1;
        xpan = 0;
        ypan = 0;
    }
    /**  Register some new content

    @param[in] x location of content, file units
    @param[in] y location of content, file units

    We do not care what the file units are,
    just assume that they are consistent

    The bounding rectangle will be enlarged to include this new point,
    if neccessary

    */
    void Update( double x, double y )
    {
        //std::cout << "brupdate " << y << "\n";
        if( ! init )
        {
            init = true;
            x1 = x;
            x2 = x;
            y1 = y;
            y2 = y;
        }
        else
        {
            if( x < x1 )
                x1 = x;
            if( x > x2 )
                x2 = x;
            if( y < y1 )
                y1 = y;
            if( y > y2 )
                y2 = y;
        }
//	std::cout << "BRUpdate  "<< x <<" "<< y <<" "<< x1 <<" "<< x2
//        <<" "<< y1 <<" "<< y2 <<"\n";
    }
    /**  Increase zoom ( make contents display larger ) */
    void ZoomIn()
    {
        myZoom *= 1.05;
    }
    /**  Decrease zoom ( make contents display smaller ) */
    void ZoomOut()
    {
        myZoom *= 0.95;
    }
    /** Pan ( move contents across display

    @param[in]  ox, oy  old location, pixels
    @param[in   nx, ny  new location, pixels

    Contents will be moved by same pixels as old -> new

    */
    void Pan( int ox, int oy, int nx, int ny )
    {
        xpan += nx - ox;
        ypan += ny - oy;
    }
    /**  Calculate scale factors

     First calculate scale that will keep the bounding rectangle inside window
     Then multiply by zoom factor.

    The same scale can be applied to every point,
    so call this just once and the scale will be applied
    by ApplyScale until this is called with new values.

    @param[in] w  window width, pixels
    @param[in] h window height, pixels

    */
    void CalcScale( int w, int h );

    /**  Apply current scale, xoom and pan

    @param[in/out] x value
    @param[in/out] y value

    */
    void ApplyScale( double& x, double& y );
    void RemoveScale( double& x, double& y );

};

/**  Base class for all dxf graphical objects
*/
class cDXFGraphObject
{
public:
    bool myfValid;

    cDXFGraphObject()
    {

    }
    cDXFGraphObject( const std::string& c )
        : myCode( c )
    {

    }

protected:
    std::string myCode;
    void ReadTwoLines( FILE * fp, int& iCode, char* lpValue );

};


/**  Structure holding parameters for the operation needed to draw all or part of graphical object

Some objects can be drawn in one operation - a line, a circle, an arc
Other objects need multiple operations - a polyline and a spline

The exact meaning of the elements of this structure depends on the graphical object

The values are in pixels, scaled to fit into the display window
*/
struct s_dxf_draw
{
    double x1;
    double y1;
    double x2;
    double y2;
    double r;
    double sa;
    double ea;
    int index;
    int index_curve;
    std::string text;
    cBoundingRectangle* rect;   // bounding rectange scaled to window
};


}

#include "line.h"
#include "arc.h"
#include "Circle.h"
#include "PolyLine.h"
#include "Spline.h"
#include "text.h"
#include "Dimension.h"

namespace dxfv
{


/** A container for the graphical objects in a .DXF file
*/
class CDxf
{
public:

    BYTE m_InitialDraw;

    enum eLoadStatus
    {
        none,
        OK,
        demo,
    } myLoadStatus;
    BYTE m_Nesting;

    std::vector< CLine > m_Line;
    std::vector< CArc > m_Arc;
    std::vector< CCircle > m_Circle;
    std::vector < CPolyLine > m_PolyLine;
    std::vector< CSpline > m_Spline;
    std::vector < CText > myText;
    std::vector < CDimension > myDimension;

    cBoundingRectangle myBoundingRectangle;

    CDxf();
    virtual ~CDxf();

    void Init();

    /** Initializing drawing parameters

    @param[in/out] draw  The drawing parameter structure to be initialized
    */
    void Init( s_dxf_draw& draw );

    void LoadFile(const std::string& filepath );
    UINT GetLineCount();
    UINT GetCircleCount();
    UINT GetArcCount();
    UINT GetLwPolyLineCount();
    UINT GetSplineCount();

    bool wxwidgets() const
    {
        return myfwxwidgets;
    }

private:
    bool  myfwxwidgets;          ///< true if using wxwidgets method for control point splines
    std::vector< cCodeValue > myCodeValue;

    void UpdateBoundingRectangle();
};
}
#endif // !defined(AFX_DXF_H__148A5C6F_8B80_49A1_A007_2259EB5655F5__INCLUDED_)

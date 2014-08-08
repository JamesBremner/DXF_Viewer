// Dxf.h: interface for the CDxf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXF_H__148A5C6F_8B80_49A1_A007_2259EB5655F5__INCLUDED_)
#define AFX_DXF_H__148A5C6F_8B80_49A1_A007_2259EB5655F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



namespace dxfv {

class cBoundingRectangle
{
public:
double x1,y1,x2,y2;
bool init;
double myScale;
double myZoom;
double x_offset, y_offset;
int myWindowHeight;

cBoundingRectangle()
: init( false )
, myZoom( 1 )
{}
void Update( double x, double y )
{
	if( ! init ) {
        init = true;
		x1 = x;
		x2 = x;
		y1 = y;
		y2 = y;
	} else {
		if( x < x1 )
			x1 = x;
		if( x > x2 )
			x2 = x;
		if( y < y1 )
			y1 = y;
		if( y > y2 )
			y2 = y;
	}
}
void ZoomIn()
{
    myZoom *= 1.2;
}
void ZoomOut()
{
    myZoom *= 0.8;
}
/**  Calculate scale factors that will keep the bounding rectangle inside a window

@param[in] w  window width, pixels
@param[in] h window height, pixels

*/
void CalcScale( int w, int h )
{
    myWindowHeight = h;

    	double dx = x2 - x1;
	double dy = y2 - y1;

	double sx = dx / w;
	double sy = dy / h;

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

	// Apply current zoom factor
	myScale *= myZoom;
}
void ApplyScale( double& x, double& y )
{
    x = (x / myScale ) + x_offset;
    y = myWindowHeight - (( y / myScale ) + y_offset );
}
};

/**  Base class for all dxf graphical objects
*/
class cDXFGraphObject
{
public:
	/**  Read details of graphical object from DXF file

	@param[in] fp open file pointer
	@param[out] code  code of next line in file that does not belong to this object
	@param[in/out] value  tagged value of current line
	@return true if object successfully read

	This virtual abstract method must be implemented by each object type to read its own dtails

	When called, the value should contain the ID of the object e.g LINE, ARC, etc
	If the value contains something else, method returns false and does nothing
	If the value contains the ID for this object, then the details are read and
	the method returns true with code and value containing the code/value pair of the next object

	*/
	virtual bool Read( FILE * fp, int& code, char* value ) = 0;

protected:
    void ReadTwoLines( FILE * fp, int& iCode, char* lpValue );

};


/**  Structure holding parameters for the operation needed to draw all or part of graphical object

Some objects can be drawn in one operation - a line, a circle, an arc
Other objects need multiple operations - a polyline and a spline

The exact meaning of the elements of this structure depends on the graphical object

The values are in pixels, scaled to fit into the display window
*/
	struct s_dxf_draw {
        double x1;
        double y1;
        double x2;
        double y2;
        double r;
        double sa;
        double ea;
        int index;
        int index_curve;
        cBoundingRectangle* rect;   // bounding rectange scaled to window
        };


}

#include "line.h"
#include "arc.h"
#include "Circle.h"	// Added by ClassView
#include "PolyLine.h"	// Added by ClassView
#include "Spline.h"	// Added by ClassView

namespace dxfv {

/** A container for the graphical objects in a .DXF file
*/
class CDxf
{
public:

	BYTE m_InitialDraw;
	BYTE m_LoadStatus;
	BYTE m_Nesting;

	std::vector< CLine > m_Line;
	std::vector< CArc > m_Arc;
	std::vector< CCircle > m_Circle;
	std::vector < CPolyLine > m_PolyLine;
	std::vector< CSpline > m_Spline;

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


private:
	void ReadTwoLines( FILE * fp, int& iCode, char* lpCode, char* lpValue );
	void ReadUntilCode(  FILE * fp, int TargetCode,  char* lpValue );
	void UpdateBoundingRectangle();
};
}
#endif // !defined(AFX_DXF_H__148A5C6F_8B80_49A1_A007_2259EB5655F5__INCLUDED_)

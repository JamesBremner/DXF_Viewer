// Arc.h: interface for the CArc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARC_H__F3B30C5E_D3C4_4380_844A_03369FF97719__INCLUDED_)
#define AFX_ARC_H__F3B30C5E_D3C4_4380_844A_03369FF97719__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace dxfv {

class CArc  : public cDXFGraphObject
{
public:
	double x,y,r,sa,ea;
	std::string m_Layer;
	BYTE m_Select;
	BYTE m_Nest;

	CArc();
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
draw.r            Radius of circle
draw.sa           Starting angle
draw.ea           Ending angle

The starting/ending angles are adjusted to the peculiarities of wxWidgets on windows v3.0.0
To run on other platforms or frameworks this code will need to be adjusted

*/
    bool getDraw( s_dxf_draw& draw );

bool Read( FILE * fp, int& code, char* value );

};
}
#endif // !defined(AFX_ARC_H__F3B30C5E_D3C4_4380_844A_03369FF97719__INCLUDED_)

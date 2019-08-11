// Circle.h: interface for the CCircle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIRCLE_H__B468D98B_7FA1_4A99_B55F_2A56932403A7__INCLUDED_)
#define AFX_CIRCLE_H__B468D98B_7FA1_4A99_B55F_2A56932403A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace dxfv {
class CCircle  : public cDXFGraphObject
{
public:
	double x,y,r;
	std::string m_Layer;
	BYTE m_Select;
	BYTE m_Nest;

	CCircle();
	CCircle( cCodeValue& cv );
	virtual ~CCircle();
	bool Append( std::vector<cCodeValue>::iterator& cvit );
	bool Read( FILE * fp, int& code, char* value );

	void Update( cBoundingRectangle& rect );

		/**  get drawing parameters, scaled to display window

@param[in/out] draw  structure holding parameters
@return true if valid line has been returned

draw.x1, draw.y1  Center of circle
draw.r            Radius of cirle

*/
    bool getDraw( s_dxf_draw& draw );
};
}
#endif // !defined(AFX_CIRCLE_H__B468D98B_7FA1_4A99_B55F_2A56932403A7__INCLUDED_)

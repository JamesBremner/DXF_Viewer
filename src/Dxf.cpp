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

namespace dxfv {

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
/**  Read next two lines from a DXF file - a group code and a value

@param[in]  fp     the open DXF file
@param[out]  iCode  the group code, as an integer
@param[out]  lpCode a 255 character array to hold the group code string
@param[out]  lpValue  a 255 character array to hold the value string ( may contain spaces )

*/
void CDxf::ReadTwoLines( FILE * fp, int& iCode, char* lpCode, char* lpValue )
{
	// read next two lines in their entirety
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
/**  Read DXF file until get a particular group code

@param[in] fp  the open DXF file
@param[in] TargetCode  The group code we are looking for
@param[out]  lpValue  a 255 character array to hold the value string ( may contain spaces )
void CDxf::ReadUntilCode(  FILE * fp, int TargetCode,  char* lpValue )

*/
void CDxf::ReadUntilCode(  FILE * fp, int TargetCode,  char* lpValue )
{
	char lpCode[256];
	int iCode;
	do {
		ReadTwoLines(fp,iCode,lpCode,lpValue );
	} while( iCode != TargetCode ) ;
}

/**  Read DXF file

@param[in] str path and filename

*/
void CDxf::LoadFile(const std::string& filepath)
{
	Init();
	FILE *fp;

	fp = fopen(filepath.c_str(),"r");
	if(fp != NULL)
	{

		char lpCode[256], lpValue[256];
		int iCode;

		ReadTwoLines( fp, iCode, lpCode, lpValue );

		while(!feof(fp))
		{
			if( iCode != 0 ) {
				ReadTwoLines( fp, iCode, lpCode, lpValue );
				continue;
			}

			CLine line;
			if( line.Read( fp, iCode, lpValue ) ) {
				m_Line.push_back( line );
				continue;
			}

			CPolyLine PolyLine;
			if( PolyLine.Read( fp, iCode, lpValue ) ) {
				m_PolyLine.push_back( PolyLine );
				continue;
			}


			CCircle circle;
			if( circle.Read( fp, iCode, lpValue ) ) {
				m_Circle.push_back( circle );
				continue;

			}

			CArc arc;
			if( arc.Read( fp, iCode, lpValue ) ) {
				m_Arc.push_back( arc );
				continue;
			}


			CSpline spline;
			if( spline.Read( fp, iCode, lpValue ) ) {
				m_Spline.push_back( spline );
				continue;
			}

			CText text;
			if( text.Read( fp, iCode, lpValue ) ) {
                myText.push_back( text );
                continue;
			}

			CDimension dim;
			if( dim.Read( fp, iCode, lpValue ) ) {
                myDimension.push_back( dim );
                continue;
			}

			ReadTwoLines( fp, iCode, lpCode, lpValue );

		}

        myLoadStatus = OK;
		#ifdef DEMO
    if( m_PolyLine.size() > 5  ||
       m_Line.size() > 5
        ) {
        myLoadStatus = demo;
    }
    #endif // DEMO


		UpdateBoundingRectangle();
	}
	else
	{
		myLoadStatus = none;
	}
}

void CDxf::UpdateBoundingRectangle()
{
#ifdef __GNUWIN32__
	myBoundingRectangle.init = false;
	for( CLine& line : m_Line ) {
		line.Update( myBoundingRectangle );
	}
	for( CCircle& circle : m_Circle ) {
        circle.Update( myBoundingRectangle );
	}
    for( CArc& arc : m_Arc ) {
        arc.Update( myBoundingRectangle );
	}
	for( CPolyLine& polyline : m_PolyLine) {
        polyline.Update( myBoundingRectangle );
	}
	for( CSpline& spline: m_Spline ) {
        spline.Update( myBoundingRectangle );
	}
#endif
}

UINT CDxf::GetLineCount()
{
#ifdef DEMO
    if( m_Line.size() > 1 ) {
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
    if( m_PolyLine.size() > 1 ) {
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

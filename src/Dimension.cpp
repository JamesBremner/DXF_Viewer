#include "stdafx.h"
#include "DXF.h"
#include "Dimension.h"


namespace dxfv
{

bool CDimension::Read( FILE * fp, int& code, char* lpValue )
{
    if( strcmp(lpValue,"DIMENSION") != 0 )
    {
        // not a text
        return false;
    }
    while( fp != NULL )
    {
        ReadTwoLines(fp, code, lpValue );
        switch ( code )
        {
        case 0:
            // a new object
            return true;
         case 10:
            x1 = atof(lpValue);
            break;
        case 20:
            y1 = atof(lpValue);
            break;
        case 42:
            // the measurememnt value
            {
                float v = atoi( lpValue );
                char buf[100];
                snprintf(buf,99,"%.2f",v);
                myText = buf;
            }
            break;
       }
    }
    return true;
}
bool CDimension::getDraw( s_dxf_draw& draw )
{
    draw.text = myText;
    draw.x1 = x1;
    draw.y1 = y1;
    draw.rect->ApplyScale(draw.x1, draw.y1);
    return true;
}
}

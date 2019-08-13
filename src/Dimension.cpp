#include "stdafx.h"
#include "DXF.h"
#include "Dimension.h"


namespace dxfv
{


bool CDimension::getDraw( cDrawPrimitiveData& draw )
{
    draw.text = myText;
    draw.x1 = x1;
    draw.y1 = y1;
    draw.rect->ApplyScale(draw.x1, draw.y1);
    return true;
}
}

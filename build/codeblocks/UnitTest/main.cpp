#include <iostream>
#include <vector>
#include "Dxf.h"
#include "Arc.h"

using namespace std;

int main()
{
    dxfv::CArc A;
    A.x = 100;          // center at 100,100
    A.y = 100;
    A.r = 50;           // radius 50
    A.sa = 45;          // start at 22.5 minutes past
    A.ea = 360 - 45;    // finish at 7.5 minutes past ( moving anticlockwise )

    dxfv::cBoundingRectangle BR;
    A.Update( BR );
    std::cout << BR.x1<<" "<<BR.y1<<", "<<BR.x2<<" "<<BR.y2<<"\n";

    return 0;
}

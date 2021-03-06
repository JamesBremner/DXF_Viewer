#include <iostream>
#include "Dxf.h"
#include "Arc.h"

using namespace std;

int main()
{
    // test bounding box calculations for CArc

    dxfv::CArc A;
    A.x = 100;          // center at 100,100
    A.y = 100;
    A.r = 50;           // radius 50
    dxfv::cBoundingRectangle BR;

    std::cout << "top right quadrant\n";
    A.sa = 5;
    A.ea = 85;
    BR.init = false;
    A.Update( BR );
    std::cout << BR.x1<<" "<<BR.y1<<", "<<BR.x2<<" "<<BR.y2<<"\n";
    if( BR.x1 < 100 || BR.y1 < 100 || BR.x2 < 100 || BR.y2 < 100 )
        std::cout << "FAILED\n";

    std::cout << "top left quadrant\n";
    A.sa = 95;
    A.ea = 175;
    BR.init = false;
    A.Update( BR );
    std::cout << BR.x1<<" "<<BR.y1<<", "<<BR.x2<<" "<<BR.y2<<"\n";
    if( BR.x1 > 100 || BR.y1 < 100 || BR.x2 > 100 || BR.y2 < 100 )
        std::cout << "FAILED\n";

    std::cout << "bottom right quadrant\n";
    A.sa = 185;
    A.ea = 265;
    BR.init = false;
    A.Update( BR );
    std::cout << BR.x1<<" "<<BR.y1<<", "<<BR.x2<<" "<<BR.y2<<"\n";
    if( BR.x1 > 100 || BR.y1 > 100 || BR.x2 > 100 || BR.y2 > 100 )
        std::cout << "FAILED\n";

    std::cout << "bottom left quadrant\n";
    A.sa = 275;
    A.ea = 355;
    BR.init = false;
    A.Update( BR );
    std::cout << BR.x1<<" "<<BR.y1<<", "<<BR.x2<<" "<<BR.y2<<"\n";
    if( BR.x1 < 100 || BR.y1 > 100 || BR.x2 < 100 || BR.y2 > 100 )
        std::cout << "FAILED\n";
    return 0;
}

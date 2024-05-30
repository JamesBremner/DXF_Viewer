// Spline.h: interface for the CSpline class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

namespace dxfv
{

    class CSpline : public cDXFGraphObject
    {
    public:
        std::string m_Layer;
        int m_FitPointCount;
        unsigned char m_Select;
        unsigned char m_Nest;

        CSpline();
        CSpline(cCodeValue &cv);
        virtual ~CSpline();
        bool Append(cvit_t &cvit);
        bool Read(FILE *fp, int &code, char *value);
        void Update(cBoundingRectangle &rect);

        /**  get drawing parameters, scaled to display window, for the 'next' line

        @param[in/out] draw  structure holding parameters, intialize index to zero on first call
        @return true if valid line has been returned

        */
        bool getDraw(cDrawPrimitiveData &draw);

        void Draw(CDxf *dxf);

        void wxwidgets(bool f = true)
        {
            myfwxwidgets = f;
        }

        // Set configuration options
        void Options(CDxf *dxf);

        /** Translate co-ords
            @param[in] ax addition to x
            @param[in] ay addition to y
        */
        void Adjust(double ax, double ay);

    private:
        bool myfwxwidgets; ///< true if using wxwidgets method for control points
        bool myfControlPointsPreferred;
        std::vector<float> Ax;
        std::vector<float> Ay;
        std::vector<float> Bx;
        std::vector<float> By;
        std::vector<float> Cx;
        std::vector<float> Cy;
        int m_KnotCount;
        std::vector<double> vknots;     // Spline knots
        int m_ControlPointCount;
        std::vector<double> vx, vy;     // Spline control points
        int myDegree;

        /// @brief check sanity of completely parsed spline
        /// @param point_index number of control points found
        /// @return true always
        /// Throws exception when problem found

        bool sanity(int point_index);

        /// @brief Initialize fit points calculation
        void Generate();

        void BSplineInit();

        void MatrixSolve(std::vector<float> &B,
                         std::vector<std::vector<float>> &Mat);

        bool getDrawControlPoint(cDrawPrimitiveData &draw);
        bool getDrawFit(cDrawPrimitiveData &draw);
        bool getDrawBSpline(cDrawPrimitiveData &draw);

        void getxydeBoor(double t, double &cx, double &cy);
        double deBoor(
            int k,
            double x,
            const std::vector<double> &c);

        /** Get point on Bezier curve defined by control points
            @param[out] x
            @param[out] y
            @param[in]  t position along curve 0 to 1
        */
        void getBezierPoint(double &x, double &y, double t);

    };

}

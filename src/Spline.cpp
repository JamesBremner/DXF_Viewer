// Spline.cpp: implementation of the CSpline class.
//
//////////////////////////////////////////////////////////////////////

#include <vector>
#include <cmath>
#include "Dxf.h"
#include "Spline.h"

namespace dxfv
{
    struct cP
    {
        double x, y;
        cP() {}
        cP(double x, double y) : x(x), y(y) {}
        cP operator+(cP b)
        {
            return cP(x + b.x, y + b.y);
        }

        cP operator-(cP b)
        {
            return cP(x - b.x, y - b.y);
        }
    };

    CSpline::CSpline(cCodeValue &cv)
        : cDXFGraphObject()
    {
        myfValid = (cv.myValue == "SPLINE");
        myType = cDXFGraphObject::eType::spline;
        m_FitPointCount = 0;
        m_ControlPointCount = 0;
        m_KnotCount = 0;
        myfwxwidgets = false;
        myDegree = 0;
        myThick = 1;
    }

    CSpline::~CSpline()
    {
    }

    bool CSpline::Append(cvit_t &cvit)
    {
        // https://help.autodesk.com/view/OARX/2023/ENU/?guid=GUID-E1F884F8-AA90-4864-A215-3182D47A9C74

        int point_index = 0;

        while (true)
        {
            cvit++;
            // std::cout << "spline " << cvit->myCode <<", " << cvit->myValue << "\n";
            switch (cvit->Code())
            {
            case 0:
                // a new object
                sanity(point_index);
                BSplineInit();
                Generate();
                cvit--;
                return false;

            case 8:
                myLayer = cvit->myValue;
                break;

            case 71:
                myDegree = atoi(cvit->myValue.c_str());
                break;

            case 74:
                m_FitPointCount = atoi(cvit->myValue.c_str());
                if (!m_FitPointCount)
                    break;
                if (m_FitPointCount >= MAXPOINTS)
                    throw std::runtime_error("Too many spline points");
                if (m_ControlPointCount)
                {
                    // both control and fit points
                    if (myfControlPointsPreferred)
                    {
                        m_FitPointCount = 0;
                    }
                    else
                    {
                        m_ControlPointCount = 0;
                    }
                }
                break;

            case 11:
                if (m_FitPointCount)
                    vx.push_back(atof(cvit->myValue.c_str()));
                break;
            case 21:
                if (m_FitPointCount)
                    vy.push_back(atof(cvit->myValue.c_str()));
                break;

            case 73:
                m_ControlPointCount = atoi(cvit->myValue.c_str());
                if (!m_ControlPointCount)
                    break;
                if (m_ControlPointCount >= MAXPOINTS)
                    throw std::runtime_error("Too many spline points");
                if (m_FitPointCount)
                {
                    // both control and fit points
                    if (myfControlPointsPreferred)
                    {
                        m_FitPointCount = 0;
                    }
                    else
                    {
                        m_ControlPointCount = 0;
                    }
                }
                break;

            case 10:
                if (m_ControlPointCount)
                    vx.push_back(atof(cvit->myValue.c_str()));
                break;

            case 20:
                if (m_ControlPointCount)
                    vy.push_back(atof(cvit->myValue.c_str()));
                break;

            case 39:

                /* Line thickness
                The DXF standard makes no mention of spline line thickness
                So this is an extension of the standard!
                It defaults to one, set in the CSpline constructor
                TID46 https://github.com/JamesBremner/DXF_Viewer/issues/46
                */
                myThick = atoi(cvit->myValue.c_str());
                break;

            case 40:
                vknots.push_back(
                    atof(cvit->myValue.c_str()));
                break;

            case 62:
                AutocadColor2RGB(atoi(cvit->myValue.c_str()));
                break;

            case 72:
                m_KnotCount = atoi(cvit->myValue.c_str());
                break;
            }
        }
        return true;
    }

    bool CSpline::sanity(int point_index)
    {
        // std::cout << "spline points " << m_FitPointCount  <<" "<< m_ControlPointCount<<" "<< point_index << "\n";
        if (m_FitPointCount == 0 && m_ControlPointCount == 0)
            throw std::runtime_error("Spline has no points");
        if (point_index != m_FitPointCount && point_index != m_ControlPointCount)
            throw std::runtime_error("Spline has unexpected number of points");
        if (m_KnotCount != vknots.size())
            throw std::runtime_error("Spline has unexpected number of knots");
        if (myDegree == -1)
            throw std::runtime_error("Spline has unspecified degree");
        return true;
    }

    void CSpline::Options(CDxf *dxf)
    {
        myfwxwidgets = dxf->wxwidgets();
        myfControlPointsPreferred = dxf->SplineControlPointsPreferred();
    }

    void CSpline::Update(cBoundingRectangle &rect)
    {
        int count = m_FitPointCount;
        if (!count)
            count = m_ControlPointCount;
        for (int k = 0; k < count; k++)
        {
            rect.Update(vx[k], vy[k]);
        }
        int dbg = 0;
    }
    void CSpline::Generate()
    {
        if (!m_FitPointCount)
            return;

        float AMag, AMagOld;
        std::vector<float> k;
        std::vector<std::vector<float>> Mat(3, std::vector<float>(m_FitPointCount));

        // vector A
        for (int i = 0; i <= (int)m_FitPointCount - 2; i++)
        {
            Ax.push_back(vx[i + 1] - vx[i]);
            Ay.push_back(vy[i + 1] - vy[i]);
        }
        // k
        AMagOld = (float)sqrt(Ax[0] * Ax[0] + Ay[0] * Ay[0]);
        for (int i = 0; i <= (int)m_FitPointCount - 3; i++)
        {
            AMag = (float)sqrt(Ax[i + 1] * Ax[i + 1] + Ay[i + 1] * Ay[i + 1]);
            k.push_back(AMagOld / AMag);
            AMagOld = AMag;
        }
        k.push_back(1.0f);

        // Matrix
        for (int i = 1; i <= (int)m_FitPointCount - 2; i++)
        {
            Mat[0][i] = 1.0f;
            Mat[1][i] = 2.0f * k[i - 1] * (1.0f + k[i - 1]);
            Mat[2][i] = k[i - 1] * k[i - 1] * k[i];
        }
        Mat[1][0] = 2.0f;
        Mat[2][0] = k[0];
        Mat[0][m_FitPointCount - 1] = 1.0f;
        Mat[1][m_FitPointCount - 1] = 2.0f * k[m_FitPointCount - 2];

        //
        Bx.push_back(3.0f * Ax[0]);
        By.push_back(3.0f * Ay[0]);
        for (int i = 1; i <= (int)m_FitPointCount - 2; i++)
        {
            Bx.push_back(3.0f * (Ax[i - 1] + k[i - 1] * k[i - 1] * Ax[i]));
            By.push_back(3.0f * (Ay[i - 1] + k[i - 1] * k[i - 1] * Ay[i]));
        }
        Bx.push_back(3.0f * Ax[m_FitPointCount - 2]);
        By.push_back(3.0f * Ay[m_FitPointCount - 2]);

        //
        MatrixSolve(Bx, Mat);
        MatrixSolve(By, Mat);

        for (int i = 0; i <= (int)m_FitPointCount - 2; i++)
        {
            Cx.push_back(k[i] * Bx[i + 1]);
            Cy.push_back(k[i] * By[i + 1]);
        }
    }

    void CSpline::MatrixSolve(
        std::vector<float> &B,
        std::vector<std::vector<float>> &Mat)
    {
        float *Work = new float[m_FitPointCount];
        float *WorkB = new float[m_FitPointCount];
        for (int i = 0; i <= m_FitPointCount - 1; i++)
        {
            Work[i] = B[i] / Mat[1][i];
            WorkB[i] = Work[i];
        }

        for (int j = 0; j < 10; j++)
        {
            ///  need convergence judge
            Work[0] = (B[0] - Mat[2][0] * WorkB[1]) / Mat[1][0];
            for (int i = 1; i < m_FitPointCount - 1; i++)
            {
                Work[i] = (B[i] - Mat[0][i] * WorkB[i - 1] - Mat[2][i] * WorkB[i + 1]) / Mat[1][i];
            }
            Work[m_FitPointCount - 1] = (B[m_FitPointCount - 1] - Mat[0][m_FitPointCount - 1] * WorkB[m_FitPointCount - 2]) / Mat[1][m_FitPointCount - 1];

            for (int i = 0; i <= m_FitPointCount - 1; i++)
            {
                WorkB[i] = Work[i];
            }
        }
        for (int i = 0; i <= m_FitPointCount - 1; i++)
        {
            B[i] = Work[i];
        }
        delete[] Work;
        delete[] WorkB;
    }

    void CSpline::BSplineInit()
    {
        if (!vknots.size())
            return;

        if (fabs(1 - vknots.back()) > 0.01)
        {
            // normalize knot values
            for (double &v : vknots)
                v /= vknots.back();
        }
    }

    cP operator*(double s, cP b)
    {
        return cP(s * b.x, s * b.y);
    }

    bool CSpline::getDraw(cDrawPrimitiveData &draw)
    {
        draw.thick = myThick;
        if (m_FitPointCount)
            return getDrawFit(draw);
        if (!vknots.size())
            return getDrawControlPoint(draw);
        return getDrawBSpline(draw);
    }

    bool CSpline::getDrawFit(cDrawPrimitiveData &draw)
    {
        draw.color = myColor;

        /* determine number of straight line segments to draw between each pait of fit points
        For good smooth looking curves ensure that no segment changes the x or y pixel by more than 2
        */

        int Ndiv = (int)(std::max(abs(vx[draw.index] - vx[draw.index + 1]), abs(vy[draw.index] - vy[draw.index + 1])) / (2 * draw.rect->myScale));

        if (draw.index_curve == 0 && draw.index == 0)
        {
            // The first point in the spline
            draw.x1 = vx[draw.index];
            draw.y1 = vy[draw.index];
            draw.rect->ApplyScale(draw.x1, draw.y1);
        }
        else if (draw.index_curve == Ndiv)
        {
            // No more points in current curve
            if (draw.index == m_FitPointCount - 2)
            {
                // No more fitting points
                return false;
            }

            // Draw the line between the last point of the previous curve
            // and the next fitting point
            draw.index++;
            draw.index_curve = 0;
            draw.x1 = draw.x2;
            draw.y1 = draw.y2;
            draw.x2 = vx[draw.index];
            draw.y2 = vy[draw.index];
            draw.rect->ApplyScale(draw.x2, draw.y2);
            return true;
        }
        else
        {
            // In the middle of drawing the curve between two fitting points
            // Start from the end of the previous line
            draw.x1 = draw.x2;
            draw.y1 = draw.y2;
        }

        // Calulate the next point in the curve between two fitting points
        float t, f, g, h;
        t = 1.0f / (float)Ndiv * (float)draw.index_curve;
        f = t * t * (3.0f - 2.0f * t);
        g = t * (t - 1.0f) * (t - 1.0f);
        h = t * t * (t - 1.0f);
        double x2 = (vx[draw.index] + Ax[draw.index] * f + Bx[draw.index] * g + Cx[draw.index] * h);
        double y2 = (vy[draw.index] + Ay[draw.index] * f + By[draw.index] * g + Cy[draw.index] * h);

        draw.rect->ApplyScale(x2, y2);
        draw.x2 = round(x2);
        draw.y2 = round(y2);

        // std::cout << t <<" "<< draw.x2 <<" "<< draw.y2 << "\n";

        draw.index_curve++;

        return true;
    }

    bool CSpline::getDrawControlPoint(cDrawPrimitiveData &draw)
    {
        draw.color = myColor;

        // check if using wxwidgets spline method
        if (myfwxwidgets)
        {
            // check if more points are available
            if (draw.index == m_ControlPointCount)
            {
                if (draw.index_curve)
                    return false;
                draw.index_curve = 1;
                return true;
            }

            draw.x1 = vx[draw.index];
            draw.y1 = vy[draw.index];
            draw.rect->ApplyScale(draw.x1, draw.y1);
            draw.x2 = vx[draw.index + 1];
            draw.y2 = vy[draw.index + 1];
            draw.rect->ApplyScale(draw.x2, draw.y2);
            draw.index++;
            draw.index_curve = 0;
            return true;
        }

        int Ndiv = 100;

        // check if more points are available
        if (draw.index == Ndiv - 1)
            return false;

        getBezierPoint(draw.x1, draw.y1, ((float)draw.index) / Ndiv);
        getBezierPoint(draw.x2, draw.y2, ((float)(draw.index + 1)) / Ndiv);
        draw.rect->ApplyScale(draw.x1, draw.y1);
        draw.rect->ApplyScale(draw.x2, draw.y2);
        draw.index++;
        draw.index_curve = 0;
        return true;
    }

    bool CSpline::getDrawBSpline(cDrawPrimitiveData &draw)
    {
        draw.color = myColor;

        // number of points drawn along curve
        const int Ndiv = 100;
        if (draw.index == Ndiv)
            return false;                      // completed
        double t = ((float)draw.index) / Ndiv; // normalize

        if (!draw.index)
        {
            // first
            getxydeBoor(t, draw.x1, draw.y1);
            draw.rect->ApplyScale(draw.x1, draw.y1);
            getxydeBoor(((float)draw.index + 1) / Ndiv, draw.x2, draw.y2);
        }
        else
        {
            draw.x1 = draw.x2;
            draw.y1 = draw.y2;
            getxydeBoor(t, draw.x2, draw.y2);
        }
        draw.rect->ApplyScale(draw.x2, draw.y2);
        draw.index++;
        return true; // more points to come
    }

    /* python code from https://en.wikipedia.org/wiki/De_Boor%27s_algorithm#Example_implementation

    def deBoor(k: int, x: int, t, c, p: int):
    """Evaluates S(x).

    Arguments
    ---------
    k: Index of knot interval that contains x.
    x: Position.
    t: Array of knot positions, needs to be padded as described above.
    c: Array of control points.
    p: Degree of B-spline.
    """
    d = [c[j + k - p] for j in range(0, p + 1)]

    for r in range(1, p + 1):
        for j in range(p, r - 1, -1):
            alpha = (x - t[j + k - p]) / (t[j + 1 + k - r] - t[j + k - p])
            d[j] = (1.0 - alpha) * d[j - 1] + alpha * d[j]

    return d[p]

    Note 1:  input x "the position" is declared to be an integer.  This makes no sense.
        IMHO this should be a double in the range [0,1] where 0 is the first point and 1 is the last.
        [] indicates end points included in range

    Note 2: input c: array of control points is an array of values for one dimension of the control points
        for a 2D curve this must be called twice, once for the x values and once for the y values

    Here is my port to C++

    /// @brief deBoor algorithim for B-Spline generation
    /// @param k index of greatest knot less than x
    /// @param x position along the curve in the range [0,1] where 0 is the first point and 1 is the last.
    /// @param t knots in the range [0,1] where 0 is the first point and 1 is the last
    /// @param c values for one dimension of the control points ( e.g x or y for 2D curve)
    /// @param p degree of spline
    /// @return value of spline curve in dimenson provided by c
    double deBoor(
        int k,
        double x,
        const std::vector<double> &t,
        const std::vector<double> &c,
        int p)
    {
        std::vector<double> d;
        for (int j1 = 0; j1 < p + 1; j1++)
            d.push_back(c[j1 + k - p]);

        for (int r = 1; r < p + 2; r++)
        {
            for (int j = p; j > r - 1; j--)
            {
                double alpha = (x - t[j + k - p]) / (t[j + 1 + k - r] - t[j + k - p]);
                d[j] = (1 - alpha) * d[j - 1] + alpha * d[j];
            }
        }
        return d[p];
    }

    */

    double CSpline::deBoor(
        int k,
        double x,
        const std::vector<double> &c)
    {

        std::vector<double> d;
        for (int j1 = 0; j1 < myDegree + 1; j1++)
        {
            // quick fix: clamp index to >= 0  TID45 https://github.com/JamesBremner/DXF_Viewer/issues/45#issuecomment-2561315430
            int index = j1 + k - myDegree;
            if( index < 0 )
                index = 0;

            d.push_back(c[index]);
        }
        for (int r = 1; r < myDegree + 2; r++)
        {
            for (int j = myDegree; j > r - 1; j--)
            {
                double div = vknots[j + 1 + k - r] - vknots[j + k - myDegree];
                double alpha = 0.5; // this is a guess to prevent nan TID45
                if (fabs(div) > 0.00001)
                    alpha = (x - vknots[j + k - myDegree]) / div;
                d[j] = (1 - alpha) * d[j - 1] + alpha * d[j];
            }
        }

        if (std::isnan(d[myDegree]))
            throw std::runtime_error("CSpline::deBoor nan");

        return d[myDegree];
    }

    void CSpline::getxydeBoor(double t, double &cx, double &cy)
    {
        // index of largest knot less than t
        int ik;
        for (ik = 0; ik < vknots.size(); ik++)
            if (vknots[ik + 1] > t)
                break;

        cx = deBoor(
            ik,
            t,
            vx);
        cy = deBoor(
            ik,
            t,
            vy);

        // std::cout << "xy: " << t << " " << cx << " " << cy << "\n";
    }
    void CSpline::getBezierPoint(double &ox, double &oy, double t)
    {
        // https://stackoverflow.com/a/21642962/16582

        std::vector<cP> tmp;
        for (int k = 0; k < m_ControlPointCount; k++)
            tmp.push_back(cP(vx[k], vy[k]));
        int i = m_ControlPointCount - 1;
        while (i > 0)
        {
            for (int k = 0; k < i; k++)
                tmp[k] = tmp[k] + t * (tmp[k + 1] - tmp[k]);
            i--;
        }
        ox = tmp[0].x;
        oy = tmp[0].y;

        // std::cout << "getBezierPoint "
        //           << ox << " " << oy << " | ";
    }

    void CSpline::Adjust(double ax, double ay)
    {
        int count = m_FitPointCount;
        if (!count)
            count = m_ControlPointCount;
        for (int k = 0; k < count; k++)
        {
            vx[k] += ax;
            vy[k] += ay;
        }
    }
}

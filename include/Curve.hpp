#ifndef CURVE_HPP
#define CURVE_HPP
// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>

// Include GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>


class Curve
{
public:
	// create a curve
	Curve();

	// destructor
	~Curve()= default;
	
	// set number of points on the curve
	void setPrecision(float nbPoints);

	[[nodiscard]] int getPrecision() const;
	[[nodiscard]] int getIteration() const;
	// set controlPoints
	void setControlPoints(const std::vector<glm::vec3>& cp);

	// get controlPoints
	std::vector<glm::vec3> & getControlPoints() {return _controlPoints;}

	// create hermite cubic curve with 2 points and 2 vectors
    [[maybe_unused]] std::vector<glm::vec3> & hermiteCubicCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 v0, glm::vec3 v1);
	
	// create Bezier curve using Bernstein coeff
	std::vector<glm::vec3> & bezierCurveByBernstein(const std::vector<glm::vec3>& controlPoints);
	
	// create recursively Bezier curve using Casteljau method
	std::vector<glm::vec3> & bezierCurveByCasteljau(const std::vector<glm::vec3>& controlPoints);
    void setIteration(int it);

	void clear(){_controlPoints.clear();};
private:
	std::vector<glm::vec3> _vertices;
	std::vector<glm::vec3> _controlPoints;
	long int NB_POINTS;
    int _iterations;

	// hermite cubic curve
	static float F1(float u) { return 2.0f*powf(u, 3.0f) - 3.0f*powf(u, 2.0f) + 1; }
	static float F2(float u) { return -2.0f*powf(u, 3.0f) + 3.0f*powf(u, 2.0f); }
	static float F3(float u) { return powf(u, 3.0f) - 2.0f*powf(u, 2.0f) + u; }
    static float F4(float u) { return powf(u, 3.0f) - powf(u, 2.0f); }

	// Bezier curve using Bernstein coeff
	static float fact(double n)
	{
	    double tmp = 1.0;
	    for(int i = 1; i <= n; ++i) tmp *= i;
	    return (float)tmp;
	}
	static float binomialCoeff(double n, double k) { return (fact(n))/(fact(k)*fact(n-k)); }
	static float polynomBernstein(double m, double i, double u) { return binomialCoeff((float)m, (float)i)*powf((float)u, (float)i)*powf(1.0f-(float)u, (float)m-(float)i); }
	glm::vec3 bezierCurveByCasteljauRec(std::vector<glm::vec3> in_pts, float i);
};


#endif


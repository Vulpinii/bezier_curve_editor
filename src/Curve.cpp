#include "Curve.hpp"



















// *********************************************************************************************************************
// CODE IMPLEMENTATION
// *********************************************************************************************************************

// constructor
Curve::Curve() {
    NB_POINTS = 50;
    _iterations = (int)NB_POINTS;
}

// set number of points on the curve
void Curve::setPrecision(float nbPoints){NB_POINTS = (long)nbPoints;_iterations=(int)nbPoints;}
void Curve::setIteration(int it){if (it < NB_POINTS && it > 0) _iterations = it;}

// set controlPoints
void Curve::setControlPoints(const std::vector<glm::vec3>& cp){_controlPoints=cp;}

// create hermite cubic curve with 2 points and 2 vectors
[[maybe_unused]] std::vector<glm::vec3> & Curve::hermiteCubicCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 v0, glm::vec3 v1){
    _vertices.clear();
    _vertices.push_back(p0);
    float length = glm::distance(p0, p1);

    auto etape = (float)_iterations;
    if(etape < 0) etape = 0;
    if(etape > (float)NB_POINTS*(float)_controlPoints.size()) etape = (float)NB_POINTS;
    long int nbU = NB_POINTS;

    float i = 0.0;

    while(i < etape/(float)nbU){
        glm::vec3 p = F1(i) * p0 + F2(i) * p1 + F3(i) * v0 + F4(i) * v1;
        p.x = i*length;
        p.z = 0.0;
        _vertices.push_back(p);
        i+=1.0f/(float)nbU;
    }
    _vertices.push_back(p1);
    return _vertices;
}

// create Bezier curve using Bernstein coeff
std::vector<glm::vec3> & Curve::bezierCurveByBernstein(const std::vector<glm::vec3>& controlPoints){
    _controlPoints = controlPoints;
    _vertices.clear();

    auto etape = (float)_iterations;
    if(etape < 0) etape = 0;
    if(etape > (float)NB_POINTS*(float)_controlPoints.size()) etape = (float)NB_POINTS;
    long int nbU = NB_POINTS;
    double n = controlPoints.size()-1.0;

    double i = 0.0;
    while(i <=  etape/(float)nbU){
        auto p = glm::vec3(0.0);
        for(int j = 0; j <= n; ++j)
        {
            p += polynomBernstein(n, j, i)*controlPoints.at(j);
        }
        _vertices.push_back(p);
        i+=1.0/(float)nbU;
    }
    if( etape/(float)nbU == 1) _vertices.push_back(controlPoints.at(n));

    return _vertices;
}

// create recursively Bezier curve using Casteljau method
std::vector<glm::vec3> & Curve::bezierCurveByCasteljau(const std::vector<glm::vec3>& controlPoints){
    _controlPoints = controlPoints;
    _vertices.clear();

    auto etape = (float)_iterations;
    if(etape < 0) etape = 0;
    if(etape > (float) NB_POINTS*_controlPoints.size()) etape = (float) NB_POINTS;
    long int nbU = NB_POINTS;

    float i = 0.0f;
    while(i <=  etape/(float)nbU){
        glm::vec3 v = bezierCurveByCasteljauRec(controlPoints, (float) i);
        //std::cout << v.x << " " << v.y << " " << v.z << std::endl;
        _vertices.push_back(v);
        i+=1.0f/(float)nbU;
    }
    if( etape/(float)nbU == 1) _vertices.push_back(controlPoints.at(controlPoints.size()-1));
    return _vertices;
}

glm::vec3 Curve::bezierCurveByCasteljauRec(std::vector<glm::vec3> in_pts, float i){
    if(in_pts.size() == 1) return in_pts.at(0);
    std::vector<glm::vec3> pts ;
    for(unsigned int it = 0 ; it < in_pts.size() - 1; it++){
        glm::vec3 vecteur = in_pts.at(it + 1 ) - in_pts.at(it);
        vecteur = vecteur * i;
        pts.push_back(in_pts.at(it) + vecteur);
    }

    return bezierCurveByCasteljauRec(pts, i);
}

int Curve::getPrecision() const {return (int)NB_POINTS;}

int Curve::getIteration() const {return _iterations;}



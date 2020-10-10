// Include standard headers
#ifndef CURVERENDERER_HPP
#define CURVERENDERER_HPP
// Include shader
#include "Shader.hpp"

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>



class CurveRenderer
{
public:
    // constructor
	CurveRenderer() : m_VAOcurve(0), m_VAOconstruct(0), m_VAOlines(0),
	    m_VBOcurve(0), m_VBOconstruct(0), m_VBOlines(0), m_VBOcolorlines(0){}

    // initialisation of VAOs, VBOs
	void init();

    // feed VBOs with data
	void setUpVBO(std::vector<glm::vec3>& c, std::vector<glm::vec3>& v, int iteration, int nbPoints);

	// draw curves
	void draw(const Shader & shaderCurve, const Shader & shaderEdition, const Shader & shaderConstruct,
              bool hideConstruction = false, bool showSteps = false);

	// destroy VAOS, VBOS
	void cleanUp();

	// clear datas
	void clear();

private:
	unsigned int m_VAOcurve, m_VAOconstruct, m_VAOlines;
	unsigned int m_VBOcurve, m_VBOconstruct, m_VBOlines, m_VBOcolorlines;

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_controlPoints;
    std::vector<glm::vec3> m_lines, _linecolors;

    // function to construct intermediate lines
    void constructionLines(const std::vector<glm::vec3> &in_pts, float u,
                           std::vector<glm::vec3> &positions, std::vector<glm::vec3> & colors);
};


#endif

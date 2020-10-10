#include "CurveRenderer.hpp"

// *********************************************************************************************************************
// CODE IMPLEMENTATION
// *********************************************************************************************************************

void CurveRenderer::init() {
    glGenVertexArrays(1, &m_VAOcurve);
    glGenBuffers(1, &m_VBOcurve);

    glGenVertexArrays(1, &m_VAOconstruct);
    glGenBuffers(1, &m_VBOconstruct);

    glGenVertexArrays(1, &m_VAOlines);
    glGenBuffers(1, &m_VBOlines);
    glGenBuffers(1, &m_VBOcolorlines);
}

/* @in_pts : points de controle
 * @positions : pointer sur les positions des points formant les lignes de construction
*/
void CurveRenderer::constructionLines(const std::vector<glm::vec3> & in_pts, float u, std::vector<glm::vec3> & positions, std::vector<glm::vec3> & colors){
    if(in_pts.size() != 1){
        for(unsigned int i = 0; i <  in_pts.size() -1 ; i++){ // on enregistre les points actuels
            positions.push_back(in_pts.at(i));
            positions.push_back(in_pts.at(i+1));
            colors.emplace_back((sin(in_pts.size()/2.0)+1.0) / 2.0 , (cos(in_pts.size()/2.0)+1.0) / 2.0 + 0.1, 0.8);
            colors.emplace_back((sin(in_pts.size()/2.0)+1.0) / 2.0 , (cos(in_pts.size()/2.0)+1.0) / 2.0 + 0.1, 0.8);
        }
        std::vector<glm::vec3> pts;
        for(unsigned int i = 0 ; i < in_pts.size() - 1; i++){// on produit les points pour l'appel suivant
            glm::vec3 vecteur = in_pts.at(i + 1 ) - in_pts.at(i);
            vecteur = vecteur * u;
            pts.push_back(in_pts.at(i) + vecteur);
        }
        constructionLines(pts, u, positions, colors);
    }
}

void CurveRenderer::setUpVBO(std::vector<glm::vec3> &c, std::vector<glm::vec3> &v, int iteration, int nbPoints) {
    m_vertices = v;
    glBindVertexArray(m_VAOcurve);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOcurve);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*m_vertices.size(), m_vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
    glEnableVertexAttribArray(0);

    m_controlPoints = c;
    glBindVertexArray(m_VAOconstruct);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOconstruct);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*m_controlPoints.size(), m_controlPoints.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
    glEnableVertexAttribArray(0);

    auto etape = (float) iteration;
    if(etape < 0) etape = 0;
    if(etape > (float)nbPoints) etape = (float)nbPoints;
    m_lines.clear();
    _linecolors.clear();
    constructionLines(c, (float) etape/ (float) nbPoints, m_lines, _linecolors);
    glBindVertexArray(m_VAOlines);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOlines);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*m_lines.size(), m_lines.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(m_VAOlines);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOcolorlines);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*_linecolors.size(), _linecolors.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
    glEnableVertexAttribArray(1);
}



void CurveRenderer::cleanUp() {
    glDeleteVertexArrays(1, &m_VAOcurve);
    glDeleteVertexArrays(1, &m_VAOconstruct);
    glDeleteVertexArrays(1, &m_VAOlines);
    glDeleteBuffers(1, &m_VBOcurve);
    glDeleteBuffers(1, &m_VBOconstruct);
    glDeleteBuffers(1, &m_VBOlines);
    glDeleteBuffers(1, &m_VBOcolorlines);
}

void CurveRenderer::draw(const Shader & shaderCurve, const Shader & shaderEdition, const Shader & shaderConstruct, bool hideConstruction, bool showSteps) {
    glLineWidth(8.0);
    shaderCurve.use();
    glBindVertexArray(m_VAOcurve);
    glDrawArrays(GL_LINE_STRIP, 0, m_vertices.size());

    if (!hideConstruction){
        shaderEdition.use();
        glBindVertexArray(m_VAOconstruct);
        glDrawArrays(GL_POINTS, 0, m_controlPoints.size());
        glPointSize(15.0f);
        glLineWidth(2.0);
        glDrawArrays(GL_LINE_STRIP, 0, m_controlPoints.size());
    }

    if(showSteps) {
        shaderConstruct.use();
        glBindVertexArray(m_VAOlines);
        glLineWidth(2.0);
        glDrawArrays(GL_LINES, 0, m_lines.size());
    }
}

void CurveRenderer::clear() {
    m_vertices.clear();
    m_controlPoints.clear();
}

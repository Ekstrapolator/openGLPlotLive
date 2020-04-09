//
// Created by bcub3d-desktop on 28/3/20.
//


#include "plot.h"

namespace GLPL {

    Plot::Plot(float x, float y, float width, float height, std::shared_ptr<IWindow> windowPt, Shader* textShaderPt) :
            axes(0.15,0.15,0.80,0.75,windowPt,textShaderPt) {
        // Set Size and Position
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->windowPt = windowPt;

        // Setup Buffers
        createAndSetupBuffers();

        // Setup Font Shader
        setupFontShader(windowPt->getWidth(),windowPt->getHeight());
    }

    Plot::~Plot() {
        std::cout << "Destructed Plot" << std::endl;
    }

    void Plot::createAndSetupBuffers() {
        /* Create Buffers */
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);

        /* Setup Buffers */
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER, boxVerts.size()*sizeof(GLfloat),&boxVerts[0],GL_STATIC_DRAW);

        /* Position Attributes */
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

        glBindVertexArray(0);
    }

    void Plot::setupFontShader(GLuint screenWidth, GLuint screenHeight) {
        // Sets up the uniforms for the font shader
        this->axes.getTextShaderPt()->Use();
        glm::mat4 textProjection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);
        glUniformMatrix4fv(glGetUniformLocation(this->axes.getTextShaderPt()->Program,"textProjection"), 1, GL_FALSE, glm::value_ptr(textProjection));
    }

    void Plot::drawPlotOutline(Shader shader, glm::mat4 plotViewportTrans) {
        // Draw temp box for plot limits
        shader.Use();
        glUniformMatrix4fv(glGetUniformLocation(shader.Program,"transformViewport"), 1, GL_FALSE, glm::value_ptr(plotViewportTrans));
        glm::vec4 inColor = glm::vec4(1.0,0.0,0.0,1.0);
        glUniform4fv(glGetUniformLocation(shader.Program,"inColor"),1,glm::value_ptr(inColor));
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_LOOP,0,4);
        glBindVertexArray(0);
    }

    void Plot::Draw(Shader shader) {
        // Calculate Viewport Transformation
        glm::mat4 plotViewportTrans = GLPL::Transforms::viewportTransform(x, y, width, height);

        // Draw Plot Box Obutline
        drawPlotOutline(shader,plotViewportTrans);

        // Draw Axes
        axes.Draw(shader, plotViewportTrans);
    }

    void Plot::addLine(std::shared_ptr<ILine2D> linePt) {
        this->axes.addLine(linePt);
    }

    void Plot::setAutoScaleRound(bool newAutoScaleRound) {
        this->axes.setAutoScaleRound(newAutoScaleRound);

    }

    void Plot::setEqualAxes(bool equalAxesBool) {
        this->axes.setEqualAxes(equalAxesBool);
    }


}
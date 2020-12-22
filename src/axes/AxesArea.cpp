//
// Created by tbatt on 19/06/2020.
//

#include "AxesArea.h"
#include "../shadedLines/ShadedLine2D2CircularVecs.h"
#include "../lines/Line2D2Vecs.h"

#include <utility>

namespace GLPL {

    AxesArea::AxesArea(float x, float y, float width, float height, std::shared_ptr<ParentDimensions> parentDimensions) :
        IDrawable() {
        // Set bounding box color
        boundingBoxColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        // Set Not Hoverable
        setHoverable(false);

        AxesArea::setParentDimensions(std::move(parentDimensions));

        // Initialise data
        AxesArea::setPosition(x, y);
        AxesArea::setSize(width, height);
        IDrawable::createAndSetupBuffers();

        // Calculate transform
        AxesArea::updateAxesViewportTransform();

        // Add Title
        AxesArea::addText("Axes Title", 0.5, 1.1, 14, CENTRE_BOTTOM);

        // Add Axes Label
        AxesArea::addText("x label", 0.5, -0.11, 12, CENTRE_TOP);
        AxesArea::addText("y label", -0.175, 0.5, 12, CENTRE_RIGHT);

        // Add Buttons
        AxesArea::addButton("Interactor", 1.0, 1.01, 0.08, 0.08, BOTTOM_RIGHT, true);
        AxesArea::addButton("Axes Limits Scaling", 0.91, 1.01, 0.08, 0.08, BOTTOM_RIGHT, true);
        AxesArea::addButton("Grid", 0.82, 1.01, 0.08, 0.08, BOTTOM_RIGHT, true);

        // Create Interactor
        AxesArea::createInteractor();

        // Create Grid
        AxesArea::createGrid();

    }

    std::string AxesArea::getID() {
        return "AxesArea:" + std::to_string(x) + ":" + std::to_string(y);
    }

    void AxesArea::Draw() {
        // Update Axes Limits
        AxesArea::updateAxesLimits();

        // Draw Axes
        for(auto & i : axesLines) {
            i.second->Draw();
        }

        // Draw Grid
        AxesArea::drawGrid();

        // Scissor Test
        glEnable(GL_SCISSOR_TEST);
        glScissor(xPx, yPx, widthPx, heightPx);

        // Draw lines
        for(auto & i : lineMap) {
            i.second->Draw();
        }

        // Disable Scissor Testing
        glDisable(GL_SCISSOR_TEST);

        // Draw attachments
        for(auto & i : axesItems) {
            i->Draw();
        }

        // Draw Text
        for(auto & i : textStringMap) {
            i.second->Draw();
        }

        // Draw Buttons
        for(auto & i : buttonMap) {
            i.second->Draw();
        }

        // Draw mouse over line
        AxesArea::drawInteractor();

        // Draw Axes box
        if (axesBoxOn) {
            AxesArea::drawAxesBox();
        }

    }

    void AxesArea::setAxesBoxOn(bool axesBoxOnBool) {
        axesBoxOn = axesBoxOnBool;
    }

    void AxesArea::setAxesBoxColor(glm::vec4 newAxesBoxColour) {
        axesBoxColor = newAxesBoxColour;
    }

    void AxesArea::addAxesLine(const std::string& axesName, AxesDirection axesDirection) {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Create Axes
        std::shared_ptr<AxesLineTicks> newAxes = std::make_shared<AxesLineTicks>(axesDirection, newParentPointers);
        // Store Axes
        axesLines.insert(std::pair<std::string, std::shared_ptr<AxesLineTicks>>(axesName, newAxes));
        // Register as a child
        AxesArea::registerChild(newAxes);
    }

    void AxesArea::setAxesLimits(float newXMin, float newXMax, float newYMin, float newYMax) {
        // Set values
        xmin = newXMin;
        xmax = newXMax;
        ymin = newYMin;
        ymax = newYMax;
        // Update Axes Lines
        for(const auto& axesLine : axesLines) {
            axesLine.second->setMinMax(newXMin, newXMax, newYMin, newYMax);
        }
        // Update Grid
        grid->setMinMax(newXMin, newXMax, newYMin, newYMax);
        std::vector<float> xAxesPos = axesLines.at("x")->getAxesTickPos();
        std::vector<float> yAxesPos = axesLines.at("y")->getAxesTickPos();
        grid->setXLines(xAxesPos);
        grid->setYLines(yAxesPos);
        // Update Axes Area
        AxesArea::updateAxesViewportTransform();
    }

    std::vector<float> AxesArea::calculateScissor(glm::mat4 axesLimitsViewportTrans) {
        // Calculate corners of axes limits area
        glm::vec4 a = axesLimitsViewportTrans * glm::vec4(xmin,ymin,0,1); // -1 to 1
        glm::vec4 b = axesLimitsViewportTrans * glm::vec4(xmax,ymax,0,1); // -1 to 1
        // Transform back to 0 to 1
        float x1 = 0.5*a[0] + 0.5;
        float y1 = 0.5*a[1] + 0.5;
        float x2 = 0.5*b[0] + 0.5;
        float y2 = 0.5*b[1] + 0.5;
        // Form vector
        std::vector<float> xyVec = {x1,y1,x2,y2};

        return xyVec;
    }

    std::shared_ptr<ILine2D> AxesArea::addLine(std::vector<float> *dataPtX, std::vector<float> *dataPtY,
            LineType lineType, glm::vec3 colour, float opacityRatio) {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Create Line
        std::shared_ptr<IDrawable> lineObj;
        std::shared_ptr<ILine2D> linePt;

        switch(lineType) {
            case SINGLE_LINE: {
                lineObj = std::make_shared<Line2D2Vecs>(dataPtX, dataPtY, newParentPointers);
                linePt = std::dynamic_pointer_cast<Line2D2Vecs>(lineObj);
                break;
            }
            case SHADED_LINE: {
                lineObj = std::make_shared<ShadedLine2D2CircularVecs>(dataPtX, dataPtY, newParentPointers);
                linePt = std::dynamic_pointer_cast<ShadedLine2D2CircularVecs>(lineObj);
                break;
            }
            default: {
                lineObj = std::make_shared<ShadedLine2D2CircularVecs>(dataPtX, dataPtY, newParentPointers);
                linePt = std::dynamic_pointer_cast<ShadedLine2D2CircularVecs>(lineObj);
            }
        }
        // Set Attributes
        linePt->setLineColour(colour);
        linePt->setOpacityRatio(opacityRatio);

        // Register Children
        AxesArea::registerChild(lineObj);
        // Set axes area transform
        linePt->setAxesViewportTransform(axesViewportTransformation);
        // Store line
        lineMap.insert(std::pair<unsigned int, std::shared_ptr<ILine2D>>(lineCount, linePt));
        lineCount += 1;

        // Update limits for axes
        AxesArea::updateAxesLimits();

        return linePt;
    }

    std::shared_ptr<IPlotable> AxesArea::getLine(unsigned int lineId) {
        if (lineMap.count(lineId) > 0) {
            return lineMap.at(lineId);
        } else {
            std::cout << "Line " << lineId << " does not exist!" << std::endl;
            return nullptr;
        }
    }

    void AxesArea::removeLine(unsigned int lineId) {
        if (lineMap.count(lineId) > 0) {
            std::shared_ptr<IPlotable> line2Remove = lineMap.at(lineId);
            std::shared_ptr<IDrawable> drawable2Remove = std::dynamic_pointer_cast<GLPL::IDrawable>(line2Remove);
            // Remove child
            IDrawable::removeChild(drawable2Remove);
            // Remove axes
            lineMap.erase(lineId);
        } else {
            std::cout << "Cannot remove Line " << lineId << ", Line does not exist!" << std::endl;
        }
    }

    void AxesArea::addText(const char* textString, float x, float y, float fontSize, AttachLocation attachLocation) {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Register Child
        std::shared_ptr<IDrawable> textStringObj = std::make_shared<TextString>(textString, x, y, fontSize, newParentPointers);
        std::shared_ptr<TextString> textStringPt = std::dynamic_pointer_cast<TextString>(textStringObj);
        // Set pin position
        textStringPt->setAttachLocation(attachLocation);
        // Register Child
        AxesArea::registerChild(textStringObj);
        // Store Text String
        textStringMap.insert(std::pair<unsigned int, std::shared_ptr<TextString>>(textStringCount, textStringPt));
        textStringCount += 1;
    }

    std::shared_ptr<TextString> AxesArea::getText(unsigned int textStringId) {
        if (textStringMap.count(textStringId) > 0) {
            return textStringMap.at(textStringId);
        } else {
            std::cout << "TextString " << textStringId << " does not exist!" << std::endl;
            return nullptr;
        }
    }

    float AxesArea::convertMouseX2AxesX() {
        // Calculate mouse position in x axes
        // x interpolation
        float xminVal = 2*getLeft() - 1;
        float xmaxVal = 2*getRight() - 1;
        float sx1 = (parentTransform * glm::vec4(xminVal, 0.0f, 0.5f, 1.0f))[0];;
        float sx2 = (parentTransform * glm::vec4(xmaxVal, 0.0f, 0.5f, 1.0f))[0];
        float ax1 = xmin;
        float ax2 = xmax;
        float mx = (ax2 - ax1) / (sx2 - sx1);
        float cx = ax2 - (mx*sx2);
        float mouseXAx = (mx*(float)mouseX) + cx;

        return mouseXAx;
    }

    float AxesArea::convertMouseY2AxesY() {
        // Calculate mouse position in y axes
        // y interpolation
        float yminVal = 2*getBottom() - 1;
        float ymaxVal = 2*getTop() - 1;
        float sx1 = (parentTransform * glm::vec4(0.0f, yminVal, 0.5f, 1.0f))[1];;
        float sx2 = (parentTransform * glm::vec4(0.0f, ymaxVal, 0.5f, 1.0f))[1];
        float ax1 = ymin;
        float ax2 = ymax;
        float mx = (ax2 - ax1) / (sx2 - sx1);
        float cx = ax2 - (mx*sx2);
        float mouseYAx = (mx*(float)mouseY) + cx;

        return mouseYAx;
    }

    float AxesArea::convertMouseX2RelativeX() {
        // Calculate mouse position in relative x
        // x interpolation
        float rx1 = getLeft();
        float rx2 = getRight();
        float ax1 = (overallTransform * glm::vec4(getLeft(), 0.0f, 0.5f, 1.0f))[0];
        float ax2 = (overallTransform * glm::vec4(getRight(), 0.0f, 0.5f, 1.0f))[0];
        float mx = (rx2 - rx1) / (ax2 - ax1);
        float cx = rx2 - (mx*ax2);
        float mouseRelX = (mx*(float)mouseX) + cx;
        mouseRelX = (mouseRelX + 1) / 2.0f;

        return mouseRelX;
    }

    float AxesArea::convertMouseY2RelativeY() {
        // Calculate mouse position in relative y
        // y interpolation
        float ry1 = getLeft();
        float ry2 = getRight();
        float ay1 = (overallTransform * glm::vec4(0.0f,getBottom(), 0.5f, 1.0f))[1];
        float ay2 = (overallTransform * glm::vec4(0.0f,getTop(), 0.5f, 1.0f))[1];
        float mx = (ry2 - ry1) / (ay2 - ay1);
        float cx = ry2 - (mx*ay2);
        float mouseRelY = (mx*(float)mouseY) + cx;
        mouseRelY = (mouseRelY + 1) / 2.0f;

        return mouseRelY;
    }

    int AxesArea::getHoverCursor() {
        if (buttonMap["Interactor"]->isActive()) {
            return GLFW_CROSSHAIR_CURSOR;
        } else {
            return 0;
        }
    }

    void AxesArea::removeTextString(unsigned int textStringId) {
        if (textStringMap.count(textStringId) > 0) {
            std::shared_ptr<TextString> textString2Remove = textStringMap.at(textStringId);
            // Remove child
            IDrawable::removeChild(textString2Remove);
            // Remove axes
            textStringMap.erase(textStringId);
        } else {
            std::cout << "Cannot remove TextString " << textStringId << ", TextString does not exist!" << std::endl;
        }
    }

    void AxesArea::addButton(const std::string& buttonName, float x, float y, float width, float height,
                             AttachLocation attachLocation, bool activeState) {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Register Child
        std::shared_ptr<IDrawable> buttonObj = std::make_shared<IButton>(buttonName, x, y, width, height, newParentPointers);
        std::shared_ptr<IButton> buttonObjPt = std::dynamic_pointer_cast<IButton>(buttonObj);
        // Set pin position
        buttonObjPt->setAttachLocation(attachLocation);
        // Set state
        buttonObjPt->setActive(activeState);
        // Register Child
        AxesArea::registerChild(buttonObj);
        // Store button
        buttonMap.insert(std::pair<std::string, std::shared_ptr<IButton>>(buttonName, buttonObjPt));
    }

    void AxesArea::setButtonState(const std::string& buttonName, bool activeState) {
        if (buttonMap.count(buttonName) > 0) {
            buttonMap.at(buttonName).get()->setActive(activeState);
        } else {
            std::cout << "Button " << buttonName << " does not exist!" << std::endl;
        }
    }

    void AxesArea::updateAxesViewportTransform() {
        // Update Transform
        glm::mat4 viewportTransform = GLPL::Transforms::viewportTransform(x, y, width, height);
        glm::mat4 axesLimitsTransform = AxesArea::scale2AxesLimits();
        axesViewportTransformation = std::make_shared<glm::mat4>(parentTransform * viewportTransform * axesLimitsTransform);
        // Update Children Lines
        for(auto & i : lineMap) {
            i.second->setAxesViewportTransform(axesViewportTransformation);
        }
        // Update Grid
        if (grid != nullptr) {
            grid->setAxesViewportTransform(axesViewportTransformation);
        }
    }

    void AxesArea::setPosition(float newX, float newY) {
        // Set position
        this->x = newX;
        this->y = newY;
        // Set position in pixels
        IDrawable::updatePositionPx();
        // Update Transforms
        IDrawable::updateTransforms();
        AxesArea::updateAxesViewportTransform();
        // Update Children
        IDrawable::updateChildren();
    }

    void AxesArea::setSize(float newWidth, float newHeight) {
        // New width and height relative to their parent
        // Update size
        this->width = newWidth;
        this->height = newHeight;
        // Update in pixels
        updateSizePx();
        // Update Transforms
        IDrawable::updateTransforms();
        AxesArea::updateAxesViewportTransform();
        // Update Children
        this->updateChildren();
    }

    void GLPL::AxesArea::updateSizePx() {
        this->widthPx = (int) (this->width * (float)parentWidthPx);
        this->heightPx = (int) (this->height * (float)parentHeightPx);
    }

    void GLPL::AxesArea::setParentDimensions(glm::mat4 newParentTransform,
                                                          int newParentXPx,
                                                          int newParentYPx,
                                                          int newParentWidthPx,
                                                          int newParentHeightPx) {
        this->parentXPx = newParentXPx;
        this->parentYPx = newParentYPx;
        this->parentWidthPx = newParentWidthPx;
        this->parentHeightPx = newParentHeightPx;
        this->parentTransform = newParentTransform;
        updatePositionPx();
        updateSizePx();
        updateTransforms();
        AxesArea::updateAxesViewportTransform();
        // Update Children
        updateChildren();
    }

    void GLPL::AxesArea::setParentDimensions(std::shared_ptr<ParentDimensions> parentDimensions) {
        this->parentTransform = parentDimensions->parentTransform;
        this->parentXPx = parentDimensions->parentXPx;
        this->parentYPx = parentDimensions->parentYPx;
        this->parentWidthPx = parentDimensions->parentWidthPx;
        this->parentHeightPx = parentDimensions->parentHeightPx;
        this->shaderSetPt = parentDimensions->shaderSet;
        updatePositionPx();
        updateSizePx();
        updateTransforms();
        AxesArea::updateAxesViewportTransform();
        // Update Children
        updateChildren();
    }

    glm::mat4 AxesArea::scale2AxesLimits() {
        // Creates a transformation matrix to scale points to the axes limits
        // Calculate center of current limits
        float xShift = ((xmin+xmax)/2.0)/(xmax-xmin) * 2.0; // xavg/width * 2.0, *2 to take it to -1 to 1
        float yShift = ((ymin+ymax)/2.0)/(ymax-ymin) * 2.0; // yavg/height * 2.0, *2 to take it to -1 to 1

        // Translate by offset
        glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(-xShift, -yShift,0));

        // Scale to limits
        float scaleX = 2.0/(xmax-xmin); // Inverted due to -1 to 1 mapping (less than abs(1) region)
        float scaleY = 2.0/(ymax-ymin); // Inverted due to -1 to 1 mapping (less than abs(1) region)
        glm::mat4 scale = glm::scale(trans, glm::vec3(scaleX,scaleY,1));

        return scale;
    }

    void GLPL::AxesArea::drawAxesBox() {
        // Draw bounding box
        std::shared_ptr<Shader> shader = shaderSetPt->getPlot2dShader();
        shader->Use();
        glUniformMatrix4fv(glGetUniformLocation(shader->Program,"transformViewport"), 1, GL_FALSE, glm::value_ptr(overallTransform));
        glUniform4fv(glGetUniformLocation(shader->Program,"inColor"),1,glm::value_ptr(axesBoxColor));
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_LOOP,0,4);
        glBindVertexArray(0);
    }

    void GLPL::AxesArea::drawGrid() {
        if (buttonMap["Grid"]->isActive()) {
            grid->Draw();
        }
    }

    void GLPL::AxesArea::drawInteractor() {
        if (buttonMap["Interactor"]->isActive()) {
            if (isHovered() && isMouseOver(mouseX, mouseY)) {
                // Calculate mouse position in x
                float mouseXAx = convertMouseX2AxesX();
                float mouseYAx = convertMouseY2AxesY();
                float mouseRelX = convertMouseX2RelativeX();
                float mouseRelY = convertMouseY2RelativeY();

                for (auto &i : lineMap) {
                    if (i.second->isSelected()) {
                        std::tuple<float, float> pt = i.second->getClosestPoint(mouseXAx);
                        if (pt != std::make_tuple(0.0, 0.0)) {
                            float x = std::get<0>(pt);
                            float y1 = std::get<1>(pt);
                            float y2 = mouseYAx;
                            // Update line
                            interactorLine->clearData();
                            interactorLine->dataPtX->push_back(x);
                            interactorLine->dataPtX->push_back(x);
                            interactorLine->dataPtY->push_back(y1);
                            interactorLine->dataPtY->push_back(y2);
                            interactorLine->updateInternalData();
                            // Update Text String
                            char textBuf[50];
                            const char *formatStr;
                            if (abs(x) > 1000 || abs(y) > 1000) {
                                formatStr = "(%.2e, %.2e)";
                            } else {
                                formatStr = "(%.2f, %.2f)";
                            }
                            sprintf(textBuf, formatStr, x, y1);
                            interactorText->setPosition((float) mouseRelX, (float) mouseRelY);
                            //std::cout << textBuf << std::endl;
                            interactorText->setTextString(textBuf);
                            // Check if the text string should be above or below
                            if (y1 > y2) {
                                interactorText->setAttachLocation(CENTRE_TOP);
                            } else {
                                interactorText->setAttachLocation(CENTRE_BOTTOM);
                            }
                        }
                    }
                }
            } else {
                interactorLine->clearData();
                interactorText->setTextString("");
            }
        } else {
            interactorLine->clearData();
            interactorText->setTextString("");
        }
    }

    void GLPL::AxesArea::updateAxesLimits() {
        if (buttonMap["Axes Limits Scaling"]->isActive()) {
            // Get the overall maximum and minimum from all lines
            xmin = -0.0;
            xmax = 0.0;
            ymin = -0.0;
            ymax = 0.0;
            for (std::pair<unsigned int, std::shared_ptr<ILine2D>> lineInfo : lineMap) {
                std::vector<float> minMax = lineInfo.second->getMinMax();
                if (minMax[0] < xmin) { xmin = minMax[0]; };
                if (minMax[1] > xmax) { xmax = minMax[1]; };
                if (minMax[2] < ymin) { ymin = minMax[2]; };
                if (minMax[3] > ymax) { ymax = minMax[3]; };
            }
            // Match axes lines sizing
            float xFontSize = axesLines.at("x")->getFontSize();
            float yFontSize = axesLines.at("y")->getFontSize();
            if (xFontSize < yFontSize) {
                axesLines.at("y")->setMajorTickFontSize(xFontSize);
            } else if (yFontSize < xFontSize) {
                axesLines.at("x")->setMajorTickFontSize(yFontSize);
            }

            // Set axes limits
            AxesArea::setAxesLimits(xmin, xmax, ymin, ymax);
        }
    }

    void GLPL::AxesArea::createInteractor() {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();

        // Create Line
        interactorLine = std::make_shared<Line2D2Vecs>(&interactorDataX, &interactorDataY, newParentPointers);
        // Register Children
        AxesArea::registerChild(interactorLine);
        // Set axes area transform
        interactorLine->setAxesViewportTransform(axesViewportTransformation);
        // Set not hoverable
        interactorLine->setHoverable(false);
        // Store line
        lineMap.insert(std::pair<unsigned int, std::shared_ptr<ILine2D>>(lineCount, interactorLine));
        lineCount += 1;

        // Create text label
        // Create label
        interactorText = std::make_shared<TextString>("", x, y, 8, newParentPointers);
        interactorText->setAttachLocation(CENTRE_BOTTOM);
        interactorText->setHoverable(false);
        // Register Child
        AxesArea::registerChild(interactorText);
        // Store Text String
        textStringMap.insert(std::pair<unsigned int, std::shared_ptr<TextString>>(textStringCount, interactorText));
        textStringCount += 1;

    }

    void GLPL::AxesArea::createGrid() {
        // Create Parent Dimensions
        std::shared_ptr<ParentDimensions> newParentPointers = IDrawable::createParentDimensions();
        // Create grid
        grid = std::make_shared<Grid>(newParentPointers);
        // Register Children
        AxesArea::registerChild(grid);
        // Set axes area transform
        grid->setAxesViewportTransform(axesViewportTransformation);
        // Set not hoverable
        grid->setHoverable(false);
    }
}
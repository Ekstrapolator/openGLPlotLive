//
// Created by wolek on 28.06.24.
//
// Standard Includes
#include <chrono>
#include <thread>

// Project Includes
#include "../../src/window/window.h"
#include "../../src/lines/Line2DVecGLMV3.h"
#include "udpServer.hpp"
#include "qformat.hpp"
#include "msg.hpp"

int main(int argc, char **argv) {
  /* Create udp class*/
  udp::UdpServer udpServer(22359);

  // Window Size
  int windowWidth = 1920;
  int windowHeight = 1080;

  // Init GLFW
  std::shared_ptr<GLPL::IWindow> window = std::shared_ptr<GLPL::IWindow>(new GLPL::Window(windowWidth, windowHeight));
  std::shared_ptr<GLPL::Window> window2 = std::dynamic_pointer_cast<GLPL::Window>(window);

  // Create Plot
  std::shared_ptr<GLPL::Plot>
      myplot = std::make_shared<GLPL::Plot>(0, 0.23, 1, 0.75, window2->getParentDimensions(), 2, 2);
  std::shared_ptr<GLPL::IDrawable> myPlotPt = std::dynamic_pointer_cast<GLPL::IDrawable>(myplot);
  window2->addPlot(myPlotPt);

  // Graph 2
  std::vector<float> xVec9;
  std::vector<float> yVec9;
  xVec9.reserve(2500);
  yVec9.reserve(2500);


  // Create axes
  std::shared_ptr<GLPL::Axes2D> axesPt = std::dynamic_pointer_cast<GLPL::Axes2D>(myplot->getAxes(0));
  // Add points to line
  std::shared_ptr<GLPL::ILine2D> line12 = axesPt->addLine(&xVec9, &yVec9, GLPL::SINGLE_LINE, LC_YELLOW, 0.5);
  // Adjust axes settings
  std::shared_ptr<GLPL::Line2D2Vecs> line12buff = std::dynamic_pointer_cast<GLPL::Line2D2Vecs>(line12);

  axesPt->setAxesBoxOn(false);
  axesPt->setButtonState("Grid", false);
  axesPt->setXLabel("Time (s)");
  axesPt->setYLabel("Pitch (degrees)");
  axesPt->setTitle("Pitch measurement");
  axesPt->setButtonState("X Axes Limits Scaling", false);
  axesPt->setButtonState("Y Axes Limits Scaling", true);
  axesPt->setYLabelRotation(GLPL::SIDEWAYS_RIGHT);

  size_t i{1}, ix{1};
  LogPacket buff[10];
  while (!glfwWindowShouldClose(window->getWindow())) {

    // Pre-loop draw
    window2->preLoopDraw(true);
    udpServer.reciveData(reinterpret_cast<uint8_t*>(&buff[0]), sizeof(LogPacket));
    xVec9.push_back(static_cast<float>(i));
    yVec9.push_back(4 * sin(i / 100.0));
    line12buff->updateInternalData();

    i++;

    // Get range of last 1000 points, provided the points in the a axis are in order
    unsigned int minInd = std::max((long) 0, (long) line12buff->dataPtX->size() - 1000);
    unsigned int maxInd = line12buff->dataPtX->size() - 1;
    float xmin = (*line12buff->dataPtX)[minInd];
    float xmax = (*line12buff->dataPtX)[maxInd];
    axesPt->setXAxesLimits(xmin, xmax);


    // Draw Plot
    myplot->Draw();

    // Post-loop draw
    window2->postLoopDraw();
  }
}
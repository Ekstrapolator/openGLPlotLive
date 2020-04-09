//
// Created by bcub3d-desktop on 28/3/20.
//

#include "window.h"
#include "inputCallbacks.h"

namespace GLPL {

    Window::Window(int windowWidth, int windowHeight, int transparentBackground) {
        // Set window size
        this->windowWidth = windowWidth;
        this->windowHeight = windowHeight;
        this->transparentBackground = transparentBackground;

        // Initialise GLFW
        Window::initGLFW();

        // Update Stored Size
        Window::updateStoredSize();

    }

    void Window::initGLFW() {
        // Init GLFW
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE); // Set core profile

        // Make background transparent
        if (transparentBackground) {
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
        }

        // Screen Properties
        window = glfwCreateWindow(windowWidth,windowHeight,"openGLPlotLive",nullptr,nullptr);
        glfwMakeContextCurrent(window);

        // Initialise GLAD
        Window::initGLAD();

        // Setup Callbacks for user input
        glfwSetKeyCallback(window, GLPL::key_callback);
        glfwSetWindowSizeCallback(window, GLPL::reDraw);

        // Set viewport size
        glViewport(0,0,windowWidth,windowHeight); // Origin is bottom left

        // Test for objects in front of each other
        glEnable(GL_DEPTH_TEST);

        // Line Width
        glLineWidth(1);
    }

    void Window::updateStoredSize() {
        // Updates the stored window size used for scaling and
        // transformations with the current window size.
        glfwGetWindowSize(window,&windowWidth,&windowHeight);
        // Update Viewport Dimensions
        glViewport(0, 0, windowWidth, windowHeight);
    }

    void Window::initGLAD() {
        // Initialise GLAD - setup OpenGL pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }
    }

    void Window::setKeysByIndex(int index, bool boolean) {
        keys[index] = boolean;
    }

    void Window::setToggleKeysByIndex(int index, bool boolean) {
        toggleKeys[index] = boolean;
    }

    bool Window::getToggleKeyStateByIndex(int index) {
        return toggleKeys[index];
    }

    GLFWwindow* Window::getWindow() {
        return this->window;
    }

    void Window::preLoopDraw(bool clearBuffer) {
        // To be run before a drawing inside a loop or single draw call is used
        // clearBuffer:	True if the colour buffer should be reset
        //				Set to false for transparent background

        // Check Events
        glfwPollEvents();

        // Update window size
        Window::updateStoredSize();

        if (clearBuffer) {
            // Clear the colour buffer
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }

    void Window::postLoopDraw() {
        // Swap buffers
        // To be run after drawing inside a loop or after a single draw call is used
        glfwSwapBuffers(Window::getWindow());
    }

    int Window::getWidth() {
        return windowWidth;
    }

    int Window::getHeight() {
        return windowHeight;
    }

    void Window::setFrameless(bool framelessOn) {
        glfwSetWindowAttrib(window, GLFW_DECORATED, !framelessOn);
    }

    void Window::setAlwaysOnTop(bool alwaysOnTop) {
        glfwSetWindowAttrib(window, GLFW_FLOATING , alwaysOnTop);
    }

};
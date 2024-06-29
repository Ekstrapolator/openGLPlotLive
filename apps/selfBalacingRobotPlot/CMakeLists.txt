project(robotPlot)

# Resolves issue with unable to debug with CLion, reverts back to g++9
add_compile_options(-gdwarf-4)

# Create the exectuable
add_executable(robotPlot main.cpp)

add_library(udpServer_lib
        STATIC
        UdpServer.cpp
)

target_include_directories(udpServer_lib
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(robotPlot
        PRIVATE
        openGLPlotLive ${LIBS}
        udpServer_lib)


# Copy Shader files to build directory
add_custom_command(TARGET robotPlot PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ../../../Shaders $<TARGET_FILE_DIR:robotPlot>/Shaders)

# Copy Texture files to build directory
add_custom_command(TARGET robotPlot PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ../../../textures $<TARGET_FILE_DIR:robotPlot>/textures)
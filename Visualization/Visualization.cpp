#include <glad/glad.h>
#include <glfw3.h>

#include <random>
#include <iostream>
#include <iterator>
#include <fstream>

#include "ConfigurationReader.h"

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

void saveColors(std::vector<float>& colors, std::string path)
{
    std::ofstream outFile(path);
    for (const auto& e : colors) outFile << e << "\n";
}

struct Point
{
    float x, y;
};

struct Triangle
{
    Point middle, left, right;
};

Triangle calculateTriangleByIndex(uint64_t index, uint64_t n)
{
    uint64_t row = index / n;
    uint64_t column = index % n;

    float dx;
    float dy;

    Point middle, left, right;

    if (n % 2 == 0)
    {
        dx = float(4) / (n + 1) / sqrt(3);
        dy = float(2) / n;

        if (row % 2 == 0)
        {
            if (index % 2 == 0)
            {
                middle.x = -1 + dx * float(column + 1) / 2;
                middle.y = 1 - dy * float(row);

                left.x = middle.x - dx / 2;
                left.y = middle.y - dy;

                right.x = middle.x + dx / 2;
                right.y = middle.y - dy;
            }
            else
            {
                middle.x = -1 + dx * float(column + 1) / 2;
                middle.y = 1 - dy * float(row + 1);

                left.x = middle.x - dx / 2;
                left.y = middle.y + dy;

                right.x = middle.x + dx / 2;
                right.y = middle.y + dy;
            }
        }
        else
        {
            if (index % 2 == 0)
            {
                middle.x = -1 + dx * float(column + 1) / 2;
                middle.y = 1 - dy * float(row + 1);

                left.x = middle.x - dx / 2;
                left.y = middle.y + dy;

                right.x = middle.x + dx / 2;
                right.y = middle.y + dy;
            }
            else
            {
                middle.x = -1 + dx * float(column + 1) / 2;
                middle.y = 1 - dy * float(row);

                left.x = middle.x - dx / 2;
                left.y = middle.y - dy;

                right.x = middle.x + dx / 2;
                right.y = middle.y - dy;
            }
        }
    }
    else
    {
        dx = float(2) / (float(n / 2) + float(1))  / sqrt(3);
        dy = float(2) / n;

        if (index % 2 == 0)
        {
            middle.x = -1 + dx * (float(column) + 1) / 2;
            middle.y = 1 - float(row) * dy;

            left.x = middle.x - dx / 2;
            left.y = middle.y - dy;

            right.x = middle.x + dx / 2;
            right.y = middle.y - dy;
        }
        else
        {
            middle.x = -1 + dx * (float(column) + 1) / 2;
            middle.y = 1 - float(row+1) * dy;

            left.x = middle.x - dx / 2;
            left.y = middle.y + dy;

            right.x = middle.x + dx / 2;
            right.y = middle.y + dy;
        }
    }
    
    return { middle,left,right };
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

static int triangleVisualization(std::string path, double sec, std::string screenName)
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Triangular lattice", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // add a new set of vertices to form a second triangle (a total of 6 vertices); 
    // the vertex attribute configuration remains the same (still one 3-float position vector per vertex)
    ConfigurationReader reader(path, sec);
    reader.latticeToPixelmap(SCR_WIDTH);

    const uint64_t Num_Of_Triangles = 1e4;
    const uint64_t n = sqrt(Num_Of_Triangles);

    std::vector<Triangle> verticesOfTriangle(Num_Of_Triangles);
    std::vector<float> vertices(18 * Num_Of_Triangles);

    for (uint64_t index = 0; index < Num_Of_Triangles; ++index)
        verticesOfTriangle[index] = calculateTriangleByIndex(index, n);

    for (uint64_t index = 0; index < Num_Of_Triangles; ++index)
    {
        //left
        vertices[18 * index] = verticesOfTriangle[index].left.x;
        vertices[18 * index+1] = verticesOfTriangle[index].left.y;
        vertices[18 * index + 2] = 0.0f;
        vertices[18 * index + 3] = reader._configuration[index];
        vertices[18 * index + 4] = 0.5f;
        vertices[18 * index + 5] = 0.0f;
        //right
        vertices[18 * index + 6] = verticesOfTriangle[index].right.x;
        vertices[18 * index + 7] = verticesOfTriangle[index].right.y;
        vertices[18 * index + 8] = 0.0f;
        vertices[18 * index + 9] = reader._configuration[index];
        vertices[18 * index + 10] = 0.5f;
        vertices[18 * index + 11] = 0.0f;
        //mid
        vertices[18 * index+12] = verticesOfTriangle[index].middle.x;
        vertices[18 * index + 13] = verticesOfTriangle[index].middle.y;
        vertices[18 * index + 14] = 0.0f;
        vertices[18 * index + 15] = reader._configuration[index];
        vertices[18 * index + 16] = 0.5f;
        vertices[18 * index + 17] = 0.0f;
    }

    //const uint64_t n = SCR_WIDTH;
    //const uint64_t Num_Of_Triangles = n * n;
    //std::vector<Triangle> verticesOfTriangle(Num_Of_Triangles);
    //std::vector<float> vertices(18 * Num_Of_Triangles);

    //for (uint64_t index = 0; index < Num_Of_Triangles; ++index)
    //    verticesOfTriangle[index] = calculateTriangleByIndex(index, n);

    //float yellow[3] = { 1.0f, 1.0f, 0.0f };
    //float blue[3] = { 0.0f, 0.0f, 1.0f };
    //float color[3] = {0, 0, 0};

    //for (uint64_t index = 0; index < Num_Of_Triangles; ++index)
    //{
    //    color[2] = reader._pixelMap[index];
    //    //if (reader._pixelMap[index] > 0.0f)
    //    //    std::copy(std::begin(blue), std::end(blue), std::begin(color));
    //    //else
    //    //    std::copy(std::begin(yellow), std::end(yellow), std::begin(color));

    //    //left
    //    vertices[18 * index] = verticesOfTriangle[index].left.x;
    //    vertices[18 * index+1] = verticesOfTriangle[index].left.y;
    //    vertices[18 * index + 2] = 0.0f;
    //    vertices[18 * index + 3] = color[0];
    //    vertices[18 * index + 4] = color[1];
    //    vertices[18 * index + 5] = /*reader._pixelMap[index]*/color[2];
    //    //right
    //    vertices[18 * index + 6] = verticesOfTriangle[index].right.x;
    //    vertices[18 * index + 7] = verticesOfTriangle[index].right.y;
    //    vertices[18 * index + 8] = 0.0f;
    //    vertices[18 * index + 9] = color[0];
    //    vertices[18 * index + 10] = color[1];
    //    vertices[18 * index + 11] = /*reader._pixelMap[index]*/color[2];
    //    //mid
    //    vertices[18 * index+12] = verticesOfTriangle[index].middle.x;
    //    vertices[18 * index + 13] = verticesOfTriangle[index].middle.y;
    //    vertices[18 * index + 14] = 0.0f;
    //    vertices[18 * index + 15] = color[0];
    //    vertices[18 * index + 16] = color[1];
    //    vertices[18 * index + 17] = /*reader._pixelMap[index]*/color[2];
    //}

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUseProgram(shaderProgram);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3 * Num_Of_Triangles); // set the count to 6 since we're drawing 6 vertices now (2 triangles); not 3!
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void justWriteData(std::string path, double sec, std::string colorsFileName)
{
    ConfigurationReader reader(path, sec);
    reader.latticeToPixelmap(SCR_WIDTH);
    saveColors(reader._pixelMap, colorsFileName);
    return;
}

int main(int argc, char* argv[])
{
    //justWriteData(argv[1], std::stod(argv[2]), argv[3]);
    triangleVisualization(argv[1], std::stod(argv[2]), argv[3]);
}
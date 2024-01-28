#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cstdlib>          // EXIT_FAILURE

using namespace std; // Uses the standard namespace
/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Tutorial 2.2"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    struct GLMesh {
        unsigned int vao; //vertex array object
        unsigned int vbos[2]; //handles for the vbo
        unsigned int nIndices;
    };
    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMesh;
    // Shader program
    unsigned int gProgramId;
}


/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
//need to leave these globally defined for now
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
void URender();
void UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, unsigned int& programId);
void UDestroyShaderProgram(unsigned int programId);

/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1
out vec4 vertexColor; // variable to transfer color data to the fragment shader
uniform mat4 shaderTransform; // 4x4 matrix variable for transforming vertex data
void main()
{
    gl_Position = shaderTransform * vec4(position, 1.0f); // transforms vertex data using matrix
    vertexColor = color; // references incoming color data
}
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec4 vertexColor; // Variable to hold incoming color data from vertex shader
out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(vertexColor);
}
);

// main function. Entry point to the OpenGL program
int main(int argc, char* argv[])
{
    //// Main GLFW window
    //GLFWwindow* window = nullptr; 

    ////create my mesh struct object
    //GLMesh mesh;
    
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    //create a mesh before beginning our render loop
    UCreateMesh(gMesh);

    //create the shader
    UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId);

    // Set the background color of the window to black (it will be implicitely used by glClear).
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // input
        // -----
        UProcessInput(gWindow);

        //call our render function to render our triangle, buffer swapping happens in that function so we dont need to do it in here
        URender();

        glfwPollEvents();
    }
    //destroy mesh
    UDestroyMesh(gMesh);

    //destroy shader program
    UDestroyShaderProgram(gProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}

bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure (specify desired OpenGL version)
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}

//input polling
void UProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}



//function for actually rendering, in this I set my background color and ensure to clear the buffer
// set the shader by Using the Program, bind my vertex arrays and actually draw the triangle, then deactivate the array object
//any transformation we want to apply to a matrix is done here before the elements are drawn
void URender() {
    
    //clear the background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. Scales the shape down by half of its original size in all 3 dimensions
    glm::mat4 scale = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));

    // 2. Rotates shape by 45 degrees on the z axis
    glm::mat4 rotation = glm::rotate(45.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    // 3. Translates by 0.5 in the y axis
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.5f, 0.0f));

    // Transformations are applied right-to-left order
    glm::mat4 transformation = translation * rotation * scale;

    // set the shader being used
    glUseProgram(gProgramId);

    // Sends transform information to the Vertex shader
    GLuint transformLocation = glGetUniformLocation(gProgramId, "shaderTransform");
    glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformation));

    //activate my vertex buffer object by binding the array object its related to
    glBindVertexArray(gMesh.vao);

    //draw the mofo by calling the type of shape, the starting index in the buffer, and the size of buffer which is held by the nvertices variable
    //draw elements as opposed to arrays allows us to work through indices of an array so we can reuse vertex points
    glDrawElements(GL_TRIANGLES, gMesh.nIndices,GL_UNSIGNED_SHORT,NULL);

    //deactivates the array object
    glBindVertexArray(0);

    //buffer swap flips buffers constantly so that we can poll for events in between rendering, for keystrokes and junk
    glfwSwapBuffers(gWindow);

}

//create the mesh with my triangle vertex points
void UCreateMesh(GLMesh& mesh)
{
    // Specifies Normalized Device Coordinates (x,y,z) and color (r,g,b,a) for triangle vertices
    GLfloat verts[] =
    {
        // Vertex Positions    // Colors
    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f, // Top-Right Vertex 0
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, 1.0f, // Bottom-Right Vertex 1
   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f, // Bottom-Left Vertex 2
   -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f, 1.0f  // Top-Left Vertex 3
    };
   

    //generate my vertex array object so I can bind it to a buffer
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

   //create 2 buffers, 1 for vertex data and another for the indices of the array
    glGenBuffers(2, mesh.vbos); //creates
    glBindBuffer(GL_ARRAY_BUFFER,mesh.vbos[0]); //activates
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); //send the data to the gpu

   
    //buffer object for the indices
    unsigned short indices[] = { 0,1,3, //triangle 1 for 1/2 of a square
                                 1,2,3       }; //triangle 2 for 1/2 of square
    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),indices, GL_STATIC_DRAW);


    //create a vertex attribute pointer and we start at 0 since we are beginning with the first index location of our array 
    const unsigned int floatsPerVertex = 3; //number of coordinates used to establish a vertex
    const unsigned int floatsPerColor = 4;

    //we need to set the strides in between vertex coordinates, which will be six for now since we aren't using the 3d point space
    int stride = sizeof(float) * (floatsPerVertex + floatsPerColor);
    
    //create the pointers for both the vertex and the colors
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0); //enable the array

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride,(char*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);
}

//destroy the mesh
void UDestroyMesh(GLMesh& mesh) {
    glDeleteVertexArrays(1, &mesh.vao); //delete my vertex array
    glDeleteBuffers(2, mesh.vbos); //delete my buffer object
}

//function to create our shader program that takes a vertex shader, fragment shader, and program id number
void UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, unsigned int &programId) {
    
    //create shader program object
    programId = glCreateProgram();

    //create the vertex and shader objects which are integers because opengl uses these integers as a pointer of sort, like a handle
    //to access the shader and program information without calling it directly by name, just a vague abstract identifier thats a number 
    unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    //retrieving the shader source, written up above the actualy shaders themselves and binding them to our shader objects in this shader program
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    //compile the shaders
    glCompileShader(vertexShaderId);
    glCompileShader(fragmentShaderId);

    //attached compiled shaders to the shader program, once again we know these are integers, however their numbers are basically like the index of a hashmap
    //for opengl to then go look for
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    //link our program
    glLinkProgram(programId);

}

//removing the shader
void UDestroyShaderProgram(unsigned int programId) {
    glDeleteProgram(programId);
}

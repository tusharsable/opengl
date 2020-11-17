
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include<sstream>
#include <GLUT/glut.h> 


// state variables tied to keyboard input
static bool running = true;
static bool pause = false;
static float rotate_x = 0.0, drot_x = 0.0;
static float rotate_y = 0.0, drot_y = 0.0;
static float rotate_z = 0.0, drot_z = 0.0;

// keyboard input callback function
static void key_callback(GLFWwindow* window, int key, int scancode,
    int action, int mods)
{
    // quit application when the user presses ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        running = false;
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // pause application when user presses SPACEBAR and keeps
    // it pressed
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        pause = true;
    }

    // continue running the application when the user releases
    // the SPACEBAR
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        pause = false;
    }

    // rotate about Z axis if user presses the RIGHT arrow key 
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        drot_z = 1.0;
    }
    // stop rotation when the key is released
    if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
        drot_z = 0.0;
    }

    // rotate about -Z axis if user presses the LEFT arrow key 
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        drot_z = -1.0;
    }
    // stop rotation when the key is released
    if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
        drot_z = 0.0;
    }

    // rotate about X axis if user presses the UP arrow key 
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        drot_x = -1.0;
    }
    // stop rotation when the key is released
    if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
        drot_x = 0.0;
    }

    // rotate about -X axis if user presses the DOWN arrow key 
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        drot_x = 1.0;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
        drot_x = 0.0;
    }
}



struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;

    ShaderProgramSource(std::string string1,std::string string2)
        :VertexSource(string1),FragmentSource(string2)
    {

    }
};


static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE=-1,VERTEX=0,FRAGMENT=1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;


    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                //set mode to vertex
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                //set mode to fagment
                type = ShaderType::FRAGMENT;

        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }
    
    return ( ShaderProgramSource( ss[0].str(), ss[1].str()) );
}

static unsigned int CompileShader( unsigned int type,const std::string& source )
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result==GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message =(char*) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile"<<(type==GL_VERTEX_SHADER? "vertex":"fragment")
            <<" shader\n";
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program= glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return program;

}
GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

extern void drawGeometry(GLFWwindow*, float*, int, float, float, float);

//int main()
int render(char* data,unsigned int numberOfTriangles,int fileSize)
{ 
        
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);



    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1080, 720, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

//    std::cout << glGetString(GL_VERSION) << std::endl;

    // register the keyboard input callback function defined at the top
    glfwSetKeyCallback(window, key_callback);

    float positions[9] = { -0.5f, -0.5f,
                             0.5f, -0.5f,
                             0.5f, 0.5f,
                             0.5f, 0.5f,
                             -0.5f
 
    };


      unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float),positions,GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER,(unsigned int)fileSize, data, GL_STATIC_DRAW);


    /*
    // enable use of vertex coordinate information from the array
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(float) * 6, 0);

    // enable use of vertex normal information from the array
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof(float) * 6, (GLvoid*)(sizeof(float)*3));*/
    // fill mode or wireframe mode
    glPolygonMode(GL_FRONT,  // options: GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
        GL_FILL);           // options: GL_POINT, GL_LINE, GL_FILL (default)

// shading model
    glShadeModel(GL_SMOOTH);
   

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << "Vertex" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "Fragment" << std::endl;
    std::cout << source.FragmentSource << std::endl;

    unsigned int shader = CreateShader(source.VertexSource,source.FragmentSource);
    glUseProgram(shader);




    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT); 

        drawGeometry(window,(float*)data, 3 * numberOfTriangles,
            rotate_x, rotate_y, rotate_z);
        glCheckError();

        if (!pause)
        {

            // changes viewing angle
            rotate_x += drot_x;
            rotate_y += drot_y;
            rotate_z += drot_z;


            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
     }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}
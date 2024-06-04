#include "Wall.h"
#include "stb_image.h"

const char* Wall::vertexShaderSource = R"glsl(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;

    uniform mat4 projectionMatrix;

    void main()
    {
        gl_Position = projectionMatrix * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
)glsl";

const char* Wall::fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;

    in vec2 TexCoord;

    uniform sampler2D texture1;

    void main()
    {
        FragColor = texture(texture1, TexCoord);
    }
)glsl";

Wall::Wall(const char* texturePath, float verts[]){
    // Load texture using stb_image
    int width, height, nrChannels;
    for (int i = 0; i < 20; i++){
        vertices[i] = verts[i];
    }
    unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
        return;
    }

    // Determine the texture format based on the number of channels
    GLenum format;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    // Generate and bind texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

}

GLuint Wall::getShaderProgram() {
    return shaderProgram;
}

Wall::~Wall() {
    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture);
}

void Wall::draw() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

GLuint Wall::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint Wall::createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void Wall::rotate(double cosValue, double sinValue, int index1, int index2, bool reverse){
    IdentityMatrix toOrigin(4);
    toOrigin[0][3] = -vertices[0];
    toOrigin[1][3] = -vertices[1];
    toOrigin[2][3] = -vertices[2];
    IdentityMatrix toOriginal(4);
    toOriginal[0][3] = vertices[0];
    toOriginal[1][3] = vertices[1];
    toOriginal[2][3] = vertices[2];

    IdentityMatrix rotate(4);
    rotate[index1][index1] = rotate[index2][index2] = cosValue;
    if (reverse){
        rotate[index1][index2] = sinValue;
        rotate[index2][index1] = -sinValue;
    } else {
        rotate[index1][index2] = -sinValue;
        rotate[index2][index1] = sinValue;
    }

    Matrix transform = toOriginal * rotate * toOrigin;
    moveScene(transform);
}


void Wall::move(float x, float y, float z){
        IdentityMatrix transform(4);
        transform[0][3] = x;
        transform[1][3] = y;
        transform[2][3] = z;
    
        moveScene(transform);
    }

void Wall::scale(float x, float y, float z){
    IdentityMatrix transform(4);
    transform[0][0] = x;
    transform[1][1] = y;
    transform[2][2] = z;

    moveScene(transform);

}

void Wall::moveScene(Matrix &transform){
    int size = 20;
    for (int j = 0; j < size; j+=5){
        double ** arr = new double*[4];
        arr[0] = new double[1]{vertices[j]};
        arr[1] = new double[1]{vertices[j + 1]};
        arr[2] = new double[1]{vertices[j + 2]};
        arr[3] = new double[1]{1};
        Matrix point(4, 1, arr);
        Matrix res = transform * point;
        vertices[j]  = res[0][0];
        vertices[j + 1] = res[1][0];
        vertices[j + 2] = res[2][0];
    }

}
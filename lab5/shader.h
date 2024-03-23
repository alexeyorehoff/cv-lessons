#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "GL/glew.h"

unsigned int CompileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    GLint compiled = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE ){
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        auto log = (char*)malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, log);
        std::cerr << "Error while compiling a " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader, log = " << log << std::endl;
        delete log;
        glDeleteShader(id);
        exit(1);
    }
    return id;
}

unsigned int CreateShader(const GLchar* vs_path, const GLchar* fs_path){
    std::string vertexShaderSource;
    std::string fragmentShaderSource;
    std::ifstream fs_file;
    std::ifstream vs_file;

    vs_file.exceptions(std::ifstream::badbit);
    fs_file.exceptions(std::ifstream::badbit);
    try{
        vs_file.open(vs_path);
        fs_file.open(fs_path);
        std::stringstream vs_stream, fs_stream;
        vs_stream << vs_file.rdbuf();
        fs_stream << fs_file.rdbuf();
        vs_file.close();
        fs_file.close();
        vertexShaderSource = vs_stream.str();
        fragmentShaderSource = fs_stream.str();
    }
    catch (const std::ifstream::failure& e){
        std::cout << "Shader file read failed" << std::endl;
    }

    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

class Shader {
public:
    GLuint ID;

    Shader(const char* vertex_source, const char* fragment_source);

    void use() const { glUseProgram(ID); }

    void destroy() const { glDeleteProgram(ID); }
};

Shader::Shader(const char *vertex_source, const char *fragment_source) {
    ID = CreateShader(vertex_source, fragment_source);
    this->use();
}


#endif //SHADER_CLASS_H

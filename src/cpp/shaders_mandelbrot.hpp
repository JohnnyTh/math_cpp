#ifndef SHADERS_MANDELBROT_HPP
#define SHADERS_MANDELBROT_HPP

namespace shaders_mandelbrot {

static const char *VERTEX_SRC = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
)glsl";

static const char *FRAGMENT_SRC = R"glsl(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler1D colormap;
uniform sampler2D complexSet;

uniform int n_iterations;
uniform float threshold;


vec2 complexMul(vec2 a, vec2 b) {
    float real = a.x * b.x - a.y * b.y;
    float imag = a.x * b.y + a.y * b.x;
    return vec2(real, imag);
}

vec2 mandelbrotFunc(vec2 z_val, vec2 complex_val) {
    return vec2(complexMul(z_val, z_val) + complex_val);
}


// coonsider using nonlinear (e.g., logarithmic) scale
vec3 computeColorIteration(int iter) {
    vec3 color;
    if (iter == n_iterations) {
        // last iteration - always black color
        color = vec3(0.0, 0.0, 0.0);
    }
    else {
        // important - cast to float before the divison

        //version 3: log scale, colormap:
        float color_ = log(float(iter)) / log(float(n_iterations));
        color = texture(colormap, color_).rgb;
    }
    return color;
}

void main()
{
    // complex_val.x - real, complex_val.y - imag
    vec2 complex_val = texture(complexSet, TexCoord).rg;

    vec2 z_value_iterated = vec2(0.0, 0.0);
    int iter = 0;

    for (iter; iter < n_iterations; iter++) {
        z_value_iterated = mandelbrotFunc(z_value_iterated, complex_val);

        if (length(z_value_iterated) > threshold) {
            break;
        }
    }
    vec3 color = computeColorIteration(iter);

    FragColor = vec4(color.r, color.g, color.b, 1.0);
}
)glsl";

} // namespace shaders_mandelbrot

#endif

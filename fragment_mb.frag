#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

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
float computeColorIteration(int iter) {
    float color;
    if (iter == n_iterations) {
        // last iteration - always black color
        color = 0.0;
    }
    else {
        // important - cast to float before the divison
        color = float(iter) / float(n_iterations);
    }
    return color;
}

void main()
{
    // complex_val.x - real, complex_val.y - imag
    vec2 complex_val = texture(complexSet, TexCoord).rg; // Assuming TexCoord is available

    vec2 z_value_iterated = vec2(0.0, 0.0);
    int iter = 0;

    for (iter; iter < n_iterations; iter++) {
        z_value_iterated = mandelbrotFunc(z_value_iterated, complex_val);

        if (length(z_value_iterated) > threshold) {
            break;
        }
    }
    float color = computeColorIteration(iter);

    FragColor = vec4(color, color, color, 1.0);
}
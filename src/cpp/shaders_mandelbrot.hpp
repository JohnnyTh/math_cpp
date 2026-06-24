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

// double-single: a vec2 where x=hi, y=lo, and the true value is x+y
vec2 ds_add(vec2 a, vec2 b) {
    float s = a.x + b.x;
    float v = s - a.x;
    float e = (a.x - (s - v)) + (b.x - v) + a.y + b.y;
    return vec2(s, e);
}

vec2 ds_sub(vec2 a, vec2 b) {
    return ds_add(a, vec2(-b.x, -b.y));
}

vec2 ds_mul(vec2 a, vec2 b) {
    float p = a.x * b.x;
    float e = a.x * b.y + a.y * b.x + (a.x * b.x - p);
    return vec2(p, e);
}

// ds_sq is faster than ds_mul(a, a)
vec2 ds_sq(vec2 a) {
    float p = a.x * a.x;
    float e = 2.0 * a.x * a.y + (a.x * a.x - p);
    return vec2(p, e);
}

vec3 computeColorIteration(int iter) {
    if (iter == n_iterations)
        return vec3(0.0);
    float t = log(float(iter)) / log(float(n_iterations));
    return texture(colormap, t).rgb;
}

void main()
{
    // texture stores: r=real_hi, g=real_lo, b=imag_hi, a=imag_lo
    vec4 c_tex = texture(complexSet, TexCoord);
    vec2 cr = vec2(c_tex.r, c_tex.g); // real part as double-single
    vec2 ci = vec2(c_tex.b, c_tex.a); // imag part as double-single

    vec2 zr = vec2(0.0, 0.0);
    vec2 zi = vec2(0.0, 0.0);
    int iter = 0;

    for (iter = 0; iter < n_iterations; iter++) {
        vec2 zr2 = ds_sq(zr);
        vec2 zi2 = ds_sq(zi);

        // escape check on hi parts (sufficient for threshold test)
        if ((zr2.x + zi2.x) > threshold * threshold)
            break;

        vec2 new_zr = ds_add(ds_sub(zr2, zi2), cr);
        vec2 new_zi = ds_add(ds_mul(vec2(2.0, 0.0), ds_mul(zr, zi)), ci);
        zr = new_zr;
        zi = new_zi;
    }

    FragColor = vec4(computeColorIteration(iter), 1.0);
}
)glsl";

} // namespace shaders_mandelbrot

#endif

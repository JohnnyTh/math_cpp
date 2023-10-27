#version 430

out vec4 fragColor;
uniform vec2 resolution;
const float MAX_ITER = 10;

float mandelbrot(vec2 uv) {
    vec2 c = 5.0 * uv - vec2(0.7, 0.0);
    vec2 z = vec2(0.0);
    float iter = 0.0;
    for (float i; i < MAX_ITER; i++) {
        z = vec2(z.x * z.x - z.y * z.y,
                2.0 * z.x * z.y) + c;
        if (dot(z, z) > 4.0) return iter / MAX_ITER;
        iter++;
    }
    return 0.0;
}

void main() {
    vec2 uv = (gl_FragCoord.xy - 0.5 * resolution.xy) / resolution.y;
    vec3 col = vec3(0.0);
    float m = mandelbrot(uv);
    col += m;
    fragColor = vec4(col, 1.0);
}
#version 430
out vec4 FragColor;

uniform vec4 u_Color;
uniform float glitchIntensity;
uniform float time;
uniform bool hit;

void main(void)
{
    if (hit) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // чистый красный
        return;
    }

    // иначе — глитч‑логика
    vec4 base = u_Color;
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);

    float offset = glitchIntensity * 0.01;
    float r = base.r + sin(time + uv.y * 10.0) * offset;
    float g = base.g;
    float b = base.b + cos(time + uv.x * 10.0) * offset;

    float noise = fract(sin(dot(uv * time, vec2(12.9898, 78.233))) * 43758.5453);
    if (noise > 0.95 && glitchIntensity > 0.5) {
        r = 1.0 - r;
    }

    FragColor = vec4(r, g, b, 1.0);
}


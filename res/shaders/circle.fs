#version 330 core

uniform vec2 center;
uniform float radius;

out vec4 fragColor;

void main()
{
    if (distance(gl_FragCoord.xy, center) <= radius) {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        discard;
    }
    // fragColor = vec4(gl_FragCoord.x / 400.0, gl_FragCoord.y / 400.0, 0.0, 1.0);
}

#version 330 core

uniform vec2 position;
uniform vec2 size;
uniform float time;
uniform float speed;

out vec4 fragColor;

void main()
{
    float wave = speed * 0.5 * sin(2.0 * 3.14 * 10.0 * gl_FragCoord.y + time);
    if (gl_FragCoord.x >= position.x + wave && gl_FragCoord.x <= position.x + size.x + wave && gl_FragCoord.y >= position.y && gl_FragCoord.y <= position.y + size.y) {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        discard;
    }
}

#version 330 core

uniform vec2 position;
uniform vec2 size;

out vec4 fragColor;

void main()
{
    if (gl_FragCoord.x >= position.x && gl_FragCoord.x <= position.x + size.x && gl_FragCoord.y >= position.y && gl_FragCoord.y <= position.y + size.y) {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        discard;
    }
}

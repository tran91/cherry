precision highp float;
layout (location = 0) out vec4 frag_color;

uniform vec4 u_color;

void main()
{
    frag_color = u_color;
}

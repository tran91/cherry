precision highp float;
layout (location = 0) out vec4 frag_color;

in vec2 texcoord;

uniform sampler2D diffuse;

void main()
{
    frag_color = vec4(texture(diffuse, texcoord).rgb, 1.0);
}

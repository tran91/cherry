layout (location = 0) in vec3 a_pos;

out vec2 texcoord;

void main()
{
    texcoord = a_pos.xy * 0.5 + 0.5;
    gl_Position = vec4(a_pos, 1.0);
}
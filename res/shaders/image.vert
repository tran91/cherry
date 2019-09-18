#ifdef GL_ES
    #define input(name, type, index) attribute type name
    #define output(name, type) varying type name
#else
    #define input(name, type, index) layout (location = index) in type name
    #define output(name, type) out type name
#endif

input(position, vec3, 0);
output(texcoord, vec2);

void main()
{
    texcoord = position.xy * 0.5 + 0.5;
    gl_Position = vec4(position, 1.0);
}
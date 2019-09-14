#ifdef GL_ES
    #define input(name, type, index) attribute type name
    #define output(name, type) varying type name
#else
    #define input(name, type, index) layout (location = index) in type name
    #define output(name, type) out type name
#endif

input(a_pos, vec3, 0);

void main()
{
    gl_Position = vec4(a_pos, 1.0);
}
#ifdef GL_ES
    #define input(name, type, index) attribute type name
    #define output(name, type) varying type name
#else
    #define input(name, type, index) layout (location = index) in type name
    #define output(name, type) out type name
#endif

input(position, vec3, 0);
input(transform_index, float, 1);

uniform vec4 component[108];

void main()
{
    // vec3 pos = vec3(component[int(transform_index) * 7 + 0].value, component[int(transform_index) * 7 + 1].value, component[int(transform_index) * 7 + 2].value);
    vec3 pos = component[int(transform_index) * 2].xyz;
    gl_Position = vec4(position + pos, 1.0);
}
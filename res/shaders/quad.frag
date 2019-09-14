precision highp float;

#ifdef GL_ES
    #define input(name, type) varying type name
    #define get_pixel       texture2D
#else
    #define input(name, type) in type name
    #define get_pixel       texture
    layout (location = 0) out vec4 gl_FragColor;
#endif

uniform vec4 u_color;

void main()
{
    gl_FragColor = u_color;
}

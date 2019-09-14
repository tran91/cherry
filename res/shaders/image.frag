precision highp float;

#ifdef GL_ES
    #define input(name, type) varying type name
    #define get_pixel       texture2D
#else
    #define input(name, type) in type name
    #define get_pixel       texture
    layout (location = 0) out vec4 gl_FragColor;
#endif

input(texcoord, vec2);
uniform sampler2D diffuse;

void main()
{
    gl_FragColor = vec4(get_pixel(diffuse, texcoord).rgb, 1.0);
}

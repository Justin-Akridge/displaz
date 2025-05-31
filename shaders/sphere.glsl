#version 150

#ifdef VERTEX_SHADER

uniform mat4 modelViewProjectionMatrix;

in vec3 position;
in vec3 normal;

out vec3 fragNormal;

void main()
{
    fragNormal = normal;
    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
}

#elif defined(FRAGMENT_SHADER)

in vec3 fragNormal;

out vec4 fragColor;

void main()
{
    vec3 lightDir = normalize(vec3(1,1,1));
    float diffuse = max(dot(normalize(fragNormal), lightDir), 0.0);
    vec3 baseColor = vec3(0.2, 0.6, 1.0);
    fragColor = vec4(baseColor * diffuse, 1.0);
}
#endif


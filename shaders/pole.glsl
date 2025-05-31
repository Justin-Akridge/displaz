#version 150

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform vec3 cylinderCenter;
uniform float cylinderRadius;
uniform float cylinderHeight;
uniform vec3 cylinderColor;
uniform float cylinderAlpha;

#if defined(VERTEX_SHADER)

// Use layout locations to match our vertex setup
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 worldPos;
out vec3 worldNormal;
out vec2 ftexCoord;
out vec3 viewPos;

void main()
{
    // Transform vertex position: scale by radius and height, then translate to center
    vec3 scaledPos = vec3(position.x * cylinderRadius, 
                         position.y * cylinderHeight, 
                         position.z * cylinderRadius);
    vec3 worldPosition = scaledPos + cylinderCenter;
    
    // Transform to clip space
    gl_Position = modelViewProjectionMatrix * vec4(worldPosition, 1.0);
    
    // Pass data to fragment shader
    worldPos = worldPosition;
    worldNormal = normalize(mat3(modelViewMatrix) * normal);
    ftexCoord = texCoord;
    viewPos = (modelViewMatrix * vec4(worldPosition, 1.0)).xyz;
}

#elif defined(FRAGMENT_SHADER)

in vec3 worldPos;
in vec3 worldNormal;
in vec2 ftexCoord;
in vec3 viewPos;

uniform sampler2D texture0;
uniform bool useTexture;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 ambientColor;

out vec4 fragColor;

void main()
{
    vec3 baseColor = cylinderColor;
    
    if (useTexture) {
        vec4 texColor = texture(texture0, ftexCoord);
        baseColor *= texColor.rgb;
    }
    
    vec3 normal = normalize(worldNormal);
    vec3 lightDir = normalize(-lightDirection);
    
    float diffuse = max(dot(normal, lightDir), 0.0);
    
    vec3 finalColor = ambientColor * baseColor + lightColor * baseColor * diffuse;
    
    fragColor = vec4(finalColor, cylinderAlpha);
}

#endif

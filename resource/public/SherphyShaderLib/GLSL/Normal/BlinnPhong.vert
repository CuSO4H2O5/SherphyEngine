#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

vec3 light_pos = {0, 0, 1};

void main() 
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}


void main() {
    gl_Position = ubo.uModelViewProj * vec4(inPosition, 1.0);

    vec4 worldPos = ubo.uModel * vec4(inPosition, 1.0);
    vec4 viewPos = ubo.uModelView * vec4(inPosition, 1.0);
    vec3 normal = normalize(mat3(ubo.uNormal) * inNormal);
    vec3 viewDir = normalize(-vec3(viewPos));

    outNormal = normal;
    outViewDir = viewDir;
    outTexCoord = inTexCoord;
}
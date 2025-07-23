#version 460 core

out vec4 FragColor;

uniform float windowWidth;
uniform float windowHeight;

uniform vec3 cameraOrigin;
uniform vec3 cameraForward;
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform vec3 cameraVelocity;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

const float PI = 3.14159265359;
const mat4 minkowskiMetric = mat4(
    -1.0, 0.0, 0.0, 0.0,
    0.0,  1.0, 0.0, 0.0,
    0.0, 0.0,  1.0, 0.0,
    0.0, 0.0, 0.0,  1.0
    );

vec2 equirectangularToUV(vec3 direction)
{
    float longitude = atan(direction.z, direction.x);
    float latitude = asin(direction.y);

    float u = (longitude + PI) / (2.0 * PI);
    float v = (latitude + (PI / 2.0)) / PI;

    return vec2(u, v);
}

vec4 lorentzTransform(vec4 X, vec3 v)
{
    vec3 x = vec3(X.yzw);
    float t = X.x;
    
    float gamma = 1.0/sqrt(1.0-dot(v, v));

    vec3 xPrime = x + (gamma - 1.0) * dot(v, x)*v/dot(v, v) + gamma*v*t;
    float tPrime = gamma*t + gamma*dot(v, x);

    return vec4(tPrime, xPrime);
}

float sphereSDF(vec3 spherePos, float sphereRadius, vec3 pos)
{
    return length(spherePos - pos) - sphereRadius;
}

float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

void main()
{
	vec2 resolution = vec2(windowWidth, windowHeight);
	vec2 uv = (gl_FragCoord.xy - resolution * 0.5)/resolution.x;

    float f = 0.3;
    vec4 rayOrigin = vec4(0.0, cameraOrigin);

    float t = sqrt(uv.x*uv.x + uv.y*uv.y + f*f);

	vec4 rayDir = vec4(t ,normalize(cameraRight * uv.x + cameraUp * uv.y + cameraForward * f));

    vec4 boostedRayPos = lorentzTransform(rayOrigin, -cameraVelocity);
    vec4 boostedRayDirection = lorentzTransform(rayDir, -cameraVelocity);

    FragColor = vec4(0.0);

    int steps = 64;
    vec3 pos = boostedRayPos.yzw;
    float sdf = 0.0;
    for (int i = 0; i < steps; i++)
    {
        float sdf = sdBox(vec3(0.0, 0.0, 0.0) - cameraVelocity * sdf, 10.0, pos);
        pos = pos + normalize(boostedRayDirection.yzw) * sdf;
        if (sdf < 0.01) FragColor = vec4(normalize(pos), 1.0);
    }
}
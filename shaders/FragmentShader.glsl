#version 460 core

out vec4 FragColor;

uniform float windowWidth;
uniform float windowHeight;

uniform vec3 cameraOrigin;
uniform vec3 cameraForward;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

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

mat4 vecToDiagMatrix(vec4 a)
{
    return mat4(a.x,0,0,0,
                0,a.y,0,0,
                0,0,a.z,0,
                0,0,0,a.w);
}

vec4 diagMatrixToVec(mat4 diagMatrix)
{
    return vec4(diagMatrix[0][0], diagMatrix[1][1], diagMatrix[2][2], diagMatrix[3][3]);
}

mat4 inverseDiagMatrix(mat4 diagMatrix)
{
    return mat4(
    1.0/diagMatrix[0][0], 0.0, 0.0, 0.0,
    0.0, 1.0/diagMatrix[1][1], 0.0, 0.0,
    0.0, 0.0, 1.0/diagMatrix[2][2], 0.0,
    0.0, 0.0, 0.0, 1.0/diagMatrix[3][3]
    );
}

bool kerrNeumannMetric = true;

uniform float a;
uniform float m;
uniform float Q;

mat4 metric(vec4 x)
{
    if (kerrNeumannMetric)
    {
        vec3 p = x.yzw;
        float rho = dot(p,p) - a*a;
        float r2 = 0.5*(rho + sqrt(rho*rho + 4.0*a*a*p.z*p.z));
        float r = sqrt(r2);
        vec4 k = vec4(1, (r*p.x + a*p.y)/(r2 + a*a), (r*p.y - a*p.x)/(r2 + a*a), p.z/r);
        float f = r2*(2.0*m*r - Q*Q)/(r2*r2 + a*a*p.z*p.z);
        return f*mat4(k.x*k, k.y*k, k.z*k, k.w*k)+vecToDiagMatrix(vec4(-1,1,1,1));
    }
}

float Hamiltonian(vec4 x, vec4 p)
{
    mat4 g_inv = inverseDiagMatrix(metric(x));
    return 0.5*dot(g_inv*p,p);
}

vec4 HamiltonianGradient(vec4 x, vec4 p)
{
    const float eps = 0.01;
    return (vec4(Hamiltonian(x + vec4(eps,0,0,0), p),
                 Hamiltonian(x + vec4(0,eps,0,0), p),
                 Hamiltonian(x + vec4(0,0,eps,0), p),
                 Hamiltonian(x + vec4(0,0,0,eps), p)) - Hamiltonian(x,p))/eps;
}   

vec4 GetNullMomentum(vec4 x, vec3 dir)
{
    return metric(x) * vec4(1.0, normalize(dir));
}

vec3 GetDirection(vec4 x, vec4 p)
{
    vec4 dxdt = inverseDiagMatrix(metric(x)) * p;
    return normalize(dxdt.yzw);
}

vec3 traceGeodesic(inout vec3 pos, inout vec3 dir, inout float time)
{
    vec4 x = vec4(time, pos);
    vec4 p = GetNullMomentum(x, dir);

    int steps = 32;
    float dt = 0.15;

    mat4 g = metric(x);
    float prevR = sqrt(dot(pos, pos));
    for(int i = 0; i < steps; i++)
    {
        pos = x.yzw;
        float R = sqrt(dot(pos, pos));
        
        if (kerrNeumannMetric)
        {
            float rho = dot(pos, pos) - a*a;
            float r2 = 0.5*(rho + sqrt(rho*rho + 4.0*a*a*pos.z*pos.z));
            float r = sqrt(r2);
            if (r < 2.0*m && a < 1.0) return vec3(0.0);
        }
        else {
            if (R < 2.0*m) return vec3(0.0);
        }

        dt = R*0.05;

        if(prevR - R > 0.0 && steps < 128) {steps = steps + 1;}
        prevR = R;
        
        p = p - dt * HamiltonianGradient(x, p);
        x = x + dt * inverseDiagMatrix(g) * p;

        g = minkowskiMetric;
        if (R < 8.0 * 2.0*m) {g = metric(x);}

    }

    pos = x.yzw;
    time = x.x;
    dir = GetDirection(x, p);
    return  texture(texture2, equirectangularToUV(dir)).xyz;
}

void main()
{
	vec2 resolution = vec2(windowWidth, windowHeight);
	vec2 uv = (gl_FragCoord.xy - resolution * 0.5)/resolution.x;

    vec3 rayPos = cameraOrigin;
	vec3 rayDir = normalize(cameraRight * uv.x + cameraUp * uv.y + cameraForward * 0.5);
    float time = 0.0;

	FragColor = vec4(traceGeodesic(rayPos, rayDir, time), 1.0);
}
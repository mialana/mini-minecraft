#version 400

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself

in vec4 vs_Pos;             // The array of vertex positions passed to the shader
in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec3 vs_ColInstanced;    // The array of vertex colors passed to the shader.
in vec3 vs_OffsetInstanced; // Used to position each instance of the cube

out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_Col;            // The color of each vertex. This is implicitly passed to the fragment shader.

const vec4 lightDir = normalize(vec4(0.5, 1, 0.75, 0));  // The direction of our virtual light, which is used to compute the shading of
                                        // the geometry in the fragment shader.

// tv static
vec2 noise2D(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)),
                 dot(p, vec2(269.5,183.3))))
                 * 43758.5453);
}

float noise1D( vec2 p ) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) *
                 43758.5453);
}


float interpNoise(float x, float y) {
    int intX = int(floor(x));
    float fractX = fract(x);
    int intY = int(floor(y));
    float fractY = fract(y);

    float v1 = noise1D(vec2(intX, intY));
    float v2 = noise1D(vec2(intX + 1, intY));
    float v3 = noise1D(vec2(intX, intY + 1));
    float v4 = noise1D(vec2(intX + 1, intY + 1));

    // mix is a glsl fn that returns a lin interp btwn 2 vals based on some t s.t. 0<t<1
    float i1 = mix(v1, v2, fractX);
    float i2 = mix(v3, v4, fractX);
    return mix(i1, i2, fractY);
}

//vec2 smoothF(vec2 uv) {

//}

// range 0 to 1
float fbm(const in vec2 uv) {
    float total = 0;
    float persistence = 0.5f;
    int octaves = 8;
    float freq = 2.f;
    float amp = 0.5f;
    for(int i = 1; i <= octaves; i++) {
        total += interpNoise(uv.x * freq,
                               uv.y * freq) * amp;
        freq *= 2.f;
        amp *= persistence;
    }
    return total;
}

float surflet(vec2 P, vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial
    float distX = abs(P.x - gridPoint.x);
    float distY = abs(P.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.f) + 15 * pow(distX, 4.f) - 10 * pow(distX, 3.f);
    float tY = 1 - 6 * pow(distY, 5.f) + 15 * pow(distY, 4.f) - 10 * pow(distY, 3.f);
    // Get the random vector for the grid point
    vec2 gradient = 2.f * noise2D(gridPoint) - vec2(1.f);
    // Get the vector from the grid point to P
    vec2 diff = P - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}

// range -1 to 1
float perlin(in vec2 uv) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
            for(int dy = 0; dy <= 1; ++dy) {
                    surfletSum += surflet(uv, floor(uv) + vec2(dx, dy));
            }
    }
    return surfletSum;
}

// range 0 to 1
float worley(vec2 uv) {
    uv *= 10; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);
    float minDist = 1.0; // Minimum distance initialized to max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            vec2 neighbor = vec2(float(x), float(y)); // Direction in which neighbor cell lies
            vec2 point = noise2D(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell
            vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }
    return minDist;
}

void main()
{
    vec4 offsetPos = vs_Pos + vec4(vs_OffsetInstanced, 0.);
    fs_Pos = offsetPos;
    fs_Col = vec4(vs_ColInstanced, 1.);  // Pass the vertex colors to the fragment shader for interpolation

    fs_Nor = vs_Nor;

    fs_LightVec = (lightDir);  // Compute the direction in which the light source lies

    gl_Position = u_ViewProj * offsetPos;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices


    // terrace hills
//    vec4 offsetPos = vs_Pos + vec4(vs_OffsetInstanced, 0.);
//    vec2 xz = vs_OffsetInstanced.xz;

//    float h = 0;
//    float freq = 200.f;
//    float dF = 0.725;

//    for (int i = 0; i < 4; ++i) {
//        h += perlin(xz / freq);
//        freq *= dF;
//    }

//    float flatten = 2.f;
//    float sharpen = 1.25f;
//    float max = -0.25f;
//    float min = -0.9f;
//    if (h > max) {
//        h -= max;
//        h /= flatten;
//        h += max;
//    }

//    if (h < min) {
//        h -= min;
//        h *= sharpen;
//        h += min;
//    }

//    offsetPos.y *= floor(178.f + h * 50);

    // mountains
//    vec4 offsetPos = vs_Pos + vec4(vs_OffsetInstanced, 0.);
//    vec2 xz = vs_OffsetInstanced.xz;

//    float h = 0;

//    float amp = 0.5;
//    float freq = 175.f;

//    for (int i = 0; i < 4; ++i) {
//        float h1 = perlin(xz / freq);
//        h1 = 1. - abs(h1);
//        h1 = pow(h1, 1.25);
//        h += h1 * amp;

//        amp *= 0.5;
//        freq *= 0.5;
//    }

//    offsetPos.y *= floor(50.f + h * 200.f);

    // forest
//    vec4 offsetPos = vs_Pos + vec4(vs_OffsetInstanced, 0.);
//    vec2 xz = vs_OffsetInstanced.xz;

//    float h = 0;

//    float amp = 0.5;
//    float freq = 90.f;

//    for (int i = 0; i < 4; ++i) {
//        h += amp * perlin(xz / freq);
//        freq *= 0.5;
//        amp *= 0.5;
//    }

//    offsetPos.y *= floor(148.f + h * 20);

    // islands
//    vec4 offsetPos = vs_Pos + vec4(vs_OffsetInstanced, 0.);
//    vec2 xz = vs_OffsetInstanced.xz;

//    float h = 0;

//    float amp = 0.5;
//    float freq = 200.f;

//    for (int i = 0; i < 4; ++i) {
//        h += amp * perlin(xz / freq);
//        freq *= 0.25;
//        amp *= 0.25;
//    }

//    offsetPos.y *= floor(35.f + h * 100);

    fs_Pos = offsetPos;
    fs_Col = vec4(vs_ColInstanced, 1.);

    fs_Nor = vs_Nor;

    fs_LightVec = (lightDir);  // Compute the direction in which the light source lies

    gl_Position = u_ViewProj * offsetPos;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices
}

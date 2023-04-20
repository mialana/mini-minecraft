#version 330
// ^ Change this to version 130 if you have compatibility issues

// This is a fragment shader. If you've opened this file first, please
// open and read lambert.vert.glsl before reading on.
// Unlike the vertex shader, the fragment shader actually does compute
// the shading of geometry. For every pixel in your program's output
// screen, the fragment shader is run for every bit of geometry that
// particular pixel overlaps. By implicitly interpolating the position
// data passed into the fragment shader by the vertex shader, the fragment shader
// can compute what color to apply to its pixel based on things like vertex
// position, light position, and vertex color.

uniform vec4 u_Color; // The color with which to render this instance of geometry.
uniform sampler2D u_TextureSampler; //
uniform int u_Time; //

// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_Col;
in vec2 fs_UV;
in vec4 fs_BiomeWts; // mountains = 0, hills = 1, forest = 2, islands = 3

flat in int fs_TexIdx;

in vec4 fs_LightVec;

out vec4 out_Col; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.

float random1(vec3 p) {
    return fract(sin(dot(p,vec3(127.1, 311.7, 191.999)))
                 *43758.5453);
}

float mySmoothStep(float a, float b, float t) {
    t = smoothstep(0, 1, t);
    return mix(a, b, t);
}

float cubicTriMix(vec3 p) {
    vec3 pFract = fract(p);
    float llb = random1(floor(p) + vec3(0,0,0));
    float lrb = random1(floor(p) + vec3(1,0,0));
    float ulb = random1(floor(p) + vec3(0,1,0));
    float urb = random1(floor(p) + vec3(1,1,0));

    float llf = random1(floor(p) + vec3(0,0,1));
    float lrf = random1(floor(p) + vec3(1,0,1));
    float ulf = random1(floor(p) + vec3(0,1,1));
    float urf = random1(floor(p) + vec3(1,1,1));

    float mixLoBack = mySmoothStep(llb, lrb, pFract.x);
    float mixHiBack = mySmoothStep(ulb, urb, pFract.x);
    float mixLoFront = mySmoothStep(llf, lrf, pFract.x);
    float mixHiFront = mySmoothStep(ulf, urf, pFract.x);

    float mixLo = mySmoothStep(mixLoBack, mixLoFront, pFract.z);
    float mixHi = mySmoothStep(mixHiBack, mixHiFront, pFract.z);

    return mySmoothStep(mixLo, mixHi, pFract.y);
}

float fbm(vec3 p) {
    float amp = 0.5;
    float freq = 4.0;
    float sum = 0.0;
    for(int i = 0; i < 8; i++) {
        sum += cubicTriMix(p * freq) * amp;
        amp *= 0.5;
        freq *= 2.0;
    }
    return sum;
}

float mod(float a, float b) {
    int div = int (a / b);
    return a - (div * b);
}

vec4 tint(vec4 origCol, vec4 tintCol, float tintWt) {

    vec4 finalCol = origCol * (1 - tintWt) + tintCol * tintWt;
    finalCol.a = tintCol.a;
    return finalCol;
}
vec4 color(vec4 origCol, vec4 newCol, float newWt) {
    float grayscaleCol = (0.21 * origCol.r) + (0.72 * origCol.g) + (0.07 * origCol.b);
    vec4 finalCol = newCol * newWt + (grayscaleCol * (1 - newWt) * grayscaleCol);
    finalCol.a = newCol.a;
//    vec4 finalCol = newCol * grayscaleCol;
    return finalCol;
}
void main()
{
    vec2 newUV;
    if (fs_UV.x >= 0 && fs_UV.y >= 0) {
        // 0 = no change from base texture at uv coords
        // 1 = interpolate grass color
        // 2 = water (animation + biome color interpolation)
        // 3 = lava animation
        // 4 = glowing blocks/ doesn't receive shadows

        out_Col = vec4(texture(u_TextureSampler, fs_UV));
        newUV = fs_UV;

        if (fs_TexIdx == 1) {
            // mountains = 0, hills = 1, forest = 2, islands = 3
            vec4 mCol = vec4(0, 0.27, 0.235, 1) * fs_BiomeWts.x;
            vec4 hCol = vec4(0.08, 0.35, 0, 1) * fs_BiomeWts.y;
            vec4 fCol = vec4(0.04, 0.3, 0.3, 1) * fs_BiomeWts.z;
            vec4 iCol = vec4(0.157, 0.55, 0.235, 1) * fs_BiomeWts.w;
            vec4 cCol = vec4(0.03, 0.175, 0.278, 1);

            vec4 tintCol;

            if (fs_Pos.y < 110) {
                tintCol = cCol;
            } else {
                tintCol = mCol + hCol + fCol + iCol;
            }
            newUV = fs_UV;
            out_Col = vec4(texture(u_TextureSampler, fs_UV));
            out_Col = color(out_Col, tintCol, 0.6);
        } else if (fs_TexIdx == 2) {
            // water animation
            float uOffset = (0.0625 / 64.f) * float(mod(u_Time, 64));
            newUV = vec2(fs_UV.x + uOffset, fs_UV.y);
            out_Col = vec4(texture(u_TextureSampler, newUV));

            // biome color interpolation (water)
//            vec4 mCol = vec4(0, 0.243, 0.5, 0.6) * fs_BiomeWts.x;
//            vec4 hCol = vec4(0, 0.75, 1, 0.3) * fs_BiomeWts.y;
//            vec4 fCol = vec4(0, 0.435, 0.898, 0.5) * fs_BiomeWts.z;
//            vec4 iCol = vec4(0, 1, 0.95, 0.25) * fs_BiomeWts.w;
//            vec4 cCol = vec4(0, 0.196, 0.8235, 0.75);
            vec4 mCol = vec4(0, 0.243, 0.5, 0.9) * fs_BiomeWts.x;
            vec4 hCol = vec4(0, 0.75, 1, 0.9) * fs_BiomeWts.y;
            vec4 fCol = vec4(0, 0.435, 0.898, 0.9) * fs_BiomeWts.z;
            vec4 iCol = vec4(0, 1, 0.95, 0.9) * fs_BiomeWts.w;
            vec4 cCol = vec4(0, 0.196, 0.8235, 0.9);

            vec4 tintCol;
            if (fs_Pos.y < 100) {
                tintCol = cCol;
            } else {
                tintCol = mCol + hCol + fCol + iCol;
            }
            out_Col = tint(out_Col, tintCol, 0.9);

        } else if (fs_TexIdx == 3) {
            // lava animation
            float frame = mod(u_Time, 256);
            float uOffset;
            float vOffset;

            if (frame < 64) {
                vOffset = (0.0625 / 64.f) * frame;
                uOffset = 0;
            } else if (frame < 128) {
                vOffset = 0.0625;
                uOffset = (0.0625 / 64.f) * (frame - 64);
            } else if (frame < 192) {
                vOffset = 0.0625 - ((0.0625 / 64.f) * (frame - 128));
                uOffset = 0.0625;
            } else {
                vOffset = 0;
                uOffset = 0.0625 - ((0.0625 / 64.f) * (frame - 192));
            }

            if (fbm(fs_Pos.xyz) > 0.5) {
                newUV = fs_UV + vec2(uOffset, vOffset);
            } else {
                newUV = fs_UV + vec2(vOffset, uOffset);
            }
            out_Col = texture(u_TextureSampler, newUV);
        }
    } else {
        // Material base color (before shading)
            vec4 diffuseColor = fs_Col;
            out_Col = diffuseColor * (0.5 * fbm(fs_Pos.xyz) + 0.5);
    }

    if (fs_TexIdx != 4) {
        float alpha = texture(u_TextureSampler, newUV).a;
        float diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec));
                    // Avoid negative lighting values
        diffuseTerm = clamp(diffuseTerm, 0, 1);

        float ambientTerm = 0.3;

        float lightIntensity = diffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
                                                            //to simulate ambient lighting. This ensures that faces that are not
                                                            //lit by our point light are not completely black.

        // Compute final shaded color
        out_Col = vec4(out_Col.rgb * lightIntensity, alpha);
    }
    if (out_Col.a == 0.f) {
        discard;
    }
}

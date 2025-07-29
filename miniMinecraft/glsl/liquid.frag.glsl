#version 330

uniform sampler2D u_FrameBufferTexture;
uniform vec4 u_playerPosBiomeWts;
uniform vec3 u_playerPos;
uniform vec4 u_Color;

in vec4 fs_Col;
in vec4 fs_Pos;

out vec4 out_Col;

vec4 tint(vec4 origCol, vec4 tintCol, float tintWt)
{
    vec4 finalCol = origCol * (1 - tintWt) + tintCol * tintWt;
    finalCol.a = tintCol.a;
    return finalCol;
}

vec4 mySmoothStep(vec4 a, vec4 b, float t)
{
    t = smoothstep(0, 1, t);
    vec4 newCol;
    newCol.r = mix(a.r, b.r, t);
    newCol.g = mix(a.g, b.g, t);
    newCol.b = mix(a.b, b.b, t);
    newCol.a = mix(a.a, b.a, t);
    return newCol;
}

void main()
{
    vec4 frame = texture(u_FrameBufferTexture, vec2(fs_Col.x, fs_Col.y));
    if (u_Color == vec4(0.f, 0.f, 1.f, 1.f)) {
        // biome based water color interpolation
        vec4 mCol = vec4(0, 0.243, 0.5, 0.6) * u_playerPosBiomeWts.x;
        vec4 hCol = vec4(0, 0.75, 1, 0.3) * u_playerPosBiomeWts.y;
        vec4 fCol = vec4(0, 0.435, 0.898, 0.5) * u_playerPosBiomeWts.z;
        vec4 iCol = vec4(0, 1, 0.95, 0.25) * u_playerPosBiomeWts.w;
        vec4 cCol = vec4(0, 0.196, 0.8235, 0.75);

        vec4 tintCol;
        if (fs_Pos.y < 100) {
            tintCol = cCol;
        } else if (fs_Pos.y > 110) {
            tintCol = mCol + hCol + fCol + iCol;
        } else {
            vec4 col1 = mCol + hCol + fCol + iCol;
            vec4 col2 = cCol;
            tintCol = mySmoothStep(col1, col2, (110.f - fs_Pos.y) / 10.f);
        }
        out_Col = frame * 0.4 + tint(u_Color, tintCol, 0.9) * 0.6;
        //        out_Col = tint(u_Color, tintCol, 0.9) * 0.6;

    } else if (u_Color == vec4(1.f, 0.f, 0.f, 1.f)) {
        out_Col = frame * 0.4 + u_Color * 0.6;
    } else {
        out_Col = frame;
    }
}

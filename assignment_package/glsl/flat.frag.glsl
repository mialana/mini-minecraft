#version 330
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments
uniform sampler2D u_PlayerTexture;

in vec4 fs_UV;
in vec4 fs_Pos;
in vec4 fs_Nor;

out vec4 out_Col;

void main() {
    out_Col = vec4(1, 0, 0, 1);
}

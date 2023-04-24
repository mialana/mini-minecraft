#version 330
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

in vec4 vs_Nor;
in vec4 vs_Pos;
in vec4 vs_UV;

out vec4 fs_Pos;
out vec4 fs_Nor;
out vec4 fs_UV;

void main() {
    fs_UV = vs_UV;
    fs_Pos = vs_Pos;
    fs_Nor = vs_Nor;

    vec4 modelposition = u_Model * vs_Pos;

    //built-in things to pass down the pipeline
    gl_Position = u_ViewProj * modelposition;

}

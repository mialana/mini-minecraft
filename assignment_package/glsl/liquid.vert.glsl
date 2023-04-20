#version 150
in vec4 vs_Pos;
in vec4 vs_Col;

out vec4 fs_Col;
out vec4 fs_Pos;

void main()
{
    fs_Col = vs_Col;

    gl_Position = vs_Pos;

    fs_Pos = vs_Pos;
}

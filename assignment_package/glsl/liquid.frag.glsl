#version 330

uniform sampler2D u_FrameBufferTexture;
uniform vec4 u_Color;

in vec4 fs_Col;

out vec4 out_Col;

void main() {
    vec4 frame = texture(u_FrameBufferTexture, vec2(fs_Col.x, fs_Col.y));

    if (u_Color != vec4(0.f, 0.f, 0.f, 1.f)) {
        out_Col = frame * 0.4 + u_Color * 0.6;
    } else {
        out_Col = frame;
    }

}

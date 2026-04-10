#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D source;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float blurRadius;
    float effectOpacity;
    float effectEnabled;
    float texelStepX;
    float texelStepY;
};

void main()
{
    vec4 base = texture(source, qt_TexCoord0);

    if (effectEnabled < 0.5 || blurRadius < 0.001 || texelStepX <= 0.0 || texelStepY <= 0.0) {
        fragColor = base * effectOpacity * qt_Opacity;
        return;
    }

    float radiusScale = blurRadius * 0.35;
    vec2 stepSize = vec2(texelStepX, texelStepY) * radiusScale;

    vec4 sum = vec4(0.0);
    sum += texture(source, qt_TexCoord0 + vec2(-1.0, -1.0) * stepSize);
    sum += texture(source, qt_TexCoord0 + vec2( 0.0, -1.0) * stepSize);
    sum += texture(source, qt_TexCoord0 + vec2( 1.0, -1.0) * stepSize);
    sum += texture(source, qt_TexCoord0 + vec2(-1.0,  0.0) * stepSize);
    sum += texture(source, qt_TexCoord0 + vec2( 0.0,  0.0) * stepSize);
    sum += texture(source, qt_TexCoord0 + vec2( 1.0,  0.0) * stepSize);
    sum += texture(source, qt_TexCoord0 + vec2(-1.0,  1.0) * stepSize);
    sum += texture(source, qt_TexCoord0 + vec2( 0.0,  1.0) * stepSize);
    sum += texture(source, qt_TexCoord0 + vec2( 1.0,  1.0) * stepSize);

    vec4 blurred = sum / 9.0;
    fragColor = blurred * effectOpacity * qt_Opacity;
}
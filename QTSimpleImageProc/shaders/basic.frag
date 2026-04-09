#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    vec4 tintColor;
};

void main()
{
    /**
        This creates a 3-component color vector.

        You are constructing RGB from:
        red = horizontal coordinate
        green = vertical coordinate
        blue = constant 0.25

        So for a pixel near the left edge:
        red is low

        For a pixel near the right edge:
        red is high

        For a pixel near one vertical edge:
        green is low or high depending on direction
        Blue stays fixed at 0.25 everywhere.

        That is why you see a gradient.
        This line is really the core of the demo:
        you are converting position into color.
    */
    vec3 color = vec3(qt_TexCoord0.x, qt_TexCoord0.y, 0.25);

    /**
        This part is not fundamentally required for shader math,
        but it is required if you want your shader to behave properly inside Qt’s UI system.
        That is the subtle part.

        Why Qt wants this
        Imagine in QML you later write:

        ShaderEffect {
            opacity: 0.2
        }

        You would expect your rectangle to become mostly transparent.
        Qt passes that opacity value into the shader as: qt_Opacity

        If your shader ignores it, then your item might not fade correctly.

        So this line:

        * qt_Opacity

        is basically saying:
        “Respect whatever opacity Qt says this item should have.”
    */
    fragColor = vec4(color, 1.0) * qt_Opacity;
}
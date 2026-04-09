#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
};

/**
    This declares a texture input for the shader.

    sampler2D means:
    "I expect a 2D image-like texture that I can sample from."

    The name "source" matters because in QML we will define:

        property var source: sourceTexture

    Qt will connect that QML property to this shader input.

    So this shader is NOT automatically finding test.png by itself.

    Instead, the chain is:

        test.png
        -> loaded by Image in QML
        -> wrapped as a texture by ShaderEffectSource
        -> passed into this shader as "source"

    Then inside the shader, we can ask:
    "At this UV coordinate, what color does the source image have?"
*/
layout(binding = 1) uniform sampler2D source;

void main()
{
    /**
        This samples the source texture at the current UV coordinate.

        qt_TexCoord0 is the coordinate for the current fragment/pixel
        within the rectangle, usually normalized from 0.0 to 1.0.

        texture(source, qt_TexCoord0) means:

        "Look at the input image bound to 'source',
        go to this position in that image,
        and return the color found there."

        The returned value is a vec4:
        red, green, blue, alpha

        So unlike the previous version of the shader, where we
        CREATED a color from position mathematically, here we are
        READING a color from an actual image.
    */
    vec4 srcColor = texture(source, qt_TexCoord0);

    /**
        This outputs the sampled image color.

        Right now we are displaying it unchanged, which means:
        whatever color we sampled from the image is what we draw.

        We still multiply by qt_Opacity so that if the QML item's
        opacity changes, the shader respects that setting.

        So this is basically:

        final pixel color = sampled image color
                            times Qt's overall opacity
    */
    fragColor = srcColor * qt_Opacity;
}
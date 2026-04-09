#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    vec4 controls1;
};

/**
    controls1 packs several UI-controlled values into one vec4.

    controls1.x = grayscaleAmount
    controls1.y = invertAmount
    controls1.z = brightness
    controls1.w = contrast

    Grouping values this way is cleaner and helps avoid uniform
    alignment surprises.
*/
layout(binding = 1) uniform sampler2D source;

void main()
{
    /**
        Sample the original image color at this pixel.
    */
    vec4 srcColor = texture(source, qt_TexCoord0);

    /**
        Start with the source RGB as the working color.
    */
    vec3 color = srcColor.rgb;

    /**
        Convert to grayscale brightness using perceptual weights.
    */
    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    vec3 grayRgb = vec3(gray);

    /**
        Blend between original color and grayscale.
    */
    color = mix(color, grayRgb, controls1.x);

    /**
        Build a fully inverted version of the current color,
        then blend toward it by invert amount.
    */
    vec3 inverted = vec3(1.0) - color;
    color = mix(color, inverted, controls1.y);

    /**
        Brightness shifts the color up or down.
        A value of 0.0 means no change.
    */
    color += vec3(controls1.z);

    /**
        Contrast scales distance from middle gray (0.5).
        1.0 means no contrast change.
        Below 1.0 reduces contrast.
        Above 1.0 increases contrast.
    */
    color = (color - vec3(0.5)) * controls1.w + vec3(0.5);

    /**
        Clamp so values stay in the visible 0..1 range.
    */
    color = clamp(color, 0.0, 1.0);

    /**
        Output final color with original alpha and Qt opacity.
    */
    fragColor = vec4(color, srcColor.a) * qt_Opacity;
}
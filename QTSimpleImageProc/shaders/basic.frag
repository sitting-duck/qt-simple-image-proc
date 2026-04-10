#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    vec4 controls1;
    vec4 controls2;
    vec4 controls3;
};

/**
    controls1:
        x = grayscaleAmount
        y = invertAmount
        z = brightness
        w = contrast

    controls2:
        x = thresholdAmount
        y = thresholdCutoff
        z = saturation
        w = blurAmount

    controls3:
        x = texelStepX
        y = texelStepY
        z = effectOpacity
        w = blurRadius

    A texel is a texture pixel.

    texelStepX and texelStepY are the UV-sized distances of one texel
    in the horizontal and vertical directions.

    blurRadius scales how far away the neighboring blur samples are taken.
*/
layout(binding = 1) uniform sampler2D source;

void main()
{
    vec2 texel = vec2(controls3.x, controls3.y);
    vec2 uv = qt_TexCoord0;

    /**
        blurRadius controls how far away our neighbor samples are.

        radius = 1.0 means sample immediate neighbors.
        Larger values spread the sample positions farther apart,
        making the blur much more obvious.
    */
    float radius = controls3.w;

    vec4 s00 = texture(source, uv + vec2(-texel.x * radius, -texel.y * radius));
    vec4 s10 = texture(source, uv + vec2( 0.0,              -texel.y * radius));
    vec4 s20 = texture(source, uv + vec2( texel.x * radius, -texel.y * radius));

    vec4 s01 = texture(source, uv + vec2(-texel.x * radius,  0.0));
    vec4 s11 = texture(source, uv);
    vec4 s21 = texture(source, uv + vec2( texel.x * radius,  0.0));

    vec4 s02 = texture(source, uv + vec2(-texel.x * radius,  texel.y * radius));
    vec4 s12 = texture(source, uv + vec2( 0.0,               texel.y * radius));
    vec4 s22 = texture(source, uv + vec2( texel.x * radius,  texel.y * radius));

    vec4 blurredSrcColor =
        (s00 + s10 + s20 +
         s01 + s11 + s21 +
         s02 + s12 + s22) / 9.0;

    /**
        controls2.w:
            0.0 = original
            1.0 = full blur mix
    */
    vec4 srcColor = mix(s11, blurredSrcColor, controls2.w);

    /**
        Start the working color pipeline.
        Every effect below operates on the current color in sequence.
    */
    vec3 color = srcColor.rgb;

    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    vec3 grayRgb = vec3(gray);
    color = mix(color, grayRgb, controls1.x);

    vec3 inverted = vec3(1.0) - color;
    color = mix(color, inverted, controls1.y);

    color += vec3(controls1.z);

    color = (color - vec3(0.5)) * controls1.w + vec3(0.5);

    float satGray = dot(color, vec3(0.299, 0.587, 0.114));
    vec3 grayRgbForSaturation = vec3(satGray);
    color = mix(grayRgbForSaturation, color, controls2.z);

    /**
        Threshold measures brightness.

        This is a luminance-like scalar computed from RGB, not just one
        channel like red or green alone.
    */
    float thresholdGray = dot(color, vec3(0.299, 0.587, 0.114));

    float binary = step(controls2.y, thresholdGray);
    vec3 thresholdRgb = vec3(binary);

    color = mix(color, thresholdRgb, controls2.x);

    color = clamp(color, 0.0, 1.0);

    /**
        Effect Opacity:
        blend the fully processed stack back over the original image.

        controls3.z:
            0.0 = original image
            1.0 = full processed result
    */
    vec3 finalRgb = mix(s11.rgb, color, controls3.z);
    float finalAlpha = mix(s11.a, srcColor.a, controls3.z);

    fragColor = vec4(finalRgb, finalAlpha) * qt_Opacity;
}
#version 460 core

in vec4 outColor;
in vec2 texCoords;

out vec4 fragColor;

uniform int codCol;
uniform sampler2D myTexture;
uniform vec2 sunScreenPos;
uniform vec2 moonScreenPos;
uniform vec2 windowSize;

void main() {
    vec4 baseColor = texture(myTexture, texCoords);
    vec2 fragmentScreenPos = gl_FragCoord.xy;

    float distanceToSun = length(sunScreenPos - fragmentScreenPos);
    float distanceToMoon = length(moonScreenPos - fragmentScreenPos);

    float sunFactor = 1.0 - clamp(distanceToSun / windowSize.x, 0.0, 1.0);
    float moonFactor = 1.0 - clamp(distanceToMoon / windowSize.x, 0.0, 1.0);

    vec4 sunTint = vec4(1.0, 1.0, 0.0, 1.0) * sunFactor;
    vec4 moonTint = vec4(0.3, 0.3, 1.0, 1.0) * moonFactor;

    vec4 combinedTint = sunTint + moonTint;

    switch (codCol) {
        case 0:
            fragColor = vec4(0.0, 1.0, 0.0, 1.0) + combinedTint;
            break;
        case 1:
        case 2:
            fragColor = baseColor;
            break;
    }
}

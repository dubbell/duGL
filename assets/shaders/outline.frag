#version 460 core

out vec4 FragColor;

uniform sampler2D coverageMask;
uniform sampler2D depthMask;
uniform float thickness;
uniform float depthBias;
uniform vec3 color;

const int DIRECTIONS = 16;
const float RING_SPACING = 1.5;
const float TAU = 6.28318530718;

void main()
{
    vec2 texel = 1.0 / vec2(textureSize(coverageMask, 0));
    vec2 uv = gl_FragCoord.xy * texel;

    // Anti-aliased coverage by the mask. If completely inside an entity, discard.
    float inside = texture(coverageMask, uv).r;
    if (inside >= 1.0) {
        discard;
    }

    float coverage = 0.0;
    float nearest = 1.0;

    // Sample points in the textures at points on rings around the fragment.
    // Take maximum coverage and minimum depth.
    int rings = max(int(ceil(thickness / RING_SPACING)), 1);
    for (int ring = 1; ring <= rings; ring++)
    {
        float radius = thickness * float(ring) / float(rings);
        float phase = 0.5 * float(ring & 1);

        for (int direction = 0; direction < DIRECTIONS; direction++)
        {
            float angle = (float(direction) + phase) * TAU / float(DIRECTIONS);
            vec2 sampleUv = uv + radius * vec2(cos(angle), sin(angle)) * texel;

            coverage = max(coverage, texture(coverageMask, sampleUv).r);

            vec4 depths = textureGather(depthMask, sampleUv);
            nearest = min(nearest, min(min(depths.x, depths.y), min(depths.z, depths.w)));
        }
    }

    // The part of this pixel within reach of the silhouette, less the part the entity itself covers.
    float alpha = coverage * (1.0 - inside);
    if (alpha <= 0.0 || nearest >= 1.0) {
        discard;
    }

    FragColor = vec4(color, alpha);

    // 1 - depth is roughly inversely proportional to distance, so this pulls the outline
    // towards the camera by a fraction of its distance.
    gl_FragDepth = max(1.0 - (1.0 - nearest) * (1.0 + depthBias), 0.0);
}

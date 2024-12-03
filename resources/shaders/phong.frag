#version 330 core

struct SceneLightData {
    bool enabled;
    int type;      // GLSL doesn't directly support enum. 0 = Directional, 1 = Point, 2 = Spot

    vec4 color;
    vec3 function; // Attenuation function

    vec4 pos; // Position with CTM applied (Not applicable to directional lights)
    vec4 dir; // Direction with CTM applied (Not applicable to point lights)

    float penumbra; // Only applicable to spot lights, in RADIANS
    float angle;    // Only applicable to spot lights, in RADIANS
};

struct Material {
    vec4 cAmbient, cDiffuse, cSpecular;
    float shininess;
    float blend;
};

in vec2 uvPos;
in vec3 worldPos;
in vec3 worldNormal;

out vec4 fragColor;

// // scene constants
// uniform float k_a, k_d, k_s;
// // object constants
// uniform Material material;
// // light constants
// uniform SceneLightData lights[8];
// camera position
uniform vec4 camPos;

// // textureS
// uniform sampler2D textImgSampler;
// uniform SceneFileMap filemap;


vec3 getDirectionToLight(vec3 pos, SceneLightData light)
{
  if (light.type == 0)
  {
    return normalize(vec3(-light.dir));
  }
  else
  {
    return normalize(vec3(light.pos) - pos);
  }
}

float falloff(float angleToLight, float innerAngle,
              float outerAngle)
{
  if (angleToLight < innerAngle)
    return 0.0;
  if (angleToLight > outerAngle)
    return 1.0;
  float angularCoeff = (angleToLight - innerAngle) / (outerAngle - innerAngle);
  return -2.0 * angularCoeff * angularCoeff * angularCoeff + 3.0 * angularCoeff * angularCoeff;
}

float calcAttenuation(SceneLightData light, vec3 worldPos)
{
    if (light.type == 0) return 1.0;
    vec3 L = getDirectionToLight(worldPos, light);
    float dist = length(vec3(light.pos) - worldPos);
    float attenuation = min(1.0, 1.0 / (light.function.x + light.function.y * dist + light.function.z * dist * dist));
    if (light.type == 2)
    {
      float angleFromLight = acos(dot(-L, normalize(vec3(light.dir))));
      float fallOffCoef = falloff(angleFromLight, light.angle - light.penumbra, light.angle);
      attenuation *= (1 - fallOffCoef);
    }
    return attenuation;
}


void main() {
    fragColor = vec4(abs(worldNormal), 1.0);
    // fragColor = vec4((k_a * material.cAmbient.rgb), 1.0);
    // vec3 N = normalize(worldNormal);            // surface normal
    // vec3 E = normalize(camPos.xyz - worldPos);  // intersection to camera
    
    // // Accumulate contributions from all lights
    // for(int i = 0; i < 8; i++) {

    //     if (!lights[i].enabled) continue; // Skip disabled lights
    //     float attenuation = calcAttenuation(lights[i], worldPos);        
            
    //     vec3 L = getDirectionToLight(worldPos, lights[i]);      // Directional to light
    //     // Calculate diffuse component
    //     float diffuse = max(dot(N, L), 0.0);
        
    //     // Calculate specular component
    //     vec3 R = reflect(-L, N);        // Reflected light direction wrt normal
    //     float RdotE = max(dot(R, E), 0.0);
    //     float specular = (RdotE > 0.0) ? pow(RdotE, material.shininess) : 0.0;
    //     vec3 objDiffuseColor = k_d * material.cDiffuse.rgb;
    //     vec3 objSpecularColor = k_s * material.cSpecular.rgb;
    //     if (filemap.isUsed) {
    //       ivec2 size = textureSize(textImgSampler, 0);
    //       vec2 repeatUV = fract(uvPos * vec2(filemap.repeatU, filemap.repeatV));
    //       vec3 texColor = texture(textImgSampler, repeatUV).rgb;
    //       objDiffuseColor = mix(objDiffuseColor, texColor, material.blend);
    //     }
        
    //     // Add this light's contribution
    //     vec3 contribution = lights[i].color.rgb * attenuation * 
    //                       (objDiffuseColor * diffuse+ objSpecularColor * specular);
    //     fragColor.rgb += contribution;
    // }
    
    // Ensure final color doesn't exceed maximum intensity
    fragColor = vec4(clamp(fragColor.rgb, 0.0, 1.0), 1.0);
}

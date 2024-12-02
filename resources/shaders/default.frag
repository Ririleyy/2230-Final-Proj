#version 330 core
#define PI     3.14159265359

// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
in vec3 worldPos;
in vec3 worldNormal;

// Task 10: declare an out vec4 for your output color
out vec4 fragColor;

// Task 12: declare relevant uniform(s) here, for ambient lighting
uniform float k_a;

// Task 13: declare relevant uniform(s) here, for diffuse lighting
uniform float k_d;
uniform vec4 lightPos;

// Task 14: declare relevant uniform(s) here, for specular lighting
uniform float k_s;
uniform float shininess;
uniform vec4 camPos;

const float t = 2.0;
const vec3 A = vec3(0.1787, -1.4630, -0.0193);
const vec3 B = vec3(-0.3554, 0.4275, 0.0888);
const vec3 C = vec3(-0.0227, 5.3251, -0.0005);
const vec3 D = vec3(0.1206, -2.5771, 0.2849);
const vec3 E = vec3(-0.0670, 0.3703, -0.2658);

vec3 XYZ2RGB(vec3 xyz) {
    return mat3(2.3706743,  -0.9000405, -0.4706338,
		        -0.5138850,  1.4253036,  0.0885814,
 		        0.0052982,  -0.0146949,  1.0093968) * xyz;
}

vec3 Yxy2XYZ( in vec3 Yxy )
{
	float Y = Yxy.r;
	float x = Yxy.g;
	float y = Yxy.b;

	float X = x * ( Y / y );
	float Z = ( 1.0 - x - y ) * ( Y / y );

	return vec3(X,Y,Z);
}

vec3 Yxy2RGB( in vec3 Yxy )
{
    return XYZ2RGB( Yxy2XYZ( Yxy ) );
}

void calculatePerezDistribution( in float t, out vec3 A, out vec3 B, out vec3 C, out vec3 D, out vec3 E )
{
	A = vec3(  0.1787 * t - 1.4630, -0.0193 * t - 0.2592, -0.0167 * t - 0.2608 );
	B = vec3( -0.3554 * t + 0.4275, -0.0665 * t + 0.0008, -0.0950 * t + 0.0092 );
	C = vec3( -0.0227 * t + 5.3251, -0.0004 * t + 0.2125, -0.0079 * t + 0.2102 );
	D = vec3(  0.1206 * t - 2.5771, -0.0641 * t - 0.8989, -0.0441 * t - 1.6537 );
	E = vec3( -0.0670 * t + 0.3703, -0.0033 * t + 0.0452, -0.0109 * t + 0.0529 );
}

vec3 perez(float theta, float gamma) {
    return (1.0+A*exp(B/cos(theta)))*(1.0+C*exp(D*gamma)+E*cos(gamma)*cos(gamma));
}

vec3 calculateZenithLuminanceYxy( in float t, in float thetaS )
{
	float chi  	 	= ( 4.0 / 9.0 - t / 120.0 ) * ( PI - 2.0 * thetaS );
	float Yz   	 	= ( 4.0453 * t - 4.9710 ) * tan( chi ) - 0.2155 * t + 2.4192;

	float theta2 	= thetaS * thetaS;
    float theta3 	= theta2 * thetaS;
    float T 	 	= t;
    float T2 	 	= t * t;

	float xz =
      ( 0.00165 * theta3 - 0.00375 * theta2 + 0.00209 * thetaS + 0.0)     * T2 +
      (-0.02903 * theta3 + 0.06377 * theta2 - 0.03202 * thetaS + 0.00394) * T +
      ( 0.11693 * theta3 - 0.21196 * theta2 + 0.06052 * thetaS + 0.25886);

    float yz =
      ( 0.00275 * theta3 - 0.00610 * theta2 + 0.00317 * thetaS + 0.0)     * T2 +
      (-0.04214 * theta3 + 0.08970 * theta2 - 0.04153 * thetaS + 0.00516) * T +
      ( 0.15346 * theta3 - 0.26756 * theta2 + 0.06670 * thetaS + 0.26688);

	return vec3( Yz, xz, yz );
}

vec3 calcPreethamLuminance(float theta, float theta_s, float gamma, float t)
{
    vec3 A, B, C, D, E;
    calculatePerezDistribution(t, A, B, C, D, E);
    vec3 f_ThetaGamma = perez(theta, gamma);
    vec3 f_ThetaS = perez(0.0, theta_s);
    vec3 Yxy = calculateZenithLuminanceYxy(t, theta_s);
    vec3 Yp = f_ThetaGamma * f_ThetaS / Yxy.x;
    return Yxy2RGB(Yp);
}

void main() {
    // Remember that you need to renormalize vectors here if you want them to be normalized

    // Task 10: set your output color to white (i.e. vec4(1.0)). Make sure you get a white circle!
    // fragColor = vec4(1.0);

    // Task 11: set your output color to the absolute value of your world-space normals,
    //          to make sure your normals are correct.
    fragColor = vec4(abs(worldNormal), 1.0);

    // Task 12: add ambient component to output color
    // fragColor = vec4(k_a, k_a, k_a, 1.0);
    // Task 13: add diffuse component to output color
    // vec3 L = normalize(lightPos.xyz - worldPos);
    // vec3 N = normalize(worldNormal);
    // float diffuse = clamp(dot(N, L), 0.0, 1.0);
    // fragColor += vec4(k_d * diffuse, k_d * diffuse, k_d * diffuse, 1.0);

    // Task 14: add specular component to output color
    // vec3 R = reflect(-L, N);
    // vec3 E = normalize(camPos.xyz - worldPos);
    // fragColor += vec4(k_s * specular, k_s * specular, k_s * specular, 1.0);
    // float specular = pow(clamp(dot(R, E), 0.0, 1.0), shininess);
}

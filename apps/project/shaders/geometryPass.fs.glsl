#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;
in vec3 vViewDirection;

layout(location = 0) out vec3 fPosition;
layout(location = 1) out vec3 fNormal;
layout(location = 2) out vec3 fAmbient;
layout(location = 3) out vec3 fDiffuse;
layout(location = 4) out vec4 fGlossyShininess;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;

uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform sampler2D uKaSampler;
uniform sampler2D uKdSampler;
uniform sampler2D uKsSampler;
uniform sampler2D uShininessSampler;
uniform sampler2D uNormalSampler;

mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv )
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
 
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord )
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    vec3 map = texture2D( uNormalSampler, vTexCoords ).xyz;
    map = map * 255./127. - 128./127.;
    mat3 TBN = cotangent_frame( N, -V, texcoord );
    return TBN * map;
}

void main()
{
    fPosition = vViewSpacePosition;
    vec3 normalMapTexel = vec3(texture(uNormalSampler, vTexCoords));
    
    if(normalMapTexel != vec3(0, 0, 0)){
		fNormal = normalize( vViewSpaceNormal );
		fNormal = perturb_normal( fNormal, vViewDirection, vTexCoords );
		fNormal = normalize(fNormal);
	}
	else{
		fNormal = normalize(vViewSpaceNormal);
	}
	
    vec3 ka = uKa * vec3(texture(uKaSampler, vTexCoords));
    vec3 kd = uKd * vec3(texture(uKdSampler, vTexCoords));
    vec3 ks = uKs * vec3(texture(uKsSampler, vTexCoords));
    float shininess = uShininess * vec3(texture(uShininessSampler, vTexCoords)).x;

    fAmbient = ka;
    fDiffuse = kd;
    fGlossyShininess = vec4(ks, shininess);
}


//#extension GL_OES_standard_derivatives : enable

uniform vec2 uResolution;
uniform vec2 test;
varying vec2 vTextureCoord;
uniform sampler2D uSampler;

const float sqrt2 = 1.4142;

float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(11.9898, 5.1414))) * 4358.5453);
}

float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);
	
	float res = mix(
		mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
		mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
	return res*res;
}

float height(vec2 p) {
	float h = pow(noise(p), 4.);
	h = mix(h, noise(p*2.3), h/2.);
	h = mix(h, noise(p*4.1), h/3.);
	h = mix(h, noise(p*7.5), (1.-h)/256.);
	return h;
}

vec3 sample(vec2 uv) {
	const float uvScale = 20.0;
	float h = height(uv * uvScale);
	
	float thresh = 1.0;
	const float stp = 1.0 / 2048.0;
	for(float thresh = 1.0; thresh > 0.0; thresh -= stp * 10.) {
		if(h > thresh || h < 0.0)
			break;
		uv.y += (sqrt2 * stp);
		h = height(uv * uvScale);
		
	}
	return vec3(uv, h);
}

vec3 msample(vec2 uv) {
	vec3 s = vec3(0);
	const float stp = 1.0 / 1024.0;
	for(float x = -stp; x <= stp; x += stp) {
		for(float y = -stp; y <= stp; y += stp) {
			s += sample(uv+vec2(x,y));
		}
	}
	s /= 9.0;
	return s;
}

vec3 normal(vec2 uv) {
	float stp = 1.0 / uResolution.y;
	vec3 t = sample(uv + vec2(0, 0));
	vec3 b = sample(uv - vec2(0, stp));
	vec3 r = sample(uv + vec2(stp, 0));
	vec3 l = sample(uv - vec2(stp, 0));
	vec3 nx = r - l;
	vec3 ny = t - b;
	const float thresh = 0.025;
	if(length(ny) > thresh) {
		b = t;
		t = sample(uv + vec2(0, stp));		
		ny = t - b;
	}
	return normalize(cross(nx, ny));
}

vec3 light(vec2 uv) {
	vec3 sunDir = normalize(vec3(0.8, -0.9, 0.5));
	vec3 l = vec3(1.0, 0.9, 0.8) * max(dot(normal(uv), sunDir), 0.0);
	vec3 a = vec3(0.1, 0.2, 0.3);
	return max(l, a);
}

void main( void ) {

	vec2 uv = vTextureCoord * uResolution.y * 0.002;
	uv.y = 1.0 - uv.y;
	uv.y *= sqrt2; 
	
	vec3 n = light(uv); 

	gl_FragColor = vec4( n, 1.0 );

}

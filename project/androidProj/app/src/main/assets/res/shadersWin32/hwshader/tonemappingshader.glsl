#ifdef GEAR_VERTEX_SHADER
attribute vec4 a_vertex_coord_v4;
uniform mat4 u_mvp_m4x4;
attribute vec2 a_uv_coord0_v2;
varying vec2 v_uvcoord0;

void main()
{
	gl_Position = u_mvp_m4x4 * a_vertex_coord_v4;
    
    v_uvcoord0=a_uv_coord0_v2;
}

#elif defined (GEAR_FRAGMENT_SHADER)
uniform sampler2D u_diffuse_texture;
varying vec2 v_uvcoord0;
uniform sampler2D u_bloomblur_texture;
uniform float u_exposure;

void main()
{
	const float bloomfactor = 1.2;
	const float brightMax = 6.0;
	
    vec3 hdrColor = texture2D(u_diffuse_texture, v_uvcoord0).rgb;      
    vec3 bloomColor = texture2D(u_bloomblur_texture, v_uvcoord0).rgb;
    hdrColor += bloomColor * bloomfactor; // additive blending
	
	// Perform tone-mapping
	//float Y = dot(vec3(0.30, 0.59, 0.11), hdrColor);
	float YD = u_exposure * (u_exposure/brightMax + 1.0) / (u_exposure + 1.0);
	hdrColor *= YD;
	vec4 color = vec4(hdrColor, 1.0);
	gl_FragColor = color;
}
#endif

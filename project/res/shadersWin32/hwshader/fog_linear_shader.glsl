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

uniform sampler2D u_base_texture;
uniform sampler2D u_depth_texture;
varying vec2 v_uvcoord0;
//uniform float fog_start;
uniform float fog_end;
uniform float fog_scale;
uniform vec4 fog_color;

void main()
{
	vec4 depth_pixel=texture2D(u_depth_texture, v_uvcoord0);
	float depth_z = 1.0-depth_pixel.z;
	float fog = (fog_end + depth_z) * fog_scale;
	vec4 color = mix(fog_color, texture2D(u_base_texture, v_uvcoord0), fog);
    gl_FragColor = color;
}
#endif

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
uniform vec2 u_uvtile;
varying vec2 v_uvcoord0;
uniform vec4 u_color;

void main()
{
	gl_FragColor = texture2D(u_diffuse_texture, v_uvcoord0*u_uvtile)*u_color;
}
#endif

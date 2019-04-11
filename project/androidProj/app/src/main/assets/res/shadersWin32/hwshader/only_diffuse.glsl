#ifdef GEAR_VERTEX_SHADER
attribute vec4 a_vertex_coord_v4;
uniform mat4 u_mvp_m4x4;

void main()
{
	gl_Position = u_mvp_m4x4 * a_vertex_coord_v4;
}

#elif defined (GEAR_FRAGMENT_SHADER)

uniform vec4 u_diffuse_v4;

void main()
{
	gl_FragColor = u_diffuse_v4;
}
#endif

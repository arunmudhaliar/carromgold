#ifdef GEAR_VERTEX_SHADER
attribute vec4 a_vertex_coord_v4;
uniform mat4 u_depth_mvp_m4x4;
uniform mat4 u_mvp_m4x4;

void main()
{
	gl_Position = (u_depth_mvp_m4x4 * u_mvp_m4x4) * a_vertex_coord_v4;
}

#elif defined (GEAR_FRAGMENT_SHADER)

//uniform vec4 diffuse;

void main()
{
	//gl_FragColor = diffuse;
}
#endif

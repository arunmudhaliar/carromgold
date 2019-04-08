#ifdef GEAR_VERTEX_SHADER
attribute vec4 a_vertex_coord_v4;
attribute vec4 a_color_coord_v4;

uniform mat4 u_mvp_m4x4;

varying vec4 v_color;

void main()
{
	v_color = a_color_coord_v4;
	gl_Position = u_mvp_m4x4 * a_vertex_coord_v4;
}

#elif defined (GEAR_FRAGMENT_SHADER)

varying vec4 v_color;
void main()
{
	gl_FragColor = v_color;
}

#endif

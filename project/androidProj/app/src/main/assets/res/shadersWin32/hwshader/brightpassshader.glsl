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

void main()
{
    vec4 color = texture2D(u_diffuse_texture, v_uvcoord0);    
	vec4 output_rgb= vec4(0.0, 0.0, 0.0, 1.0);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));	//0.2126, 0.7152, 0.0722
	if(brightness > 0.5)
        output_rgb = color;

    gl_FragColor = output_rgb;
}
#endif

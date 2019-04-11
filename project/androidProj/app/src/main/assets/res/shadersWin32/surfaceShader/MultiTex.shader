Shader "MultiTex" {
Properties {
	_Color ("Main Color", Color) = (1,1,1,1)
	_MainTex ("Base (RGB) Gloss (A)", Tex2D) = "white" {}
	_SecTex ("Secondry Map", Tex2D) = "secondry" {}
	_MainTexFactor ("MainTex Factor", Range) = (0, 1) = 0.5
	_SecTexFactor ("SecTex Factor", Range) = (0, 1) = 0.5
	_ColorFactor ("Color Factor", Range) = (0, 1) = 0.5
	_GlobalFactor ("Global Factor", Range) = (0, 1) = 1
	_Global2Factor ("Global2 Factor", Range) = (0, 1) = 0.7
	_Color2 ("Color2", Color) = (0.5,0.5,0.5,1)

}

SubShader {
__Pass{
	__includeModule
	{
		PointLightStruct
		MaterialStruct
	}
	
	__vertex{
		// User-specified properties
		attribute vec2 uv_in_SecTex;
		varying vec2 uv_out_SecTex;
		attribute vec2 uv_in_MainTex;
		varying vec2 uv_out_MainTex;

		vec4 vertex_function()
		{
            uv_out_SecTex = uv_in_SecTex;
			uv_out_MainTex = uv_in_MainTex;
			return GEAR_MVP * vIN_Position;
		}
	}

	__fragment{
	    uniform sampler2D sampler2d_SecTex;
	    uniform sampler2D sampler2d_MainTex;
		varying vec2 uv_out_SecTex;
		varying vec2 uv_out_MainTex;

		vec4 fragment_function()
		{
 
            return vec4((texture2D(sampler2d_MainTex, vec2(uv_out_MainTex.s+GEAR_Time.z, uv_out_MainTex.t))*_MainTexFactor + texture2D(sampler2d_SecTex, vec2(uv_out_SecTex.s+GEAR_Time.y, uv_out_SecTex.t))*_SecTexFactor + _Color*_ColorFactor + _Color2*_ColorFactor).rgb*_GlobalFactor*_Global2Factor, 1);
		}
	}
}
}
}
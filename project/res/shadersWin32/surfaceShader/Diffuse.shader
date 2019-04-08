Shader "Diffuse" {
Properties {
	_MainTex ("Base (RGB)", Tex2D) = "white" {}
}

SubShader {
__Pass{

	__tag{
		CULLFACE( GL_BACK )
	}
	
	__vertex{
		attribute vec2 uv_in_MainTex;
		varying vec2 uv_out_MainTex;
		
		vec4 vertex_function()
		{
			uv_out_MainTex = uv_in_MainTex;
			return GEAR_MVP * vIN_Position;
		}
	}

	__fragment{
		varying vec2 uv_out_MainTex;
		uniform sampler2D sampler2d_MainTex;

		vec4 fragment_function()
		{
			return texture2D(sampler2d_MainTex, uv_out_MainTex);
		}
	}
}

__Pass{
	__includeModule
	{
		PointLightStruct
		MaterialStruct
	}
	
	__tag{
		CULLFACE( GL_BACK )
		SHADOW( NORMAL )
	}

	__vertex{
		 
		varying vec4 vOUT_Position;
		varying vec4 vOUT_Normal;
		vec4 vertex_function()
		{
			vOUT_Position = GEAR_MODEL_MATRIX * vIN_Position;
			vOUT_Normal = vIN_Normal;
			return GEAR_MVP * vIN_Position;
		}
	}

	__fragment{
		uniform vec3 _WorldSpaceCameraPos;
		varying vec4 vOUT_Position;
		varying vec4 vOUT_Normal;
		vec4 pointLight()
		{
            mat4 modelMatrixInverse = GEAR_MODEL_INVERSE; // unity_Scale.w is unnecessary because we normalize vectors
 
            vec3 normalDirection = normalize(vec3(vOUT_Normal * modelMatrixInverse));
            vec3 viewDirection = normalize(_WorldSpaceCameraPos - vec3(vOUT_Position));
            vec3 lightDirection;
            float attenuation;
 
            if (0.0 == light.position.w) // directional light?
            {
               attenuation = 1.0; // no attenuation
               lightDirection = normalize(vec3(light.position));
            } 
            else // point or spot light
            {
               vec3 vertexToLightSource = vec3(light.position - vOUT_Position);
               float distance = length(vertexToLightSource);
               //attenuation = 1.0 / distance; // linear attenuation 
				attenuation = 1.0 / (light.constant_attenuation +
						 light.linear_attenuation * distance +
						 light.quadratic_attenuation * distance * distance);
               lightDirection = normalize(vertexToLightSource);
            }
 
            vec3 ambientLighting = vec3(light.ambient) * vec3(material.ambient);
            vec3 diffuseReflection = attenuation * vec3(light.diffuse) * vec3(material.diffuse) * max(0.0, dot(normalDirection, lightDirection));
 
            vec3 specularReflection;
            if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
            {
               specularReflection = vec3(0.0, 0.0, 0.0); 	// no specular reflection
            }
            else // light source on the right side
            {
               specularReflection = attenuation * vec3(light.specular) * vec3(material.specular) * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), material.shininess);
            }
 
            vec3 diffuseColor = ambientLighting + diffuseReflection;
            vec3 specularColor = specularReflection;

			return vec4(diffuseColor + specularColor, 1.0);
		}
		
		vec4 fragment_function()
		{
			return pointLight();
		}
	}
}
}
}
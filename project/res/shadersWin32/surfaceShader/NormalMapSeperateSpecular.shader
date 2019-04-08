Shader "NormalMapSeperateSpecular" {
Properties {
	_MainTex ("Base (RGB) Gloss (A)", Tex2D) = "white" {}
	_BumpMap ("Normal Map", Tex2D) = "bump" {}
	_SpecMap ("Specular Map", Tex2D) = "spec" {}
}

SubShader {
__Pass{
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
	
	__vertex{
		// User-specified properties
		attribute vec2 uv_in_BumpMap;
		varying vec2 uv_out_BumpMap;
		attribute vec2 uv_in_SpecMap;
		varying vec2 uv_out_SpecMap;
		attribute vec4 Tangent;

		// The following built-in uniforms (except _LightColor0) 
		// are also defined in "UnityCG.glslinc", 
		// i.e. one could #include "UnityCG.glslinc" 
		uniform vec3 _WorldSpaceCameraPos; 
		varying mat3 localSurface2World; // mapping from local 
		// surface coordinates to world coordinates
		
		varying vec3 vOUT_WorldSpaceCameraPos;
		varying vec4 vOUT_Position;
		vec4 vertex_function()
		{
			mat4 modelMatrix = GEAR_MODEL_MATRIX;
            mat4 modelMatrixInverse = GEAR_MODEL_INVERSE; // unity_Scale.w 
            // is unnecessary because we normalize vectors
 
            localSurface2World[0] = normalize(vec3(GEAR_MODEL_MATRIX * vec4(vec3(Tangent), 0.0)));
            //localSurface2World[0] = normalize(vec3(GEAR_MODEL_MATRIX * vec4(Tangent, 0.0)));
            localSurface2World[2] = normalize(vec3(vIN_Normal * GEAR_MODEL_INVERSE));
            localSurface2World[1] = normalize(cross(localSurface2World[2], localSurface2World[0]) * Tangent.w); // factor Tangent.w is specific to Unity
 
			vOUT_WorldSpaceCameraPos = _WorldSpaceCameraPos;
            vOUT_Position = GEAR_MODEL_MATRIX * vIN_Position;
            uv_out_BumpMap = uv_in_BumpMap;
			uv_out_SpecMap = uv_in_SpecMap;
			return GEAR_MVP * vIN_Position;
		}
	}

	__fragment{
	    uniform sampler2D sampler2d_BumpMap;
		uniform sampler2D sampler2d_SpecMap;
		varying vec2 uv_out_BumpMap;
		varying vec2 uv_out_SpecMap;
		varying vec4 vOUT_Position;
		varying vec3 vOUT_WorldSpaceCameraPos;
		varying mat3 localSurface2World;
		//uniform vec4 _BumpMap_ST;
		vec4 fragment_function()
		{
            // in principle we have to normalize the columns of 
            // "localSurface2World" again; however, the potential 
            // problems are small since we use this matrix only to
            // compute "normalDirection", which we normalize anyways
 
            //vec4 encodedNormal = texture2D(sampler2d_BumpMap, _BumpMap_ST.xy * uv_out_BumpMap.xy + _BumpMap_ST.zw);
            vec4 encodedNormal = texture2D(sampler2d_BumpMap, uv_out_BumpMap);
			vec3 localCoords = normalize (vec3(encodedNormal * 2.0 - 1.0));
            localCoords.z = sqrt(dot(localCoords, localCoords));
               // approximation without sqrt: localCoords.z = 
               // 1.0 - 0.5 * dot(localCoords, localCoords);
            vec3 normalDirection = normalize(localSurface2World * localCoords);
            vec3 viewDirection = normalize(vOUT_WorldSpaceCameraPos - vec3(vOUT_Position));
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
 
			vec4 spectextureColor = texture2D(sampler2d_SpecMap, uv_out_SpecMap);
            vec3 specularReflection;
            if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
            {
               specularReflection = vec3(0.0, 0.0, 0.0); 	// no specular reflection
            }
            else // light source on the right side
            {
               specularReflection = attenuation * vec3(light.specular) * vec3(material.specular) * (spectextureColor.g) * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), material.shininess);
			}

            return vec4(ambientLighting + diffuseReflection + specularReflection, 1.0);
		}
	}
}
}
}
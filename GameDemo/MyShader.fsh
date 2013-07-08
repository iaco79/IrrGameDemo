#ifdef GL_ES
precision highp float;
precision highp int;
#endif


varying vec4 varVertexColor;
varying vec4 varTexCoord; 
uniform sampler2D uTextureUnit0;


vec4 renderColor(void)
{
	vec4 color;
	/*vec4 texel = texture2D(uTextureUnit0, varTexCoord.xy);	
	
	color.rgb = texel.a > 0.9 ? clamp((texel.rgb/4.0*3.0) + varVertexColor.rgb, 0.7,1.0)
							   : varVertexColor.rgb;
		
	
	color.a = texel.a; */
	
	color = varVertexColor;
	color.a = varVertexColor.a;
	
	return color;
	 
}

void main (void)
{	
	gl_FragColor = renderColor();
	
		
}

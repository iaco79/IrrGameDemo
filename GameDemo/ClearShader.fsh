#ifdef GL_ES
precision highp float;
precision highp int;
#endif


varying vec4 varVertexColor;
varying vec4 varTexCoord; 
uniform sampler2D uTextureUnit0;


vec4 renderColor(void)
{
	
	vec4 texel = texture2D(uTextureUnit0, varTexCoord.xy);	
	
	return texel;
	 
}

void main (void)
{	
	gl_FragColor = renderColor();	
}

#version 130
uniform sampler2D texSM, tex;
uniform float alpha_th;
uniform float isLabelDepth;
in vec4 theColor;


void main(void)
{
    vec4 temp_a = texture2D(texSM, gl_TexCoord[0].st);
    vec4 temp_c = texture2D(tex, gl_TexCoord[0].st);

    if(temp_a.a<alpha_th)
        discard;
    else
    {
        if(isLabelDepth>0.5)
        {
            gl_FragColor = theColor;
//            if(theColor.g>1)
//                gl_FragColor = vec4(0.0,0.0,1.0,1.0);

//            if(theColor.g<0)
//                gl_FragColor = vec4(0.0,0.0,1.0,1.0);
        }
        else
            gl_FragColor = temp_c;

    }

}

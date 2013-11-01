void main()
{

    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[7] = gl_MultiTexCoord0+vec4(0.1,0.1,1.0,0.0);
    gl_Position = ftransform();
}

attribute vec4 gl_Color;
varying out vec4 theColor;

void main()
{

    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = ftransform();
    theColor = gl_Color;
}

#version 130
out vec4 theColor;
out vec4 thePosition;

void main()
{

    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = ftransform();
    thePosition = gl_Position;
    theColor = gl_Color;
}

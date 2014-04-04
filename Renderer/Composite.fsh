#version 130


uniform sampler2D qt_Texture0;
varying highp vec4 qt_TexCoord0;

uniform float alpha;
uniform sampler2D tex_SM, tex_LD,  tex_DI_Dark, tex_DI_Bright, tex_BG, tex_Env, tex_Shade, tex_Disp;
uniform float dist;

//varying float F;
uniform float width, height;

//const float half_col = 3, half_row = 3;
uniform float filter_size;
uniform float amb_strength;
uniform float surface_disp;
uniform float LOD;

//varying float filter_sum;
uniform vec3 light_dir;
uniform bool toggle_Mirror;
uniform int toggle_ShaAmbCos;

//0-render results, 1-ShapeMap, 2-DarkImage, 3-BrightImage, 4-BGImage, 5-LabelImage, 6-DepthImage
uniform int cur_tex;
uniform float SM_Quality;
uniform float Cartoon_sha=0.1;
uniform bool toggle_Point;

//the array that stores all the reflection/refraction parameters
uniform float refValues[10];
uniform int refToggled[10];

//the area that stores all the average normals and center positions(it is now bottom right position)
uniform vec3 normalValues[10];
uniform vec3 centerDepth[10];
//every 4 of them represent bounding value for -x,x,-y,y
uniform float boundingbox[40];
//whether this layer can cast shadow on other layer
uniform int shadowcreator[10];

in vec4 thePosition;

void getCurLabelDepth(in vec3 p, out int label, out float depth)
{
    vec4 label_depth = texture2D(tex_LD, (p.xy+1)/2);
    label = int(label_depth.b*255+0.1);
    depth = label_depth.g;
}
bool findLayerIntersect(in vec3 Sp, in vec3 Sv, in int LayerID, out vec3 Ip)
{
    vec3 l0 = Sp;
    vec3 l = Sv;
    vec3 P0 = centerDepth[LayerID];
    vec3 n = normalValues[LayerID];
    float d;
    float dot_d = dot(n,l);
    if(dot_d==0)
        return false;
    d = dot(P0-l0,n)/dot_d;
//    if(d<0)
//        return false;
//    else
    {
        Ip = l0+d*l;
        int label;
        float depth;
        getCurLabelDepth(Ip, label, depth);
        if(label == LayerID&&shadowcreator[label]!=0)
            return true;
    }
//    return true;
}



void getEstNormal(in vec2 coord, in float sm_smooth, out vec4 raw_value, out vec3 est_normal)
{
    //Shape map normal of current position
    //the SM value of current point pos
    //cur_p is in the secene space, x-(-1,1), y-(-1,1)
    //it has to be converted to texture space
    raw_value.rg = 1*1*(2*texture2D(tex_SM,coord).rg-1.0);
    raw_value.ba = 1*1*(texture2D(tex_SM,coord)).ba;
   //raw_value = raw_value*raw_value.a;
    //assuming a normalized normal value
    float   temp    = 1-raw_value.r*raw_value.r-raw_value.g*raw_value.g;
    if(temp<0.)
        temp=0.;
    //SM_normal - n_i the normal from shape map
    est_normal   = normalize(vec3(raw_value.r, raw_value.g, sqrt(temp)));
}

////function to calculate the reflection and refraction effect of image
//void ReflectRefract(in vec4 cood_color, out vec4 refractColor, out vec4 reflectColor)
//{
//    //get color(coordinates) of surface image

//    vec2 cood_refl = (cood_color.st);
//    vec2 cood_displace = (cood_color.st-0.49803921568627450980392156862745)*cood_color.p*dist;
//    vec2 cood_refr = gl_TexCoord[0].st+cood_displace;

////    refractColor =  texture2D(tex_BG, cood_refr, LOD+20*length(cood_displace));

//    refractColor =  textureLod(tex_BG, cood_refr, LOD+20*length(cood_displace));
//    reflectColor =  textureLod(tex_env, cood_refl+(light_dir.rg)*0.2/(0.1+abs(light_dir.b)), LOD);
//}

void GetReflectColor(in vec3 true_position, in vec3 SM_normal, in int label, out vec4 ref_color)
{
    vec3 cur_normal = mix(SM_normal,normalValues[label], 0.95);
    vec3 ref_eye = (dist+1)*2*cur_normal.z*cur_normal+vec3(0,0,-1);
    vec3 ref_position;

    for(int j=1;j<10;j++)
    {
        int LayerInd = j;
        //if this layer is empty
        if(length(normalValues[LayerInd])==0)
            break;
        if(LayerInd == label)
            continue;
        if(findLayerIntersect(true_position, ref_eye, LayerInd, ref_position))
        {
            ref_color = texture2D(tex_Shade, (ref_position.rg+1)/2);
            return;
        }
    }
    if(ref_eye.z>0)
    {
        ref_color =  textureLod(tex_Env, gl_TexCoord[0].st+normalize(ref_eye.xy)*true_position.z+(light_dir.rg)*0.1/(0.1+abs(light_dir.b)), LOD);
//        ref_color =  textureLod(tex_Env, (gl_TexCoord[0].st+normalize(ref_eye.xy)*true_position.z), LOD+20*length(refValues[label]));
    }
    else
    {
        ref_color =  textureLod(tex_BG, (gl_TexCoord[0].st+normalize(ref_eye.xy)*true_position.z), LOD+20*length(refValues[label]));
    }
}

void GetEnvColor(in vec3 true_position, in vec3 SM_normal, in int label, out vec4 reflectColor)
{
    vec3 cur_normal = mix(SM_normal,normalValues[label], 0.5);
    vec2 cood_refl = gl_TexCoord[0].st+cur_normal.xy;
    reflectColor =  textureLod(tex_Env, cood_refl+(light_dir.rg)*0.2/(0.1+abs(light_dir.b)), LOD);
}

void GetRefractColor(in vec3 true_position, in vec3 SM_normal, in int label, out vec4 refractColor)
{
    vec3 cur_normal = mix(SM_normal,normalValues[label], 0.5);
    vec2 cood_refr = gl_TexCoord[0].st+cur_normal.xy*refValues[label];
    vec4 label_depth = texture2D(tex_LD, cood_refr);

//    float true_depth;
//    getCurLabelDepth(true_position,label,true_depth);
//    true_position.b = true_depth;

//    if(true_position.b>label_depth.g)
//    {
////        cood_refr = gl_TexCoord[0].st+cur_normal.xy*refValues[label]*(true_position.b-label_depth.g);
//        refractColor =  textureLod(tex_Shade, cood_refr, LOD+20*length(refValues[label]));
//    }
//    else
        refractColor =  textureLod(tex_BG, cood_refr, LOD+20*length(refValues[label]));

}

void FrenselBlend(in vec3 SM_normal, in vec4 refractColor, in vec4 reflectColor, in int label, out vec4 Ref_Fresnel)
{
    vec3 cood_color = mix(SM_normal,normalValues[label], 0);
    float Ratio;
    if(refToggled[label]!=0)
    {
        Ratio=1;
        //if(cood_color.p < 1) Ratio=Ratio*cood_color.p;
    }
    else
    {
        float temp = abs(cood_color.s);
        float temp2 = abs(cood_color.t);
        if(temp < temp2 ) temp = temp2;
        float teta = 2.0* temp;
        float t = (0.5+0.5*refValues[label]);
        float c = 0.8*(1.0-t)+1.0*t;
        float bc= 0.6*(1.0-t)+0.7*t;
        float ac= 0.5*(1.0-t)+0.2*t;
        float b=  1.0*(1.0-t)+0.1*t;
        if(teta< ac)    {
            Ratio= b- b/ac*teta;
        }
        else
            if(teta< bc)    {
                Ratio = 0.0;
            }
            else        {
                Ratio= (teta -bc) /(c-bc);
            }
        if(Ratio>1.0)
            Ratio = 1.0;
        //    cood_color.p*2-1;
        Ratio=Ratio/4.0;
    }
    //the reflection and refraction blended based on Fresnel
    Ref_Fresnel  = mix(refractColor, reflectColor, Ratio);
}


void main(void)
{
    if(cur_tex == 0)
    {
        vec3 true_position = thePosition.rgb;
        int label;
        float true_depth;
        getCurLabelDepth(true_position,label,true_depth);
        true_position.b = true_depth;

        //this position does not have any object
        if(label==0)
        {
            gl_FragColor = texture2D(tex_BG, gl_TexCoord[0].st);
            return;
        }
        //    gl_FragColor = vec4(refToggled[label]);
        //    return;

        vec4 Direct_color = texture2D(tex_Shade, gl_TexCoord[0].st);//texture2D(tex_SM, qt_TexCoord0.st);


        //no transparancy
        if((Direct_color.a==1)&&(refToggled[label]==0))
        {
            gl_FragColor = Direct_color;
            //        return;
        }

        vec3 SM_normal;
        vec4 SM_value;
        getEstNormal((thePosition.rg+1)/2,1, SM_value, SM_normal);
        //    gl_FragColor = vec4(normalValues[label],1.0);
        vec4 refl_color;
        //    if(!GetReflectColor(true_position, SM_normal, label, refl_color))
        //    {
        //        GetEnvColor(true_position,SM_normal,label,refl_color);
        //    }
        GetReflectColor(true_position, SM_normal, label, refl_color);
        if(refToggled[label]!=0)
        {
            gl_FragColor = refl_color;
            return;
        }

        vec4 refractColor;
        GetRefractColor(true_position,SM_normal,label, refractColor);

        //    gl_FragColor = org_color;


        vec4 Indirect_color;
        FrenselBlend(SM_normal,refractColor,refl_color,label,Indirect_color);
        vec4 Fore_blend = mix(Indirect_color, Direct_color, alpha*Direct_color.a);

        //    gl_FragColor = Indirect_color;


        vec4 cood_center = (texture2D(tex_SM, gl_TexCoord[0].st));
        float Final_Mix = cood_center.a;
        if (cood_center.b<0.001)
            Final_Mix=Final_Mix*cood_center.b;
        gl_FragColor = vec4(mix(texture2D(tex_BG, gl_TexCoord[0].st).rgb, Fore_blend.rgb, Final_Mix), 1.0);
    }
    else if(cur_tex == 1)
            gl_FragColor = texture2D(tex_SM, gl_TexCoord[0].st);
    else if(cur_tex == 2)
            gl_FragColor = texture2D(tex_DI_Dark, gl_TexCoord[0].st);
    else if(cur_tex == 3)
            gl_FragColor = texture2D(tex_DI_Bright, gl_TexCoord[0].st);
    else if(cur_tex == 4)
            gl_FragColor = texture2D(tex_BG, gl_TexCoord[0].st);
    else if(cur_tex == 5)
            gl_FragColor = texture2D(tex_LD, gl_TexCoord[0].st).bbba;
    else if(cur_tex == 6)
            gl_FragColor = texture2D(tex_LD, gl_TexCoord[0].st).ggga;
    else if(cur_tex == 7)
            gl_FragColor = texture2D(tex_Env, gl_TexCoord[0].st);
    else if(cur_tex == 8)
            gl_FragColor = texture2D(tex_Disp, gl_TexCoord[0].st);
}

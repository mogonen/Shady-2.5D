#version 130


uniform float alpha;
uniform sampler2D tex_SM, tex_DI_Dark, tex_DI_Bright, tex_BG, tex_LD, tex_Env;
uniform float dist;

//varying float F;
uniform float width, height;

//const float half_col = 3, half_row = 3;
uniform float filter_size;
uniform float amb_strength;
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
//the arra that stores all the average normals
uniform vec3 normalValues[10];

//function to calculate ambient occlusion effect of image
void AmbientShadow(in vec4 center, out float Amb)
{
    float i,j;
    float max, min;
    //    float sum = float((half_row*(half_row+1)+half_row+1)*(2*half_col+1)+(half_col*(half_col+1)+half_col+1)*(2*half_row+1));
    float diff = 0.0;
    float step = filter_size/2.0;
    float amb_filter_size = filter_size/8.0;
    float    filter_sum = 0.1;
    float lenght;
    if(step>0.01)
    {
        for(i=-(filter_size);i<(filter_size)+step/2.0;i+=step)
            for(j=-(filter_size);j<(filter_size)+step/2.0;j+=step)
            {
                vec4 neighbor = texture2D(tex_SM, vec2(gl_TexCoord[0].s+i/width, gl_TexCoord[0].t+j/height));
                if((i>step/2.0||i<-step/2.0)&&(j>step/2.0||j<-step/2.0))
                {
                    lenght= pow(2,-(i*i+j*j)/(filter_size*filter_size));
                    diff = diff + dot(neighbor.rg - center.rg,vec2(i,j))*lenght*neighbor.b*neighbor.a;//sqrt(i*i+j*j)
                    filter_sum = filter_sum+lenght*neighbor.b*neighbor.a;
                }
            }

    }
    Amb = diff*center.a/filter_sum;

    float c_center=0.3*amb_strength-0.2;
    float c_delta=Cartoon_sha;

    float b=c_center+c_delta;
    float a=c_center-c_delta;
    if(Cartoon_sha!=0)
        Amb=(Amb-a)/(b-a);
    if(Amb<0.0)
        Amb=0.0;
    if(Amb>1.0)
        Amb=1.0;

}




//function to calculate point light shadow effect of image
void Shadow1(in vec2 center, out float center_cos, out float Sha, out float Spe)
{
    float pure_cos;
    vec2 point_light = 0.5*light_dir.rg + gl_TexCoord[0].st - vec2(0.5,0.5);
    float point_light_lenght = length(point_light);
    vec2 point_light_norm;
    point_light_norm = normalize(point_light);

    vec4 neighbor = SM_Quality*(2*texture2D(tex_SM,gl_TexCoord[0].st )-1.0);
    float   temp    = 1-neighbor.r*neighbor.r-neighbor.g*neighbor.g;  if(temp<0.) temp=0.;
    vec3 center_normal   = normalize(vec3(neighbor.r, neighbor.g, sqrt(temp)));
    vec2 center_normal_2D   = vec2(neighbor.r, neighbor.g);


    pure_cos = dot(center_normal_2D, -point_light) +0.0125;
    pure_cos = pure_cos/(point_light_lenght+0.0125);


    float i;
    float diff = 0.0;
    //float step = (point_light_lenght*10.0+1.0)*filter_size/(point_light_lenght*20.0+2.0);
    float step = filter_size/4.;
    float integral = 0.0;
    float max_int=-1;
    int count = 0;
    float shadow_amount=1.0;

    if(step>0.005)
    {
        for(i=0;i<((point_light_lenght*10.0+5.0)*filter_size);i+=step)
        {
            vec4 neighbor = texture2D(tex_SM, gl_TexCoord[0].st - i*point_light_norm/vec2(width, height));
            float coef= neighbor.b*neighbor.a;
            if(coef >0.2)
            {
                vec4 neighbor = SM_Quality*(2.0*neighbor-1.0);
                temp = 1-neighbor.r*neighbor.r-neighbor.g*neighbor.g; if(temp<0.1) temp=0.1;
                neighbor.z =      sqrt(temp);
                neighbor = normalize(neighbor);
                integral += (point_light_norm.x*neighbor.r+point_light_norm.y*neighbor.g);

                count++;
                if(max_int <   integral/count -light_dir.z/2 +0.1) max_int =  integral/count -light_dir.z/2+0.1;



            }
            else
            {
                integral -= 1.0;
                count++;
                if(max_int < integral/count -light_dir.z/2 +0.1) max_int =  integral/count -light_dir.z/2+0.1;

            }
        }

    }

    float c_center=0.3*amb_strength-0.3;
    float c_delta=Cartoon_sha;

    float b=c_center+c_delta;
    float a=c_center-c_delta;

    if(Cartoon_sha!=0)
        Sha=(1-max_int-a)/(b-a);
    else
        Sha = 1-max_int;
    if(Sha<0.0)
        Sha=0.0;
    if(Sha>1.0)
        Sha=1.0;

    if(Cartoon_sha!=0)
        center_cos=(pure_cos-a)/(b-a);
    else
        center_cos = pure_cos;
    if(center_cos<0.0)
        center_cos=0.0;
    if(center_cos>1.0)
        center_cos=1.0;
}



//function to calculate parallel light shadow effect of image
void Shadow2(in vec2 center, out float center_cos, out float Sha, out float Spe)
{
    float pure_cos;
    vec2 light_dir_xy = normalize(light_dir.rg);
    vec3 light_dir_n = normalize(light_dir);


    vec4 neighbor = SM_Quality*(2*texture2D(tex_SM,gl_TexCoord[0].st )-1.0);
    float   temp    = 1-neighbor.r*neighbor.r-neighbor.g*neighbor.g;  if(temp<0.) temp=0.;
    vec3 center_normal   = normalize(vec3(neighbor.r, neighbor.g, sqrt(temp)));
    vec2 center_normal_2D   = vec2(neighbor.r, neighbor.g);


    /***Parallel lights. They give slightly different effects.***/
    //pure_cos = dot(center_normal, -light_dir_n);
    pure_cos = dot(center_normal_2D, (light_dir.rg)*light_dir.b);
    // pure_cos = dot(center_normal_2D, (-light_dir.rg));



    float i;
    float diff = 0.0;
    float step = filter_size/4.;
    float integral = 0.0;
    float max_int=-1;
    int count = 0;
    float shadow_amount=1.0;

    if(step>0.005)
    {
        for(i=0;i<(4*filter_size);i+=step)
        {
            vec4 neighbor = texture2D(tex_SM, gl_TexCoord[0].st - i*light_dir_xy/vec2(width, height));
            float coef= neighbor.b*neighbor.a;
            if(coef >0.2)
            {
                vec4 neighbor = SM_Quality*(2.0*neighbor-1.0);
                temp = 1-neighbor.r*neighbor.r-neighbor.g*neighbor.g; if(temp<0.1) temp=0.1;
                neighbor.z =      sqrt(temp);
                neighbor = normalize(neighbor);
                integral += (light_dir_xy.x*neighbor.r+light_dir_xy.y*neighbor.g);//(neighbor.z-SM_Quality);

                count++;
                if(max_int <   integral/count -light_dir.z/2 +0.1) max_int =  integral/count -light_dir.z/2+0.1;

            }
            else{integral -= 1.0; count++;
                if(max_int < integral/count -light_dir.z/2 +0.1) max_int =  integral/count -light_dir.z/2+0.1;

            }
        }
    }


    float c_center=0.3*amb_strength-0.2;
    float c_delta=Cartoon_sha;

    float b=c_center+c_delta;
    float a=c_center-c_delta;

    if(Cartoon_sha!=0)
        Sha=(1-max_int-a)/(b-a);
    else
        Sha = 1-max_int;
    if(Sha<0.0)
        Sha=0.0;
    if(Sha>1.0)
        Sha=1.0;

    if(Cartoon_sha!=0)
        center_cos=(pure_cos-a)/(b-a);
    else
        center_cos = pure_cos;
    if(center_cos<0.0)
        center_cos=0.0;
    if(center_cos>1.0)
        center_cos=1.0;
}





//function to calculate the reflection and refraction effect of image
void ReflectRefract(in vec4 cood_color, out vec4 refractColor, out vec4 reflectColor)
{
    //get color(coordinates) of surface image

    vec2 cood_refl = (cood_color.st);
    vec2 cood_displace = (cood_color.st-0.49803921568627450980392156862745)*cood_color.p*dist;
    vec2 cood_refr = gl_TexCoord[0].st+cood_displace;

//    refractColor =  texture2D(tex_BG, cood_refr, LOD+20*length(cood_displace));

    refractColor =  textureLod(tex_BG, cood_refr, LOD+20*length(cood_displace));
    reflectColor =  textureLod(tex_Env, cood_refl+(light_dir.rg)*0.2/(0.1+abs(light_dir.b)), LOD);
}

void FrenselBlend(in vec4 cood_color, in vec4 refractColor, in vec4 reflectColor, out vec4 Ref_Fresnel)
{
    float Ratio;
    if(toggle_Mirror)
    {
        Ratio=1;
        //if(cood_color.p < 1) Ratio=Ratio*cood_color.p;
    }
    else
    {
        float temp = abs(cood_color.s -0.5);
        float temp2 = abs(cood_color.t -0.5);
        if(temp < temp2 ) temp = temp2;
        float teta = 2.0* temp;
        float t = (0.5+0.5*dist);
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



void main()
{
    //get the (x, y) normal vector from R and G channel
    vec4 cood_center = (texture2D(tex_SM, gl_TexCoord[0].st));
    float label_center = (texture2D(tex_SM, gl_TexCoord[0].st)).r;
    //the mask from foreground image(specular image)
    //vec4 mask = (texture2D(tex_SI, gl_TexCoord[0].st));

    //shadow and ambient occlusion calculated based on functions
    //these are the outputs, corresponds to cos_theta, Shadow strength, Ambiend shadow strength
    float center_cos, Sha, Amb,  Spe;

    if(toggle_Point)
        Shadow1(cood_center.st,center_cos, Sha, Spe);
    else
        Shadow2(cood_center.st,center_cos, Sha, Spe);
    AmbientShadow(cood_center, Amb);
    vec3 light_dir_n = normalize(light_dir);


    if(toggle_ShaAmbCos == 0)
        center_cos = 0;
    else if(toggle_ShaAmbCos == 1)
        center_cos = center_cos;
    else if(toggle_ShaAmbCos == 2)
        center_cos = Amb;
    else if(toggle_ShaAmbCos == 4)
        center_cos = Sha;
    else if(toggle_ShaAmbCos == 3)
        center_cos = center_cos*Amb;
    else if(toggle_ShaAmbCos == 5)
        center_cos = center_cos*Sha;
    else if(toggle_ShaAmbCos == 6)
        center_cos=Sha*Amb;
    else if(toggle_ShaAmbCos == 7)
        center_cos=center_cos*Sha*Amb;


//    center_cos = Sha;

    //blend Diffusion image 1 and 2 based on shadow strength
    vec4 Shadow_blend = mix( texture2D(tex_DI_Dark, gl_TexCoord[0].st),  texture2D(tex_DI_Bright, gl_TexCoord[0].st), center_cos);

    //we were planning to add specular, but finally we removed it
    //    vec4 Light_direct = mix(Shadow_blend, texture2D(tex_SI, gl_TexCoord[0].st), 0);
    vec4 Light_direct = Shadow_blend;

    //vec4 Light_direct = mix(vec4(0.0),vec4(1.0), Sha*Amb);
    //vec4 Light_direct =vec4(1.0)*center_cos;

    //calculate refraction/reflection with Fresnel
    vec4 refl_color, refr_color, Light_indirect; //reflection, refraction, and blended color
    ReflectRefract(cood_center, refr_color, refl_color);
    FrenselBlend(cood_center, refr_color, refl_color, Light_indirect);

    //blend Shadow image and reflection/refraction image half-half

    vec4 Fore_blend = mix(Light_indirect, Light_direct, alpha*Light_direct.a);

    //blend all image with foreground image
    //    gl_FragColor   = Light_indirect;
    //    gl_FragColor =  textureLod(tex_BG, gl_TexCoord[0].st, filter_size*2);
    float Final_Mix = cood_center.a;
    if (cood_center.b<0.001)
        Final_Mix=Final_Mix*cood_center.b;
//    gl_FragColor = vec4(mix(texture2D(tex_BG, gl_TexCoord[0].st).rgb, Fore_blend.rgb, Final_Mix), 1.0);


    if(cur_tex == 0)
        gl_FragColor = vec4(mix(texture2D(tex_BG, gl_TexCoord[0].st).rgb, Fore_blend.rgb, Final_Mix), 1.0);
    else if(cur_tex == 1)
            gl_FragColor = texture2D(tex_SM, gl_TexCoord[0].st);
    else if(cur_tex == 2)
            gl_FragColor = texture2D(tex_DI_Dark, gl_TexCoord[0].st);
    else if(cur_tex == 3)
            gl_FragColor = texture2D(tex_DI_Bright, gl_TexCoord[0].st);
    else if(cur_tex == 4)
            gl_FragColor = texture2D(tex_BG, gl_TexCoord[0].st);
    else if(cur_tex == 5)
            gl_FragColor = texture2D(tex_LD, gl_TexCoord[0].st).rrra;
    else if(cur_tex == 6)
            gl_FragColor = texture2D(tex_LD, gl_TexCoord[0].st).ggga;
    else if(cur_tex == 7)
            gl_FragColor = texture2D(tex_Env, gl_TexCoord[0].st);

    int label = int(texture2D(tex_LD, gl_TexCoord[0].st).b*255);
    gl_FragColor = vec4(normalValues[0],1.0);

//        gl_FragColor = vec4(1.0)*Sha;
    //    gl_FragColor = vec4(1.0)*Spe;
//    gl_FragColor = vec4(1.0,1.0,0.0,1.0);
}

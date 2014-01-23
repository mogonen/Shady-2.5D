#version 130


uniform float alpha;
uniform sampler2D tex_SM, tex_DI_Dark, tex_DI_Bright, tex_BG, tex_LD, tex_Env;
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
//the arra that stores all the average normals
uniform vec3 normalValues[10];
uniform vec3 centerDepth[10];
//every 4 of them represent bounding value for -x,x,-y,y
uniform float boundingbox[40];
in vec4 thePosition;


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

    float c_center= 0.3*amb_strength-0.2;
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
                temp = 1-neighbor.r*neighbor.r-neighbor.g*neighbor.g;
                if(temp<0.1)
                    temp=0.1;
                neighbor.z =      sqrt(temp);
                neighbor = normalize(neighbor);
                integral += (light_dir_xy.x*neighbor.r+light_dir_xy.y*neighbor.g);//(neighbor.z-SM_Quality);

                count++;
                if(max_int <   integral/count -light_dir.z/2 +0.1)
                    max_int =  integral/count -light_dir.z/2+0.1;

            }
            else{integral -= 1.0; count++;
                if(max_int < integral/count -light_dir.z/2 +0.1)
                    max_int =  integral/count -light_dir.z/2+0.1;

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


const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

int ComputeOutCode(vec2 Pp, int LayerInd)
{

        int code = INSIDE;          // initialised as being inside of clip window

        if (Pp.x < boundingbox[4*LayerInd])           // to the left of clip window
                code |= LEFT;
        else if (Pp.x > boundingbox[4*LayerInd+1])      // to the right of clip window
                code |= RIGHT;
        if (Pp.y < boundingbox[4*LayerInd+2])           // below the clip window
                code |= BOTTOM;
        else if (Pp.y > boundingbox[4*LayerInd+3])      // above the clip window
                code |= TOP;
        return code;
}

// Cohen–Sutherland clipping algorithm clips a line from
// P0 = (x0, y0) to P1 = (x1, y1) against a rectangle with
// diagonal from (xmin, ymin) to (xmax, ymax)
bool LineClipping(inout vec3 Lp, inout vec3 Pp, int LayerInd)
{
        // compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
        int outcode0 = ComputeOutCode(Lp.xy, LayerInd);
        int outcode1 = ComputeOutCode(Pp.xy, LayerInd);

        float x0 = Lp.x;
        float y0 = Lp.y;
        float x1 = Pp.x;
        float y1 = Pp.y;

        float xmin = boundingbox[4*LayerInd];
        float xmax = boundingbox[4*LayerInd+1];
        float ymin = boundingbox[4*LayerInd+2];
        float ymax = boundingbox[4*LayerInd+3];

        int m=0;
        while (m<8) {
                if ((outcode0|outcode1) == 0) { // Bitwise OR is 0. Trivially accept and get out of loop
                        break;
                }
                else if ((outcode0&outcode1) != 0) { // Bitwise AND is not 0. Trivially reject and get out of loop
                    return false;
                } else {
                    // failed both tests, so calculate the line segment to clip
                    // from an outside point to an intersection with clip edge
                    float x, y;

                    // At least one endpoint is outside the clip rectangle; pick it.
                    int outcodeOut = (outcode0!=0)? outcode0 : outcode1;

                    // Now find the intersection point;
                    // use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
                    if ((outcodeOut&TOP) !=0) {           // point is above the clip rectangle
                        x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
                        y = ymax;
                    } else
                        if ((outcodeOut&BOTTOM) !=0) { // point is below the clip rectangle
                        x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
                        y = ymin;
                    } else if ((outcodeOut&RIGHT) !=0) {  // point is to the right of clip rectangle
                        y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
                        x = xmax;
                    } else if ((outcodeOut&LEFT) !=0) {   // point is to the left of clip rectangle
                        y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
                        x = xmin;
                    }

                    // Now we move outside point to intersection point to clip
                    // and get ready for next pass.
                    if (outcodeOut == outcode0) {
                        x0 = x;
                        y0 = y;
                        outcode0 = ComputeOutCode(vec2(x0, y0),LayerInd);
                        if(outcode0==outcodeOut)
                            break;
                    } else {
                        x1 = x;
                        y1 = y;
                        outcode1 = ComputeOutCode(vec2(x1, y1),LayerInd);
                        if(outcode1==outcodeOut)
                            break;
                    }
                }
                m++;
        }
        //estimate depth
        float len = length(Lp-Pp);
        vec3 n_Lp = mix(Lp,Pp,length(vec2(x0,y0)-Lp.xy)/len);
        vec3 n_Pp = mix(Pp,Lp,length(vec2(x1,y1)-Pp.xy)/len);
        Lp = n_Lp;
        Pp = n_Pp;
        return true;
}

bool checkEqual(vec2 pos1, vec2 pos2, float range)
{
    if(pos1.x<pos2.x+range&&pos1.x>pos2.x-range&&pos1.y<pos2.y+range&&pos1.y>pos2.y-range)
        return true;
    else
        return false;
}

bool checkOnline(vec2 v, vec2 w, vec2 p, float range)
{
  // Return minimum distance between line segment vw and point p
  float l2 = dot(w-v,w-v);  // i.e. |w-v|^2 -  avoid a sqrt
  float dd;

  if (l2 == 0.0)
      dd = distance(p, v);   // v == w case
  // Consider the line extending the segment, parameterized as v + t (w - v).
  // We find projection of point p onto the line.
  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
  float t = dot(p - v, w - v) / l2;
  if (t < 0.0)
      dd = distance(p, v);       // Beyond the 'v' end of the segment
  else
      if (t > 1.0)
          dd = distance(p, w);  // Beyond the 'w' end of the segment
  else
      {
        vec2 projection = v + t * (w - v);  // Projection falls on the segment
        dd = distance(p, projection);
      }
  if(dd <range)
      return true;
  else
      return false;
}

vec3 projectOnLayerInd(in vec3 Pp, in int LayerInd, out float height)
{
    vec3 LayerNormal = normalValues[LayerInd];
    vec3 LayerCenter = centerDepth[LayerInd];
    float dist = dot(Pp-LayerCenter, LayerNormal);
    height = dist;
    return Pp-dist*LayerNormal;
//    return LayerNormal;
}

void getEstNormal(in vec2 coord,in float sm_smooth, out vec4 raw_value, out vec3 est_normal)
{
    //Shape map normal of current position
    //the SM value of current point pos
    //cur_p is in the secene space, x-(-1,1), y-(-1,1)
    //it has to be converted to texture space
    raw_value.rg = sm_smooth*SM_Quality*(2*texture2D(tex_SM,coord).rg-1.0);
    raw_value.ba = sm_smooth*SM_Quality*(texture2D(tex_SM,coord)).ba;
   //raw_value = raw_value*raw_value.a;
    //assuming a normalized normal value
    float   temp    = 1-raw_value.r*raw_value.r-raw_value.g*raw_value.g;
    if(temp<0.)
        temp=0.;
    //SM_normal - n_i the normal from shape map
    est_normal   = normalize(vec3(raw_value.r, raw_value.g, sqrt(temp)));
}


void getCurLabelDepth(in vec3 p, out int label, out float depth)
{
    vec4 label_depth = texture2D(tex_LD, (p.xy+1)/2);
    label = int(label_depth.b*255);
    depth = label_depth.g;
}

float accHeightInLayer(vec3 Lp, vec3 Pp, int Plabel, float amb)
{
    float max_shadow_strength;
    float shadow_strength=0;
    int count = 0;
    float sm_smooth = 1;

    float int_length = 0;

    for(int j=1;j<10;j++)
    {
        int LayerInd = j;
        //if this layer is empty
        if(length(normalValues[LayerInd])==0)
            break;

        //projected lighting position
        float lHeight; //Light distance to this layer plain
        vec3 Lpp = projectOnLayerInd(Lp, LayerInd, lHeight);
        //projected point position
        float pHeight; //point distance to this layer plain
        vec3 Ppp = projectOnLayerInd(Pp, LayerInd, pHeight);


        float old_length = length(Lpp.xy-Ppp.xy);
        vec3 plight_direct = (Lpp-Ppp)/old_length;

//        //crop the position with bounding box
//        if(!LineClipping(Lpp,Ppp,LayerInd))
//            continue;

        //projected light direction
        float light_length = length(Lpp.xy-Ppp.xy);

        //layer normal - N
        vec3 layer_normal = normalize(normalValues[LayerInd]);

        int i;
        //    int n_steps = 100;
        //step size is ratio to width
        int n_step = int(100+32/(filter_size+1));
        float step = (filter_size+1)/length(vec2(width,height));
        if(LayerInd != Plabel)
        {
            sm_smooth = 0.1;
            n_step = int(light_length/step);
        }
        else
        {
            sm_smooth = 1;
            if(step*n_step>light_length)
                n_step = int(light_length/step);
        }

        float multfactor;
        float smp=1.0/n_step;

        //the integration height
        float acc_height = 0;
        //cur_p - p, estimated position of surface
        vec3 cur_p =Ppp, next_p;
        //cur_p_h - p_hat, position along light direction
        vec3 cur_p_h = cur_p, next_p_h;
        vec3 SM_normal;
        vec4 SM_value;
        float negativenum;


        vec3 step_vec = plight_direct*step;
        //loop for 10 layers

        for(i=0;i<n_step;i++)
        {
            //check the stepped position is still inside the image
            if(cur_p_h.x<1&&cur_p_h.x>-1&&cur_p_h.y<1&&cur_p_h.x>-1)
            {
                getEstNormal((cur_p_h.rg+1)/2,sm_smooth, SM_value, SM_normal);
                //move one step along the porjected lighting direction
                next_p_h = cur_p_h + step_vec;
                //get the label
                int label;
                vec4 label_depth = texture2D(tex_LD, (next_p_h.xy+1)/2);
                label = int(label_depth.b*255);
                //            next_p_h.z = label_depth.g*128.0/127.0;

                //            //check if this point is transparant
                //            float coef= SM_value.a*SM_value.b;

                //make sure integral does not go to another layer
                if(label == LayerInd)
                {
                    //step_vec - v_i
                    //temp - t
                    float t = -dot(SM_normal,step_vec)/(abs(dot(SM_normal,layer_normal))+0.1);
                    //next_p - p_i
                    next_p = cur_p+step_vec+layer_normal*t;

                    //calculate the current height
                    acc_height = dot(next_p-next_p_h, layer_normal);
                    //set next integral position
                    cur_p = next_p;

                    float cur_light_height;
                    //current light height, a ratio of step size and light direction length\
                    if(Plabel==label)
                      cur_light_height = mix(amb*0.01,lHeight,i*step/light_length);
                    else
                       cur_light_height = mix(pHeight,lHeight,i*step/light_length);

//                    float cur_light_height = mix(amb,lHeight,i*step/light_length);
//                    float cur_light_height = mix(0,lHeight,i*step/light_length);
      //                float cur_light_height = mix(lHeight,pHeight,i*step/light_length);


                    //see if light ray goes through the shape
                    negativenum =4*(acc_height + surface_disp- cur_light_height)* (acc_height - (SM_value.b/16. - surface_disp) - cur_light_height)/ (SM_value.b*SM_value.b);
                    count++;

                    //multfactor= (1.-count*smp);
                    multfactor=0.15*step;
                    if (multfactor<0)
                        multfactor=0;

                        if(negativenum<0)
//                            shadow_strength+=multfactor/n_step;
//                            if(Plabel==label)
                                shadow_strength+=multfactor;
//                            else
//                                shadow_strength+=2*multfactor;

                    //shadow_strength-=negativenum;
                }

                cur_p_h= next_p_h;

            }
        }
        //end of layer loop
    }


    max_shadow_strength = surface_disp/(shadow_strength+surface_disp);

    float Sha ;

    float c_center = 0.3*amb_strength+0.2;
    float c_delta = Cartoon_sha;

    float b = c_center+c_delta;
    float a = c_center-c_delta;
//    if(a>0)
//        a = 0;

    if(Cartoon_sha!=0)
//        Sha=(b-2*max_shadow_strength-a)/(b-a);
        Sha=(max_shadow_strength-a)/(b-a);
    else
        Sha = max_shadow_strength;

    if(Sha<0.0)
        Sha=0.0;
    if(Sha>1.0)
        Sha=1.0;
    return (Sha);
//    return int_length/2;
}



void GetCenterCos(in vec3 thePos, out float center_cos)
{
    vec4 raw_SM;
    vec3 est_normal;
    getEstNormal((thePos.rg+1)/2,1.0, raw_SM, est_normal);
    /***Parallel lights. They give slightly different effects.***/
    float pure_cos;
    //pure_cos = dot(center_normal, -light_dir_n);
    vec3 light_dir_n = normalize(light_dir-thePos);

    pure_cos = (dot(est_normal, light_dir_n));
//     pure_cos = dot(center_normal_2D, (-light_dir.rg));


    float c_center=0.3*amb_strength-0.2;
    float c_delta=Cartoon_sha;

    float b=c_center+c_delta;
    float a=c_center-c_delta;

    if(Cartoon_sha!=0)
        center_cos=(pure_cos-a)/(b-a);
    else
        center_cos = pure_cos;

    if(center_cos<0.0)
        center_cos=0.0;
    if(center_cos>1.0)
        center_cos=1.0;
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

//    if(toggle_Point)
//        Shadow1(cood_center.st,center_cos, Sha, Spe);
//    else
//        Shadow2(cood_center.st,center_cos, Sha, Spe);

    AmbientShadow(cood_center, Amb);
    vec3 light_dir_n = normalize(light_dir);


    int label;
    vec3 true_position = thePosition.rgb;
    float new_shadow;
    float true_depth;
    getCurLabelDepth(true_position,label,true_depth);
//    if(label==0)
//        return;

    true_position.b = true_depth;
    GetCenterCos(true_position,center_cos);

    float h;
    vec3 pLight1 = projectOnLayerInd(light_dir, 1, h);
    vec3 pLight2 = projectOnLayerInd(light_dir, 2, h);


            //        gl_FragColor = vec4(vec3(accHeightInLayer(-vec3(light_dir.rg,-light_dir.b), true_position, label)),1.0);
//            gl_FragColor = mix(accHeightInLayer(-vec3(light_dir.rg,-light_dir.b), true_position, label),vec4(vec3(center_cos),1.0),0.5);
            new_shadow = accHeightInLayer(light_dir, true_position,label, Amb);
//            gl_FragColor = vec4(vec3(center_cos),1.0);


    if(toggle_ShaAmbCos == 0)
        center_cos = 0;
    else if(toggle_ShaAmbCos == 1)
        center_cos = center_cos;
    else if(toggle_ShaAmbCos == 2)
        center_cos = Amb;
    else if(toggle_ShaAmbCos == 4)
        center_cos = new_shadow;
    else if(toggle_ShaAmbCos == 3)
        center_cos = center_cos*Amb;
    else if(toggle_ShaAmbCos == 5)
    {
        center_cos = center_cos*new_shadow*new_shadow;
//        center_cos = min(center_cos,new_shadow*new_shadow);
    }
    else if(toggle_ShaAmbCos == 6)
        center_cos=new_shadow*Amb;
    else if(toggle_ShaAmbCos == 7)
        center_cos=center_cos*new_shadow*Amb;


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

    if(cur_tex == 0)
    {
//        if(checkEqual(pLight1.xy, thePosition.rg,0.02))
//            gl_FragColor = vec4(0.0,0.5,1.0,1.0);
//        else
//            if(checkEqual(pLight2.xy, thePosition.rg,0.02))
//                gl_FragColor = vec4(1.0,0.5,0.5,1.0);
//            else
            {
//                vec3 Lp = light_dir.xyz;
//                vec3 Pp = vec3(0.0,0.0,0.0);
//                LineClipping(Lp,Pp, 1);
//                if(checkEqual(Lp.xy, thePosition.rg,0.02))
//                    gl_FragColor = vec4(1.0,0.0,1.0,1.0);
                gl_FragColor = vec4(mix(texture2D(tex_BG, gl_TexCoord[0].st).rgb, Fore_blend.rgb, Final_Mix), 1.0);
            }
//        int t_label;
//        vec3 pp = thePosition.rgb;
//        getCurLabelDepth(pp, t_label);
//        gl_FragColor = vec4(vec3(t_label), 1.0);
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
            gl_FragColor = texture2D(tex_LD, gl_TexCoord[0].st).rrra;
    else if(cur_tex == 6)
            gl_FragColor = texture2D(tex_LD, gl_TexCoord[0].st).ggga;
    else if(cur_tex == 7)
            gl_FragColor = texture2D(tex_Env, gl_TexCoord[0].st);




//    vec3 Ppp =  projectOnLayerInd(thePosition.rgb, 1);
//    if(Ppp.x<-1||Ppp.x>1||Ppp.y<-1||Ppp.y>1)
//        gl_FragColor = vec4(1.0,1.0,0.0,1.0);
//    else
//        gl_FragColor = (thePosition+1)/2;


//    gl_FragColor = vec4(vec3(((thePosition.rg+1)/2-gl_TexCoord[0].st),0.0),1.0);
//    gl_FragColor = texture2D(tex_SM,(thePosition.rg+1)/2);

//      gl_FragColor = accHeightInLayer(-vec3(light_dir.rg,-light_dir.b), vec3(thePosition.rg,0.0), 1);
//    float ht = dot(-vec3(light_dir.rg,-light_dir.b), thePosition.rgb);
//    gl_FragColor = vec4((getShapeMapNormal(thePosition.rg)/2+0.5),1.0);

//    vec3 pLight = projectOnLayerInd(-vec3(light_dir.rg,-light_dir.b), 1);
//    if(checkEqual(pLight.xy, thePosition.rg,0.01))
//            gl_FragColor = vec4(0.5,0.5,0.5,1.0);
//    if(checkOnline(pLight.xy, vec2(0.0,0.0), thePosition.rg,0.01))
//        gl_FragColor = vec4(0.5,0.5,0.0,1.0);

//    pLight = projectOnLayerInd(-light_dir, 2);
//    if(checkEqual(pLight.xy, thePosition.rg,0.01))
//            gl_FragColor = vec4(0.5,0.5,0.5,1.0);
//    if(checkOnline(pLight.xy, vec2(0.0,0.0), thePosition.rg,0.01))
//        gl_FragColor = vec4(0,0.5,0.5,1.0);

//    gl_FragColor = vec4((thePosition/2+0.5).rg,0.0,1.0);
//    gl_FragColor = gl_TexCoord[0];
//    gl_FragColor = vec4(vec3(dot(-light_dir, projectOnLayerInd(-light_dir, 1))),1.0);

//    int label = int(texture2D(tex_LD, gl_TexCoord[0].st).b*255);
//    gl_FragColor = vec4(normalValues[label],1.0);

//    gl_FragColor = vec4(texture2D(tex_LD, gl_TexCoord[0].st).b);

//        gl_FragColor = vec4(1.0)*Sha;
    //    gl_FragColor = vec4(1.0)*Spe;
//    gl_FragColor = vec4(1.0,1.0,0.0,1.0);
}

#if !OPENGL_ES

R"(
#if   defined VERTEX_SHADER

uniform vec4 u_info;

attribute vec3 a_position;
attribute vec2 a_texCoords;
attribute vec4 a_color;

varying vec4 v_color;
varying vec2 v_texCoords;

void main(){
    gl_Position = vec4(u_info.xy*(a_position.xy-u_info.zw), a_position.z, 1.0);
    
    v_texCoords = a_texCoords;
    v_color = a_color;
}

#elif defined FRAGMENT_SHADER 

uniform sampler2D u_sampler;

varying vec2 v_texCoords;
varying vec4 v_color;

void main(){
    gl_FragColor = v_color*vec4(1.0, 1.0, 1.0, texture2D(u_sampler, v_texCoords).r);
}

#endif
)"

#else

R"(
#if   defined VERTEX_SHADER

uniform mediump vec4 u_info;

attribute mediump vec3 a_position;
attribute mediump vec2 a_texCoords;
attribute mediump vec4 a_color;

varying mediump vec4 v_color;
varying mediump vec2 v_texCoords;

void main(){
    gl_Position = vec4(u_info.xy*(a_position.xy-u_info.zw), a_position.z, 1.0);
    
    v_texCoords = a_texCoords;
    v_color = a_color;
}

#elif defined FRAGMENT_SHADER

uniform sampler2D u_sampler;

varying mediump vec2 v_texCoords;
varying mediump vec4 v_color;

void main(){
    mediump float t = texture2D(u_sampler, v_texCoords).r;
    if(t < 0.1)
        discard;
    gl_FragColor = v_color*vec4(1.0, 1.0, 1.0, t);
}

#endif
)"

#endif

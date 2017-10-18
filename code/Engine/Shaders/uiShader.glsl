#if !OPENGL_ES

R"(
#if   defined VERTEX_SHADER

uniform vec4 u_info;

attribute vec3 a_position;
attribute vec4 a_color;

varying vec4 v_color;

void main(){
    gl_Position = vec4(u_info.xy*(a_position.xy-u_info.zw), a_position.z, 1.0);
    
    v_color = a_color;
}

#elif defined FRAGMENT_SHADER

varying vec4 v_color;

void main(){
    gl_FragColor = v_color;
}

#endif
)"

#else

R"(
#if   defined VERTEX_SHADER

uniform mediump vec4 u_info;

attribute mediump vec3 a_position;
attribute mediump vec4 a_color;

varying mediump vec4 v_color;

void main(){
    gl_Position = vec4(u_info.xy*(a_position.xy-u_info.zw), a_position.z, 1.0);
    
    v_color = a_color;
}

#elif defined FRAGMENT_SHADER

varying mediump vec4 v_color;

void main(){
    gl_FragColor = v_color;
}

#endif
)"

#endif

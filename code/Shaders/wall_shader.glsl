#if !OPENGL_ES

R"(
#if   defined VERTEX_SHADER

uniform mat4 u_matrix;
uniform vec3 u_light_direction;
uniform vec3 u_directional_light_color;
uniform vec3 u_ambient_light_color;

attribute vec3 a_position;
attribute vec3 a_color;
attribute vec3 a_normal;

varying vec4 v_color;

void main(){
    gl_Position = u_matrix*vec4(a_position, 1.0);
    
    v_color = vec4(a_color * (u_ambient_light_color + dot(a_normal, u_light_direction)*u_directional_light_color), 1.0);
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

uniform mediump mat4 u_matrix;
uniform mediump vec3 u_light_direction;
uniform mediump vec3 u_directional_light_color;
uniform mediump vec3 u_ambient_light_color;

attribute mediump vec3 a_position;
attribute mediump vec3 a_color;
attribute mediump vec3 a_normal;

varying mediump vec4 v_color;

void main(){
    gl_Position = u_matrix*vec4(a_position, 1.0);
    
    v_color = vec4(a_color * (u_ambient_light_color + dot(a_normal, u_light_direction)*u_directional_light_color), 1.0);
}
#elif defined FRAGMENT_SHADER

varying mediump vec4 v_color;

void main(){
    gl_FragColor = v_color;
}

#endif
)"

#endif

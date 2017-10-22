#if !OPENGL_ES

R"(
#if   defined VERTEX_SHADER

uniform mat4 u_matrix;
uniform vec3 u_light_direction;
uniform vec3 u_directional_light_color;
uniform vec3 u_ambient_light_color;

attribute vec3 a_position;
attribute vec2 a_tex_coords;
attribute vec3 a_normal;

varying vec4 v_color_multiplier;
varying vec2 v_tex_coords;

void main(){
    gl_Position = u_matrix*vec4(a_position, 1.0);
    vec3 new_normal = (u_matrix*vec4(a_normal, 0.0)).xyz;
    
    v_color_multiplier = vec4(u_ambient_light_color + dot(new_normal, u_light_direction)*u_directional_light_color, 1.0);
    v_tex_coords = a_tex_coords;
}

#elif defined FRAGMENT_SHADER

uniform vec4 u_background_color;
uniform sampler2D u_sampler;

varying vec2 v_tex_coords;
varying vec4 v_color_multiplier;

void main(){
    gl_FragColor = mix(texture2D(u_sampler, v_tex_coords), u_background_color, max(4.0*gl_FragCoord.z-3.0, 0.0));
    //gl_FragColor = vec4(vec3(0.0), 1.0);
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
attribute mediump vec2 a_tex_coords;
attribute mediump vec3 a_normal;

varying mediump vec4 v_color_multiplier;
varying mediump vec2 v_tex_coords;

void main(){
    gl_Position = u_matrix*vec4(a_position, 1.0);
    vec3 new_normal = (u_matrix*vec4(a_normal, 0.0)).xyz;
    
    v_color_multiplier = vec4(u_ambient_light_color + dot(new_normal, u_light_direction)*u_directional_light_color, 1.0);
    v_tex_coords = a_tex_coords;
}
#elif defined FRAGMENT_SHADER

uniform mediump vec4 u_background_color;
uniform sampler2D u_sampler;

varying mediump vec2 v_tex_coords;
varying mediump vec4 v_color_multiplier;

void main(){
    gl_FragColor = mix(texture2D(u_sampler, v_tex_coords), u_background_color, max(4.0*gl_FragCoord.z-3.0, 0.0));
}

#endif
)"

#endif

#version 420

/* this is an attribute-less shader woooo!
position and texture coords are derived from vertex number,
which can be accessed as a built-in int gl_VertexID variable

02
13 triangle strip vertex order

*/

vec2 data[4] = vec2[]
(
	vec2 (-1.0,  1.0),
	vec2 (-1.0, -1.0),
	vec2 ( 1.0,  1.0),
	vec2 ( 1.0, -1.0)
);

out vec2 st;

void main () {
	vec4 vpos = vec4 (data[gl_VertexID], 0.0, 1.0);
	st.s = vpos.x * 0.5 + 0.5;
	st.t = vpos.y * 0.5 + 0.5;
	gl_Position = vpos;
}

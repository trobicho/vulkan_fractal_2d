#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(origin_upper_left) in vec4 gl_FragCoord;

layout( push_constant ) uniform constants
{
	vec2	min;
	vec2	max;
	int		max_iter;
	float	time;
}push_constants;

int		batman(vec2 z1)
{
	float	zrs;
	float	zis;
	float	zir;
	vec2		z = z1;
	vec2		tmpz;
	zrs = z1.x * z1.x;
	zis = z1.y * z1.y;

	for (int i = 0; i < push_constants.max_iter; i++)
	{
		tmpz.x = cos(z.x) * cosh(z.y) + z1.x / (zrs + zis);
		tmpz.y = -sin(z.x) * sinh(z.y) + z1.y / (zrs + zis);
		z = tmpz;
		if (z.x *  z.x + z.y * z.y > 50.0)
			return (i);
	}
	return (push_constants.max_iter);
}

void	main()
{
	vec3	color;
	vec2	uv = vec2(gl_FragCoord.xy);
	float	coloradd = 0.203005;

	//vec2 min = vec2(0.278587, -0.012560);
	//vec2 max = vec2(0.285413, -0.007440);
	vec2 min = push_constants.min;
	vec2 max = push_constants.max;

	vec2 z = uv * (max - min) + min;
	vec2 c = mix(vec2(min.x, min.y), vec2(max.x, max.y), gl_FragCoord.xy
		/ vec2(1920, 1080));

	int col = batman(c);
	if (col == push_constants.max_iter)
		color = vec3(0.0, 0.0, 0.0);
	else
	{
		col %= 30;
		if (col >= 15)
			col = 30 - col;
		float time = push_constants.time / 500;
		color = vec3(sin(col + time)
			, 1.0 / (col + sin(time / 10.0))
			, 1.0 / (col + cos(time)));
	}
	outColor = vec4(color, 1.0);
}

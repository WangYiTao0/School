
float random2(int float2 vec)
{
	return frac(sin(dot(vec.xy, float(12.9898, 78.233)))
		* 43758.5453123);
}
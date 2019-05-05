
using UnityEngine;

public static class GlobalMembers
{
	public static string loadFile(string filename)
	{
        return "";
	}

	public static string pad(int x)
    {
        return "";
    }
	public static string number_format(int x)
    {
        return "";
    }

	public static float uniformRandomVariable()
	{
        const int RAND_MAX = 0x7fff;

        float temp = UnityEngine.Random.Range(0, RAND_MAX);

        return temp / RAND_MAX;
	}
	public static complex gaussianRandomVariable()
	{
		float x1;
		float x2;
		float w;
		do
		{
			x1 = 2.0f * uniformRandomVariable() - 1.0f;
			x2 = 2.0f * uniformRandomVariable() - 1.0f;
			w = x1 * x1 + x2 * x2;
		} while (w >= 1.0f);
		w = Mathf.Sqrt((-2.0f * Mathf.Log(w)) / w);
		return new complex(x1 * w, x2 * w);
	}
}
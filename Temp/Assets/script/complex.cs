public class complex
{
	public float a;
	public float b;
	public static uint additions;
	public static uint multiplications;
	public complex()
	{
		this.a = 0.0f;
		this.b = 0.0f;
	}
	public complex(float a, float b)
	{
		this.a = a;
		this.b = b;
	}
	public complex conj()
	{
		return new complex(this.a, -this.b);
	}

    static public complex operator *(complex c1, complex c2)
    {
        complex.multiplications++;
        return new complex(c1.a * c2.a - c1.b * c2.b, c1.a * c2.b + c1.b * c2.a);
    }

    static public complex operator +(complex c1, complex c2)
    {
        complex.additions++;
        return new complex(c1.a + c2.a, c1.b + c2.b);
    }

    static public complex operator -(complex c1, complex c2)
    {
        complex.additions++;
        return new complex(c1.a - c2.a, c1.b - c2.b);
    }

    static public complex operator *(complex c, float f)
    {
        return new complex(c.a * f, c.b * f);
    }
    public void reset()
	{
		complex.additions = 0;
		complex.multiplications = 0;
	}
}

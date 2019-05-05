using System;
using System.Collections.Generic;
using UnityEngine;

//C++ TO C# CONVERTER CRACKED BY X-CRACKER 2017 WARNING: The following #include directive was ignored:
//#include "DXUT.h"


public class ComplexW
{
    public List<complex> w;
}

public class cFFT : System.IDisposable
{
	private int N;
	private int which;
	private int log_2_N;
	private float pi2;
	private int[] reversed;
	//private complex[,] W;
    private ComplexW[] W;
	private complex[,] c;
	public cFFT(int N)
	{
		this.N = N;
		this.pi2 = 2 * 3.1415926f;

		log_2_N = (int)(Mathf.Log((float)N) / Math.Log(2.0));

		reversed = new int[N]; // prep bit reversals
		for (int i = 0; i < N; i++)
		{
			reversed[i] = reverse(i);
		}

		int pow2 = 1;
        W = Arrays.InitializeWithDefaultInstances<ComplexW>(log_2_N);
        for (int i = 0; i < log_2_N; i++)
		{
            ComplexW W_ = W[i];

            complex[] temp = Arrays.InitializeWithDefaultInstances<complex>(pow2);
            W_.w = new List<complex>(temp);

            for (int j = 0; j < pow2; j++)
			{
                W_.w[j] = w(j, pow2 * 2);

            }
			pow2 *= 2;
		}

        c = new complex[2, N];
        for (int i = 0; i < N; i++)
        {
            c[0, i] = new complex();
            c[1, i] = new complex();
        }
        //c[0,] = Arrays.InitializeWithDefaultInstances<complex>((int)N);
        //c[1,] = Arrays.InitializeWithDefaultInstances<complex>((int)N);
        which = 0;
	}
	public void Dispose()
	{

	}
	public int reverse(int i)
	{
		int res = 0;
		for (int j = 0; j < log_2_N; j++)
		{
			res = (res << 1) + (i & 1);
			i >>= 1;
		}
		return res;
	}
	public complex w(int x, int N)
	{
		return new complex(Mathf.Cos(pi2 * x / N), Mathf.Sin(pi2 * x / N));
	}
	public void fft(complex[] input, complex[] output, int stride, int offset)
	{
		for (int i = 0; i < N; i++)
		{
			c[which,i] = input[reversed[i] * stride + offset];
		}

		int loops = N >> 1;
		int size = 1 << 1;
		int size_over_2 = 1;
		int w_ = 0;
		for (int i = 1; i <= log_2_N; i++)
		{
			which ^= 1;
			for (int j = 0; j < loops; j++)
			{
				for (int k = 0; k < size_over_2; k++)
				{
					c[which,size * j + k] = c[which ^ 1,size * j + k] + c[which ^ 1,size * j + size_over_2 + k] * W[w_].w[k];
				}

				for (int k = size_over_2; k < size; k++)
				{
					c[which,size * j + k] = c[which ^ 1,size * j - size_over_2 + k] - c[which ^ 1,size * j + k] * W[w_].w[k - size_over_2];
				}
			}
			loops >>= 1;
			size <<= 1;
			size_over_2 <<= 1;
			w_++;
		}

		for (int i = 0; i < N; i++)
		{
			output[i * stride + offset] = c[which,i];
		}
	}
}
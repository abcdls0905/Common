using System;
using System.Collections.Generic;
using UnityEngine;

//C++ TO C# CONVERTER CRACKED BY X-CRACKER 2017 WARNING: The following #include directive was ignored:
//#include "DXUT.h"


//C++ TO C# CONVERTER CRACKED BY X-CRACKER 2017 WARNING: The following #include directive was ignored:
//#include "src/misc.h"
//C++ TO C# CONVERTER CRACKED BY X-CRACKER 2017 WARNING: The following #include directive was ignored:
//#include "src/fft.h"


public class vertex_ocean
{
	public float x;
	public float y;
	public float z;
	public float nx;
	public float ny;
	public float nz;
	public float a;
	public float b;
	public float c;
	public float _a;
	public float _b;
	public float _c;
	public float ox;
	public float oy;
	public float oz;

    public float offset_x;
    public float offset_y;
    public float offset_z;
}

public class complex_vector_normal
{
	public complex h = new complex();
	public Vector2 D = new Vector2();
	public Vector3 n = new Vector3();
}


public class Ocean : System.IDisposable
{
	public bool geometry;

	public float g; //����
	public int N; //dimension  =2^������
	public int Nplus1;
	public float A; //phillips specturn parameter
	public Vector2 w = new Vector2(); //��
	public float length;
	public complex[] h_tilde;
	public complex[] h_tilde_slopex;
	public complex[] h_tilde_slopez;
	public complex[] h_tilde_dx;
	public complex[] h_tilde_dz;
	public cFFT fft;

	public vertex_ocean[] vertices;
	public int[] indices;
	public int indices_count;
	public int vbo_vrtices;
	public int vbo_indices;

	public int glProhram;
	public int glShaderV;
	public int glShaderF;
	public int vertex;
	public int normal;
	public int texture;
	public int light_position;
	public int projection;
	public int view;
	public int model;
	public Ocean(int N, float A, Vector2 w, float length, bool geometry)
	{
		this.g = 9.81F;
		this.geometry = geometry;
		this.N = N;
		this.Nplus1 = N + 1;
		this.A = A;
		this.w = w;
		this.length = length;
		h_tilde = Arrays.InitializeWithDefaultInstances<complex>(N * N);
		h_tilde_slopex = Arrays.InitializeWithDefaultInstances<complex>(N * N);
		h_tilde_slopez = Arrays.InitializeWithDefaultInstances<complex>(N * N);
		h_tilde_dx = Arrays.InitializeWithDefaultInstances<complex>(N * N);
		h_tilde_dz = Arrays.InitializeWithDefaultInstances<complex>(N * N);
		fft = new cFFT(N);
		vertices = Arrays.InitializeWithDefaultInstances<vertex_ocean>(Nplus1 * Nplus1);
        //indices = new int[Nplus1 * Nplus1 * 10];
        indices = new int[N * N * 6];

		int index;

		complex htilde0 = new complex();
		complex htilde0mk_conj = new complex();
		for (int m_prime = 0; m_prime < Nplus1; m_prime++)
		{
			for (int n_prime = 0; n_prime < Nplus1; n_prime++)
			{
				index = m_prime * Nplus1 + n_prime;

				htilde0 = hTilde_0(n_prime, m_prime);
				htilde0mk_conj = hTilde_0(-n_prime, -m_prime).conj();

				vertices[index].a = htilde0.a;
				vertices[index].b = htilde0.b;
				vertices[index]._a = htilde0mk_conj.a;
				vertices[index]._b = htilde0mk_conj.b;

				vertices[index].ox = vertices[index].x = (n_prime - N / 2.0f) * length / N;
				vertices[index].oy = vertices[index].y = 0.0f;
				vertices[index].oz = vertices[index].z = (m_prime - N / 2.0f) * length / N;

				vertices[index].nx = 0.0f;
				vertices[index].ny = 1.0f;
				vertices[index].nz = 0.0f;
			}
		}

		indices_count = 0;
		for (int m_prime = 0; m_prime < N; m_prime++)
		{
			for (int n_prime = 0; n_prime < N; n_prime++)
			{
				index = m_prime * Nplus1 + n_prime;

				if (geometry)
				{
					indices[indices_count++] = index; // lines
					indices[indices_count++] = index + 1;
					indices[indices_count++] = index;
					indices[indices_count++] = index + Nplus1;
					indices[indices_count++] = index;
					indices[indices_count++] = index + Nplus1 + 1;
					if (n_prime == N - 1)
					{
						indices[indices_count++] = index + 1;
						indices[indices_count++] = index + Nplus1 + 1;
					}
					if (m_prime == N - 1)
					{
						indices[indices_count++] = index + Nplus1;
						indices[indices_count++] = index + Nplus1 + 1;
					}
				}
				else
				{
					indices[indices_count++] = index; // two triangles
					indices[indices_count++] = index + Nplus1;
					indices[indices_count++] = index + Nplus1 + 1;
					indices[indices_count++] = index;
					indices[indices_count++] = index + Nplus1 + 1;
					indices[indices_count++] = index + 1;
				}
			}
		}
	}
	public void Dispose()
	{
        
	}
//C++ TO C# CONVERTER CRACKED BY X-CRACKER 2017 TODO TASK: The implementation of the following method could not be found:
//	void release();

	public float dispersion(int n_prime, int m_prime)
	{
		float w_0 = 2.0f * DefineConstants.M_PI / 200.0f;
		float kx = DefineConstants.M_PI * (2 * n_prime - N) / length;
		float kz = DefineConstants.M_PI * (2 * m_prime - N) / length;

		return Mathf.Floor(Mathf.Sqrt(g * Mathf.Sqrt(kx * kx + kz * kz)) / w_0) * w_0;
	}
	public float phillips(int n_prime, int m_prime)
	{
		Vector2 k = new Vector2(DefineConstants.M_PI * (2 * n_prime - N) / length, DefineConstants.M_PI * (2 * m_prime - N) / length);
		float k_length = k.magnitude;
		if (k_length < 0.000001F)
		{
			return 0F;
		}

		float k_length2 = k_length * k_length;
		float k_length4 = k_length2 * k_length2;

        //float k_dot_w = k.normalized * w.normalized;
        float k_dot_w = Vector2.Dot(k.normalized, w.normalized);
        float k_dot_w2 = k_dot_w * k_dot_w;

		float w_length = w.magnitude;
		float L = w_length * w_length / g;
		float L2 = L * L;

		float damping = 0.001F;
		float l2 = L2 * damping * damping;

		return A * Mathf.Exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * Mathf.Exp(-k_length2 * l2);
	}
	public complex hTilde_0(int n_prime, int m_prime)
	{
		complex r = GlobalMembers.gaussianRandomVariable();
		float t = phillips(n_prime, m_prime) / 2.0f;
		return r * Mathf.Sqrt(t);
	}
	public complex hTilde(float t, int n_prime, int m_prime)
	{
		int index = m_prime * Nplus1 + n_prime;

		complex htilde0 = new complex(vertices[index].a, vertices[index].b);
		complex htilde0mkconj = new complex(vertices[index]._a, vertices[index]._b);

		float omegat = dispersion(n_prime, m_prime) * t;

		float cos_ = Mathf.Cos(omegat);
		float sin_ = Mathf.Sin(omegat);

		complex c0 = new complex(cos_, sin_);
		complex c1 = new complex(cos_, -sin_);

		//complex res = htilde0 * c0 + htilde0mkconj*c1;

		return htilde0 * c0 + htilde0mkconj * c1;

	}
	public complex_vector_normal h_D_and_n(Vector2 x, float t)
	{
		complex h = new complex(0.0f, 0.0f);
		Vector2 D = new Vector2(0.0f, 0.0f);
		Vector3 n = new Vector3(0.0f, 0.0f, 0.0f);

		complex c = new complex();
		complex res = new complex();
		complex htilde_c = new complex();
		Vector2 k = new Vector2();
		float kx;
		float kz;
		float k_length;
		float k_dot_x;

		for (int m_prime = 0; m_prime < N; m_prime++)
		{
			kz = 2.0f * DefineConstants.M_PI * (m_prime - N / 2.0f) / length;
			for (int n_prime = 0; n_prime < N; n_prime++)
			{
				kx = 2.0f * DefineConstants.M_PI * (n_prime - N / 2.0f) / length;
				k = new Vector2(kx, kz);

				k_length = k.magnitude;
				k_dot_x = Vector2.Dot(k, x);

				c = new complex(Mathf.Cos(k_dot_x), Mathf.Sin(k_dot_x));
				htilde_c = hTilde(t, n_prime, m_prime) * c;

				h = h + htilde_c;

				n = n + new Vector3(-kx * htilde_c.b, 0.0f, -kz * htilde_c.b);

				if (k_length < 0.000001F)
				{
					continue;
				}
				D = D + new Vector2(kx / k_length * htilde_c.b, kz / k_length * htilde_c.b);
			}
		}

		n = (new Vector3(0.0f, 1.0f, 0.0f) - n).normalized;

		complex_vector_normal cvn = new complex_vector_normal();
		cvn.h = h;
		cvn.D = D;
		cvn.n = n;
		return cvn;
	}

	public void evaluateWaveFFT(float t)
	{
        float maxY = -100;
        float minY = 100;
        float minX = 100;
        float maxX = -100;
        float minZ = 100;
        float maxZ = -100;
        float kx;
		float kz;
		float len;
		float lambda = -1.0f;
		int index;
		int index1;

		for (int m_prime = 0; m_prime < N; m_prime++)
		{
			kz = DefineConstants.M_PI * (2.0f * m_prime - N) / length;
			for (int n_prime = 0; n_prime < N; n_prime++)
			{
				kx = DefineConstants.M_PI * (2 * n_prime - N) / length;
				len = Mathf.Sqrt(kx * kx + kz * kz);
				index = m_prime * N + n_prime;

				h_tilde[index] = hTilde(t, n_prime, m_prime);
				h_tilde_slopex[index] = h_tilde[index] * new complex(0, kx);
				h_tilde_slopez[index] = h_tilde[index] * new complex(0, kz);
				if (len < 0.000001f)
				{
					h_tilde_dx[index] = new complex(0.0f, 0.0f);
					h_tilde_dz[index] = new complex(0.0f, 0.0f);
				}
				else
				{
					h_tilde_dx[index] = h_tilde[index] * new complex(0, -kx / len);
					h_tilde_dz[index] = h_tilde[index] * new complex(0, -kz / len);
				}
			}
		}

		for (int m_prime = 0; m_prime < N; m_prime++)
		{
			fft.fft(h_tilde, h_tilde, 1, m_prime * N);
			fft.fft(h_tilde_slopex, h_tilde_slopex, 1, m_prime * N);
			fft.fft(h_tilde_slopez, h_tilde_slopez, 1, m_prime * N);
			fft.fft(h_tilde_dx, h_tilde_dx, 1, m_prime * N);
			fft.fft(h_tilde_dz, h_tilde_dz, 1, m_prime * N);
		}
		for (int n_prime = 0; n_prime < N; n_prime++)
		{
			fft.fft(h_tilde, h_tilde, N, n_prime);
			fft.fft(h_tilde_slopex, h_tilde_slopex, N, n_prime);
			fft.fft(h_tilde_slopez, h_tilde_slopez, N, n_prime);
			fft.fft(h_tilde_dx, h_tilde_dx, N, n_prime);
			fft.fft(h_tilde_dz, h_tilde_dz, N, n_prime);
		}

		int sign;
		float[] signs = {1.0f, -1.0f};
		Vector3 n = new Vector3();
		for (int m_prime = 0; m_prime < N; m_prime++)
		{
			for (int n_prime = 0; n_prime < N; n_prime++)
			{
				index = m_prime * N + n_prime; // index into h_tilde..
				index1 = m_prime * Nplus1 + n_prime; // index into vertices

				sign = (int)signs[(n_prime + m_prime) & 1];

				h_tilde[index] = h_tilde[index] * sign;

				// height
				vertices[index1].y = h_tilde[index].a;

                if (vertices[index1].y > maxY)
                    maxY = vertices[index1].y;
                if (vertices[index1].y < minY)
                    minY = vertices[index1].y;

                // displacement
                h_tilde_dx[index] = h_tilde_dx[index] * sign;
				h_tilde_dz[index] = h_tilde_dz[index] * sign;
				vertices[index1].x = vertices[index1].ox + h_tilde_dx[index].a * lambda;
				vertices[index1].z = vertices[index1].oz + h_tilde_dz[index].a * lambda;

                if (h_tilde_dx[index].a * lambda > maxX)
                    maxX = h_tilde_dx[index].a * lambda;
                if (h_tilde_dx[index].a * lambda < minX)
                    minX = h_tilde_dx[index].a * lambda;

                if (h_tilde_dz[index].a * lambda > maxZ)
                    maxZ = h_tilde_dz[index].a * lambda;
                if (h_tilde_dz[index].a * lambda < minZ)
                    minZ = h_tilde_dz[index].a * lambda;

                // normal
                h_tilde_slopex[index] = h_tilde_slopex[index] * sign;
				h_tilde_slopez[index] = h_tilde_slopez[index] * sign;
				n = new Vector3(0.0f - h_tilde_slopex[index].a, 1.0f, 0.0f - h_tilde_slopez[index].a).normalized;
				vertices[index1].nx = n.x;
				vertices[index1].ny = n.y;
				vertices[index1].nz = n.z;

                vertices[index1].offset_y = vertices[index1].y;
                vertices[index1].offset_x = h_tilde_dx[index].a * lambda;
                vertices[index1].offset_z = h_tilde_dz[index].a * lambda;

				// for tiling
				if (n_prime == 0 && m_prime == 0)
				{
					vertices[index1 + N + Nplus1 * N].y = h_tilde[index].a;

					vertices[index1 + N + Nplus1 * N].x = vertices[index1 + N + Nplus1 * N].ox + h_tilde_dx[index].a * lambda;
					vertices[index1 + N + Nplus1 * N].z = vertices[index1 + N + Nplus1 * N].oz + h_tilde_dz[index].a * lambda;

					vertices[index1 + N + Nplus1 * N].nx = n.x;
					vertices[index1 + N + Nplus1 * N].ny = n.y;
					vertices[index1 + N + Nplus1 * N].nz = n.z;
				}
				if (n_prime == 0)
				{
					vertices[index1 + N].y = h_tilde[index].a;

					vertices[index1 + N].x = vertices[index1 + N].ox + h_tilde_dx[index].a * lambda;
					vertices[index1 + N].z = vertices[index1 + N].oz + h_tilde_dz[index].a * lambda;

					vertices[index1 + N].nx = n.x;
					vertices[index1 + N].ny = n.y;
					vertices[index1 + N].nz = n.z;
				}
				if (m_prime == 0)
				{
					vertices[index1 + Nplus1 * N].y = h_tilde[index].a;

					vertices[index1 + Nplus1 * N].x = vertices[index1 + Nplus1 * N].ox + h_tilde_dx[index].a * lambda;
					vertices[index1 + Nplus1 * N].z = vertices[index1 + Nplus1 * N].oz + h_tilde_dz[index].a * lambda;

					vertices[index1 + Nplus1 * N].nx = n.x;
					vertices[index1 + Nplus1 * N].ny = n.y;
					vertices[index1 + Nplus1 * N].nz = n.z;
				}
			}
		}
    }
}
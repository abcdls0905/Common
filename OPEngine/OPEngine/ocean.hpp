#pragma once

#define DISP_MAP_SIZE		512					// 1024 max
#define MESH_SIZE			256					// [64, 256] (or calculate index count for other levels)
#define GRAV_ACCELERATION	9.81f				// m/s^2
#define PATCH_SIZE			20.0f				// m
#define FURTHEST_COVER		8					// full ocean size = PATCH_SIZE * (1 << FURTHEST_COVER)
#define MAX_COVERAGE		64.0f				// pixel limit for a distant patch to be rendered
#define WIND_DIRECTION		{ -0.4f, -0.9f }
#define WIND_SPEED			6.5f				// m/s
#define AMPLITUDE_CONSTANT	(0.45f * 1e-3f)		// for the (modified) Phillips spectrum

static const int IndexCounts[] = {
	0,
	0,
	0,
	0,
	0,
	0,
	961920,		// 64x64
	3705084,	// 128x128
	14500728	// 256x256
};

struct SubRangeRender
{
	int PrimitiveType;
	uint32_t AttribId;
	uint32_t IndexStart;
	uint32_t IndexCount;
	uint32_t VertexStart;
	uint32_t VertexCount;
	bool Enabled;
};

void GenerateVertice(std::vector<glm::vec3>& vertices)
{
	float tilesize = PATCH_SIZE / MESH_SIZE;

	vertices.resize((MESH_SIZE + 1)*(MESH_SIZE + 1));
	// vertex data
	for (int z = 0; z <= MESH_SIZE; ++z) {
		for (int x = 0; x <= MESH_SIZE; ++x) {
			int index = z * (MESH_SIZE + 1) + x;

			glm::vec3 vertice;
			vertice.x = (float)x;
			vertice.y = 0;
			vertice.z = (float)z;
			//vertices.push_back(vertice);
			vertices[index] = vertice;
		}
	}
}

uint32_t GenerateBoundaryMesh(int deg_left, int deg_top, int deg_right, int deg_bottom, int levelsize, uint32_t* idata)
{
#define CALC_BOUNDARY_INDEX(x, z) \
	((z) * (MESH_SIZE + 1) + (x))
	// END

	uint32_t numwritten = 0;

	// top edge
	if (deg_top < levelsize) {
		int t_step = levelsize / deg_top;

		for (int i = 0; i < levelsize; i += t_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i, 0);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + t_step / 2, 1);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + t_step, 0);

			for (int j = 0; j < t_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_left < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i, 0);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, 1);
			}

			for (int j = t_step / 2; j < t_step; ++j) {
				if (i == levelsize - t_step && j == t_step - 1 && deg_right < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + t_step, 0);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, 1);
			}
		}
	}

	// left edge
	if (deg_left < levelsize) {
		int l_step = levelsize / deg_left;

		for (int i = 0; i < levelsize; i += l_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i + l_step);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + l_step / 2);

			for (int j = 0; j < l_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_top < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j + 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j);
			}

			for (int j = l_step / 2; j < l_step; ++j) {
				if (i == levelsize - l_step && j == l_step - 1 && deg_bottom < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i + l_step);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j + 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j);
			}
		}
	}

	// right edge
	if (deg_right < levelsize) {
		int r_step = levelsize / deg_right;

		for (int i = 0; i < levelsize; i += r_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + r_step / 2);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i + r_step);

			for (int j = 0; j < r_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_top < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j + 1);
			}

			for (int j = r_step / 2; j < r_step; ++j) {
				if (i == levelsize - r_step && j == r_step - 1 && deg_bottom < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i + r_step);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j + 1);
			}
		}
	}

	// bottom edge
	if (deg_bottom < levelsize) {
		int b_step = levelsize / deg_bottom;

		for (int i = 0; i < levelsize; i += b_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i, levelsize);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + b_step, levelsize);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + b_step / 2, levelsize - 1);

			for (int j = 0; j < b_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_left < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i, levelsize);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, levelsize - 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, levelsize - 1);
			}

			for (int j = b_step / 2; j < b_step; ++j) {
				if (i == levelsize - b_step && j == b_step - 1 && deg_right < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + b_step, levelsize);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, levelsize - 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, levelsize - 1);
			}
		}
	}

	return numwritten;
}

uint32_t Log2OfPow2(uint32_t x)
{
	uint32_t ret = 0;

	while (x >>= 1)
		++ret;

	return ret;
}

void GenerateLODLevels(std::vector<SubRangeRender>& subsettable, uint32_t* numsubsets, uint32_t* idata)
{
#define CALC_INNER_INDEX(x, z) \
	((top + (z)) * (MESH_SIZE + 1) + left + (x))
	// END

	assert(numsubsets);

	int numlods = Log2OfPow2(MESH_SIZE);
	*numsubsets = (numlods - 2) * 3 * 3 * 3 * 3 * 2;
	//*subsettable = new OpenGLAttributeRange[*numsubsets];

	int currsubset = 0;
	uint32_t indexoffset = 0;
	uint32_t numwritten = 0;

	SubRangeRender* subset = 0;
	int count = 0;
	for (uint32_t level = 0; level < numlods - 2; ++level) {
		int levelsize = MESH_SIZE >> level;
		int mindegree = levelsize >> 3;

		for (int left_degree = levelsize; left_degree > mindegree; left_degree >>= 1) {
			for (int right_degree = levelsize; right_degree > mindegree; right_degree >>= 1) {
				for (int bottom_degree = levelsize; bottom_degree > mindegree; bottom_degree >>= 1) {
					for (int top_degree = levelsize; top_degree > mindegree; top_degree >>= 1) {
						int right = ((right_degree == levelsize) ? levelsize : levelsize - 1);
						int left = ((left_degree == levelsize) ? 0 : 1);
						int bottom = ((bottom_degree == levelsize) ? levelsize : levelsize - 1);
						int top = ((top_degree == levelsize) ? 0 : 1);

						// generate inner mesh (triangle strip)
						int width = right - left;
						int height = bottom - top;

						numwritten = 0;

						for (int z = 0; z < height; ++z) {
							if ((z & 1) == 1) {
								idata[numwritten++] = CALC_INNER_INDEX(0, z);
								idata[numwritten++] = CALC_INNER_INDEX(0, z + 1);

								for (int x = 0; x < width; ++x) {
									idata[numwritten++] = CALC_INNER_INDEX(x + 1, z);
									idata[numwritten++] = CALC_INNER_INDEX(x + 1, z + 1);
								}

								idata[numwritten++] = UINT32_MAX;
								//++numrestarts;
							}
							else {
								idata[numwritten++] = CALC_INNER_INDEX(width, z + 1);
								idata[numwritten++] = CALC_INNER_INDEX(width, z);

								for (int x = width - 1; x >= 0; --x) {
									idata[numwritten++] = CALC_INNER_INDEX(x, z + 1);
									idata[numwritten++] = CALC_INNER_INDEX(x, z);
								}

								idata[numwritten++] = UINT32_MAX;
								//++numrestarts;
							}
						}
						count += numwritten;

						// add inner subset
						subset = &subsettable[currsubset];

						subset->AttribId = currsubset;
						subset->Enabled = (numwritten > 0);
						subset->IndexCount = numwritten;
						subset->IndexStart = indexoffset;
						subset->PrimitiveType = 0;
						subset->VertexCount = 0;
						subset->VertexStart = 0;

						indexoffset += numwritten;
						idata += numwritten;

						++currsubset;

						// generate boundary mesh (triangle list)
						numwritten = GenerateBoundaryMesh(left_degree, top_degree, right_degree, bottom_degree, levelsize, idata);

						// add boundary subset
						subset = &subsettable[currsubset];

						subset->AttribId = currsubset;
						subset->Enabled = (numwritten > 0);
						subset->IndexCount = numwritten;
						subset->IndexStart = indexoffset;
						subset->PrimitiveType = 0;
						subset->VertexCount = 0;
						subset->VertexStart = 0;

						indexoffset += numwritten;
						idata += numwritten;

						++currsubset;
					}
				}
			}
		}
	}
}
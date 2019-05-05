using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class start : MonoBehaviour {

    public Material material;
    const int ratio = 64;
    const int rat1 = 64;

    GameObject target;
    Ocean ocean;
	// Use this for initialization
	void Start () {
        ocean = new Ocean(ratio, 0.0002f, new Vector2(0.0f, 16.0f), 64, false);
        target = new GameObject("target");
        MeshFilter mesh_filter = target.AddComponent<MeshFilter>();
        mesh_filter.mesh = new Mesh();

        List<Vector3> ver_list = new List<Vector3>();
        List<Vector3> nor_list = new List<Vector3>();
        for (int n = 0; n < rat1; n++)
        {
            for (int m = 0; m < rat1; m++)
            {
                int index = n * rat1 + m;
                float x, y, z;
                float nx, ny, nz;
                x = ocean.vertices[index].x;
                y = ocean.vertices[index].y;
                z = ocean.vertices[index].z;
                nx = ocean.vertices[index].nx;
                ny = ocean.vertices[index].ny;
                nz = ocean.vertices[index].nz;

                ver_list.Add(new Vector3(x, y, z));
                nor_list.Add(new Vector3(nx, ny, nz));
            }
        }
        mesh_filter.mesh.vertices = ver_list.ToArray();
        mesh_filter.mesh.normals = nor_list.ToArray();
        //mesh_filter.mesh.SetIndices(ocean.indices, MeshTopology.Triangles, 0);
        mesh_filter.mesh.triangles = ocean.indices;

        MeshRenderer mesh_render = target.AddComponent<MeshRenderer>();
        mesh_render.sharedMaterial = material;
    }
	
	// Update is called once per frame
	void Update () {
		
	}
}

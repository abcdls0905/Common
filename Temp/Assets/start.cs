using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class start : MonoBehaviour {

    public Texture2D texture;
    public Material material;
    const int ratio = 64;
    const int rat1 = 65;

    GameObject target;
    Ocean ocean;
	// Use this for initialization
	void Start () {
        ocean = new Ocean(ratio, 0.0002f, new Vector2(0.0f, 16.0f), 64, false);
        target = new GameObject("Ocean");
        MeshFilter mesh_filter = target.AddComponent<MeshFilter>();
        mesh_filter.mesh = new Mesh();

        MeshRenderer mesh_render = target.AddComponent<MeshRenderer>();
        mesh_render.sharedMaterial = material;
    }

    void UpdateMesh()
    {
        MeshFilter mesh_filter = target.GetComponent<MeshFilter>();
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
        mesh_filter.mesh.triangles = ocean.indices;

    }

    void UpdateTex()
    {
        if (texture == null)
            texture = new Texture2D(ratio, ratio, TextureFormat.RGBA32, false);
        List<Vector3> offset_list = new List<Vector3>();
        for (int n = 0; n < rat1; n++)
        {
            for (int m = 0; m < rat1; m++)
            {
                int index = n * rat1 + m;
                float x, y, z;
                x = ocean.vertices[index].offset_x;
                y = ocean.vertices[index].offset_y;
                z = ocean.vertices[index].offset_z;
                offset_list.Add(new Vector3(x, y, z));
            }
        }

        Vector3[] offsets = offset_list.ToArray();
        Color[] colors = new Color[rat1 * rat1];

        for (int n = 0; n < rat1; n++)
        {
            for (int m = 0; m < rat1; m++)
            {
                int index = n * rat1 + m;
                Vector3 offset = offsets[index];
                colors[index] = new Color(offset.x, offset.y, offset.z);
            }
        }
        texture.SetPixels(colors);
        texture.Apply();
    }

	// Update is called once per frame
	void Update () {
        ocean.evaluateWaveFFT(Time.realtimeSinceStartup);
        UpdateMesh();
        UpdateTex();
    }
    void OnGUI()
    {
        if (GUI.Button(new Rect(100, 60, 60, 30), "save"))
        {
            SaveTextureToPNG(texture, "displacement");
        }
    }

    public bool SaveTextureToPNG(Texture2D tex, string pngName)
    {
        Color[] colors = tex.GetPixels();
        byte[] bytes = tex.EncodeToPNG();
        FileStream file = File.Open(Application.dataPath + "/" + pngName + ".png", FileMode.Create);
        BinaryWriter writer = new BinaryWriter(file);
        writer.Write(bytes);
        file.Close();
        return true;
    }
}

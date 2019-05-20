using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Collections.Generic;

public class OceanData
{
    public GameObject OceanObj;
    public Ocean ocean;
}

public class start : MonoBehaviour {

    public int interval = 3;
    int lastinterval = 0;
    public Texture2D test_texture;
    public Texture2D texture;
    public Material material;

    const int radio_const = 64;

    const int ratio = radio_const;
    const int rat1 = radio_const + 1;

    List<OceanData> gameLists = new List<OceanData>();
    public int ocean_index = 0;
	// Use this for initialization
	void Start () {
        CreateOcean(new Vector3(0, 0, 0));
        CreateOcean(new Vector3(0, 0, rat1));
    }

    void CreateOcean(Vector3 pos)
    {
        GameObject OceanObj = new GameObject("Ocean_" + ocean_index);
        OceanObj.transform.position = pos;
        MeshFilter mesh_filter = OceanObj.AddComponent<MeshFilter>();
        mesh_filter.mesh = new Mesh();

        MeshRenderer mesh_render = OceanObj.AddComponent<MeshRenderer>();
        mesh_render.sharedMaterial = material;
        texture = new Texture2D(rat1, rat1, TextureFormat.RGBAFloat, false);

        OceanData data = new OceanData();
        data.OceanObj = OceanObj;
        Ocean ocean = new Ocean(ratio, 0.0002f, new Vector2(0.0f, 16.0f), ratio, false);
        data.ocean = ocean;
        gameLists.Add(data);
        ocean_index++;
    }

    void UpdateMesh(OceanData oceanData)
    {
        MeshFilter mesh_filter = oceanData.OceanObj.GetComponent<MeshFilter>();
        List<Vector3> ver_list = new List<Vector3>();
        List<Vector3> nor_list = new List<Vector3>();
        for (int n = 0; n < rat1; n++)
        {
            for (int m = 0; m < rat1; m++)
            {
                int index = n * rat1 + m;
                float x, y, z;
                float nx, ny, nz;
                x = oceanData.ocean.vertices[index].x;
                y = oceanData.ocean.vertices[index].y;
                z = oceanData.ocean.vertices[index].z;
                nx = oceanData.ocean.vertices[index].nx;
                ny = oceanData.ocean.vertices[index].ny;
                nz = oceanData.ocean.vertices[index].nz;

                ver_list.Add(new Vector3(x, y, z));
                nor_list.Add(new Vector3(nx, ny, nz));
            }
        }
        mesh_filter.mesh.vertices = ver_list.ToArray();
        mesh_filter.mesh.normals = nor_list.ToArray();
        mesh_filter.mesh.triangles = oceanData.ocean.indices;

    }

    int tex_idx = 0;
    void UpdateTex(OceanData oceanData)
    {
        if (lastinterval > interval)
        {
            lastinterval = 0;
        }
        lastinterval++;

        List<Vector3> offset_list = new List<Vector3>();
        for (int n = 0; n < rat1; n++)
        {
            for (int m = 0; m < rat1; m++)
            {
                int index = n * rat1 + m;
                float x, y, z;
                x = oceanData.ocean.vertices[index].offset_x;
                y = oceanData.ocean.vertices[index].offset_y;
                z = oceanData.ocean.vertices[index].offset_z;
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
                const int max_value = 6;
                offset += new Vector3(max_value, max_value, max_value);
                offset /= 2 * max_value;
                colors[index] = new Color(offset.x, offset.y, offset.z);
            }
        }
        texture.SetPixels(colors);
        texture.Apply();
        tex_idx++;
        if (tex_idx > 50 || true)
            return;
        SaveTextureToPNG(texture, "displacement" + tex_idx);
    }
    
	// Update is called once per frame
	void Update () {
        int count = gameLists.Count;
        for (int i = 0; i < count; ++i)
        {
            OceanData oceanData = gameLists[i];
            oceanData.ocean.evaluateWaveFFT(Time.realtimeSinceStartup);
            UpdateMesh(oceanData);
            UpdateTex(oceanData);
        }
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
        FileStream file = File.Open(Application.dataPath + "/Displacements/" + pngName + ".png", FileMode.Create);
        BinaryWriter writer = new BinaryWriter(file);
        writer.Write(bytes);
        file.Close();
        return true;
    }
}

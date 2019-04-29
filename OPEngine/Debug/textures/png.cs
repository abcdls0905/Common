using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class png : MonoBehaviour {

    public RenderTexture renderTex;
	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}
    void OnGUI()
    {
        if (GUI.Button(new Rect(100, 60, 60, 30), "save"))
        {
            {
                Camera cam = Camera.main;
                SaveTextureToPNG(cam.activeTexture, "displacement");
            }
        }
    }
 
    public bool SaveTextureToPNG(RenderTexture rt, string pngName)
    {
        RenderTexture prev = RenderTexture.active;
        RenderTexture.active = rt;
        Texture2D png = new Texture2D(rt.width, rt.height, TextureFormat.RGBAFloat, false);
        png.ReadPixels(new Rect(0, 0, rt.width, rt.height), 0, 0);

        Color[] colors = png.GetPixels();
        for (int i =0; i < colors.Length; i++)
        {
            Color color = colors[i];
            color *= 20;
            colors[i] = color;
        }
        png.SetPixels(colors);
        byte[] bytes = png.EncodeToPNG();
        FileStream file = File.Open(Application.dataPath + "/" + pngName + ".png", FileMode.Create);
        BinaryWriter writer = new BinaryWriter(file);
        writer.Write(bytes);
        file.Close();
        Texture2D.DestroyImmediate(png);
        png = null;
        RenderTexture.active = prev;
        return true;
    }
}

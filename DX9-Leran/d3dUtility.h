#ifndef _D3DUTILITY_H_
#define _D3DUTILITY_H_
#include <d3d9.h>
#include <d3dx9.h>
#include <exception>
#include <string>
namespace d3dUtil {


    class ProjectError:public std::exception
    {
    public:
        ProjectError(const std::string &mess): message(mess) 
        {

        }
        const char * what()  const override
        {
            return message.c_str();
        }
        ~ProjectError() = default;

    private:
        std::string message;
    };

	template<typename T>
	void Release(T t)
	{
		if (t)
		{
			t->Release();
			t = 0;
		}
	}

	template<typename T>
	void Delete(T t)
	{
		if (t)
		{
			delete t;
			t = 0;
		}
	}

	const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
	const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
	const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
	const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
	const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
	const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
	const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
	const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));

	const D3DXCOLOR BEACH_SAND(D3DCOLOR_XRGB(255, 249, 157));
	const D3DXCOLOR DESERT_SAND(D3DCOLOR_XRGB(250, 205, 135));

	const D3DXCOLOR LIGHTGREEN(D3DCOLOR_XRGB(60, 184, 120));
	const D3DXCOLOR  PUREGREEN(D3DCOLOR_XRGB(0, 166, 81));
	const D3DXCOLOR  DARKGREEN(D3DCOLOR_XRGB(0, 114, 54));

	const D3DXCOLOR LIGHT_YELLOW_GREEN(D3DCOLOR_XRGB(124, 197, 118));
	const D3DXCOLOR  PURE_YELLOW_GREEN(D3DCOLOR_XRGB(57, 181, 74));
	const D3DXCOLOR  DARK_YELLOW_GREEN(D3DCOLOR_XRGB(25, 123, 48));

	const D3DXCOLOR LIGHTBROWN(D3DCOLOR_XRGB(198, 156, 109));
	const D3DXCOLOR DARKBROWN(D3DCOLOR_XRGB(115, 100, 87));


	//-----------------------------------------------------------------------------
	// Name: createTangentSpaceVectors()
	// Desc: Given a vertex (v1) and two other vertices (v2 & v3) which define a 
	//       triangle, this function will return Tangent, BiNormal, and Normal, 
	//       vectors which can be used to define the tangent matrix for the first 
	//       vertex's position (v1).
	//
	// Args: v1        - vertex 1
	//       v2        - vertex 2
	//       v3        - vertex 3
	//       v1u, v1v  - texture-coordinates of vertex 1
	//       v2u, v2v  - texture-coordinates of vertex 2
	//       v3u, v3v  - texture-coordinates of vertex 3
	//       vTangent  - When the function returns, this will be set as the tangent vector
	//       vBiNormal - When the function returns, this will be set as the binormal vector
	//       vNormal   - When the function returns, this will be set as the normal vector
	//
	// Note: This function is based on an article by By Jakob Gath and Sbren Dreijer.
	// http://www.blacksmith-studios.dk/projects/downloads/tangent_matrix_derivation.php
	//------------------------------------------------------------------------------

	void createTangentSpaceVectors(D3DXVECTOR3 *v1,
		D3DXVECTOR3 *v2,
		D3DXVECTOR3 *v3,
		float v1u, float v1v,
		float v2u, float v2v,
		float v3u, float v3v,
		D3DXVECTOR3 *vTangent,
		D3DXVECTOR3 *vBiNormal,
		D3DXVECTOR3 *vNormal);


    //Box-Sphere Intersection
    bool checkIntersectionBoxSphere(const D3DXVECTOR3 &boxCenter,
        const D3DXVECTOR3 &boxSide1,
        const D3DXVECTOR3 &boxSide2,
        const D3DXVECTOR3 &boxSide3,
        const D3DXVECTOR3 &sphereCenter,
        float radius);

}

#endif

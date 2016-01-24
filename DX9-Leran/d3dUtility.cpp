#include "d3dUtility.h"

void d3dUtil::createTangentSpaceVectors(D3DXVECTOR3 * v1, D3DXVECTOR3 * v2, D3DXVECTOR3 * v3, float v1u, float v1v, float v2u, float v2v, float v3u, float v3v, D3DXVECTOR3 * vTangent, D3DXVECTOR3 * vBiNormal, D3DXVECTOR3 * vNormal)
{
	// Create edge vectors from vertex 1 to vectors 2 and 3.
	D3DXVECTOR3 vDirVec_v2_to_v1 = *v2 - *v1;
	D3DXVECTOR3 vDirVec_v3_to_v1 = *v3 - *v1;

	// Create edge vectors from the texture coordinates of vertex 1 to vector 2.
	float vDirVec_v2u_to_v1u = v2u - v1u;
	float vDirVec_v2v_to_v1v = v2v - v1v;

	// Create edge vectors from the texture coordinates of vertex 1 to vector 3.
	float vDirVec_v3u_to_v1u = v3u - v1u;
	float vDirVec_v3v_to_v1v = v3v - v1v;

	float fDenominator = vDirVec_v2u_to_v1u * vDirVec_v3v_to_v1v -
		vDirVec_v3u_to_v1u * vDirVec_v2v_to_v1v;

	if (fDenominator < 0.0001f && fDenominator > -0.0001f)
	{
		// We're too close to zero and we're at risk of a divide-by-zero! 
		// Set the tangent matrix to the identity matrix and do nothing.
		*vTangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		*vBiNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		*vNormal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		// Calculate and cache the reciprocal value
		float fScale1 = 1.0f / fDenominator;

		D3DXVECTOR3 T;
		D3DXVECTOR3 B;
		D3DXVECTOR3 N;

		T = D3DXVECTOR3((vDirVec_v3v_to_v1v * vDirVec_v2_to_v1.x - vDirVec_v2v_to_v1v * vDirVec_v3_to_v1.x) * fScale1,
			(vDirVec_v3v_to_v1v * vDirVec_v2_to_v1.y - vDirVec_v2v_to_v1v * vDirVec_v3_to_v1.y) * fScale1,
			(vDirVec_v3v_to_v1v * vDirVec_v2_to_v1.z - vDirVec_v2v_to_v1v * vDirVec_v3_to_v1.z) * fScale1);

		B = D3DXVECTOR3((-vDirVec_v3u_to_v1u * vDirVec_v2_to_v1.x + vDirVec_v2u_to_v1u * vDirVec_v3_to_v1.x) * fScale1,
			(-vDirVec_v3u_to_v1u * vDirVec_v2_to_v1.y + vDirVec_v2u_to_v1u * vDirVec_v3_to_v1.y) * fScale1,
			(-vDirVec_v3u_to_v1u * vDirVec_v2_to_v1.z + vDirVec_v2u_to_v1u * vDirVec_v3_to_v1.z) * fScale1);

		// The normal N is calculated as the cross product between T and B
		D3DXVec3Cross(&N, &T, &B);

		// Calculate and cache the reciprocal value
		float fScale2 = 1.0f / ((T.x * B.y * N.z - T.z * B.y * N.x) +
			(B.x * N.y * T.z - B.z * N.y * T.x) +
			(N.x * T.y * B.z - N.z * T.y * B.x));

		//
		// Use the temporary T (Tangent), (B) Binormal, and N (Normal) vectors 
		// to calculate the inverse of the tangent matrix that they represent.
		// The inverse of the tangent matrix is what we want since we need that
		// to transform the light's vector into tangent-space.
		//

		D3DXVECTOR3 vTemp;

		(*vTangent).x = D3DXVec3Cross(&vTemp, &B, &N)->x * fScale2;
		(*vTangent).y = -(D3DXVec3Cross(&vTemp, &N, &T)->x * fScale2);
		(*vTangent).z = D3DXVec3Cross(&vTemp, &T, &B)->x * fScale2;
		D3DXVec3Normalize(&(*vTangent), &(*vTangent));

		(*vBiNormal).x = -(D3DXVec3Cross(&vTemp, &B, &N)->y * fScale2);
		(*vBiNormal).y = D3DXVec3Cross(&vTemp, &N, &T)->y * fScale2;
		(*vBiNormal).z = -(D3DXVec3Cross(&vTemp, &T, &B)->y * fScale2);
		D3DXVec3Normalize(&(*vBiNormal), &(*vBiNormal));

		(*vNormal).x = D3DXVec3Cross(&vTemp, &B, &N)->z * fScale2;
		(*vNormal).y = -(D3DXVec3Cross(&vTemp, &N, &T)->z * fScale2);
		(*vNormal).z = D3DXVec3Cross(&vTemp, &T, &B)->z * fScale2;
		D3DXVec3Normalize(&(*vNormal), &(*vNormal));

		//
		// NOTE: Since the texture-space of Direct3D and OpenGL are laid-out 
		//       differently, a single normal map can't look right in both 
		//       unless you make some adjustments somewhere.
		//
		//       You can adjust or fix this problem in three ways:
		//
		//       1. Create two normal maps: one for OpenGL and one for Direct3D.
		//       2. Flip the normal map image over as you load it into a texture 
		//          object.
		//       3. Flip the binormal over when computing the tangent-space
		//          matrix.
		//
		// Since the normal map used by this sample was created for Direct3D,
		// no adjustment is necessary.
		//
		//*vBiNormal = *vBiNormal * -1.0f;
	}
}

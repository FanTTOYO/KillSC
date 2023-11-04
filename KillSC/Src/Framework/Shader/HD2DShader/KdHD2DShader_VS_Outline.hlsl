#include "inc_KdHD2DShader.hlsli"
#include "../inc_KdCommon.hlsli"

//==================================
// 頂点シェーダー
//==================================
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0, // UV座標
	float3 normal : NORMAL, // 法線
	float4 color : COLOR, // 頂点色
	float3 tangent : TANGENT // 接線
)
{
    VSOutput Out;
	
	// 頂点座標を「法線方向」に少しずらす
    pos.xyz = pos.xyz + normal * 0.01;
	
	// 「座標変換」
    Out.Pos = mul(pos, g_mWorld);
    Out.Pos = mul(Out.Pos, g_mView);
    Out.Pos = mul(Out.Pos, g_mProj);
	
    return Out;
}
#include "inc_KdHD2DShader.hlsli"
#include "../inc_KdCommon.hlsli"

//==================================
// 頂点シェーダー
//==================================
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0, // UV座標
	float3 normal : NORMAL, // 法線
	float4 color : COLOR, // 頂点色
	float3 tangent : TANGENT, // 接線
	uint4 skinIndex : SKININDEX,	// スキンメッシュのボーンインデックス(何番目のボーンに影響しているか?のデータ(最大4つぶん))
	float4 skinWeight : SKINWEIGHT	// ボーンの影響度(そのボーンに、どれだけ影響しているか?のデータ(最大4つぶん))
)
{
    VSOutput Out;
	
	// スキニング---------------->
	// 行列を合成
    row_major float4x4 mBones = 0; // 行列を0埋め
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        mBones += g_mBones[skinIndex[i]] * skinWeight[i];
    }

	// 座標と法線に適用
    pos = mul(pos, mBones);
    normal = mul(normal, (float3x3) mBones);
	// <----------------スキニング
	
	// 頂点座標を「法線方向」に少しずらす
    pos.xyz = pos.xyz + normal * 0.01;
	
	// 「座標変換」
    Out.Pos = mul(pos, g_mWorld);
    Out.Pos = mul(Out.Pos, g_mView);
    Out.Pos = mul(Out.Pos, g_mProj);
	
    return Out;
}
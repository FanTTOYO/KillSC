#include "inc_KdHD2DShader.hlsli"
#include "../inc_KdCommon.hlsli"

//==================================
// ���_�V�F�[�_�[
//==================================
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0, // UV���W
	float3 normal : NORMAL, // �@��
	float4 color : COLOR, // ���_�F
	float3 tangent : TANGENT, // �ڐ�
	uint4 skinIndex : SKININDEX,	// �X�L�����b�V���̃{�[���C���f�b�N�X(���Ԗڂ̃{�[���ɉe�����Ă��邩?�̃f�[�^(�ő�4�Ԃ�))
	float4 skinWeight : SKINWEIGHT	// �{�[���̉e���x(���̃{�[���ɁA�ǂꂾ���e�����Ă��邩?�̃f�[�^(�ő�4�Ԃ�))
)
{
    VSOutput Out;
	
	// �X�L�j���O---------------->
	// �s�������
    row_major float4x4 mBones = 0; // �s���0����
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        mBones += g_mBones[skinIndex[i]] * skinWeight[i];
    }

	// ���W�Ɩ@���ɓK�p
    pos = mul(pos, mBones);
    normal = mul(normal, (float3x3) mBones);
	// <----------------�X�L�j���O
	
	// ���_���W���u�@�������v�ɏ������炷
    pos.xyz = pos.xyz + normal * 0.01;
	
	// �u���W�ϊ��v
    Out.Pos = mul(pos, g_mWorld);
    Out.Pos = mul(Out.Pos, g_mView);
    Out.Pos = mul(Out.Pos, g_mProj);
	
    return Out;
}
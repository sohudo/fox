#ifndef MD5_CHECK_SUM_HEAD
#define MD5_CHECK_SUM_HEAD
#pragma once

///////////////////////////////////////////////////////////////////////////////////////

//初始常量
#define MD5_INIT_STATE_0	0x67452301
#define MD5_INIT_STATE_1	0xefcdab89
#define MD5_INIT_STATE_2	0x98badcfe
#define MD5_INIT_STATE_3	0x10325476

//转换常量
#define MD5_S11		7
#define MD5_S12		12
#define MD5_S13		17
#define MD5_S14		22
#define MD5_S21		5
#define MD5_S22		9
#define MD5_S23		14
#define MD5_S24		20
#define MD5_S31		4
#define MD5_S32		11
#define MD5_S33		16
#define MD5_S34		23
#define MD5_S41		6
#define MD5_S42		10
#define MD5_S43		15
#define MD5_S44		21

//1轮转换常量
#define MD5_T01		0xd76aa478 
#define MD5_T02		0xe8c7b756 
#define MD5_T03		0x242070db 
#define MD5_T04		0xc1bdceee 
#define MD5_T05		0xf57c0faf 
#define MD5_T06		0x4787c62a 
#define MD5_T07		0xa8304613 
#define MD5_T08		0xfd469501 
#define MD5_T09		0x698098d8 
#define MD5_T10		0x8b44f7af 
#define MD5_T11		0xffff5bb1 
#define MD5_T12		0x895cd7be 
#define MD5_T13		0x6b901122 
#define MD5_T14		0xfd987193 
#define MD5_T15		0xa679438e 
#define MD5_T16		0x49b40821 

//2轮转换常量
#define MD5_T17		0xf61e2562 
#define MD5_T18		0xc040b340 
#define MD5_T19		0x265e5a51 
#define MD5_T20		0xe9b6c7aa 
#define MD5_T21		0xd62f105d 
#define MD5_T22		0x02441453 
#define MD5_T23		0xd8a1e681 
#define MD5_T24		0xe7d3fbc8 
#define MD5_T25		0x21e1cde6 
#define MD5_T26		0xc33707d6 
#define MD5_T27		0xf4d50d87 
#define MD5_T28		0x455a14ed 
#define MD5_T29		0xa9e3e905 
#define MD5_T30		0xfcefa3f8 
#define MD5_T31		0x676f02d9 
#define MD5_T32		0x8d2a4c8a 

//3轮转换常量
#define MD5_T33		0xfffa3942 
#define MD5_T34		0x8771f681 
#define MD5_T35		0x6d9d6122 
#define MD5_T36		0xfde5380c 
#define MD5_T37		0xa4beea44 
#define MD5_T38		0x4bdecfa9 
#define MD5_T39		0xf6bb4b60 
#define MD5_T40		0xbebfbc70 
#define MD5_T41		0x289b7ec6 
#define MD5_T42		0xeaa127fa 
#define MD5_T43		0xd4ef3085 
#define MD5_T44		0x04881d05 
#define MD5_T45		0xd9d4d039 
#define MD5_T46		0xe6db99e5 
#define MD5_T47		0x1fa27cf8 
#define MD5_T48		0xc4ac5665 

//4轮转换常量
#define MD5_T49		0xf4292244 
#define MD5_T50		0x432aff97 
#define MD5_T51		0xab9423a7 
#define MD5_T52		0xfc93a039 
#define MD5_T53		0x655b59c3 
#define MD5_T54		0x8f0ccc92 
#define MD5_T55		0xffeff47d 
#define MD5_T56		0x85845dd1 
#define MD5_T57		0x6fa87e4f 
#define MD5_T58		0xfe2ce6e0 
#define MD5_T59		0xa3014314 
#define MD5_T60		0x4e0811a1 
#define MD5_T61		0xf7537e82 
#define MD5_T62		0xbd3af235 
#define MD5_T63		0x2ad7d2bb 
#define MD5_T64		0xeb86d391 


//填充数据
static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

///////////////////////////////////////////////////////////////////////////////////////

//MD5校验
class CMD5Checksum  
{
	//存储变量
private:
	BYTE                  m_lpszBuffer[64];             //输入缓冲
	ULONG                 m_nCount[2];                  //位数计数
	ULONG                 m_lMD5[4];                    //MD5校验和

	//函数定义
protected:
	CMD5Checksum();
	virtual ~CMD5Checksum();

	//静态函数
public:
	static CString GetMD5(BYTE* pBuf, UINT nLength);
	static CString GetMD5(CFile& File);
	static CString GetMD5(const CString& strFilePath);

    //辅助函数
protected:
	CString Final();

	//辅助函数
protected:
	void Transform(BYTE Block[64]);
	void Update(BYTE* Input, ULONG nInputLen);
	void DWordToByte(BYTE* Output, DWORD* Input, UINT nLength);
	void ByteToDWord(DWORD* Output, BYTE* Input, UINT nLength);

	//内联函数
protected:
	inline DWORD RotateLeft(DWORD x, int n);
	inline void FF( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void GG( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void HH( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void II( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
};

///////////////////////////////////////////////////////////////////////////////////////

CMD5Checksum::CMD5Checksum()
{
	//初始成员
	memset( m_lpszBuffer, 0, 64 );
	m_nCount[0] = m_nCount[1] = 0;

	//初始变量
	m_lMD5[0] = MD5_INIT_STATE_0;
	m_lMD5[1] = MD5_INIT_STATE_1;
	m_lMD5[2] = MD5_INIT_STATE_2;
	m_lMD5[3] = MD5_INIT_STATE_3;
}

CMD5Checksum::~CMD5Checksum()
{
}

CString CMD5Checksum::GetMD5(const CString& strFilePath)
{
	try
	{
		//打开文件
		CFile File(strFilePath, CFile::shareDenyNone);

		//返回校验和
		return GetMD5(File);
	}
	catch(CFileException* e)
	{
		throw e;
	}	
}

CString CMD5Checksum::GetMD5(CFile& File)
{
	try
	{
		//变量定义
		CMD5Checksum MD5Checksum;  
		int nLength = 0;  
		const int nBufferSize = 1024; 
		BYTE Buffer[nBufferSize];  

		//校验文件
		while ((nLength = File.Read( Buffer, nBufferSize )) > 0 )
		{
			MD5Checksum.Update( Buffer, nLength );
		}

		//完成校验
		return MD5Checksum.Final();
	}
	catch (CFileException* e )
	{
		TRACE0("CMD5Checksum::GetMD5: CFileException caught"); 
		throw e;
	}
}

CString CMD5Checksum::GetMD5(BYTE* pBuf, UINT nLength)
{
	AfxIsValidAddress(pBuf,nLength,FALSE);

	//变量定义
	CMD5Checksum MD5Checksum;

	//计算校验和
	MD5Checksum.Update( pBuf, nLength );

	//完成校验
	return MD5Checksum.Final();
}

CString CMD5Checksum::Final()
{
	//变量定义
	BYTE Bits[8];
	DWordToByte( Bits, m_nCount, 8 );

	//变量定义
	UINT nIndex = (UINT)((m_nCount[0] >> 3) & 0x3f);
	UINT nPadLen = (nIndex < 56) ? (56 - nIndex) : (120 - nIndex);
	Update( PADDING, nPadLen );

	//更新数据
	Update( Bits, 8 );

	//变量定义
	const int nMD5Size = 16;
	unsigned char lpszMD5[ nMD5Size ];
	DWordToByte( lpszMD5, m_lMD5, nMD5Size );

	//转换类型
	CString strMD5;
	for ( int i=0; i < nMD5Size; i++) 
	{
		CString Str;
		if (lpszMD5[i] == 0) 
		{
			Str = CString("00");
		}
		else if (lpszMD5[i] <= 15)  
		{
			Str.Format(TEXT("0%x"),lpszMD5[i]);
		}
		else 
		{
			Str.Format(TEXT("%x"),lpszMD5[i]);
		}
		
		ASSERT( Str.GetLength() == 2 );
		strMD5 += Str;
	}

	//结果校验
	ASSERT( strMD5.GetLength() == 32 );
	return strMD5;
}


void CMD5Checksum::Transform(BYTE Block[64])
{
	//变量定义
	ULONG a = m_lMD5[0];
	ULONG b = m_lMD5[1];
	ULONG c = m_lMD5[2];
	ULONG d = m_lMD5[3];

	//变量定义
	ULONG X[16];

	//数据转换
	ByteToDWord( X, Block, 64 );

	//1轮转换
	FF (a, b, c, d, X[ 0], MD5_S11, MD5_T01); 
	FF (d, a, b, c, X[ 1], MD5_S12, MD5_T02); 
	FF (c, d, a, b, X[ 2], MD5_S13, MD5_T03); 
	FF (b, c, d, a, X[ 3], MD5_S14, MD5_T04); 
	FF (a, b, c, d, X[ 4], MD5_S11, MD5_T05); 
	FF (d, a, b, c, X[ 5], MD5_S12, MD5_T06); 
	FF (c, d, a, b, X[ 6], MD5_S13, MD5_T07); 
	FF (b, c, d, a, X[ 7], MD5_S14, MD5_T08); 
	FF (a, b, c, d, X[ 8], MD5_S11, MD5_T09); 
	FF (d, a, b, c, X[ 9], MD5_S12, MD5_T10); 
	FF (c, d, a, b, X[10], MD5_S13, MD5_T11); 
	FF (b, c, d, a, X[11], MD5_S14, MD5_T12); 
	FF (a, b, c, d, X[12], MD5_S11, MD5_T13); 
	FF (d, a, b, c, X[13], MD5_S12, MD5_T14); 
	FF (c, d, a, b, X[14], MD5_S13, MD5_T15); 
	FF (b, c, d, a, X[15], MD5_S14, MD5_T16); 

	//2轮转换
	GG (a, b, c, d, X[ 1], MD5_S21, MD5_T17); 
	GG (d, a, b, c, X[ 6], MD5_S22, MD5_T18); 
	GG (c, d, a, b, X[11], MD5_S23, MD5_T19); 
	GG (b, c, d, a, X[ 0], MD5_S24, MD5_T20); 
	GG (a, b, c, d, X[ 5], MD5_S21, MD5_T21); 
	GG (d, a, b, c, X[10], MD5_S22, MD5_T22); 
	GG (c, d, a, b, X[15], MD5_S23, MD5_T23); 
	GG (b, c, d, a, X[ 4], MD5_S24, MD5_T24); 
	GG (a, b, c, d, X[ 9], MD5_S21, MD5_T25); 
	GG (d, a, b, c, X[14], MD5_S22, MD5_T26); 
	GG (c, d, a, b, X[ 3], MD5_S23, MD5_T27); 
	GG (b, c, d, a, X[ 8], MD5_S24, MD5_T28); 
	GG (a, b, c, d, X[13], MD5_S21, MD5_T29); 
	GG (d, a, b, c, X[ 2], MD5_S22, MD5_T30); 
	GG (c, d, a, b, X[ 7], MD5_S23, MD5_T31); 
	GG (b, c, d, a, X[12], MD5_S24, MD5_T32); 

	//3轮转换
	HH (a, b, c, d, X[ 5], MD5_S31, MD5_T33); 
	HH (d, a, b, c, X[ 8], MD5_S32, MD5_T34); 
	HH (c, d, a, b, X[11], MD5_S33, MD5_T35); 
	HH (b, c, d, a, X[14], MD5_S34, MD5_T36); 
	HH (a, b, c, d, X[ 1], MD5_S31, MD5_T37); 
	HH (d, a, b, c, X[ 4], MD5_S32, MD5_T38); 
	HH (c, d, a, b, X[ 7], MD5_S33, MD5_T39); 
	HH (b, c, d, a, X[10], MD5_S34, MD5_T40); 
	HH (a, b, c, d, X[13], MD5_S31, MD5_T41); 
	HH (d, a, b, c, X[ 0], MD5_S32, MD5_T42); 
	HH (c, d, a, b, X[ 3], MD5_S33, MD5_T43); 
	HH (b, c, d, a, X[ 6], MD5_S34, MD5_T44); 
	HH (a, b, c, d, X[ 9], MD5_S31, MD5_T45); 
	HH (d, a, b, c, X[12], MD5_S32, MD5_T46); 
	HH (c, d, a, b, X[15], MD5_S33, MD5_T47); 
	HH (b, c, d, a, X[ 2], MD5_S34, MD5_T48); 

	//4轮转换
	II (a, b, c, d, X[ 0], MD5_S41, MD5_T49); 
	II (d, a, b, c, X[ 7], MD5_S42, MD5_T50); 
	II (c, d, a, b, X[14], MD5_S43, MD5_T51); 
	II (b, c, d, a, X[ 5], MD5_S44, MD5_T52); 
	II (a, b, c, d, X[12], MD5_S41, MD5_T53); 
	II (d, a, b, c, X[ 3], MD5_S42, MD5_T54); 
	II (c, d, a, b, X[10], MD5_S43, MD5_T55); 
	II (b, c, d, a, X[ 1], MD5_S44, MD5_T56); 
	II (a, b, c, d, X[ 8], MD5_S41, MD5_T57); 
	II (d, a, b, c, X[15], MD5_S42, MD5_T58); 
	II (c, d, a, b, X[ 6], MD5_S43, MD5_T59); 
	II (b, c, d, a, X[13], MD5_S44, MD5_T60); 
	II (a, b, c, d, X[ 4], MD5_S41, MD5_T61); 
	II (d, a, b, c, X[11], MD5_S42, MD5_T62); 
	II (c, d, a, b, X[ 2], MD5_S43, MD5_T63); 
	II (b, c, d, a, X[ 9], MD5_S44, MD5_T64); 

	//设置校验和
	m_lMD5[0] += a;
	m_lMD5[1] += b;
	m_lMD5[2] += c;
	m_lMD5[3] += d;
}

void CMD5Checksum::Update( BYTE* Input, ULONG nInputLen )
{
	//变量定义
	UINT nIndex = (UINT)((m_nCount[0] >> 3) & 0x3F);

	//更新位数
	if ( ( m_nCount[0] += nInputLen << 3 )  <  ( nInputLen << 3) )
	{
		m_nCount[1]++;
	}

	//设置变量
	m_nCount[1] += (nInputLen >> 29);

	//重复转换
	UINT i=0;  
	UINT nPartLen = 64 - nIndex;
	if (nInputLen >= nPartLen)  
	{
		memcpy( &m_lpszBuffer[nIndex], Input, nPartLen );
		Transform( m_lpszBuffer );
		for (i = nPartLen; i + 63 < nInputLen; i += 64) 
		{
			Transform( &Input[i] );
		}
		nIndex = 0;
	} 
	else 
	{
		i = 0;
	}

	//拷贝缓冲
	memcpy( &m_lpszBuffer[nIndex], &Input[i], nInputLen-i);
}

void CMD5Checksum::ByteToDWord(DWORD* Output, BYTE* Input, UINT nLength)
{
	//参数校验
	ASSERT( nLength % 4 == 0 );
	ASSERT( AfxIsValidAddress(Output, nLength/4, TRUE) );
	ASSERT( AfxIsValidAddress(Input, nLength, FALSE) );

	//变量定义
	UINT nOutIndex=0; 
	UINT nLoopIndex=0; 

	//转换拷贝
	for ( ;nLoopIndex < nLength; nOutIndex++, nLoopIndex += 4)
	{
		Output[nOutIndex] = (ULONG)Input[nLoopIndex]	| 
							(ULONG)Input[nLoopIndex+1] << 8 | 
							(ULONG)Input[nLoopIndex+2] << 16 | 
							(ULONG)Input[nLoopIndex+3] << 24;
	}

	return;
}


void CMD5Checksum::DWordToByte(BYTE* Output, DWORD* Input, UINT nLength )
{
	//校验变量
	ASSERT( nLength % 4 == 0 );
	ASSERT( AfxIsValidAddress(Output, nLength, TRUE) );
	ASSERT( AfxIsValidAddress(Input, nLength/4, FALSE) );

	//转换拷贝
	UINT nInIndex = 0;
	UINT nLoopIndex = 0;
	for ( ; nLoopIndex < nLength; nInIndex++, nLoopIndex += 4) 
	{
		Output[nLoopIndex] =   (UCHAR)(Input[nInIndex] & 0xff);
		Output[nLoopIndex+1] = (UCHAR)((Input[nInIndex] >> 8) & 0xff);
		Output[nLoopIndex+2] = (UCHAR)((Input[nInIndex] >> 16) & 0xff);
		Output[nLoopIndex+3] = (UCHAR)((Input[nInIndex] >> 24) & 0xff);
	}

	return;
}

DWORD CMD5Checksum::RotateLeft(DWORD x, int n)
{
	//参数校验
	ASSERT( sizeof(x) == 4 );
	
	//返回结果
	return (x << n) | (x >> (32-n));
}

void CMD5Checksum::FF( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
{
	DWORD F = (B & C) | (~B & D);
	A += F + X + T;
	A = RotateLeft(A, S);
	A += B;
}

void CMD5Checksum::GG( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
{
	DWORD G = (B & D) | (C & ~D);
	A += G + X + T;
	A = RotateLeft(A, S);
	A += B;
}

void CMD5Checksum::HH( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
{
	DWORD H = (B ^ C ^ D);
	A += H + X + T;
	A = RotateLeft(A, S);
	A += B;
}

void CMD5Checksum::II( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
{
	DWORD I = (C ^ (B | ~D));
	A += I + X + T;
	A = RotateLeft(A, S);
	A += B;
}

///////////////////////////////////////////////////////////////////////////////////////

#endif
#include<iostream>
#include<string>
#include<cmath>
#include<time.h>
#include<random>

using namespace std;

uint32_t T[2] = { 0x79CC4519,0x7A879D8A };

//T_i
uint32_t Ti(int i) {
    if(i<=15)
        return T[0];
    else
        return T[1];
}
//FF_i
uint32_t FF(uint32_t X, uint32_t Y, uint32_t Z, int j)
{
    uint32_t ffi = 0;
    if (j >= 0 && j <= 15)
        ffi = X ^ Y ^ Z;
    else if (j >= 16 && j <= 63)
        ffi = (X & Y) | (X & Z) | (Y & Z);
    return ffi;
}
//GG_i
uint32_t GG(uint32_t X, uint32_t Y, uint32_t Z, int j)
{
    uint32_t ggi = 0;
    if (j >= 0 && j <= 15)
        ggi = X ^ Y ^ Z;
    else if (j >= 16 && j <= 63)
        ggi = (X & Y) | ((~X) & Z);
    return ggi;
}
//ѭ������
uint32_t Left(uint32_t x, int i)
{
    return (x << i) | (x >> (32 - i));
}
//�û�����P
uint32_t P_0(uint32_t x)
{
    return (x ^ Left(x, 9) ^ Left(x, 17));
}

uint32_t P_1(uint32_t x)
{
    return (x ^ Left(x, 15) ^ Left(x, 23));
}

//intתΪʮ������
string int2hex(int x) {
    string hex = "";
    int temp = 0;
    while(x!=0)
    {
        temp = x % 16;
        if (temp >= 0 && temp < 10)
            hex = to_string(temp) + hex;
        else
            hex += ('A' + (temp - 10));
        x /= 16;
    }
    return hex;
}
//���
string padding(string s) {
    string s_hex = "";
    for (int i = 0; i < s.size(); i++) 
    {
        s_hex += int2hex((int)s[i]);
    }
    int s_l = s_hex.size() * 4;//�����Ƴ���
    s_hex = s_hex + "8";//��1
    while (s_hex.size() % 128 != 112) {//l+1+k=448mod512
        s_hex = s_hex + "0";//��0
    }
    string s_len = int2hex(s_l);//���ݳ���ʮ������
    while (s_len.size() != 16) //����Ϊ64bit
    {
        s_len = "0" + s_len;
    }
    s_hex = s_hex + s_len;
    return s_hex;
}
// ʮ�������ַ���תΪ int
int hex2int(char p) {
    if (p >= 48 && p < 58)
        return (int)p - 48;
    else if (p >= 65 && p < 91)
        return (int)p - 55;
    else
        return (int)p - 97;
}
//�ַ���תΪuint
uint32_t str2uint(string s) {
    uint32_t x = 0;
    for (auto i : s)
        x = ((x << 4) | hex2int(i));
    return x;
}

//ѹ������
void CF(string str,uint32_t *V)
{
    uint32_t SS1, SS2, TT1, TT2;
    uint32_t W[68], W_1[64];
    uint32_t A, B, C, D, E, F, G, H;

    //��Ϣ��չ
    for (int i = 0; i < 16; i++)
    {
        W[i] = str2uint(str.substr(8 * i, 8));
    }
    for (int i = 16; i < 68; i++)
    {
        W[i] = P_1(W[i - 16] ^ W[i - 9] ^ Left(W[i - 3], 15)) ^ Left(W[i - 13], 7) ^ W[i - 6];
    }
    for (int i = 0; i < 64; i++)
    {
        W_1[i] = W[i] ^ W[i + 4];
    }

    A = V[0];
    B = V[1];
    C = V[2];
    D = V[3];
    E = V[4];
    F = V[5];
    G = V[6];
    H = V[7];
  
    for (int j = 0; j < 64; j++)
    {
        SS1 = (Left((Left(A, 12) + E + Left(Ti(j), j)), 7));
        SS2= SS1 ^ Left(A, 12);
        TT1 = FF(A, B, C, j) + D + SS2 + W_1[j];
        TT2 = GG(E, F, G, j) + H + SS1 + W[j];
        D = C;
        C = Left(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = Left(F, 19);
        F = E;
        E = P_0(TT2);
    }
    V[0] = A ^ V[0];
    V[1] = B ^ V[1];
    V[2] = C ^ V[2];
    V[3] = D ^ V[3];
    V[4] = E ^ V[4];
    V[5] = F ^ V[5];
    V[6] = G ^ V[6];
    V[7] = H ^ V[7];

}

//uintתΪʮ�������ַ���
string uint2hex(uint32_t x, int k) {
    string hex = "";
    uint32_t temp = 0;
    for (int i = 0; i < k; i++)
    {
        temp = x % 16;
        if (temp >= 0 && temp < 10)
            hex = hex + to_string(temp);
        else
            hex = hex + char('A' + (temp - 10));
        x = x / 16;
    }
    return string(hex.rbegin(), hex.rend());
}

//����ʵ��
string SM3(string s)
{
    //IV=7380166f 4914b2b9 172442d7 da8a0600 a96f30bc 163138aa e38dee4d b0fb0e4e
    uint32_t V[8] = { 0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600, 0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E };
    int size = int(s.size()) * 4;//�����Ƴ���
    int n = (size + 1) % 512;
    int k;
    if (n < 448)
        k = 448 - n;
    else//���ز����Լ���64bit����
        k = 960 - n;
    s = padding(s);
    int num = (size + k + 65) / 512;//�ֿ�
    string B="";
    for (int i = 0; i < num; i++)
    {
        B = s.substr(i * 128, 128);
        CF(B,V);
    }
    string V_n = "";
    for (int i = 0; i < 8; i++)
    {
        V_n += uint2hex(V[i],8);
    }
    return V_n;
}

//ʮ������תΪ�ַ��������ã�
string hex2str(const string& hex)
{
    string str;
    for (size_t i = 0; i < hex.length(); i = i + 2)
    {
        string byte = hex.substr(i, 2);
        char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
        str.push_back(chr);
    }
    return str;
}
//�ַ���תΪʮ�����ƣ����ã�
string str2hex(string s)
{
    string ret;
    static const char* hex = "0123456789ABCDEF";
    for (auto i : s)
    {
        ret.push_back(hex[(i >> 4) & 0xf]);
        ret.push_back(hex[i & 0xf]);
    }
    return ret;
}

//��������Ϊʮ�������ַ���
string padding2(string s)
{
    int s_l = s.size() * 4;//�����Ƴ���
    s = s + "8";//��1
    while (s.size() % 128 != 112) {//l+1+k=448mod512
        s = s + "0";//��0
    }
    s = s + uint2hex(s_l,16);
    return s;
}
//����Ϊʮ�������ַ���
string SM3_2(string s) 
{
    uint32_t V[8] = { 0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600, 0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E };
    int size = int(s.size()) * 4;//�����Ƴ���
    int n = (size + 1) % 512;
    int k;
    if (n < 448)
        k = 448 - n;
    else//���ز����Լ���64bit����
        k = 960 - n;
    s = padding2(s);
    int num = (size + k + 65) / 512;//�ֿ�
    string B = "";
    for (int i = 0; i < num; i++)
    {
        B = s.substr(i * 128, 128);
        CF(B, V);
    }
    string V_n = "";
    for (int i = 0; i < 8; i++)
    {
        V_n += uint2hex(V[i], 8);
    }
    return V_n;
}
int main() {
    
    //������չ����
    string M1 = "abc";
    string M2 = SM3(M1);
    string M3 = str2hex("abc");
    string d = SM3_2(M2+M3);
    cout << d << endl;

    string a = "abc";
    uint32_t V[8] = { 0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600, 0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E };
    int size = int(a.size()) * 4;//�����Ƴ���
    int n = (size + 1) % 512;
    int k;
    if (n < 448)
        k = 448 - n;
    else//���ز����Լ���64bit����
        k = 960 - n;
    a = padding(a);//�����a��ʮ�����ƣ�
    //cout << a<< endl;
    int num = (size + k + 65) / 512;//�ֿ�
    string B = "";
    for (int i = 0; i < num; i++)
    {
        B = a.substr(i * 128, 128);
        CF(B, V);
    }
    //��ʱiv=V��SM3��a����
    string b="def";
    int size2 = int(b.size()) * 4;//�����Ƴ���
    int n2 = (size2 + 1) % 512;
    int k2;
    if (n2 < 448)
        k2 = 448 - n2;
    else//���ز����Լ���64bit����
        k2 = 960 - n2;
    b = padding(b);
    int num2 = (size2 + k2 + 65) / 512;//�ֿ�
    string B2 = "";
    for (int i = 0; i < num2; i++)
    {
        B2 = b.substr(i * 128, 128);
        CF(B2, V);
    }
    
    string V_n = "";
    for (int i = 0; i < 8; i++)
    {
        V_n += uint2hex(V[i], 8);
    }
    string c = a + "646566";
    //cout << c << endl;
    cout << "IV=SM3(a)ʱ��b���ܽ��Ϊ��"<< V_n << endl;
    cout <<"SM3(a+padding+b)="<< SM3_2(c) << endl;
}
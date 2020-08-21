/*
	12141565 안형준 AES 구현과제 
*/

#include<iostream>
#include<fstream>
#include <string>
#include<cmath>
#include<vector>
#include<bitset>

using namespace std;

bitset<8> RC[10];
bitset<8> sbox[16][16];
bitset<8> inversed_sbox[16][16];

//source 배열은 s-box를 만드는데 사용하는 고정배열
bitset<8> source[8];

//MCmat[1] 은 mix columns 시에 사용, MCmat[0] 은 inverse 행렬 
bitset<8> MCmat[2][4][4];

//Irreducible Polynomial
const bitset<18> IP("000000000101001101");

const char toHex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

bitset<8> plaintext[4][4];
bitset<8> key[11][4][4];


//Euclidean Algorithm. 다항식의 곱셈 후 Irreducible Polynomial 로 나누는데 사용
pair<bitset<18>, bitset<18>> EA(bitset<18> d1, bitset<18> d2 , int q) {
	
	/*if (d1.to_ulong() < d2.to_ulong())
		return { bitset<18>(0), d1 };*/

	int gap = 0;

	for (int i = 17; i >= 0; i--) {
		if (d1[i]) {
			for (int j = 17; j >= 0; j--) {
				if (d2[j]) {
					gap = i - j;
					break;
				}
			}
			break;
		}
	}	

	bitset<18> remainder = (d1 ^ (d2 << gap));

	if (remainder.to_ulong() >= d2.to_ulong())
		return EA(remainder, d2, q | (1<<gap));
	else 
		return { bitset<18>(q | (1 << gap)), remainder };
	
}

//Extended Euclidean Algorithm. 역원을 return.
bitset<8> EEA(bitset<8> bit) {
	bitset<18> newbit(0);

	for (int i = 0; i < 8; i++) 
		if (bit[i])	newbit.set(i);
	

	bitset<18> Q, A1(1), A2(0), A3(IP), B1(0), B2(1), B3(newbit);
	

	//나머지가 1일때까지 반복하는 loop
	while (B3 != bitset<18>("000000000000000001")) {
		pair<bitset<18>, bitset<18>> p = EA(A3, B3, 0);
		Q = p.first;
		bitset<18> nxtA1(B1), nxtA2(B2);
		A3 = B3;

		bitset<18> QxB1(0);
		for (int i = 0; i < 9; i++) {
			if (B1[i])
				QxB1 ^= (Q << i);
		}
		pair<bitset<18>, bitset<18>> QxB1_pair = EA(QxB1, IP, 0);

		bitset<18> QxB2(0);
		for (int i = 0; i < 9; i++) {
			if (B2[i])
				QxB2 ^= (Q << i);
		}
		pair<bitset<18>, bitset<18>> QxB2_pair = EA(QxB2, IP, 0);

		B1 = A1 ^ QxB1_pair.second;
		B2 = A2 ^ QxB2_pair.second;
		A1 = nxtA1;
		A2 = nxtA2;
		B3 = p.second;
	}

	bitset<8> ret(0);
	for (int i = 0; i < 8; i++) 
		if (B2[i]) ret.set(i);
	

	return ret;
}


void makeSbox() {

	int num = 0x00;

	//s-box 생성시 사용되는 상수 0x63 -> 0x15로 변경
	bitset<8> sbox_const(0x15);


	//s-box 초깃값은 00 01 02 .... FF 까지로 세팅
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			sbox[i][j] = bitset<8>(num);
			num++;
		}
	}

	//각 칸마다 s-box의 값을 계산함
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			bitset<8> inverse(0);

			//00은 곱셈에 대한 역원이 없으므로 그대로 00사용
			//나머지는 확장 유클리드 알고리즘으로 역원을 계산
			if (i != 0 || j != 0) 
				inverse = EEA(sbox[i][j]);
			
			bitset<8> sbox_value(0);

			//행렬곱을 계산하여 0x15상수와 더해 s-box의 값을 계산후 저장
			for (int k = 0; k < 8; k++) {
				int bit = 0;
				for (int l = 0; l < 8; l++) {
					bit ^= (source[k][7 - l] & inverse[l]);
				}
				bit ^= sbox_const[k];

				sbox_value |= (bit << k);
			}
			sbox[i][j] = sbox_value;
		}
	}

	////////////////////////inverse s-box 만들기///////////////////////////
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			bitset<8> left(0), right(0);
			left = (sbox[i][j] & bitset<8>(0xf0)) >> 4;
			right = (sbox[i][j] & bitset<8>(0x0f));

			bitset<8> row(i << 4),col(j);
			inversed_sbox[left.to_ulong()][right.to_ulong()] = (row | col);
		}
	}
}

void make_source_and_MCmat() {
	source[0] = bitset<8>("10001111");
	source[1] = bitset<8>("11000111");
	source[2] = bitset<8>("11100011");
	source[3] = bitset<8>("11110001");
	source[4] = bitset<8>("11111000");
	source[5] = bitset<8>("01111100");
	source[6] = bitset<8>("00111110");
	source[7] = bitset<8>("00011111");

	MCmat[1][0][0] = bitset<8>(0x02);
	MCmat[1][1][0] = bitset<8>(0x01);
	MCmat[1][2][0] = bitset<8>(0x01);
	MCmat[1][3][0] = bitset<8>(0x03);

	MCmat[1][0][1] = bitset<8>(0x03);
	MCmat[1][1][1] = bitset<8>(0x02);
	MCmat[1][2][1] = bitset<8>(0x01);
	MCmat[1][3][1] = bitset<8>(0x01);

	MCmat[1][0][2] = bitset<8>(0x01);
	MCmat[1][1][2] = bitset<8>(0x03);
	MCmat[1][2][2] = bitset<8>(0x02);
	MCmat[1][3][2] = bitset<8>(0x01);

	MCmat[1][0][3] = bitset<8>(0x01);
	MCmat[1][1][3] = bitset<8>(0x01);
	MCmat[1][2][3] = bitset<8>(0x03);
	MCmat[1][3][3] = bitset<8>(0x02);
	
	MCmat[0][0][0] = bitset<8>(0x0E);
	MCmat[0][1][0] = bitset<8>(0x09);
	MCmat[0][2][0] = bitset<8>(0x0D);
	MCmat[0][3][0] = bitset<8>(0x0B);

	MCmat[0][0][1] = bitset<8>(0x0B);
	MCmat[0][1][1] = bitset<8>(0x0E);
	MCmat[0][2][1] = bitset<8>(0x09);
	MCmat[0][3][1] = bitset<8>(0x0D);

	MCmat[0][0][2] = bitset<8>(0x0D);
	MCmat[0][1][2] = bitset<8>(0x0B);
	MCmat[0][2][2] = bitset<8>(0x0E);
	MCmat[0][3][2] = bitset<8>(0x09);

	MCmat[0][0][3] = bitset<8>(0x09);
	MCmat[0][1][3] = bitset<8>(0x0D);
	MCmat[0][2][3] = bitset<8>(0x0B);
	MCmat[0][3][3] = bitset<8>(0x0E);


}

//RC배열을 만드는 함수
void makeRC() {
	int num = 1;
	for (int i = 0; i < 8; i++) {
		RC[i] = bitset<8>(num);
		num *= 2;
	}
	RC[8] = bitset<8>(0x4D);
	RC[9] = bitset<8>(0x9A);
}

//round 변수에 해당하는 라운드의 key 값을 hex로 print
void showkey(int round) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			bitset<8> left(0), right(0);
			left = ((key[round][j][i] & bitset<8>(0xf0)) >> 4);
			right = (key[round][j][i] & bitset<8>(0x0f));
			cout << toHex[left.to_ulong()] << toHex[right.to_ulong()] << " ";
		}
	}
	cout << "\n";
}

//plaintext를 print
void showplaintext() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			bitset<8> left(0), right(0);
			left = ((plaintext[j][i] & bitset<8>(0xf0)) >> 4);
			right = (plaintext[j][i] & bitset<8>(0x0f));
			cout << toHex[left.to_ulong()] << toHex[right.to_ulong()] << " ";
		}
	}
	cout << "\n";
}

//key expansion 시에 사용되는 R함수
vector<bitset<8>> Rfunction(int round) {
	vector<bitset<8>> v;
	for (int i = 0; i < 4; i++) {
		int r = 0, c = 0;
		if (i != 3) {
			r = ((key[round][i + 1][3] & bitset<8>(0xf0)) >> 4).to_ulong();
			c = (key[round][i + 1][3] & bitset<8>(0x0f)).to_ulong();
		}
		else {
			r = ((key[round][0][3] & bitset<8>(0xf0)) >> 4).to_ulong();
			c = (key[round][0][3] & bitset<8>(0x0f)).to_ulong();
		}

		bitset<8> sbox_val = sbox[r][c];

		if (i == 0)
			v.push_back(RC[round] ^ sbox_val);
		else 
			v.push_back(sbox_val);
		
	}
	return v;	
}


//key Expansion
void keyExpansion() {
	cout << "KEY EXPANSION" << "\n";

	cout << "ROUND 0: ";
	showkey(0);

	for (int round = 1; round <= 10; round++) {
		vector<bitset<8>> v = Rfunction(round - 1);
		
		for (int i = 0; i < 4; i++) 
			key[round][i][0] = key[round - 1][i][0] ^ v[i];
		
		for (int i = 1; i < 4; i++) 
			for (int j = 0; j < 4; j++) 
				key[round][j][i] = key[round - 1][j][i] ^ key[round][j][i - 1];
		cout << "ROUND " << round << ": ";
		showkey(round);
	}
}

bitset<8> toBit(string s) {
	int s0 = 0;
	int s1 = 0;

	if (s[0] >= 'A') 	s0 = s[0] - 'A' + 10;
	else	s0 = s[0] - '0';
	s0 *= 16;

	if (s[1] >= 'A')	s1 = s[1] - 'A' + 10;
	else	s1 = s[1] - '0'; 
	
	bitset<8> val(s0 + s1);

	return val;
}

// 파일을 읽어 plain text와 key를 저장 
void readfile() {
	ifstream in("plain.bin");

	int plain_r = 0;
	int plain_c = 0;

	while (!in.eof()) {
		string s;
		in >> s;

		plaintext[plain_r][plain_c] = toBit(s);
		plain_r++;
		if (plain_r == 4) {
			plain_r = 0;
			plain_c++;
		}
	}

	in.close();

	ifstream readkey("key.bin");

	int key_r = 0;
	int key_c = 0;

	while (!readkey.eof()) {
		string s;
		readkey >> s;
		
		 key[0][key_r][key_c]= toBit(s);
		 
		 key_r++;
		 if (key_r == 4) {
			 key_r = 0;
			 key_c++;
		 }
	}

	readkey.close();
}

//암호화 및 복호화 결과 파일로 저장.
void savefile(bool enc) {
	if (enc) {
		ofstream out("cipher.bin", 'w');

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				bitset<8> left = ((plaintext[j][i] & bitset<8>(0xf0)) >> 4);
				bitset<8> right = (plaintext[j][i] & bitset<8>(0x0f));
				out << toHex[left.to_ulong()] << toHex[right.to_ulong()] << " ";
			}
		}
		out.close();
	}
	else {
		ofstream out("decrypt.bin", 'w');

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				bitset<8> left = ((plaintext[j][i] & bitset<8>(0xf0)) >> 4);
				bitset<8> right = (plaintext[j][i] & bitset<8>(0x0f));
				out << toHex[left.to_ulong()] << toHex[right.to_ulong()] << " ";
			}
		}
		out.close();
	}
}
//substitute bytes
void substituteBytes(bool enc) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			bitset<8> row(0), col(0);
			row = ((plaintext[i][j] & bitset<8>(0xf0)) >> 4);
			col = (plaintext[i][j] & bitset<8>(0x0f));
			if (enc)
				plaintext[i][j] = sbox[row.to_ulong()][col.to_ulong()];
			else
				plaintext[i][j] = inversed_sbox[row.to_ulong()][col.to_ulong()];
		}
	}

	cout << "SB: ";
	showplaintext();
}

//shift rows
void shiftRows(bool enc) {
	if (enc) { 
		//ENCRYPTION 
		for (int row = 1; row < 4; row++) {
			vector<bitset<8>> v;
			for (int i = 0; i < row; i++) {
				v.push_back(plaintext[row][i]);
			}
			for (int i = 0; i < 4 - v.size(); i++) {
				plaintext[row][i] = plaintext[row][i + v.size()];
			}
			for (int i = 4 - v.size(); i < 4; i++) {
				plaintext[row][i] = v[i - 4 + v.size()];
			}
		}
	}
	else {
		//DECRYPTION
		for (int row = 1; row < 4; row++) {
			vector<bitset<8>> v;
			for (int i = 0; i < row; i++) {
				v.push_back(plaintext[row][3 - i]);
			}
			for (int i = 0; i < 4 - v.size(); i++) {
				plaintext[row][3 - i] = plaintext[row][3 - i - v.size()];
			}
			for (int i = 0; i < v.size(); i++) {
				plaintext[row][i] = v[v.size()- i - 1];
			}
		}
	}
	cout << "SR: ";
	showplaintext();
}


//Mix Columns
void mixColumns(bool enc) {
	bitset<8> inverted[4][4];
	for (int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {

			//행렬곱. shift와 xor로 구현
			for (int k = 0; k < 4; k++) {
				bitset<18> shift_result(0);
				bitset<18> newplaintext(0);
				for (int l = 0; l < 8; l++) 
					if (plaintext[k][j][l]) newplaintext.set(l);
				
				if (enc) {
					//암호화하는 경우
					if ((MCmat[enc][i][k] & bitset<8>(0x01))[0]) 
						shift_result ^= bitset<18>((newplaintext << 0));
					
					if ((MCmat[enc][i][k] & bitset<8>(0x02))[1]) 
						shift_result ^= bitset<18>((newplaintext << 1));
				}
				else {
					//복호화 하는 경우
					int num = 0x01;
					for (int l = 0; l < 4; l++) {
						if ((MCmat[enc][i][k] & bitset<8>(num))[l]) 
							shift_result ^= bitset<18>((newplaintext << l));
						num <<= 1;
					}
				}
				bitset<18> remainder(0);
				for (int l = 0; l < 18; l++) {
					if (shift_result[l]) {
						pair<bitset<18>, bitset<18>> p = EA(bitset<18>(1 << l), IP, 0);
						remainder ^= p.second;
					}
				}
				
				//XOR한 결과가 Irreducible polynomial보다 클 수 있으므로 한번 더 나눔
				pair<bitset<18>, bitset<18>> pp = EA(remainder, IP, 0);
				for (int l = 0; l < 8; l++) {
					if (pp.second[l])	inverted[i][j] ^= (1 << l);
				}
			}
		}
	}

	for (int i = 0; i < 4; i++) 
		for (int j = 0; j < 4; j++)
			plaintext[i][j] = inverted[i][j];
	
	cout << "MC: ";
	showplaintext();
}

//Add roundkey
void addRoundKey(int round) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			plaintext[i][j] = plaintext[i][j] ^ key[round][i][j];
		}
	}
	cout << "AR: ";
	showplaintext();
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(NULL);

	//key.bin과 plaintext.bin을 읽음
	readfile();	

	//RC배열 값과 SBOX를 만들기 위한 행렬, Mix Columns를 위한 행렬 값 생성
	makeRC();
	make_source_and_MCmat();

	//sbox 생성 및 inversed sbox 생성
	makeSbox();

	cout << "RC: ";
	for (int i = 0; i < 10; i++) {
		bitset<8> left(0), right(0);
		left = ((RC[i] & bitset<8>(0xf0)) >> 4);
		right = (RC[i] & bitset<8>(0x0f));
		cout << toHex[left.to_ulong()] << toHex[right.to_ulong()]<<" ";
	}

	cout << "\nPLAIN: ";
	showplaintext();
	
	cout << "KEY: ";
	showkey(0);

	//////////////////////////////////////////////////////////////////////
	//							ENCRYPTION								//
	//////////////////////////////////////////////////////////////////////

	cout << "\n<------ ENCRYPTION ------>\n\n";
	keyExpansion();

	cout << "\n" << "ROUND 0" << "\n";
	addRoundKey(0);

	for (int round = 1; round <= 9; round++) {
		cout<< "\nROUND " << round << "\n";
		substituteBytes(true);
		shiftRows(true);
		mixColumns(true);
		addRoundKey(round);		
	}
	cout << "\nROUND 10\n";
	substituteBytes(true);
	shiftRows(true);
	addRoundKey(10);


	//암호화 결과 출력
	cout << "\nCIPHER: ";
	showplaintext();
	cout << "\n\n";

	//암호화 결과 파일로 저장
	savefile(true);
	
	//////////////////////////////////////////////////////////////////////
	//							DECRYPTION								//
	//////////////////////////////////////////////////////////////////////
	cout << "\n<------ DECRYPTION ------>\n\n";

	cout << "\nROUND 0\n";
	addRoundKey(10);

	for (int round = 1; round < 10; round++) {
		cout << "\nROUND " << round << "\n";
		shiftRows(false);
		substituteBytes(false);
		addRoundKey(10 - round);
		mixColumns(false);
	}

	cout << "\nROUND 10\n";
	shiftRows(false);
	substituteBytes(false);
	addRoundKey(0);

	cout << "\nDECRYPTED: ";
	showplaintext();

	savefile(false);

	return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <bitset>

using namespace std;

vector<string> KEY_POOL;

class KeyExpansion
{
    public: 
    
    string key;

    string pc1Output;
    string pc2Output; // final round key

    string leftBits; // left 28-bits of PC1 output
    string rightBits; // right 28-bits of PC1 output
    string leftCLS; // circular-left shift on left bits
    string rightCLS; // circular-left shift on right bits

    const array<int, 16> CLS_PER_ROUND = 
    {
        1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
    };

    KeyExpansion(string k)
    {
        this->key = k;
    }

    // PC-1 table for key expansion in DES
    const int PC1_Table[56] = 
    {
        57, 49, 41, 33, 25, 17, 9,
        1, 58, 50, 42, 34, 26, 18,
        10, 2, 59, 51, 43, 35, 27,
        19, 11, 3, 60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15,
        7, 62, 54, 46, 38, 30, 22,
        14, 6, 61, 53, 45, 37, 29,
        21, 13, 5, 28, 20, 12, 4
    };

    // PC-2 table for key schedule in DES
    const int PC2_Table[48] = 
    {
        14, 17, 11, 24, 1, 5,
        3, 28, 15, 6, 21, 10,
        23, 19, 12, 4, 26, 8,
        16, 7, 27, 20, 13, 2,
        41, 52, 31, 37, 47, 55,
        30, 40, 51, 45, 33, 48,
        44, 49, 39, 56, 34, 53,
        46, 42, 50, 36, 29, 32
    };


    void MapToPC_1()
    {
        for (int i = 0; i < 56; i++)
        {
            pc1Output += key[PC1_Table[i]];
        }
    }

    void SplitPC1()
    {
        for (int i = 0; i < 56; i++)
        {
            if (i < 28)
            {
                leftBits += pc1Output[i];
            }
            else{
                rightBits += pc1Output[i];
            }
        }
    }

    void ApplyCLS_Left(int round)
    {
        int shiftAmount = CLS_PER_ROUND[round];

        for (int i = 0; i < leftBits.length(); i++)
        {
            leftCLS += leftBits[(i + shiftAmount) % leftBits.length()];
        }
    }

    void ApplyCLS_Right(int round)
    {
        int shiftAmount = CLS_PER_ROUND[round];

        for (int i = 0; i < rightBits.length(); i++)
        {
            rightCLS += rightBits[(i + shiftAmount) % rightBits.length()];
        }
    }

    void MapToPC2()
    {
        string mergedCLS;
        mergedCLS += leftCLS;
        mergedCLS += rightCLS;

        for (int i = 0; i < 48; i++)
        {
            pc2Output += mergedCLS[PC2_Table[i]];
        }
        KEY_POOL.push_back(pc2Output);
    }

};

class Encryption
{
    public:

    Encryption() {}
};

class DES
{
    public: string plainText; string key;

    // vector<array<string, 8>> INPUT_PACKETS_BITS;
    vector<string> INPUT_PACKET_BITS;

    DES() {}

    DES(string pt, string key)
    {
        this->plainText = pt;
        
        if (key.length() == 8)
        {
            this->key = key;
        }
        else
        {
            cout << "Error: Invalid key size" << endl;
            cout << "Please make key is 8 characters long" << endl;
            return;
        }
    }

    void CreateInputPackets()
    {
        int plainTextIndex = 0;

        while (plainTextIndex < plainText.length())
        {
            // array<string, 8> packet;
            string packet;

            for (int i = 0; i < 8; i++)
            {
                if (plainTextIndex < plainText.length())
                {
                    int asciiPT = static_cast<int>(plainText[plainTextIndex++]);
                    // packet[i] = bitset<8>(asciiPT).to_string();
                    packet += bitset<8>(asciiPT).to_string();
                }
                else
                {
                    int asciiPad = static_cast<int>(' '); // using space as padding
                    // packet[i] = bitset<8>(asciiPad).to_string();
                    packet += bitset<8>(asciiPad).to_string();
                }
                // cout << " | " << packet[i];
            }
            // cout << endl;
            INPUT_PACKET_BITS.push_back(packet);
        }
    }

};

int main()
{
    DES algorithm("habibkhan", "hahahaha");

    algorithm.CreateInputPackets();
}
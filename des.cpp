#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <bitset>

using namespace std;

array<string, 16> KEY_POOL;

class KeyExpansion
{
    public:
    
    string key;
    string pc1Output;
    string leftBits; // left 28-bits of PC1 output
    string rightBits; // right 28-bits of PC1 output

    const array<int, 16> CLS_PER_ROUND = 
    {
        1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
    };

    KeyExpansion()
    {
        this->key = "0001001100110100010101110111100110011011101111001101111111110001";
    }

    KeyExpansion(string k)
    {
        for (int i = 0; i < k.length(); i++)
        {
            int asciiKey = static_cast<int>(k[i]);
            this->key += bitset<8>(asciiKey).to_string();
        }
        cout << "Original Key: " << key << endl;
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

    void MapToPC1()
    {
        for (int i = 0; i < 56; i++)
        {
            pc1Output += key[PC1_Table[i] - 1]; // since DES works on 1 index
        }
        // cout << "PC1: " << pc1Output << endl;
    }

    void SplitPC1()
    {
        for (int i = 0; i < 56; i++)
        {
            if (i < 28)
            {
                leftBits += pc1Output[i];
            }
            else
            {
                rightBits += pc1Output[i];
            }
        }
    }

    void ApplyCLS_Left(int round)
    {
        string leftCLS;
        int shiftAmount = CLS_PER_ROUND[round];

        for (int i = 0; i < leftBits.length(); i++)
        {
            leftCLS += leftBits[(i + shiftAmount) % leftBits.length()];
        }
        leftBits = leftCLS;
        // cout << "CLS Left: " << leftBits << endl;
    }

    void ApplyCLS_Right(int round)
    {
        string rightCLS;
        int shiftAmount = CLS_PER_ROUND[round];

        for (int i = 0; i < rightBits.length(); i++)
        {
            rightCLS += rightBits[(i + shiftAmount) % rightBits.length()];
        }
        rightBits = rightCLS;
        // cout << "CLS Right: " << rightBits << endl;
    }

    void MapToPC2(int round)
    {
        string mergedCLS;
        mergedCLS += leftBits;
        mergedCLS += rightBits;

        string pc2Output;

        for (int i = 0; i < 48; i++)
        {
            pc2Output += mergedCLS[PC2_Table[i] - 1];
        }
        KEY_POOL[round] = pc2Output;
    }

    void ExpandKey()
    {
        MapToPC1();
        SplitPC1();

        for (int i = 0; i < 16; i++) // 16 rounds in DES
        {
            ApplyCLS_Left(i);
            ApplyCLS_Right(i);
            MapToPC2(i);
        }
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
            cout << "Please make sure key is 8 characters long" << endl;
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
    // DES algorithm("habibkhan", "hahahaha");

    // algorithm.CreateInputPackets();

    KeyExpansion Key("habibkha");
    
    // KeyExpansion Key;
    Key.ExpandKey();

    for (int i = 0; i < 16; i++)
    {
        // for (int j = 0; j < KEY_POOL[i].length(); j++)
        // {

        // }
        cout << "K" << i << " : " << KEY_POOL[i] << endl;
    }
}
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <bitset>
#include <sstream>

using namespace std;

array<string, 16> KEY_POOL;
array<string, 16> INV_KEY_POOL;

string binaryXOR(const string& a, const string& b) 
{
    // Ensure both strings have the same length
    if (a.length() != b.length()) {
        cerr << "Binary strings must have the same length." << endl;
        return "";
    }
    // Initialize result string
    string result = "";

    // Perform XOR operation on corresponding bits
    for (size_t i = 0; i < a.length(); ++i) {
        result += (a[i] == b[i]) ? '0' : '1';
    }
    return result;
}

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
        // test key
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

class FeistelStructure
{
    public:

    bool enc_dec_flag;
    string input_64_bit;
    string IP;

    // Initial Permutation (IP) table for DES
    const int IP_Table[64] = 
    {
        58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7
    };

    // Inverse Initial Permutation (IP^-1) table for DES
    const int Inv_IP_Table[64] = 
    {
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25
    };


    // E-Table for expansion permutation in DES
    const int E_Table[48] = 
    {
        32, 1, 2, 3, 4, 5,
        4, 5, 6, 7, 8, 9,
        8, 9, 10, 11, 12, 13,
        12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21,
        20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29,
        28, 29, 30, 31, 32, 1
    };

    // P-Table for final permutation in DES
    const int P_Table[32] = 
    {
        16, 7, 20, 21, 29, 12, 28, 17,
        1, 15, 23, 26, 5, 18, 31, 10,
        2, 8, 24, 14, 32, 27, 3, 9,
        19, 13, 30, 6, 22, 11, 4, 25
    };

    // S-boxes for DES
    const int S_Boxes[8][4][16] = 
    {
    // S1
        {
            {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
            {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
            {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
            {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
        },

        // S2
        {
            {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
            {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
            {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
            {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
        },

        // S3
        {
            {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
            {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
            {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
            {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
        },

        // S4
        {
            {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
            {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
            {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
            {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
        },

        // S5
        {
            {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
            {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
            {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
            {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
        },

        // S6
        {
            {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
            {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
            {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
            {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
        },

        // S7
        {
            {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
            {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
            {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
            {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
        },

        // S8
        {
            {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
            {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
            {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
            {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
        }
    };

    FeistelStructure(string input, bool flag)
    {
        if (flag == 0)
        {
            for (int i = 0; i < input.length(); i++)
            {
                int asciiInput = static_cast<int>(input[i]);
                this->input_64_bit += bitset<8>(asciiInput).to_string();
            }
        }
        else if (flag == 1)
        {
            this->input_64_bit = input;
        }
        cout << "Original Input: " << input_64_bit << endl;
    }

    void MapToIP()
    {
        for (int i = 0; i < 64; i++)
        {
            IP += input_64_bit[IP_Table[i] - 1];
        }
    }

    string MapToExpTable(string rb)
    {
        string expansion;

        for (int i = 0; i < 48; i++)
        {
            expansion += rb[E_Table[i] - 1];
        }
        return expansion;
    }

    string AddKey(int round, bool flag, string exp)
    {
        string key;

        if (flag == 0)
        {
            key = KEY_POOL[round];
        }
        else if (flag == 1)
        {
            key = INV_KEY_POOL[round];
        }

        return binaryXOR(key, exp);
    }

    string ComputeSBOX(string keyXOR)
    {
        string sBoxesOutput;

        for (int i = 0; i < 8; i++)
        {
            string row_binary;
            string col_binary;
            string subString_6_bits = keyXOR.substr(i* 6, 6);

            // first and last bit determines row
            row_binary += subString_6_bits[0];
            row_binary += subString_6_bits[5];
            // middle 4 bits for col
            col_binary = subString_6_bits.substr(1, 4);
            // convert to decimal
            unsigned long row_decimal = bitset<2>(row_binary).to_ulong();
            unsigned long col_decimal = bitset<8>(col_binary).to_ulong();

            int sBoxOutputDecimal = S_Boxes[i][row_decimal][col_decimal];
            string sBoxOutput = bitset<4>(sBoxOutputDecimal).to_string();
            sBoxesOutput += sBoxOutput; // add 4-bit SBox output to the combined result var
        }
        return sBoxesOutput;
    }

    string MapToPermTable(string sBoxesOutput)
    {
        string permutedBits;

        for (int i = 0; i < 32; i++)
        {
            permutedBits += sBoxesOutput[P_Table[i] - 1];
        }
        return permutedBits;
    }

    string MapToInvIP(string mergedBits)
    {
        string invIP;

        for (int i = 0; i < 64; i++)
        {
            invIP += mergedBits[Inv_IP_Table[i] - 1];
        }
        return invIP;
    }

    string algorithm(bool flag)
    {
        // flag => 0 for encryption | 1 for decryption

        MapToIP();

        // initial split
        string leftBits;
        string rightBits;

        for (int i = 0; i < 64; i++)
        {
            if (i < 32)
            {
                leftBits += IP[i];
            }
            else
            {
                rightBits += IP[i];
            }
        }

        for (int round = 0; round < 16; round++)
        {
            // cout << "Round " << round << " Right Bits: " << rightBits << endl;
            // cout << "Round " << round << " Left Bits: " << leftBits << endl;

            string expansion = MapToExpTable(rightBits);
            string keyXOR = AddKey(round, flag, expansion);
            string sBoxesOutput = ComputeSBOX(keyXOR);
            string permutedBits = MapToPermTable(sBoxesOutput);

            string newRightBits = binaryXOR(permutedBits, leftBits);
            leftBits = rightBits; // swap first
            rightBits = newRightBits;
        }

        // append into main string right bits to swap
        string mergedBits;
        mergedBits += rightBits;
        mergedBits += leftBits;

        string cipherTextBinary = MapToInvIP(mergedBits);
        bitset<64> set(cipherTextBinary);
        stringstream res;
        res << hex << uppercase << set.to_ullong();
        cout << "Cipher Text Hex: " << res.str() << endl;

        return cipherTextBinary;
    }

};

class DES
{
    public:

    string key;
    string plainText;
    string cipherText;
    string decryptedText;

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
            string packet;

            for (int i = 0; i < 8; i++)
            {
                if (plainTextIndex < plainText.length())
                {
                    int asciiPT = static_cast<int>(plainText[plainTextIndex++]);
                    packet += bitset<8>(asciiPT).to_string();
                }
                else
                {
                    int asciiPad = static_cast<int>(' '); // using space as padding
                    packet += bitset<8>(asciiPad).to_string();
                }
            }
            INPUT_PACKET_BITS.push_back(packet);
        }
    }

    void GenerateKeys()
    {
        KeyExpansion Key(key);
        Key.ExpandKey();

        // populate the inverse key pool for decryption
        int index = 15;
        
        for (int i = 0; i < 16; i++)
        {
            INV_KEY_POOL[i] = KEY_POOL[index--];
        }
    }

};

int main()
{
    DES Cipher("habibkha", "habibkha");
    Cipher.CreateInputPackets();
    Cipher.GenerateKeys();

    // KeyExpansion Key("habibkha");
    // Key.ExpandKey();
    FeistelStructure Encrypt("habibkha", 0);
    string cipherText = Encrypt.algorithm(0);
    FeistelStructure Decrypt(cipherText, 1);
    Decrypt.algorithm(1);

}
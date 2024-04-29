#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <bitset>

using namespace std;

class KeyExpansion
{

};

class Encryption
{
    public:

    Encryption() {}
};

class DES
{
    public: string plainText; string key;

    vector<array<string, 8>> INPUT_PACKETS_BITS;

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

    void createInputPackets()
    {
        int plainTextIndex = 0;

        while (plainTextIndex < plainText.length())
        {
            array<string, 8> packet;

            for (int i = 0; i < 8; i++)
            {
                if (plainTextIndex < plainText.length())
                {
                    int asciiPT = static_cast<int>(plainText[plainTextIndex++]);
                    packet[i] = bitset<8>(asciiPT).to_string();
                }
                else
                {
                    int asciiPad = static_cast<int>(' '); // using space as padding
                    packet[i] = bitset<8>(asciiPad).to_string();
                }
                // cout << " | " << packet[i];
            }
            // cout << endl;
            INPUT_PACKETS_BITS.push_back(packet);
        }
    }

};

int main()
{
    DES algorithm("habibkhan", "hahahaha");

    algorithm.createInputPackets();
}
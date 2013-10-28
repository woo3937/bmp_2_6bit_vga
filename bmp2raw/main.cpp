//
//  main.cpp
//  bmp2raw
//
//  Created by nicholas jackson on 5/10/13.
//  Copyright (c) 2013 nicholas jackson. All rights reserved.
//

// assuming bmp is encoded under 24 bit RGB Windows with 54 bit header 

#include <iostream>
#include <fstream>
#include <cstdio>
#include <iomanip>
#include <vector>
#include <string>

using namespace std;

class bmp2bin
{
public:
    
    // Constructor
    bmp2bin(){};
    
    // Desctructor
    ~bmp2bin(){};
    
    bool init(const string & input_image = "", const string & output_binary = "");
    
    // convert image data
    bool convert();
    
private:
    
    // check image header (54-bit header)
    bool check_header(unsigned char header[]);
    bool prepare_input_image();
    bool prepare_ouput_binary();
    
    unsigned char m_header[54];
    
    string input_image_filename;
    string output_binary_filename;
    
    ifstream input_image;
    ofstream output_binary;
    
    vector< vector<unsigned char> > m_image_data;
    
};

bool bmp2bin::init(const string & input_image, const string & output_binary)
{
    string output;
    
    if(input_image == "")
    {
        cout << "Error, empty image input." << endl;
        return false;
    }
    
    input_image_filename = input_image;
    
    if(output_binary == "")
    {
        output_binary_filename = "out.bin";
    }
    else
    {
        output_binary_filename = output_binary;
    }
    
    return true;
}

bool bmp2bin::prepare_input_image()
{
    input_image.open(input_image_filename);
    
    if(!input_image.is_open())
        return false;
    
    return true;
}

bool bmp2bin::prepare_ouput_binary()
{
    output_binary.open(output_binary_filename, ios::out | ios::binary);
    
    if(!output_binary.is_open())
        return false;
    
    return true;
}

bool bmp2bin::check_header(unsigned char header[])
{
    unsigned short signature = (header[0] << 8) + header[1];
    
    if(signature != 0x424D)
    {
        cout << "Mismatched signature " << "0x" << hex << signature << "." << endl;
        return false;
    }
    
    if(header[14] != 40 || header[10] != 54)
    {
        cout << "Header size does not match." << endl;
        return false;
    }
    
    if(header[26] != 1)
    {
        cout << "Error in image planes." << endl;
        return false;
    }
    
    if(header[30] != 0)
    {
        cout << "Wrong Image Compression." << endl;
        return false;
    }
    
    return true;
}


bool bmp2bin::convert()
{
    
    if(!prepare_input_image())
    {
        cout << "Cound not open " << input_image_filename << "." << endl;
        return false;
    }
    
    unsigned char header[54] = {0};
    
    for(size_t i = 0; i < 54 && !input_image.eof(); ++i)
    {
        header[i] = input_image.get();
    }
    
    if(!check_header(header))
        return false;
    
    size_t x = header[18] + (header[19] << 8) + (header[20] << 16) + (header[21] << 24);
    size_t y = header[22] + (header[23] << 8) + (header[24] << 16) + (header[25] << 24);
    
    if(!prepare_ouput_binary())
    {
        cout << "Could not open " << output_binary_filename << "." << endl;
        return false;
    }
    
    for(size_t i = 0; i < y; i++)
    {
        vector<unsigned char> temp;
        for(size_t j = 0; j < x; j++)
        {
            temp.push_back((unsigned char)(((unsigned char)input_image.get() & 0xC0) >> 6) + (((unsigned char)input_image.get() & 0xC0) >> 4) + (((unsigned char)input_image.get() & 0xC0) >> 2));
        }
        m_image_data.push_back(temp);
        temp.clear();
    }
    
    reverse(m_image_data.begin(), m_image_data.end());
    
    for(size_t i = 0; i < m_image_data.size(); i++)
    {
        for(size_t j = 0; j < m_image_data[i].size(); j++)
        {
            output_binary << m_image_data[i][j];
        }
    }
    
    input_image.close();
    output_binary.close();
    
    return true;
}

int main(int argc, const char * argv[])
{
    if(argc < 3)
    {
        cout << "Invalid Arguments bmp2raw <image filename> <output binary file>." << endl;
        return 1;
    }
    
    bmp2bin data;
    
    data.init(string(argv[1]), string(argv[2]));
    
    data.convert();
    
    return 0;
}


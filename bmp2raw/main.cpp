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

using namespace std;

bool check_header(unsigned char header[]);

ifstream in;

int main(int argc, const char * argv[])
{
    if(argc < 2)
    {
        cout << "Invalid Arguments bmp2raw <image filename>." << endl;
        return 1;
    }
    
    in.open(argv[1]);
    
    if(!in.is_open())
    {
        cout << "Cound not open " << argv[1] << "." << endl;
        return 1;
    }
    
    unsigned char header[54] = {0};
    
    for(size_t i = 0; i < 54 && !in.eof(); ++i)
    {
        header[i] = in.get();
    }
    
    if(!check_header(header))
        return false;
    
    size_t x = header[18] + (header[19] << 8) + (header[20] << 16) + (header[21] << 24);
    size_t y = header[22] + (header[23] << 8) + (header[24] << 16) + (header[25] << 24);
    
    if(argc < 3)
    {
        cout << "Specify output file. bmp2ram <image filename> <output filename>" << endl;
        return 1;
    }
    
    ofstream out(argv[2], ios::out | ios::binary);
    
    vector< vector<unsigned char> > m_data;
    
    for(size_t i = 0; i < y; i++)
    {
        vector<unsigned char> temp;
        for(size_t j = 0; j < x; j++)
        {
            temp.push_back((unsigned char)(((unsigned char)in.get() & 0xC0) >> 6) + (((unsigned char)in.get() & 0xC0) >> 4) + (((unsigned char)in.get() & 0xC0) >> 2));
        }
        m_data.push_back(temp);
        temp.clear();
    }
    
    reverse(m_data.begin(), m_data.end());
    
    for(size_t i = 0; i < m_data.size(); i++)
    {
        for(size_t j = 0; j < m_data[i].size(); j++)
        {
            out << m_data[i][j];
        }
    }

    
    return 0;
}

bool check_header(unsigned char header[])
{
    unsigned short signature = 0;
    
    signature = (header[0] << 8) + header[1];
    
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


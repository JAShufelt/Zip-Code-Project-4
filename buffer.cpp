#include "buffer.h"
#include <cassert>
#include <fstream>
#include <stdlib.h>
#include "indexRecord.h"
using namespace std;

int Buffer::read()
{
    if(!reader.eof())
    {
        getline(reader, buffer_storage);
        return 1;
    }
    else
    {
        return 0;
    }
};

void Buffer::unpack(string data)
{
    char COMMA = ',';
    string recordData;
    
    int recordLength;
    string state;
    double lat;
    double lon;
    int zip;
    string place;
    string county;
    int fieldOrderPointer = 0;

    int record_count = stoi(data.substr(0, 2)); //First two bytes is the record count in the block
    int temp;
    int field = 0;
    string record_string;
    string partial_field = "";
    data.erase(0, 15);                          //Skip to the first block

    for(int i = 0; i < record_count; i++)
    {
        temp = stoi(data.substr(0, 4));
        record_string = data.substr(0, temp + 1);

        for(int i = 0; i < record_string.size(); i++)
        {
            if(record_string[i] != COMMA)
            {
                partial_field = partial_field + record_string[i];
            }
            
            else
            {
                if (field == 0)
                {
                    recordLength = stoi(partial_field);
                    field++;
                }
                else if (field == 1)
                {
                    zip = stoi(partial_field);
                    field++;
                }
                else if (field == 2)
                {
                    place = partial_field;
                    field++;
                }
                else if (field == 3)
                {
                    state = partial_field;
                    field++;
                }
                else if (field == 4)
                {
                    county = partial_field;
                    field++;
                }
                else if (field == 5)
                {
                    lat = stod(partial_field);
                    field++;
                }
                else if (field == 6)
                {
                    lon = stod(partial_field);
                    field++;
                }
                partial_field = "";
            }
        }
        Record newRecord = Record(recordLength,state,lat,lon,zip,place,county);
        recordVector.push_back(newRecord);
        data.erase(0, (temp + 1));
        field = 0;
    }
}

vector<indexRecord> Buffer::constructIndex(headerBuffer &headerbuff)
{
    cout << "\nConstructing index... " << endl;
    
    vector<indexRecord> indexVector;
    int currentZipCode;
    int byteOffset;
    
    int beginning_of_Record = headerbuff.header_record_Size + 1;    //beginning of the first record is 1 char after the header due to comma

    for (int i = 0; i < recordVector.size(); i++) 
    {
        currentZipCode = recordVector[i].zipCode;
        byteOffset = beginning_of_Record;

        indexRecord newRecord = indexRecord(currentZipCode, byteOffset);
        indexVector.push_back(newRecord);

        if (i < recordVector.size() - 1)
        {
            beginning_of_Record = beginning_of_Record + recordVector[i].lengthIndicator + 1;
        }
    }
    return indexVector;
};

void Buffer::writeIndex(headerBuffer headerbuff, vector<indexRecord> indexVector)
{
    string line = "";
    ofstream out;
    out.open(headerbuff.index_file_Name);
    
    for (int i = 0; i <headerbuff.fieldOrder.size(); i++)
    {
        if (i < headerbuff.fieldOrder.size() - 1)
            line = line + headerbuff.fieldOrder[i] + ",";
        else
            line = line + headerbuff.fieldOrder[i];
    }
    
    out << line << endl;
    line = "";
    
    for (int i = 0; i < indexVector.size(); i++)
    {
        line = to_string(indexVector[i].zipCode) + "," + to_string(indexVector[i].referenceField);
        out << line << endl;
    }
    
    cout << "Index created in file: " << headerbuff.index_file_Name << endl;
    out.close();
}
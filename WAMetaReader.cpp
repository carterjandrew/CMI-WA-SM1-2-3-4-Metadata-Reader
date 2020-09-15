
//These are all of the imports that this project needs to run. They are all incuded by default in the windows c++ librairy and so there should be no additional downloads needed.
#include "iostream"
#include "fstream"
#include "cstdio"
#include "cmath"
#include "string.h"
#include "string"
#include "memory"
#include "stdio.h"
#include "stdlib.h"
#include "algorithm"
#include "experimental/filesystem" // C++-standard header file name
#include "filesystem" // Microsoft-specific implementation header file name
using namespace std::experimental::filesystem::v1;
using namespace std;
#pragma warning(disable : 4996)

/*
getData() is where the actual data is copied out of the wav file. It takes in 3 parameters to work: The file itself, an int that tells it an offput from the start,
and an int that tells it the number of characters that it shoud read. Example: string = getData("C:\Users\ConservationMetrics\Downloads\example.wav", 3567, 15);
*/
std::string getData(FILE* wavFile, int offset, int length)
{


	//This opens the file into a unicode redable format suited for wav file types. If it fails to open the file the program will return null
	if (!wavFile)
	{
		return "ERROR_001: The file was unable to open. Try removing quations around file location name";
	}


	//This will place the files "Cursor" or location to start reading from to the offset specified earlier
	fseek(wavFile, offset, SEEK_SET);

	vector<char> buffer;
	for (int i = 0; i < length; i++)
	{
		buffer.push_back(' ');
		fread(&buffer[i], 1, 1, wavFile);
	}
	std::string data(buffer.begin(), buffer.end());
	fseek(wavFile, offset, SEEK_SET);
	if (data.length() >= length + 1)
	{
		data.erase(length);
	}
	return data;
}
/*
This is a data structure that was made to read parts of the wav file header and return the total length of the junk and data parts of the file.
*/
struct wav_header_t
{
	char chunkID[4]; //"RIFF" = 0x46464952
	unsigned long chunkSize; //28 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
	char format[4]; //"WAVE" = 0x45564157
	char subchunk1ID[4]; //"fmt " = 0x20746D66
	unsigned long subchunk1Size; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	//[WORD wExtraFormatBytes;]
	//[Extra format bytes]
};

/*
This data structure is used to extract numerical data in a 32 bit format with its label
*/
struct chunk_t
{
	char ID[4]; //"data" = 0x61746164
	unsigned long size;  //Chunk data bytes
};
/*
This data structure is used to read 32 bit ints without their label
*/
struct chunk_ts
{
	unsigned long size;  //Chunk data bytes
};
/*
This data structure is used to read 16 bit ints without their label
*/
struct chunk_t16
{
	unsigned short size;  //Chunk data bytes
};
/*
This function takes in the wav file, reads through header informatioin and then returns
*/
int WavReader(FILE* wavFile)
{
	fseek(wavFile, 36, SEEK_SET);
	//Reading file
	chunk_t chunk;
	int sum = 0;
	//go to data chunk
	while (true)
	{
		fread(&chunk, sizeof(chunk), 1, wavFile);
		sum += chunk.size;
		if (*(unsigned int*)& chunk.ID == 0x61746164)
			break;
		//skip chunk data bytes
		fseek(wavFile, chunk.size, SEEK_CUR);
	}
	fseek(wavFile, 0, SEEK_SET);
	return sum;
}
//Returns a 32 bit int(4 characters) from offset in the file
int getSize(FILE* wavFile, int offset)
{
	fseek(wavFile, offset, SEEK_SET);
	chunk_ts chunk;
	fread(&chunk, sizeof(chunk), 1, wavFile);
	fseek(wavFile, offset, SEEK_SET);
	return chunk.size;
}
//Returns a 16 bit int(4 characters) from offset in the file
int getSize16(FILE* wavFile, int offset)
{
	fseek(wavFile, offset, SEEK_SET);
	chunk_t16 chunk;
	fread(&chunk, 2, 1, wavFile);
	fseek(wavFile, offset, SEEK_SET);
	return chunk.size;
}
//Returns the type of metadata from a specified ID number taken out of the file
std::string getID(int ID)
{
	if (ID == 1)
	{
		return "device_model";
	}
	if (ID == 2)
	{
		return "device_serial_number";
	}
	if (ID == 3)
	{
		return "firmware_version";
	}
	if (ID == 4)
	{
		return "device_name";
	}
	if (ID == 5)
	{
		return "start_time";
	}
	if (ID == 6)
	{
		return "initial_gps_location_longitude,initial_gps_location_latitude";
	}
	if (ID == 7)
	{
		return "gps_track_log";
	}
	if (ID == 8)
	{
		return "device_model";
	}
	if (ID == 9)
	{
		return "device_software_licence";
	}
	if (ID == 10)
	{
		return "user_notes";
	}
	if (ID == 11)
	{
		return "auto_id";
	}
	if (ID == 12)
	{
		return "manual_id";
	}
	if (ID == 13)
	{
		return "voice_notes";
	}
	if (ID == 14)
	{
		return "auto_id_stats";
	}
	if (ID == 15)
	{
		return "time_expansion_factor";
	}
	if (ID == 16)
	{
		return "device_parameter_blocks";
	}
	if (ID == 17)
	{
		return "device_run_state";
	}
	if (ID == 18)
	{
		return "microphone_type_one,microphone_type_two";
	}
	if (ID == 19)
	{
		return "microphone_sensitivty_one,microphone_sensitivty_two";
	}
	if (ID == 20)
	{
		return "final_gps_position_longitude,final_gps_position_latitude";
	}
	if (ID == 21)
	{
		return "internal_tempreture";
	}
	if (ID == 22)
	{
		return "external_tempreture";
	}
	if (ID == 23)
	{
		return "humidity";
	}
	if (ID == 24)
	{
		return "light";
	}
	if (ID >= 100)
	{
		return "end_padding";
	}
}
//The actual part of the program that runs when the .exe is launched
int main(int argc, char* argv[])
{
	//Make all of the csv asosiated variables.
	std::string folderLocation;
	std::string folderLoc;
	std::string exp = "";
	//Checks if arguments were given through command line. 
	if (argc == 3)
	{
		folderLocation = argv[1];
		folderLoc = argv[2];
	}
	//Otherwise manually enter the parameters
	else
	{
		std::cout << "Please enter the location of the folder (Read).\t";
		std::getline(std::cin, folderLocation);
		std::cout << "Please enter the location of the folder (Write). (Input 'n' for default)\t";
		std::getline(std::cin, folderLoc);
	}
	//Creates export location
	if (folderLoc == "n")
	{
		folderLoc = folderLocation;
	}
	if (folderLoc.rfind("\\") != -1)
	{
		std::string temp = folderLoc.substr(folderLoc.rfind("\\"), folderLoc.length());
		folderLoc = folderLoc + "\\" + temp;
	}
	else if (folderLoc.rfind("/") != -1)
	{
		std::string temp = folderLoc.substr(folderLoc.rfind("/"), folderLoc.length());
		folderLoc = folderLoc + "/" + temp;
	}
	//Makes array for organising
	string arr[3][27];
	arr[0][0] = "file_name";
	//Makes csv labels
	for (int i = 1; i < 25; i++)
	{
		arr[0][i] = getID(i);
	}
	for (int i = 0; i < 26; i++)
	{
		exp += arr[0][i];
		exp += ",";
		arr[0][i] = "";
	}
	exp += "\n";
	//Finds the total number of files
	int tot = 1;
	int max = 1;
	for (const auto& entry : std::experimental::filesystem::recursive_directory_iterator(folderLocation))
	{
		max++;
	}
	//Runs through the files and adds them to the array 
	for (const auto& entry : std::experimental::filesystem::recursive_directory_iterator(folderLocation))
	{
		//Prints out progress
		std::cout << "On file: " << tot << " of " << max << "\r";
		//Makes nessisairy objects
		std::string fileLocation = entry.path().string();
		long offset;
		int length, nextLength, ID;
		std::string readOut;
		//Checks if it is a wav file
		if (fileLocation.substr(fileLocation.size() - 3, 3) == "wav" || fileLocation.substr(fileLocation.size() - 3, 3) == "WAV")
		{
			//Opens the file and checks if it contains information
			FILE* wavFile = fopen(fileLocation.c_str(), "rb");
			arr[0][0] = fileLocation;
			if (getData(wavFile, 0, 4) == "RIFF")
			{
				//Checks is the file is made by Wildlife Acoustics and is SM3/4
				offset = 0;
				offset = WavReader(wavFile) + 52;
				readOut = getData(wavFile, offset, 4);
				if (readOut == "wamd")
				{
					offset += 18;
					nextLength = getSize(wavFile, offset);
					offset += 4;
					readOut = getData(wavFile, offset, nextLength);
					arr[0][1] = readOut;
					if (readOut == "SM4" || readOut == "SM3" || readOut == "SM4BAT")
					{
						if (readOut == "SM3")
						{
							offset += 3;
						}
						offset += nextLength;
						ID = getSize16(wavFile, offset);
						offset += 2;
						nextLength = getSize(wavFile, offset);
						offset += 4;
						readOut = getData(wavFile, offset, nextLength);
						arr[0][ID] = readOut;
						offset += nextLength;
						//Runs through all of the metadata that is in the file
						while (ID < 27 && nextLength < 100)
						{
							ID = getSize16(wavFile, offset);
							//Corrects error in the SM4 metadata
							if (ID > 17 && ID < 100 && (arr[0][1] == "SM4" || arr[0][1] == "SM4BAT"))
							{
								ID++;
							}
							if (ID < 27)
							{
								offset += 2;
								nextLength = getSize(wavFile, offset);
								offset += 4;
								readOut = getData(wavFile, offset, nextLength);
								//Edits certain bits of the metadata to make it more useable in csv format. 
								if (ID == 21 || ID == 6)
								{
									std::string neg = "-";
									readOut.erase(0, 1);
									int commaPos = readOut.find(",");
									readOut.erase(commaPos, 1);
									if (readOut.substr(commaPos, 1) == "S")
									{
										readOut = neg + readOut;
										readOut.erase(commaPos + 1, 1);
									}
									else
									{
										readOut.erase(commaPos, 1);
									}
									commaPos = readOut.rfind(",");
									readOut.erase(commaPos, 1);
									if (readOut.substr(commaPos, 1) == "E")
									{
										readOut.erase(commaPos, 1);
										commaPos = readOut.find(",");
										readOut = readOut.substr(0, commaPos + 1) + neg + readOut.substr(commaPos + 1, (readOut.length() - commaPos));
									}
									else
									{
										readOut.erase(commaPos, 1);
									}
								}
								if (ID == 22 || ID == 23)
								{
									readOut.erase(readOut.size() - 1, 1);
								}
								if (ID == 17 || ID == 16)
								{
									readOut.erase(4, readOut.size());
								}
								arr[0][ID] = readOut;
								offset += nextLength;
							}
						}

					}
				}
				else
				{
					//Checks range for SM2 title
					int counter = -1;
					int off = -1;
					while (off < 0 && counter < 10)
					{
						counter++;
						readOut = getData(wavFile, 58 + counter, 3);
						off = readOut.find("SM2");
					}
					if (off != -1)
					{
						//Same thing as above but for SM2
						arr[0][1] = "SM2";
						offset = 61;
						ID = 0;
						nextLength = 0;
						while (ID < 100 && nextLength < 100)//Right
						{
							ID = getSize16(wavFile, offset);
							if (ID < 100)
							{
								offset += 2;
								nextLength = getSize(wavFile, offset);
								offset += 4;
								readOut = getData(wavFile, offset, nextLength);
								if (ID == 21 || ID == 6)
								{
									std::string neg = "-";
									readOut.erase(0, 1);
									int commaPos = readOut.find(",");
									readOut.erase(commaPos, 1);
									if (readOut.substr(commaPos, 1) == "S")
									{
										readOut = neg + readOut;
										readOut.erase(commaPos + 1, 1);
									}
									else
									{
										readOut.erase(commaPos, 1);
									}
									commaPos = readOut.rfind(",");
									readOut.erase(commaPos, 1);
									if (readOut.substr(commaPos, 1) == "E")
									{
										readOut.erase(commaPos, 1);
										commaPos = readOut.find(",");
										readOut = readOut.substr(0, commaPos + 1) + neg + readOut.substr(commaPos + 1, (readOut.length() - commaPos));
									}
									else
									{
										readOut.erase(commaPos, 1);
									}
								}
								if (ID == 16 || ID == 17 || ID == 22 || ID == 23)
								{
									readOut.erase(readOut.size() - 1, 1);
								}
								arr[0][ID] = readOut;
								offset += nextLength;
							}
						}
					}
					//Writes this into the file if there is an error
					else
					{
						arr[0][10] = "not_wamd_file";
					}
				}
				tot++;
				fclose(wavFile);
			}
			else
			{
				arr[0][10] = "empty_file";

				tot++;
			}
		}
		else
		{
			arr[0][10] = "not_wav_filetype";

			tot++;
		}
		//Takes the information in the array and puts it into the string for exporting
		for (int i = 0; i < 26; i++)
		{
			exp += arr[0][i];
			exp += ",";
			arr[0][i] = "";
		}
		exp += "\n";
	}
	//Opens file to export
	std::ofstream myFile;
	myFile.open(folderLoc + "_wamd.csv");
	//Writes in data and closes file
	myFile << exp;
	myFile.close();
	return 0;
}






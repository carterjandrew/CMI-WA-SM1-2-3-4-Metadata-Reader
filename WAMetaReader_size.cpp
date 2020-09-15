/*These are all of the imports that this project needs to run. They are all incuded by default in the windows c++ librairy and so there should be no additional downloads needed.
However to run without a c++ compiler either keep the .exe file in the folder, move the .dll files with it, or use the optional installer to install the files to system32*/
#include "iostream"
#include "fstream"
#include "cstdio"
#include "cmath"
#include "string.h"
#include "string"
#include "memory"
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include "atlstr.h"
#include "algorithm"
#include <thread>
#include "experimental/filesystem" // C++-standard header file name
#include "filesystem" // Microsoft-specific implementation header file name
using namespace std::experimental::filesystem::v1;
using namespace std;
#pragma warning(disable : 4996)
//This is a globalized list of strings. It is used to export data from each thread back into the main function. If you have more than 256 threads on your computer change the number to the maximum amout that you have
std::string final[256];

/*
getData() is where ascii is copied out of the wav file. It takes in 3 parameters to work: The file itself, an int that tells it an offput from the start,
and an int that tells it the number of characters that it shoud read. Example: string = getData("C:\Users\ConservationMetrics\Downloads\example.wav", 3567, 15);
*/
std::string getData(FILE* wavFile, int offset, int length)
{

	//This checks if the file was opened properly. If not, the reader will return an error that leaves the files place in the csv blank.
	if (!wavFile)
	{
		return "ERROR_001: The file was unable to open. Try removing quations around file location name";
	}


	//This will place the files "Cursor" or location to start reading from to the offset specified earlier
	fseek(wavFile, offset, SEEK_SET);
	//This creates a dynamic array of characters that can be used to convert a VOID** into a string. 
	vector<char> buffer;
	for (int i = 0; i < length; i++)
	{
		buffer.push_back(' ');
		fread(&buffer[i], 1, 1, wavFile);
	}
	//This creates a string out of the array of chars
	std::string data(buffer.begin(), buffer.end());
	//This resets the file cursor to the original location
	fseek(wavFile, offset, SEEK_SET);
	//This gets rid of the random extra characters that (for one reason or another) pile up at the end of the read out string 
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
	UINT16 size;  //Chunk data bytes
};
/*
This function takes in the wav file, reads through header informatioin and then returns the size(in characters) not including the extra data added by SM3 and SM4
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
//This reads a 32 bit int and returns it
int getSize(FILE* wavFile, int offset)
{
	fseek(wavFile, offset, SEEK_SET);
	chunk_ts chunk;
	fread(&chunk, sizeof(chunk), 1, wavFile);
	fseek(wavFile, offset, SEEK_SET);
	return chunk.size;
}
//This reads a 16 bit int and returns it
int getSize16(FILE* wavFile, int offset)
{
	fseek(wavFile, offset, SEEK_SET);
	chunk_t16 chunk;
	fread(&chunk, 2, 1, wavFile);
	fseek(wavFile, offset, SEEK_SET);
	return chunk.size;
}
int getSize8(FILE* wavFile, int offset)
{
	if (!wavFile)
	{
		return -1;
	}
	fseek(wavFile, offset, SEEK_SET);
	uint8_t size;
	fread(&size, 1, 1, wavFile);
	fseek(wavFile, offset, SEEK_SET);
	return size;
}
unsigned char getChar(FILE* wavFile, int offset)
{
	if (!wavFile)
	{
		return NULL;
	}
	fseek(wavFile, offset, SEEK_SET);
	unsigned char buffer;
	fread(&buffer, sizeof(buffer), 1, wavFile);
	return buffer;
}
//This takes in a 16 bit int(the ID) and returns the ID tag that is outlined in wildlife acoustics metadata header. 
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
//This sets up a sungular thread for running.
void do_join(std::thread& t)
{
	t.join();
}
//This sets up all of the threads for running by sending them all through 
void join_all(std::vector<std::thread>& v)
{
	std::for_each(v.begin(), v.end(), do_join);
}
//This will actually read out all of the data of a file. It returns that data through a globalised object called final[]
int totG = 0;
void readFiles(std::string folderLocation, int startnum, int endnum, int locator, int max, int cores)
{
	//Set up an array for formatting as well as two ints used to count the file number and the actual string used to export the data.
	std::string arr[3][30];
	int tot = 0;
	std::string exp = "";
	//Actually runs through all of the ciles between startnum and endnum
	for (const auto& entry : std::experimental::filesystem::recursive_directory_iterator(folderLocation))
	{
		if (tot > startnum && tot < endnum)
		{
			//Creates other nessisairy objects
			std::string fileLocation = entry.path().string();
			long offset;
			int length, nextLength, ID;
			std::string readOut;
			//Reads through the file and puts data into the array. 
			arr[0][0] = fileLocation;
			if (fileLocation.substr(fileLocation.length() - 4, 4) == ".wav" || fileLocation.substr(fileLocation.length() - 4, 4) == ".WAV")
			{
				//Opens file
				FILE* wavFile = fopen(fileLocation.c_str(), "r");
				//Checks if its actually wav format
				if (getData(wavFile, 0, 4) == "RIFF")
				{
					//Sets up needed variables
					offset = 0;
					std::string readout;
					int nextlength;
					int dataSize;//Bytes
					int SampleRate;//Samps/Secs
					int bitsPSample;//Bits/Samps
					int channels;
					offset += 12;
					readout = getData(wavFile, offset, 3);
					//Checks if the header titles "fmt" exists
					if (readout == "fmt")
					{
						//Grabs needed information from file
						offset += 10;
						channels = getSize16(wavFile, offset);
						offset += 2;
						SampleRate = getSize(wavFile, offset);
						offset += 10;
						bitsPSample = getSize16(wavFile, offset);
						offset += 2;
						readout = getData(wavFile, offset, 4);
						//Searches for the "data" chunk so we can find the size of the data
						while (readout != "data")
						{
							offset += 4;
							nextlength = getSize(wavFile, offset);
							offset += 4;
							offset += nextlength;
							readout = getData(wavFile, offset, 4);
						}
						offset += 4;
						dataSize = getSize(wavFile, offset);
						//Calulates the duration of the file
						bitsPSample /= 8;//Bytes/Samps
						int temp = dataSize / bitsPSample;//Samps
						int result = (temp / SampleRate) / channels;
						//Places the datas into the correct places
						arr[0][24] = std::to_string(dataSize);
						arr[0][25] = std::to_string(SampleRate);
						arr[0][26] = std::to_string(bitsPSample);
						arr[0][27] = std::to_string(channels);
						arr[0][28] = std::to_string(result);
					}
					//If the file cannot be opened(is an empty file)
					else
					{
						arr[0][2] = readout;
					}
					readOut = getData(wavFile, 58, 10);
					int off = readOut.find("SM2");
					if (off != -1)
					{
						arr[0][1] = "SM2";
						offset = 61 + off;
						ID = 0;
						nextLength = 0;
						//Runs through a loop of all of the data
						while (ID < 26 && nextLength < 100)//Right
						{
							ID = getSize16(wavFile, offset);
							if (ID < 26)
							{
								offset += 2;
								nextLength = getSize(wavFile, offset);
								offset += 4;
								readOut = getData(wavFile, offset, nextLength);
								//Changes the properties of certain metadata extracted
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
								//Places the extracted data into the array and moves to the next location.
								arr[0][ID] = readOut;
								offset += nextLength;
							}
						}
					}
					//Same thing but for SM3, SM4, and SM4BAT
					else
					{
						//Skips over the size of the data and junk chunks of the file
						offset = 0;
						offset = WavReader(wavFile) + 52;
						readOut = getData(wavFile, offset, 4);
						//checks if wildlife acoustics made
						if (readOut == "wamd")
						{
							//Same process as above
							offset += 18;
							nextLength = getSize(wavFile, offset);
							offset += 4;
							readOut = getData(wavFile, offset, nextLength);
							offset += nextLength;
							arr[0][1] = readOut;
							if (readOut == "SM4" || readOut == "SM3" || readOut == "SM4BAT")
							{
								ID = getSize16(wavFile, offset);
								offset += 2;
								nextLength = getSize(wavFile, offset);
								offset += 4;
								readOut = getData(wavFile, offset, nextLength);
								arr[0][ID] = readOut; //Right
								offset += nextLength;
								while (ID < 100 && nextLength < 100)//Right
								{
									ID = getSize16(wavFile, offset);
									if (ID > 17 && ID < 100 && (arr[0][1] == "SM4" || arr[0][1] == "SM4BAT"))
									{
										ID++;
									}
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
						//Below are the error statements placed into files when they are not found compatable
						else
						{
							arr[0][10] = "not_wamd_file";
						}
					}
					//Moves on to next file and closes the previous file
					fclose(wavFile);
				}
				else
				{
					arr[0][10] = "empty_file";
				}
			}
			//If it finds a flac then it searches for elements to detirmine the length
			else if (fileLocation.substr(fileLocation.size() - 5, fileLocation.size()) == ".flac")
			{
				//Needed variables
				int offset = 0;
				std::string readout;
				int nextlength;
				int dataSize = 0;//SAM
				int SampleRate = 0;//Samps/Secs
				int temp;
				int channels;
				int result;
				FILE* flacFile = fopen(fileLocation.c_str(), "rb");
				if (getData(flacFile, 0, 4) == "fLaC")
				{
					//Makes shure it is on the "header" chunk. This is labeled by the last 7 bytes, hence the "(temp & 127)"
					offset += 4;
					temp = getSize8(flacFile, offset);
					if ((temp & 127) == 0)
					{
						//Offset to the start of the data we need to get. Then grab the next 12 bytes in the form of chars 
						offset += 14;
						unsigned char stuff[12];
						//Gets the size of the datas, in sample size. We can then convert samples to seconds
						for (int i = 0; i < 12; i++)
						{
							//Fills our chars with data
							stuff[i] = getChar(flacFile, offset + i);
							//Grabs some datas we need(for convinence)
							if (i >= 4 && i <= 7)
							{
								if (i == 4)
								{
									dataSize = (dataSize << 4) + (stuff[i] & 15);
								}
								else
								{
									dataSize = (dataSize << 8) + stuff[i];
								}
							}
						}
						//Gets the sample size 
						for (int i = 0; i < 3; i++)
						{
							temp = (temp << 8) + stuff[i];
						}
						SampleRate = temp >> 4;
						//Calculates the resulting duration
						result = dataSize / SampleRate;
						//Places the information into the correct place in the array
						arr[0][24] = ",,,," + std::to_string(dataSize);
						arr[0][25] = std::to_string(SampleRate);
						arr[0][28] = std::to_string(result);
					}
					else
					{
						arr[0][2] = "bad_flac";
					}
				}
				else
				{
					arr[0][2] = "empty_file";
				}
				fclose(flacFile);
			}
			else
			{
				arr[0][10] = "not_wav_filetype";


			}
			//Corrects the placement of the length information(It gets jostled around by some of the other data)
			if ((arr[0][19] == "" && (arr[0][1] == "SM4" || arr[0][1] == "SM4BAT")) || (arr[0][18] == "" && (arr[0][1] == "SM3" || arr[0][1] == "SM2")))
			{
				arr[0][24] = "," + arr[0][24];
			}
			if ((arr[0][21] == "" && (arr[0][1] == "SM4" || arr[0][1] == "SM4BAT")) || (arr[0][20] == "" && (arr[0][1] == "SM3" || arr[0][1] == "SM2")))
			{
				arr[0][24] = "," + arr[0][24];
			}
			if ((arr[0][23] == "" && (arr[0][1] == "SM4" || arr[0][1] == "SM4BAT")) || (arr[0][22] == "" && (arr[0][1] == "SM3" || arr[0][1] == "SM2")))
			{
				arr[0][24] = "," + arr[0][24];
			}
			if (arr[0][1] == "SM2")
			{
				arr[0][24] = "," + arr[0][24];
			}
			//Loads all of the data into a string and clears the array
			for (int i = 0; i < 30; i++)
			{
				exp += arr[0][i];
				exp += ",";
				arr[0][i] = "";
			}
			exp += "\n";
			tot++;
			totG++;
		}
		//Moves to next file if the file is outside the threads range
		else
		{
			tot++;
		}
	}
	//Exports the string of collected data
	final[locator] = exp;
}

int main(int argc, char* argv[])
{
	int max = 0;
	//Make all of the csv asosiated variables.
	std::string folderLocation;
	std::vector<std::string> folderLocations;
	std::string folderLoc;
	int cores;
	//Checks if arguements were put into the exe (Command line style)
	if (argc == 4)
	{
		folderLocation = (std::string)argv[1];
		folderLoc = (std::string)argv[2];
		cores = atoi(argv[3]);
	}
	//Checks if multiple folder locations were entered(Command line only)
	if (argc > 4)
	{
		for (int i = 1; i < argc - 2; i++)
		{
			folderLocations.push_back((std::string)argv[i]);
		}
		folderLoc = (std::string) argv[argc - 2];
		cores = atoi(argv[argc - 1]);
	}
	//If not asks for arguements manually
	else
	{
		std::cout << "Please enter the location of the folder (Read).\t";
		std::getline(std::cin, folderLocation);
		std::cout << "Please enter the location of the folder (Write). (Input 'n' for default)\t";
		std::getline(std::cin, folderLoc);
		std::string temp;
		std::cout << "Your computer has a total of: " << std::thread::hardware_concurrency() << " cores availible, enter the number to apply to this task.";
		std::cin >> cores;
	}
	//Creates export location
	if (folderLoc == "n" && argc > 4)
	{
		folderLoc = folderLocations[0];
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
	}
	else if (folderLoc == "n")
	{
		folderLoc = folderLocation;
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
	}
	//Finds the total number of files so it can divide them amongst the cores
	if (argc <= 4)
	{
		for (const auto& entry : std::experimental::filesystem::recursive_directory_iterator(folderLocation))
		{
			max++;
		}
	}
	//Creates a list of thread objects to send files to as well as a final result string and organiser array
	std::vector<std::thread> threadArr;
	std::string exp = "";
	std::string arr[3][30];
	//Makes the labels for the csv file
	arr[0][0] = "file_name";
	for (int i = 1; i < 24; i++)
	{
		arr[0][i] = getID(i);
	}
	arr[0][24] = "data_size";
	arr[0][25] = "sample_rate";
	arr[0][26] = "bits_per_sample";
	arr[0][27] = "channels";
	arr[0][28] = "duration";
	for (int i = 0; i < 30; i++)
	{
		exp += arr[0][i];
		exp += ",";
		arr[0][i] = "";
	}
	exp += "\n";
	if (argc > 4)
	{
		int total = 0;
		for (int x = 0; x < argc - 3; x++)
		{
			std::cout << "On folder: " << x + 1 << " of " << argc - 3 << "\r";
			for (const auto& entry : std::experimental::filesystem::recursive_directory_iterator(folderLocations[x]))
			{
				max++;
			}
			//Evenly divides the number of files to the cores. If it cant divide evenly the last core will get more/less files
			if (max % cores == 0)
			{
				//Creates the threads inside of the string
				for (int i = 0; i < cores; i++)
				{
					threadArr.push_back(std::thread(readFiles, folderLocations[x], (max / cores) * i, (max / cores) + ((max / cores) * i) + 1, total, max, cores));
					total++;
				}
			}
			else
			{
				//Same thiong as above but last thread has more/less
				for (int i = 0; i < cores - 1; i++)
				{
					threadArr.push_back(std::thread(readFiles, folderLocations[x], (max / cores) * i, (max / cores) + ((max / cores) * i) + 1, total, max, cores));
					total++;
				}

				threadArr.push_back(std::thread(readFiles, folderLocations[x], (max / cores) * (cores - 1), max + 1, total, max, cores));
				total++;
			}
			join_all(threadArr);
			threadArr.clear();
		}
	}
	else
	{
		std::cout << "Reading files now\r";
		//Evenly divides the number of files to the cores. If it cant divide evenly the last core will get more/less files
		if (max % cores == 0)
		{
			//Creates the threads inside of the string
			for (int i = 0; i < cores; i++)
			{
				threadArr.push_back(std::thread(readFiles, folderLocation, (max / cores) * i, (max / cores) + ((max / cores) * i) + 1, i, max, cores));
			}
			//Initialises all of the threads and runs them
			join_all(threadArr);
		}
		else
		{
			//Same thiong as above but last thread has more/less
			for (int i = 0; i < cores - 1; i++)
			{
				threadArr.push_back(std::thread(readFiles, folderLocation, (max / cores) * i, (max / cores) + ((max / cores) * i) + 1, i, max, cores));
			}

			threadArr.push_back(std::thread(readFiles, folderLocation, (max / cores) * (cores - 1), max + 1, cores - 1, max, cores));
			join_all(threadArr);
		}
	}
	//Creates the file to write to
	std::ofstream myFile;
	myFile.open(folderLoc + "_WAMD.csv");
	//Adds labels on to the file
	myFile << exp;
	//Adds the actual data on to the file.
	if (argc <= 4)
	{
		for (int i = 0; i < cores; i++)
		{
			myFile << final[i];
		}
	}
	else
	{
		for (int i = 0; i < (argc - 3) * cores; i++)
		{
			myFile << final[i];
		}
	}
	//Closes file
	myFile.close();
	//Prints out final file location
	std::cout << "File location:\t" << folderLoc + "_WAMD.csv";
	return 0;
}




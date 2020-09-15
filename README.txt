WAMetaReader(Wildlife Acoustics Meta Data Reader)
Created by: Carter Andrew, Conservation Metrics, Santa Cruz, CA
This program was created entirely by Conservation Metrics
WARNING: The multicore edition may cause errors if the files are located on a local server. However these errors have not been proven to be linked to this program. 

DESCRIPTION:

	WAMetaReader is a windows oriented program that will read MetaData and Duration out of WildLife Acoustics song meter devices as well as .flac files and place the data into a .csv table format.

	This program intakes up to 256/number of cores(only in multicore and length versions) filepaths that the .wav or .flac files are located in, a write location for the outgoing .csv, and (If using the multicore version) the number of threads you want to use.

	Example file name:
	Folder Location: C:\Users\ConservationMetrics\Downloads\Test 3\Test1
	File name: Test1_WAMD.csv
 
USE GUIDE:
	
	USING THE .EXE:
		1) Click on the .exe file you wish to use.

		2) Open the file location in the file explorer and copy the adress from the location bar.

		3) Paste this file into the prompt that shows up and hit enter

		4) Find the location you want to export the file to and place that location into the next prompt. If you decide to enter 'n' this will place the file inside of the folder you scanned.

		5) If you are using the multicore version it will ask for the number of cores you want to use. Enter an amount less than or equal to the amount displayed in the prompt.

		6) The files will now start to be read. If you are using the multicore no display of progress will show up. 

		7) The window will automatically close once it is finished scanning the files

	USING THROUGH COMMAND PROMPT:

		1) This program can be fed arguements through the command line, the arguments are as follows:

			1) The location of the files to be read, you can enter more than one of these filepaths it will place all the datas into one file. 
			
			2) The location of the files to be writen(or 'n' for default)
			
			3) The number of cores to be used (If in the multicore .exe)

		2) How this program is actually implimented depends on the language/ide you are using and therefore will vary. Here is an example in the coding language R:
	
			1) system2("C:/Users/ConservationMetrics/Desktop/WAMetaDataExtractor/Windows/WAMetaReader_multicore.exe",args = c("C:/Users/ConservationMetrics/Downloads/Test3","n","2"));

		3) Example in comand line(multiple file paths):
			C:/Users/ConservationMetrics/Desktop/WAMetaDataExtractor/Windows/WAMetaReader_multicore.exe "C:/Users/ConservationMetrics/Downloads/Test3" "C:/Users/ConservationMetrics/Downloads/Test2" "C:/Users/ConservationMetrics/Downloads/MyFile" "4"
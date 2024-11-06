# QHB_Tools

This repository holds the code and executables to extract data from the .log and .log.info files recorded by the SMIOT QHB audio cards, as well as some postprocessing or analysis programs.

# Table of Content

- [Log and Info parsers](#log-and-info-parsers)
  - [Log2Wav script](#log2wav-script)
    - [Linux](#linux)
    - [Windows](#windows)
    - [Windows with UI](#windows-with-interface)
    - [Compilation](#compilation)
  - [RapportIMU2txt](#rapportimu2txt)
  - [RapportInfo2txt](#rapportinfo2txt)
- [GPS Scripts](#gps-scripts)
  - [PPS and GPS data extraction](#pps-and-gps-data-extraction)
  - [Distance between two systems](#distance-between-two-systems)
- [Archives](#archives)
- [Documentation](#doc)
- [Config Scripts](#qhb-config-scripts)


## Log and Info parsers

The following C scripts retrieve the audio and IMU data from .log and .log.info files created by QHB cards and save them into new .wav and .csv files.

### Log2Wav script

This script allow for the conversion of .log files into .wav and .csv files.
The executables for the log2Wav program can be found in the __Release__ folder, see below for how to use them.

#### Linux

To use the log2wav program on Linux, use the following command :  
`Release/log2wav_V2.3 /path/to/your/log/file.log /path/to/the/output/wav/file.wav /path/to/the/output/csv/file.csv 1`
Extracting the .csv data is optionnal if you only need the audio, then you can write only the first two arguments. The one at the end is to use if you want to select the verbose option.

#### Windows

To use the log2wav program on Windows, use the following command :  
`Release\log2wav_V2.3.exe \path\to\your\log\file.log \path\to\the\output\directory.wav \path\to\the\output\directory.csv`

#### Windows with Interface

You can also use an interface in windows if you want more control over the data extraction. For this follow the steps :  
- Download the .zip file in __Release__
- Extract it somewhere
- Run HighBlue_logConverter file
- Select the options you want
- Click on the "Convert file" or "Convert folder" button.
- Select your source .log file or a folder containing several .log files
- Extract

Side note : To convert several .log files in a folder on Linux (when working on a remote server like the Bigpus or the Cube_X) you can use the bash file located at __/nfs/NAS7/SABIOD/METHODE/tools/log2wav_file.sh__ using the following command :  
`bash log2wav_file.sh /path/to/your/log/folder /path/to/the/output/directory`  
This code will always extract both .csv and .wav files and put them in the same output folder.

#### Compilation

If the compiled version of the log2Wav program does not work on your machine you might want to recompile it to suit your local libraries. For this you need first to verify that you have a version of gcc (the compiler) installed on your computer. Then simply open a terminal on the QHB_Tools repository and run the following command :
```
gcc Log2Wav/*.c -o Release/log2Wav_{Your computer name, model or the cube version if on a server} -lm
```
The "-lm" part links the math library. Do not forget it as it is important for the code to run correctly.

### RapportIMU2txt

This script allows for the convertion of .log.info IMU files into .csv files.

### RapportInfo2txt

This script allows for the conversion of .log.info report files into .txt files containing the CNNs detections and certainty values. To run it launch the following command :
```
/Release/RapportInfo2txt /path/to/the/file/ 
```

## GPS Scripts

Those scripts allow for extraction and analysis of the GPS data recorded by the QHBv3 cards.

### PPS and GPS data extraction

The __pps_gps_extraction.py__ script retrieve the PPS and GPS information from the .csv created by the __log2Wav__ program and create separate files for them. To run it launch :
```
python pps_gps_extraction.py /path/to/your/input/file/or/folder/ --output_path /path/to/the/output/folder/
```
Adding the `--merge_data True` argument allow to merge the data from all files if the input path is a folder and combine this data into a single output .csv.

### Distance between two systems

The __gps_to_dist.py__ script computes the distance between two cards at roughly corresponding timestamps (+-1mn) and can plot their positions over the local bathymetric data. For this it uses the gps .csv files created by the __pps_gps_extraction.py__ script. To run it use the following command :
```
python gps_to_dist.py /path/to/your/first/system/gps/file.csv /path/to/your/second/system/gps/file.csv --output_path /path/to/your/output/folder/
```
If you want to plot and save the positions of the systems add the argument `--plot_files True`.

## Archives

This folder contains the executables for older versions of the log2wav program.

## Doc

This folder contains the user manuals and technical documentation for the QHB audio cards (stating at v2).

## QHB Config Scripts

The folder QHB_Scripts contains examples of the JCONFIG.CFG files for each QHB version (starting at v2).

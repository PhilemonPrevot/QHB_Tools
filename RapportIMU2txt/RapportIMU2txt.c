// by paul
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define NTOAS_MAX 200

#define RORQUAL_SAMPLE_RATE 4000
#define RORQUAL_LENSIG RORQUAL_SAMPLE_RATE*60 // load 60sec
#define RORQUAL_WINSIZE 4096
#define RORQUAL_LENSPEC (RORQUAL_LENSIG - RORQUAL_WINSIZE)/RORQUAL_HOPSIZE
#define RORQUAL_HOPSIZE 256
#define RORQUAL_LENPRED RORQUAL_LENSPEC - (5-1)*3 //3 layers of kernel size 5
#define RORQUAL_RAPPORT_NSAMPLESTOSEND 3 // see cacha
#define RORQUAL_RAPPORT_SAMPLE_RATE 12800
#define RORQUAL_RAPPORT_SAMPLESPERSAMPLE 25600 // see cacha

#define CACHA_SAMPLE_RATE 64000
#define CACHA_LENSIG CACHA_SAMPLE_RATE*60 // load 10sec
#define CACHA_WINSIZE 512
#define CACHA_LENSPEC (CACHA_LENSIG - CACHA_WINSIZE)/CACHA_HOPSIZE
#define CACHA_HOPSIZE 256
#define CACHA_LENPRED (((CACHA_LENSPEC - 6)/2 - 6)/2 -6)/2 // 3 layers hopsize 2 kernel 7
#define CACHA_RAPPORT_NSAMPLESTOSEND 10 // a sample is positionned at a high pred from the cnn, we extract the audio signal arround to send back via network
#define CACHA_RAPPORT_SAMPLE_RATE 128000
#define CACHA_RAPPORT_SAMPLESPERSAMPLE 12800// number of audio samples per high pred sample to send back

#define RAPPORT_CHANNELS 5 // number of channels recorded to include in the report
#define ADDITIONNAL_DATA_SIZE 736
#define trameSize 31

typedef struct{
    float predsC[CACHA_LENPRED]; //len of preds for 10sec signal
    short numDetectionsCachalot;
    float predsR[RORQUAL_LENPRED]; //len of preds for 60sec signal
    short numDetectionsRorqual;
    char fileName[50];      //Nom du fichier concerne
    // int ToAs_cacha[NTOAS_MAX];
    // unsigned char hydros_ToAs_cacha[NTOAS_MAX];
    short predPeaksR[RORQUAL_RAPPORT_NSAMPLESTOSEND]; //indices of predPeaks for rorqual
    short predPeaksC[CACHA_RAPPORT_NSAMPLESTOSEND]; //indices of predPeaks for cachalot
    unsigned char imuR[RORQUAL_RAPPORT_NSAMPLESTOSEND][ADDITIONNAL_DATA_SIZE];
    unsigned char imuC[RORQUAL_RAPPORT_NSAMPLESTOSEND][ADDITIONNAL_DATA_SIZE];
    int16_t samplesR[RORQUAL_RAPPORT_NSAMPLESTOSEND][RORQUAL_RAPPORT_SAMPLESPERSAMPLE][RAPPORT_CHANNELS]; // samples to send back for rorqual
    int16_t samplesC[CACHA_RAPPORT_NSAMPLESTOSEND][CACHA_RAPPORT_SAMPLESPERSAMPLE][RAPPORT_CHANNELS]; // samples to send back for cachalot
}RAPPORT;
int main(int argc, char* argv[]){
//  printf("Have you checked rorqual and cacha lensigs and config ?? (needs to match with pic32\'s)");
  FILE* infile = fopen(argv[1], "rb");
  if(infile==NULL){
    printf("Failed to open input file\n");
    return 0;
  }
  static RAPPORT rapport;
  int i, j, timestamp;
  unsigned char* curData;
  short int val;
  fread(&rapport, sizeof(RAPPORT), 1, infile);
  printf("%d cacha and %d rorqual", rapport.numDetectionsCachalot, rapport.numDetectionsRorqual);
    
  strcpy(argv[1] + strlen(argv[1])-5, "_imuC.txt\0");
  FILE* outfile = fopen(argv[1], "w+");
  if(outfile==NULL){
    printf("Failed to open output file\n");
    return 0;
  }
  for(j=0; j<rapport.numDetectionsCachalot; j++){
    curData = rapport.imuC[j] + 6;
    while(curData + trameSize + 6 < rapport.imuC[j] + ADDITIONNAL_DATA_SIZE){
      if(!(curData[0]==0xFE && curData[1]==0x0A && curData[2]==0x0A && curData[5]==0x08)){
        // skip trame if header is incorrect
        curData += trameSize + 6;
        continue;
      }
      curData += 3 + 2; // skip trame header, trame length
      timestamp = *((int*) (curData + 9));
      timestamp = ((timestamp & 0xFF000000)>>24) | ((timestamp & 0x00FF0000)>>8) | ((timestamp & 0x0000FF00)<<8) | ((timestamp & 0x000000FF)<<24);
      fprintf(outfile, "%d,", timestamp);
      for(i=13; i<31; i+=2){
        val = *((short int*) (curData + i));
        val = ((val & 0x00FF)<<8) | ((val & 0xFF00)>>8);
        if(i<29){
          fprintf(outfile, "%hd,", val);
        }else{
          fprintf(outfile, "%hd\n", val);
        }
      }
      curData += trameSize + 1;
    }
    fprintf(outfile, "\n");
  }
  fclose(outfile);

  strcpy(argv[1] + strlen(argv[1])-5, "R.txt\0");
  outfile = fopen(argv[1], "w+");
  if(outfile==NULL){
    printf("Failed to open output file\n");
    return 0;
  }
  for(j=0; j<rapport.numDetectionsRorqual; j++){
    curData = rapport.imuR[j] + 6;
    while(curData + trameSize + 6 < rapport.imuR[j] + ADDITIONNAL_DATA_SIZE){
      if(!(curData[0]==0xFE && curData[1]==0x0A && curData[2]==0x0A && curData[5]==0x08)){
        // skip trame if header is incorrect
        curData += trameSize + 6;
        continue;
      }
      curData += 3 + 2; // skip trame header, trame length
      timestamp = *((int*) (curData + 9));
      timestamp = ((timestamp & 0xFF000000)>>24) | ((timestamp & 0x00FF0000)>>8) | ((timestamp & 0x0000FF00)<<8) | ((timestamp & 0x000000FF)<<24);
      fprintf(outfile, "%d,", timestamp);
      for(i=13; i<31; i+=2){
        val = *((short int*) (curData + i));
        val = ((val & 0x00FF)<<8) | ((val & 0xFF00)>>8);
        if(i<29){
          fprintf(outfile, "%hd,", val);
        }else{
          fprintf(outfile, "%hd\n", val);
        }
      }
      curData += trameSize + 1;
    }
    fprintf(outfile, "\n");
  }
  fclose(outfile);
  fclose(infile);
  return 0;
}

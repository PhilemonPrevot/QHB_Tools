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
    unsigned char imuC[CACHA_RAPPORT_NSAMPLESTOSEND][ADDITIONNAL_DATA_SIZE];
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
  strcpy(argv[1] + strlen(argv[1])-4, "txt\0");
  FILE* outfile = fopen(argv[1], "w+");
  if(outfile==NULL){
    printf("Failed to open output file\n");
    return 0;
  }
  static RAPPORT rapport;
  fread(&rapport, sizeof(RAPPORT), 1, infile);
  printf("Writing into %s with %d fin whale pulses and %d sperm whale clicks \n", argv[1], rapport.numDetectionsRorqual, rapport.numDetectionsCachalot);
  int i, j, k;
  fprintf(outfile, "Filename : %s \n", rapport.fileName);
  fprintf(outfile, "\n rorqual preds\n");
  for(i=0; i<RORQUAL_LENPRED; i++){
    fprintf(outfile, "%f,", rapport.predsR[i]);
  }
  fprintf(outfile, "\n rorqual predPeaks\n");
  for(i=0; i<rapport.numDetectionsRorqual; i++){
    fprintf(outfile, "%hd,", rapport.predPeaksR[i]);
    printf("%f ", rapport.predsR[rapport.predPeaksR[i]]);
  }
  if(rapport.numDetectionsRorqual > 0){
    printf("\n");
  }
  fprintf(outfile, "\n rorqual samples\n");
  for(i=0; i<rapport.numDetectionsRorqual; i++){
    for(j=0; j<RORQUAL_RAPPORT_SAMPLESPERSAMPLE; j++){
      for(k=0; k<RAPPORT_CHANNELS; k++){
        fprintf(outfile, "%hd,", rapport.samplesR[i][j][k]);
      }
    }
    fprintf(outfile, "\n");
  }
  fprintf(outfile, "cacha preds\n");
  for(i=0; i<CACHA_LENPRED; i++){
    fprintf(outfile, "%f,", rapport.predsC[i]);
  }
  fprintf(outfile, "\n cacha predPeaks\n");
  for(i=0; i<rapport.numDetectionsCachalot; i++){
    fprintf(outfile, "%hd,", rapport.predPeaksC[i]);
    printf("%f ", rapport.predsC[rapport.predPeaksC[i]]);
  }
  if(rapport.numDetectionsCachalot > 0){
    printf("\n");
  }
  fprintf(outfile, "\n cacha samples\n");
  for(i=0; i < fmin(rapport.numDetectionsCachalot, CACHA_RAPPORT_NSAMPLESTOSEND); i++){
    for(j=0; j<CACHA_RAPPORT_SAMPLESPERSAMPLE; j++){
      for(k=0; k<RAPPORT_CHANNELS; k++){
        fprintf(outfile, "%hd,", rapport.samplesC[i][j][k]);
      }
    }
    fprintf(outfile, "\n");
  }
  fclose(infile);
  fclose(outfile);
  return 0;
}

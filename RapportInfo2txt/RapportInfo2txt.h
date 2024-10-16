#ifndef _RAPPORTINFO2TXT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define NTOAS_MAX 200
#define ADDITIONNAL_DATA_SIZE 736

//////////////////////
/// BOMBYX PROJECT ///
//////////////////////

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

#define BOMBYX_RAPPORT_CHANNELS 5 // number of channels recorded to include in the report

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
    int16_t samplesR[RORQUAL_RAPPORT_NSAMPLESTOSEND][RORQUAL_RAPPORT_SAMPLESPERSAMPLE][BOMBYX_RAPPORT_CHANNELS]; // samples to send back for rorqual
    int16_t samplesC[CACHA_RAPPORT_NSAMPLESTOSEND][CACHA_RAPPORT_SAMPLESPERSAMPLE][BOMBYX_RAPPORT_CHANNELS]; // samples to send back for cachalot
}BOMBYX_RAPPORT;

///////////////////////
/// PSIBIOM PROJECT ///
///////////////////////

#define BIRD_SAMPLE_RATE 32000
#define BIRD_LENSIG BIRD_SAMPLE_RATE*10 // load 10sec
#define BIRD_WINSIZE 512
#define BIRD_HOPSIZE 256
#define BIRD_LENSPEC (BIRD_LENSIG - BIRD_WINSIZE)/BIRD_HOPSIZE
#define BIRD_SPECHEIGHT BIRD_WINSIZE/2
#define BIRD_WINSIZELOG2 9
#define BIRD_MELFEAT 64
#define BIRD_MINFREQ 50
#define BIRD_MAXFREQ 16000
#define BIRD_LENPRED (((BIRD_LENSPEC - 6)/2 - 6)/2 -6)/2 // 3 layers hopsize 256 kernel 5
#define BIRD_RAPPORT_NSAMPLESTOSEND 10 // a sample is positionned at a high pred from the cnn, we extract the audio signal arround to send back via network => PAS ENCORE TROUVE
#define BIRD_RAPPORT_SAMPLE_RATE 128000 // => PAS ENCORE TROUVE
#define BIRD_RAPPORT_SAMPLESPERSAMPLE 12800 // number of audio samples per high pred sample to send back => PAS ENCORE TROUVE

#define CHIRO_SAMPLE_RATE 256000
#define CHIRO_LENSIG CHIRO_SAMPLE_RATE*5 // load 10sec
#define CHIRO_WINSIZE 512
#define CHIRO_HOPSIZE 256
#define CHIRO_LENSPEC (CHIRO_LENSIG - CHIRO_WINSIZE)/CHIRO_HOPSIZE
#define CHIRO_SPECHEIGHT CHIRO_WINSIZE/2
#define CHIRO_WINSIZELOG2 9
#define CHIRO_MELFEAT 64
#define CHIRO_MINFREQ 10000
#define CHIRO_MAXFREQ 128000
#define CHIRO_LENPRED (((CHIRO_LENSPEC - 6)/2 - 6)/2 -6)/2 // 3 layers hopsize 256 kernel 5
#define CHIRO_RAPPORT_NSAMPLESTOSEND 10 // a sample is positionned at a high pred from the cnn, we extract the audio signal arround to send back via network => PAS ENCORE TROUVE
#define CHIRO_RAPPORT_SAMPLE_RATE 128000 // => PAS ENCORE TROUVE
#define CHIRO_RAPPORT_SAMPLESPERSAMPLE 12800 // number of audio samples per high pred sample to send back => PAS ENCORE TROUVE

#define PSIBIOM_RAPPORT_CHANNELS 2

typedef struct{
    double acousticACI;
    float acousticADI;
    float predsAnura[BIRD_LENPRED];
    float predsBirds[BIRD_LENPRED];
    float predsHemi[BIRD_LENPRED];
    float predsOrtho[BIRD_LENPRED];
    float predsCurrucaCommunis[BIRD_LENPRED]; // len of preds for Curruca communis
    float predsEmberizaCirlus[BIRD_LENPRED]; // len of preds for Emberiza cirlus
    float predsLullulaArborea[BIRD_LENPRED]; // len of preds for Lullula arborea
    float predsEmberizaCalandra[BIRD_LENPRED]; // len of preds for Emberiza calandra
    float predsSaxicolaRubetra[BIRD_LENPRED]; // len of preds for Saxicola rubetra
    float predsEmberizaCitrinella[BIRD_LENPRED]; // len of preds for Emberiza citrinella
    float predsEmberizaHortulana[BIRD_LENPRED]; // len of preds for Emberiza hortulana
    float predsCoturnixCoturnix[BIRD_LENPRED]; // len of preds for Coturnix coturnix
    float predsAlaudaArvensis[BIRD_LENPRED]; // len of preds for Alauda arvensis
    float predsAnthusPratensis[BIRD_LENPRED]; // len of preds for Anthus pratensis
    float predsPipistrellus[CHIRO_LENPRED];
    float predsRhinolophus[CHIRO_LENPRED];
    float predsNyctalus[CHIRO_LENPRED];
    float predsPlecotus[CHIRO_LENPRED];
    float predsMyotis[CHIRO_LENPRED];
    short numDetectionsAnura;
    short numDetectionsBirds;
    short numDetectionsHemi;
    short numDetectionsOrtho;
    short numDetectionsCurrucaCommunis;
    short numDetectionsEmberizaCirlus;
    short numDetectionsLullulaArborea;
    short numDetectionsEmberizaCalandra;
    short numDetectionsSaxicolaRubetra;
    short numDetectionsEmberizaCitrinella;
    short numDetectionsEmberizaHortulana;
    short numDetectionsCoturnixCoturnix;
    short numDetectionsAlaudaArvensis;
    short numDetectionsAnthusPratensis;
    short numDetectionsPipistrellus;
    short numDetectionsRhinolophus;
    short numDetectionsNyctalus;
    short numDetectionsPlecotus;
    short numDetectionsMyotis;
    char fileName[50];      //Nom du fichier concerne
    // int ToAs_cacha[NTOAS_MAX];
    // unsigned char hydros_ToAs_cacha[NTOAS_MAX];
    short predPeaksAnura[BIRD_RAPPORT_NSAMPLESTOSEND];
    short predPeaksBirds[BIRD_RAPPORT_NSAMPLESTOSEND];
    short predPeaksHemi[BIRD_RAPPORT_NSAMPLESTOSEND];
    short predPeaksOrtho[BIRD_RAPPORT_NSAMPLESTOSEND];
    short predPeaksCurrucaCommunis[BIRD_RAPPORT_NSAMPLESTOSEND]; // indices of predPeaks for Curruca communis
    short predPeaksEmberizaCirlus[BIRD_RAPPORT_NSAMPLESTOSEND]; // indices of predPeaks for Emberiza cirlus
    short predPeaksLullulaArborea[BIRD_RAPPORT_NSAMPLESTOSEND]; // indices of predPeaks for Lullula arborea
    short predPeaksEmberizaCalandra[BIRD_RAPPORT_NSAMPLESTOSEND]; // indices of predPeaks for Emberiza calandra
    short predPeaksSaxicolaRubetra[BIRD_RAPPORT_NSAMPLESTOSEND]; // indices of predPeaks for Saxicola rubetra
    short predPeaksEmberizaCitrinella[BIRD_RAPPORT_NSAMPLESTOSEND]; // indices of predPeaks for Emberiza citrinella
    short predPeaksEmberizaHortulana[BIRD_RAPPORT_NSAMPLESTOSEND]; // indices of predPeaks for Emberiza hortulana
    short predPeaksCoturnixCoturnix[BIRD_RAPPORT_NSAMPLESTOSEND]; // indices of predPeaks for Coturnix coturnix
    short predPeaksAlaudaArvensis[BIRD_RAPPORT_NSAMPLESTOSEND]; // indices of predPeaks for Alauda arvensis
    short predPeaksAnthusPratensis[BIRD_RAPPORT_NSAMPLESTOSEND]; // indices of predPeaks for Anthus pratensis
    short predPeaksPipistrellus[CHIRO_RAPPORT_NSAMPLESTOSEND];
    short predPeaksRhinolophus[CHIRO_RAPPORT_NSAMPLESTOSEND];
    short predPeaksNyctalus[CHIRO_RAPPORT_NSAMPLESTOSEND];
    short predPeaksPlecotus[CHIRO_RAPPORT_NSAMPLESTOSEND];
    short predPeaksMyotis[CHIRO_RAPPORT_NSAMPLESTOSEND];
}PSIBIOM_RAPPORT;

int bombyx_parse(char**, FILE*, FILE*);
int psibiom_parse(char**, FILE*, FILE*);
void write_species_data(FILE*, const char*, float*, int, short* predPeaks, int);
int main(int, char**);

#endif
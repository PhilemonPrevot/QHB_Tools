// Original code by Paul Best
// Edited by Philémon Prévot in 10/2024 to add PSIBIOM support
#include "RapportInfo2txt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

////////////////////////
/// MAIN AND METHODS ///
////////////////////////

int bombyx_parse(char** argv, FILE* infile, FILE* outfile){
  BOMBYX_RAPPORT rapport;
  fread(&rapport, sizeof(BOMBYX_RAPPORT), 1, infile);
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
      for(k=0; k<BOMBYX_RAPPORT_CHANNELS; k++){
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
      for(k=0; k<BOMBYX_RAPPORT_CHANNELS; k++){
        fprintf(outfile, "%hd,", rapport.samplesC[i][j][k]);
      }
    }
    fprintf(outfile, "\n");
  }
  fclose(infile);
  fclose(outfile);
  return 0;
}

int psibiom_parse(char** argv, FILE* infile, FILE* outfile){
  PSIBIOM_RAPPORT rapport;
  fread(&rapport, sizeof(PSIBIOM_RAPPORT), 1, infile);
  printf("Writing into %s with the following detections :\n"
  "ACI = %f, \n"
  "ADI = %f, \n"
  "%d Anura detections, \n"
  "%d Birds detections, \n"
  "%d Hemiptera detections, \n"
  "%d Orthoptera detections, \n"
  "%d Curruca Communis detections, \n"
  "%d Emberiza Cirlus detections, \n"
  "%d Lullula Arborea detections, \n"
  "%d Emberiza Calandra detections, \n"
  "%d Saxicola Rubetra detections, \n"
  "%d Emberiza Citrinella detections, \n"
  "%d Emberiza Hortulana detections, \n"
  "%d Coturnix Coturnix detections, \n"
  "%d Alauda Arvensis detections, \n"
  "%d Anthus Pratensis detections, \n"
  "%d Pipistrellus detections, \n"
  "%d Rhinolophus detections, \n"
  "%d Nyctalus detections, \n"
  "%d Plecotus detections, \n"
  "%d Myotis detections.\n",
  argv[1],
  rapport.acousticACI,
  rapport.acousticADI,
  rapport.numDetectionsAnura,
  rapport.numDetectionsBirds,
  rapport.numDetectionsHemi,
  rapport.numDetectionsOrtho,
  rapport.numDetectionsCurrucaCommunis,
  rapport.numDetectionsEmberizaCirlus,
  rapport.numDetectionsLullulaArborea,
  rapport.numDetectionsEmberizaCalandra,
  rapport.numDetectionsSaxicolaRubetra,
  rapport.numDetectionsEmberizaCitrinella,
  rapport.numDetectionsEmberizaHortulana,
  rapport.numDetectionsCoturnixCoturnix,
  rapport.numDetectionsAlaudaArvensis,
  rapport.numDetectionsAnthusPratensis,
  rapport.numDetectionsPipistrellus,
  rapport.numDetectionsRhinolophus,
  rapport.numDetectionsNyctalus,
  rapport.numDetectionsPlecotus,
  rapport.numDetectionsMyotis);

  // Write data for Anura
  write_species_data(outfile, "Anura", rapport.predsAnura, BIRD_LENPRED, rapport.predPeaksAnura, rapport.numDetectionsAnura);

  // Write data for Birds
  write_species_data(outfile, "Birds", rapport.predsBirds, BIRD_LENPRED, rapport.predPeaksBirds, rapport.numDetectionsBirds);

  // Write data for Hemiptera 
  write_species_data(outfile, "Hemiptera", rapport.predsHemi, BIRD_LENPRED, rapport.predPeaksHemi, rapport.numDetectionsHemi);

  // Write data for Orthoptera 
  write_species_data(outfile, "Orthoptera", rapport.predsOrtho, BIRD_LENPRED, rapport.predPeaksOrtho, rapport.numDetectionsOrtho);

  // Write data for Curruca Communis
  write_species_data(outfile, "Curruca Communis", rapport.predsCurrucaCommunis, BIRD_LENPRED, rapport.predPeaksCurrucaCommunis, rapport.numDetectionsCurrucaCommunis);

  // Write data for Emberiza Cirlus
  write_species_data(outfile, "Emberiza Cirlus", rapport.predsEmberizaCirlus, BIRD_LENPRED, rapport.predPeaksEmberizaCirlus, rapport.numDetectionsEmberizaCirlus);

  // Write data for Lullua Arborea
  write_species_data(outfile, "Lullula Arborea", rapport.predsLullulaArborea, BIRD_LENPRED, rapport.predPeaksLullulaArborea, rapport.numDetectionsLullulaArborea);

  // Write data for Emberiza Calandra
  write_species_data(outfile, "Emberiza Calandra", rapport.predsEmberizaCalandra, BIRD_LENPRED, rapport.predPeaksEmberizaCalandra, rapport.numDetectionsEmberizaCalandra);

  // Write data for Saxicola Ruberta
  write_species_data(outfile, "Saxicola Ruberta", rapport.predsSaxicolaRubetra, BIRD_LENPRED, rapport.predPeaksSaxicolaRubetra, rapport.numDetectionsSaxicolaRubetra);

  // Write data for Emberiza Citrinella
  write_species_data(outfile, "Emberiza Citrinella", rapport.predsEmberizaCitrinella, BIRD_LENPRED, rapport.predPeaksEmberizaCitrinella, rapport.numDetectionsEmberizaCitrinella);

  // Write data for Emberiza Hortulana
  write_species_data(outfile, "Emberiza Hortulana", rapport.predsEmberizaHortulana, BIRD_LENPRED, rapport.predPeaksEmberizaHortulana, rapport.numDetectionsEmberizaHortulana);

  // Write data for Coturnix Coturnix
  write_species_data(outfile, "Coturnix Coturnix", rapport.predsCoturnixCoturnix, BIRD_LENPRED, rapport.predPeaksCoturnixCoturnix, rapport.numDetectionsCoturnixCoturnix);

  // Write data for Alauda Arvensis
  write_species_data(outfile, "Alauda Arvensis", rapport.predsAlaudaArvensis, BIRD_LENPRED, rapport.predPeaksAlaudaArvensis, rapport.numDetectionsAlaudaArvensis);

  // Write data for Anthus Pratensis
  write_species_data(outfile, "Anthus Pratensis", rapport.predsAnthusPratensis, BIRD_LENPRED, rapport.predPeaksAnthusPratensis, rapport.numDetectionsAnthusPratensis);

  // Write data for Pipistrellus
  write_species_data(outfile, "Pipistrellus", rapport.predsPipistrellus, CHIRO_LENPRED, rapport.predPeaksPipistrellus, rapport.numDetectionsPipistrellus);

  // Write data for Rhinolophus
  write_species_data(outfile, "Rhinolophus", rapport.predsRhinolophus, CHIRO_LENPRED, rapport.predPeaksRhinolophus, rapport.numDetectionsRhinolophus);

  // Write data for Nyctalus
  write_species_data(outfile, "Nyctalus", rapport.predsNyctalus, CHIRO_LENPRED, rapport.predPeaksNyctalus, rapport.numDetectionsNyctalus);

  // Write data for Plecotus
  write_species_data(outfile, "Plecotus", rapport.predsPlecotus, CHIRO_LENPRED, rapport.predPeaksPlecotus, rapport.numDetectionsPlecotus);

  // Write data for Myotis
  write_species_data(outfile, "Myotis", rapport.predsMyotis, CHIRO_LENPRED, rapport.predPeaksMyotis, rapport.numDetectionsMyotis);
  
  fclose(infile);
  fclose(outfile);
  return 0;
}

void write_species_data(FILE* outfile, const char* species_name, float* preds, int len_preds, short* predPeaks, int num_detections){
  int i, j, k;

  // Write preds
  fprintf(outfile, "\n%s preds\n", species_name);
  for (i = 0; i < len_preds; i++) {
      fprintf(outfile, "%f,", preds[i]);
  }

  // Write pred_peaks
  fprintf(outfile, "\n %s predPeaks\n", species_name);
  for(i=0; i < num_detections; i++){
    fprintf(outfile, "%hd,", predPeaks[i]);
    printf("%f ", preds[predPeaks[i]]);
  }

  if (num_detections > 0) {
      printf("\n");
  }
}

int main(int argc, char* argv[]){
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
  if(strcmp(argv[2], "BOMBYX") == 0 || !argv[2]){
    bombyx_parse(argv, infile, outfile);
  } else if (strcmp(argv[2], "PSIBIOM") == 0){
    psibiom_parse(argv, infile, outfile);
  } else {
    printf("Wrong project argument given : %s. Try 'PSIBIOM' or 'BOMBYX'. \n", argv[2]);
  }
  return 0;
}

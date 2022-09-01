//This is a helper function
//Should do all the reading and passes that to main File
//All my includes
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
 //This is a helper function
//Should do all the reading and passes that to main File

//All my includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "beadPool.h"

//Just a test

#include <limits.h>

locusEntry* locus_entry;


//atrributes. The sizes of these still needs to be calculated
//Would rather store them all on the heap for some flexibility

int main(){
  printf("Starting...\n");

  uint8_t t =  __parse_file("/home/jonahk/H3Africa.bpm");

  return 0;
}



//Actually declaring the functions
//I think these should be put into anther file and imported. Just figuring out the syntax

uint8_t __parse_file(char* filename){
 ht_t *ht = ht_create();


  char buffer[3];

  char* control_config;
  char* manifest_name;



  FILE *ptr;
  uint16_t version_flag = 4096;
  char versionChar;

  uint8_t readIntVer;
  uint32_t num_loci;


  ptr = fopen(filename,"rb");
  if (ptr == NULL) {
    printf("Cannot find filename.");
    exit(-9);
  }


   uint8_t readAttempt = fread(buffer,3,1,ptr);

  if (readAttempt != 1) {
    printf("Unable to read Magic Number: <%d>\n",readAttempt);
    exit(-10);
  }

  if (strncmp(buffer,"BPM",3) != 0) {
    printf("Given file is not BPM <%d>\n",readAttempt);
    exit(-11);
  }

  //Possibly could have this in its own function
  readAttempt = fread(&versionChar, 1, 1, ptr);
  if(readAttempt != 1){
    printf("Unable to read versionn: %d\n", readAttempt);
    exit(-141);
  }

    if(versionChar != 1){
    printf("Wrong BPM version\n"); // not a BPM 1 file
    exit(-12);
  }

  readAttempt = fread(&readIntVer, 4, 1, ptr);
  if(readAttempt != 1){
    printf("Error reading in int: %d\n", readAttempt);
    exit(-12);
  }

  if(readIntVer & version_flag == version_flag){
    readIntVer = readIntVer ^ version_flag;
  }

  if(readIntVer > 5 || readIntVer < 3){
    printf("Unsupported BPM version");
    exit(-13);
  }

 
  

  manifest_name = NULL;
  read_test(ptr, &manifest_name);


  printf("Manifest name read succesfully:%s\n\n", manifest_name);

  if(readIntVer > 1){
    read_test(ptr, &control_config);
    printf("control_config read successfully\n\n");
  }

  num_loci = read_int(ptr);
  printf("Num loci:%i\n\n", num_loci);

  locus_entry = (locusEntry*)malloc(sizeof(locusEntry));

  char** names = (char**)malloc(num_loci * sizeof(char*));
  
  fseek(ptr, 4 * num_loci, SEEK_CUR);

  for(uint32_t i = 0; i < num_loci; ++i){
    read_test(ptr, &names[i]);
    ht_set(ht, names[i], i);
  }

  for(uint32_t i = 0; i < num_loci; ++i){
      free(names[i]);
  }

  int* normalization_ids = (int*)calloc(num_loci, sizeof(uint8_t));

  for(uint32_t i = 0; i < num_loci; ++i){
    normalization_ids[i] = read_byte(ptr);
  }

  int* assay_types = (int*)calloc(num_loci, 4);
  int* addresses = (int*)calloc(num_loci, 4);
  char** snps = (char**)calloc(num_loci, sizeof(char*));
  char** chroms = (char**)calloc(num_loci, sizeof(char*));
  int* map_infos = (int*)calloc(num_loci, sizeof(int));
  int* ref_strands = (int*)calloc(num_loci, sizeof(int));
  int* source_strands = (int*)calloc(num_loci, sizeof(int));

  for(int i = 0; i < num_loci; ++i){
    locusEntry *locus_entry = create_locus_entry(ptr);
    int arrayPos = ht_get(ht, locus_entry->name);

    assay_types[arrayPos] = locus_entry->assay_type;
    addresses[arrayPos] = locus_entry->address_a;
    snps[arrayPos] = locus_entry->snp;
    chroms[arrayPos] = locus_entry->chrom;

    map_infos[arrayPos] = locus_entry->map_info;
    ref_strands[arrayPos] = locus_entry->ref_strand;
    source_strands[arrayPos] = locus_entry->source_strand;
  }

  //I want to compare number of elements in each array
  if( sizeof(normalization_ids)/4 != sizeof(assay_types)/4){
    printf("Manifest format error: read invalid number of assay entries\n\n");
    exit(-59);
  }

  int* all_norms_ids = (int*)calloc(257, sizeof(int));
  for(uint32_t i = 0; i < num_loci; ++i){
    normalization_ids[i] += 100 * assay_types[i];
    //Trying to recreate  sorted set in python
    //To mimic the byte-wrapping behavior from GenomeStudio, AutoCall, IAAP take the mod of 256
    normalization_ids[i] %= 256;
    all_norms_ids[normalization_ids[i]] = 1;
  }

  uint16_t key[256];

  uint16_t counter = 0;
  for(uint16_t i = 0; i < 256; ++i){
    if(all_norms_ids[i] == 1){
      key[i] = counter;
      counter ++;
    }
  }

   int* normalization_lookups = (int*)calloc(num_loci, sizeof(int));

   //Not sure which one ...
   //    for(int i = 0; i < num_loci; ++i){
    for(int i = 0; i < 1; ++i){
      normalization_lookups[i] =  key[normalization_ids[i]];
    }

    printf("Moving to GTC Reader\n");


    uint16_t num_gtc_files = 1;
    uint16_t actual_use = 0;


     // I know 512 seems random. But 256 is the limit of dirent struct. So added for path name
  char directoryName[512] =  "/home/jonahk/GTCs";
  
  DIR *d;
  struct dirent *dir;
  d = opendir(directoryName);

  char** gtcFiles = (char**)calloc(num_gtc_files, sizeof(char*));
  
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if(EndsWithGTC(dir->d_name) == 0){

	strcat(directoryName, "/");
	
	strcat(directoryName, dir->d_name);
	gtcFiles[actual_use] = (char*)malloc(strlen(dir->d_name)+ 1);
	gtcFiles[actual_use] = directoryName;
	actual_use ++;
      }
    }
    closedir(d);
  }

  //  buffer = "";
  for(int i = 0; i < num_gtc_files; i++){
    //Here I  satrt genotype calls.
    FILE* ptr;
    ptr = fopen(gtcFiles[i], "rb");
    if(ptr == NULL){
      printf("Cannot find File\n\n");
      exit(1);
    }
    uint8_t readAttempt = fread(buffer, 3,1,ptr);


    if(readAttempt != 1){
      printf("Unable to read magic number");
      exit(10);
    }
    if (strncmp(buffer,"gtc",3) != 0) {
      printf("Given file is not gtc  <%d>\n",readAttempt);
      exit(-11);
    }

    uint8_t version = read_byte(ptr);
    printf("%i is the version \n\n", version);

     //Check if version is supported
    if(version <  3 || version > 5){
      //An Issue
      printf("Unsupported GTC File vesion");
      exit(10);
    }

    int number_toc_entries = read_int(ptr);


    //This should be changed to a Hashtable later!
    //Now just not effiecent
    //Its a signed 16 bit number, thus 32 768?
    uint16_t id;
    uint32_t offset;

    //My onw bad implementation of a hashtabkle
    //We do not know how big Num_toc really is. so should make this more flexible.
    int keys[256];
    int values[256];
    
    for(uint32_t i = 0; i < number_toc_entries; ++ i){
     
      readAttempt = fread(&id, 2, 1, ptr);
      readAttempt = fread(&offset, 4, 1, ptr);

      keys[i] = id;
      values[i] = offset;      
      
    }


    uint32_t seekValue = 0;
    if(version == 5){
      seekValue = 1016;
      for(int ii = 0; ii < 256; ii ++){
	if(keys[ii] == 1016){
	  seekValue = values[ii];
	}

      }
    }
    if(is_write_complete(version, gtcFiles[i], seekValue, 0, 0) == 0){
      printf("GTC file is incomplete\n");
      exit(10);
    }

    for(int ii = 0; ii < 256; ++ ii){
      if(keys[ii] == 1002){

	seekValue = values[ii];
      }
    }
    uint8_t* genotypes = get_genotypes(gtcFiles[i], seekValue);

    char ** plus_array =  get_base_calls_plus_strand (snps,ref_strands,genotypes, seekValue, gtcFiles[i], num_loci);
    printf("Plus Array read in\n\n");
 
  
    //Watch out, these are local varibles. Need to axtract datat before freeing
    for(uint32_t j = 0; j < num_loci; j ++){
      free(plus_array[j]);
    }
    free(plus_array);
    
  }
    
     
  printf("I made it to the end.\n");

  

  for(uint32_t i = 0; i < num_loci; ++ i){
    free(snps[i]);
    free(chroms[i]);

  }
  free(all_norms_ids);
  free(manifest_name); 
  free(control_config);
  free(normalization_lookups);
  free(normalization_ids);
  free(assay_types);
  free(addresses);
  free(snps);
  free(chroms);
  free(map_infos);
  free(ref_strands);
  free(source_strands);
  free(locus_entry);
  printf("Memory has been freed\n\n\n");
  return 1;

}

uint8_t read_byte(FILE* ptr){
  char readIn;
  int readAttempt = fread(&readIn, 1, 1, ptr);
  if(readAttempt != 1){
    printf("Unable to read in byte: %d\n", readAttempt);
    exit(-14);
  }
  return readIn;
}

uint32_t read_int(FILE* ptr){
  uint32_t num;
  int success = fread(&num, 4, 1, ptr);
  if(success != 1){
    printf("int was read unsuccessfully:%i \n\n", success);
    exit(-14);
  }
   return num;
}

 char* read_string(FILE* ptr){
   uint32_t total_length = 0;
   uint32_t num_bytes = 0;
   uint32_t read; 
   char readIn; 
   uint32_t readAttempt;
   uint8_t partial_length = read_byte(ptr);
   if(partial_length < 0){
     partial_length = 256 + partial_length;
   }

   uint16_t b = partial_length & 0x80;
   while(b > 0){ 
     total_length += (partial_length & 0x7F) << (7 * num_bytes); 
     readAttempt = fread(&partial_length, sizeof(char), 1, ptr); 
     if(readAttempt != 1){
       printf("Error with reading String"); 
       exit(-15); 
     } 
     b = partial_length & 0x80; 
     num_bytes += 1; 
   }

   total_length += partial_length << (7 * num_bytes); 

   char* result = (char*)malloc(total_length+1);

   if(total_length == 0){
     result = "";
   }else{
     readAttempt = fread(result, total_length, 1, ptr); 
     if(readAttempt != 1){ 
       printf("Error reading in string"); 
       exit(-12); 
     }

     result[total_length] = '\0';
     if(strlen(result) < total_length){
       printf("Unable to read the entire string");
       exit(-12);
     } 
   } 

   return result;
}


void read_test(FILE* ptr, char** a){
   uint32_t total_length = 0;
   uint32_t num_bytes = 0;
   uint32_t read; 
   char readIn; 
   uint32_t readAttempt;
   int partial_length = read_byte(ptr);
   if(partial_length < 0){
     partial_length = 256 + partial_length;
   }
   int b = partial_length & 0x80;
   while(b > 0){ 
     total_length += (partial_length & 0x7F) << (7 * num_bytes); 
     readAttempt = fread(&partial_length, sizeof(char), 1, ptr); 
     if(readAttempt != 1){
       printf("Error with reading String"); 
       exit(-15); 
     } 
     b = partial_length & 0x80; 
     num_bytes += 1; 
   }
   total_length += partial_length << (7 * num_bytes);
   *a = (char*)malloc(total_length + 1);
    readAttempt = fread(*a, total_length, 1, ptr);
    (*a)[total_length] = '\0';
 
   if(total_length == 0){

     printf("Note there was an 0\n\n");
 

   }
}
 

//LocusEntry object functions

locusEntry* create_locus_entry(FILE* ptr){
  //  locusEntry* locus_entry = (locusEntry*)malloc(sizeof(locusEntry));

  //This is initiallisation Phase
  char* ilmn_id = "";
  char* name = "";
  char* snp = "";
  char* chrom = "";
  int map_info = -1;
  int assay_type = -1;
  int address_a = -1;
  int address_b = -1;
  int ref_strand = 0;
  int source_strand = 0;
  //Entering the pass_file phase
  int version = read_int(ptr);

   if(version == 6 || version == 7 || version == 8){

     read_test(ptr, &ilmn_id);
      
    int count = 0;
    int usedCounter = 0;

    int target = -1;
    for(int i = 0; ilmn_id[i]!=0; i++){
      if(ilmn_id[i] == '_'){
        target = i;
      }
    }

    char value = ilmn_id[target - 1];

    source_strand = from_string(value);

    name = read_string(ptr);

 char* a;
 //For some reason, cannot use test
    for(int i = 0; i < 3; ++i){
      a = read_string(ptr);
    }

    int tempRead;
    int readAttempt = fread(&tempRead, 4, 1, ptr);


    for(int i = 0; i < 2; ++i){
      a = read_string(ptr);
    }


    snp = read_string(ptr);

    chrom = read_string(ptr);


    for(int i = 0; i < 2; ++ i){
      a = read_string(ptr);
    }


    map_info = atoi(read_string(ptr));


    for(int i = 0; i < 2; ++i){
      a = read_string(ptr);
    }


    address_a = read_int(ptr);
    address_b = read_int(ptr);

    for(int i = 0; i < 7; ++ i){
      a = read_string(ptr);
    }

    readAttempt = fread(&tempRead, 3, 1, ptr);
    assay_type = read_byte(ptr);
    if(assay_type != 0 && assay_type != 1 && assay_type != 2){
      printf("Format error in reading assay type from locus entry\n\n");
      exit(15);

    }
  if(address_b == 0){
      if(assay_type != 0){
        printf("Manifest format error: Assay type is inconsistent with address B");
        exit(15);
      }
    }else{
      if(assay_type == 0){
        printf("Manifest format error: Assay type is inconsistent with address B");
        exit(15);
      }
    }

  }
  if(version == 7 || version == 8){
    int tempRead = 0;
    int readAttempt = fread(&tempRead, 4 * 4, 1, ptr);

  }

  if(version == 8){
    ref_strand = ref_from_string(read_string(ptr)[0]);
  }


  locus_entry->ilmn_id = ilmn_id;
  locus_entry->name = name;
  locus_entry->snp = snp;

  locus_entry->chrom = chrom;
  locus_entry->map_info = map_info;
  locus_entry->assay_type = assay_type;
  locus_entry->address_a = address_a;
  locus_entry->address_b = address_b;
  locus_entry->ref_strand = ref_strand;
  locus_entry->source_strand = source_strand;

  return locus_entry;
}

//The functions from the SourceStrand object
char to_string(int source_strand){
  if(source_strand == 0){
    return 'U';
  }
  else if(source_strand == 1){
    return 'F';
  }
  else if(source_strand == 2){
    return 'R';

  }

  //Should not get here
  printf("Invalid source_stand given to the to_string function\n");
  exit(-14);
}



int from_string(char source_strand){
  if(source_strand == 'U' || source_strand == '\0'){
    return 0;
  }
  else if(source_strand == 'F'){
    return 1;
  }
  else if(source_strand == 'R'){
    return 2;
  }

  //This should not have run
  printf("invalid source_strand given in from_string function");
  exit(-13);
}
int ref_from_string(char ref_strand){
  if(ref_strand == 'U' || ref_strand == '\0'){
    return 0;
  }else if(ref_strand == '+'){
    return 1;
  }else if(ref_strand == '-'){
    return 2;
  }
}

unsigned int hash(const char *key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    // do several rounds of multiplication
    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }

    // make sure value is 0 <= value < TABLE_SIZE
    value = value % TABLE_SIZE;

    return value;
}


entry_t *ht_pair(const char *key, const int value) {
    // allocate the entry
    entry_t *entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = 0;

      //(int* )malloc(sizeof(value) + 1);

    // copy the key and value in place
    strcpy(entry->key, key);
    entry->value = value;

    // next starts out null but may be set later on
    entry->next = NULL;

    return entry;
}


ht_t *ht_create(void) {
    // allocate table
    ht_t *hashtable = malloc(sizeof(ht_t) * 1);

    // allocate table entries
    hashtable->entries = malloc(sizeof(entry_t*) * TABLE_SIZE);

    // set each to null (needed for proper operation)
    int i = 0;
    for (; i < TABLE_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}


void ht_set(ht_t *hashtable, const char *key, int value) {
    unsigned int slot = hash(key);

    // try to look up an entry set
    entry_t *entry = hashtable->entries[slot];

    // no entry means slot empty, insert immediately
    if (entry == NULL) {
        hashtable->entries[slot] = ht_pair(key, value);
        return;
    }

    entry_t *prev;

    // walk through each entry until either the end is
    // reached or a matching key is found
    while (entry != NULL) {
        // check key
        if (strcmp(entry->key, key) == 0) {
            // match found, replace value
          //            free(entry->value);
          //            entry->value = malloc(strlen(value) + 1);
            entry->value = value;
            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;
    }

    // end of chain reached without a match, add new
    prev->next = ht_pair(key, value);
}

int ht_get(ht_t *hashtable, const char *key) {
    unsigned int slot = hash(key);

    // try to find a valid slot
    entry_t *entry = hashtable->entries[slot];

    // no slot means no entry
    if (entry == NULL) {
        return -1;
    }

    // walk through each entry in the slot, which could just be a single thing
    while (entry != NULL) {
        // return value if found
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }

        // proceed to next key if available
        entry = entry->next;
    }
    // reaching here means there were >= 1 entries but no key match
    return -1;
}



int EndsWithGTC( char *string ){
  string = strrchr(string, '.');

  if( string != NULL )
    return( strcmp(string, ".gtc") );

  return( -1 );
  
}


uint8_t is_write_complete(uint8_t version, char* fileName, uint32_t seekValue, uint32_t offset, uint32_t count){

  if(version == 3){
    if( get_num_intensity_only(fileName, seekValue) == 1){
      return 1;
    }
  }else if(version == 4){
    return get_logr_ratios(fileName, seekValue, offset, 4, version, count);
  }

  else if(version == 5){
    return get_slide_identifier(fileName, seekValue);
  }
 }

uint8_t get_slide_identifier(char* fileName, uint32_t seekValue){
  return __get_generic(fileName, seekValue);
}

uint8_t __get_generic(char* fileName, uint32_t seekValue){
  FILE* ptr;
  ptr = fopen(fileName, "rb");
  if(ptr == NULL){
    printf("Cannot find FileName");
    exit(10);
  }

  fseek(ptr, seekValue, SEEK_CUR);

  char* strValue;
  strValue = read_string(ptr);
 
  //  printf("str value is: \n%s\n", strValue);
  free(strValue);
  fclose(ptr);
  return 1;
}



int get_num_intensity_only(char* fileName, uint32_t seekValue){
  //Runs if GTC version is 3, thus I have nver run it, and no idea if it works
  FILE* ptr;
  ptr = fopen(fileName, "rb");
  if(ptr == NULL){
    printf("Cannot find FileName");
    exit(10);
  }

  int testValue;
  fseek(ptr, seekValue + 12, SEEK_CUR);
  testValue = read_int(ptr);
  printf("Value read and is: %i\n\n", testValue);
  fclose(ptr);
  return 1;
}

int get_logr_ratios(char* fileName, uint32_t seekValue, uint32_t offset, uint8_t numBytes, uint8_t version, uint32_t count){

  if(version < 4){
    printf("LogR ratios unavailable in GTC File version %i",version );
    exit(10);
  }
  return __get_generic_array_numpy(fileName, seekValue, offset, numBytes, count);

}

uint32_t __get_generic_array_numpy(char* fileName, uint32_t seekValue, uint32_t offset, uint8_t numBytes, uint32_t count){
  FILE* ptr;
  ptr = fopen(fileName, "rb");
  if(ptr == NULL){
    printf("Cannot read file in get Generic Array ");
    exit(10);
  }

  fseek(ptr, seekValue, SEEK_CUR);
  uint32_t num_entries = read_int(ptr) - 12;

  if(count != 0){
    if(count < num_entries){
      num_entries = count;
    }
  }
  if(offset > 0){
    fseek(ptr, seekValue + 4 + offset*numBytes, SEEK_CUR);
  }

  //return frombuffer(gtc_handle.read(num_entries * numpy_type.itemsize), dtype=numpy_type)

  uint8_t readAttempt;
  float* generic_array = (float*)calloc(num_entries, numBytes);
  readAttempt = fread(generic_array, numBytes, num_entries,ptr);
  
  
  
  fclose(ptr);
  //  return generic_array;
  return readAttempt;//Trying to ake sure everything is okay
  
}



uint8_t* get_genotypes(char* fileName, uint32_t seekValue){
  uint8_t* result = __get_generic_array(seekValue,  0,0, fileName);
  return result;
  
  
}
  
uint8_t* __get_generic_array(uint32_t seekValue, uint32_t offset, uint32_t count, char* fileName){
  uint8_t itemSize = 1;
  
  FILE* ptr;

  ptr = fopen(fileName, "rb");
  if(ptr == NULL){
    printf("Cannot read file in get Generic Array ");
    exit(10);
  }
  printf("Seek value is %i\n\n", seekValue);
  fseek(ptr, seekValue, SEEK_CUR);
  uint32_t num_entries = read_int(ptr) - offset;
  printf("Num entries is %i\n\n", num_entries);
  
  if(count != 0){
    if(count < num_entries){
      num_entries = count;
    }
  }
  
  if(offset > 0){
    uint32_t newSeekVal = seekValue + 4 + offset*itemSize;
    fseek(ptr, newSeekVal, SEEK_CUR);
  }
  
  uint8_t* result = malloc(num_entries + 1);

  int readAttempt = fread(result, 1, num_entries, ptr);

  fclose(ptr);
  return result;

}

char** get_base_calls_plus_strand(char** snps, int* ref_strands, uint8_t* genotypes, uint32_t seekValue, char* fileName, uint32_t num_loci){
  return get_base_calls_generic(snps, ref_strands, genotypes, seekValue, fileName, num_loci);
}

char** get_base_calls_generic(char** snps, int* ref_strands, uint8_t* genotypes, uint32_t seekValue, char* fileName, uint32_t num_loci){
  //  uint8_t genotypes = getGenotypes();
  uint8_t report_strand = 1;
  uint8_t unknown_annotation = 0;


  //Thise is the code 2 genotpye global python varible
  char code2genotype [46][8] = {
    {"NC"},
    {"AA"},
    {"AB"},
    {"BB"},
    {"NULL"},
    {"A"},
    {"B"},
    {"AAA"},
    {"AAB"},
    {"ABB"},
    {"BBB"},
    {"AAAA"},
    {"AAAB"},
    {"AABB"},
    {"ABBB"},
    {"BBBB"},
    {"AAAAA"},
    {"AAAAB"},
    {"AAABB"},
    {"AABBB"},
    {"ABBBB"},
    {"BBBBB"},
    {"AAAAAA"},
    {"AAAAAB"},
    {"AAAABB"},
    {"AAABBB"},
    {"AABBBB"},
    {"ABBBBB"},
    {"BBBBBB"},
    {"AAAAAAA"},
    {"AAAAAAB"},
    {"AAAAABB"},
    {"AAAABBB"},
    {"AAABBBB"},
    {"AABBBBB"},
    {"ABBBBBB"},
    {"BBBBBBB"},
    {"AAAAAAAA"},
    {"AAAAAAAB"},
    {"AAAAAABB"},
    {"AAAAABBB"},
    {"AAAABBBB"},
    {"AAABBBBB"},
    {"AABBBBBB"},
    {"ABBBBBBB"},
    {"BBBBBBBB"}
  };

  FILE* ptr;

  ptr = fopen(fileName, "rb");
  if(ptr == NULL){
    printf("Cannot read file in get Generic Array ");
    exit(10);
  }
  
  fseek(ptr, seekValue, SEEK_CUR);
  uint32_t num_entries = read_int(ptr);

  if(num_entries != num_loci){
    printf("The number of SNPs must match the number of loci in the GTC file");
    exit(12);
  }

  //We need to allocate certain amount of memory. How much?
  char** temp = (char**)malloc(num_loci*sizeof(char*));
 

  //  char** temp = (char**)malloc(3+10*sizeof(char*));

   

  
 for(uint32_t i = 0; i < num_loci; ++i){
     
    char ab_genotype [8];
 
    char a_nucleotide = snps[i][1];
    char b_nucleotide = snps[i][strlen(snps[i])-2];
    temp[i] = (char*)malloc(sizeof(char*));
    
    
    strcpy(ab_genotype, code2genotype[genotypes[i]] );

    //    printf("ab: %s\n", ab_genotype);
    //    printf("a: %c\n", a_nucleotide);
    //    printf("b: %c\n", b_nucleotide);
    
     
    if(a_nucleotide == 'N' || b_nucleotide == 'N' ||  ref_strands[i] == 0 || ( ab_genotype[0] == 'N' && ab_genotype[1] == 'C' )  || strcmp(ab_genotype, "NULL") == 1){
      strcpy(temp[i], "-");
    }else{
      // printf("No fail\n");
      uint8_t ab_len = strlen(code2genotype[genotypes[i]]);
      for(uint8_t ii = 0; ii < ab_len; ii++){
	char nucleotide_allele;
	char holder [8];
	if(ab_genotype[ii] == 'A'){
          nucleotide_allele = a_nucleotide;
        }else{
           nucleotide_allele = b_nucleotide;
        }
	//printf("Nucleo allel: %c\n", nucleotide_allele);
	//printf("stand %i\n", ref_strands[i]);
	
	char comp;
	comp = complement(nucleotide_allele);

	if(ref_strands[i] == 1){
	  holder[ii] = nucleotide_allele;
        }else{
	  holder[ii] = comp;
        }
	if(ii + 1 == ab_len){
	  holder[ab_len] = '\0';
	  strcpy(temp[i], holder);
	}
      }
    }
  }
  return temp;
}


char complement(char nucleotide){

  if(nucleotide == 'A'){
    return 'T';
  }else if (nucleotide == 'T'){
    return 'A';
  }else if (nucleotide == 'C'){
    return 'G';
  }else if (nucleotide == 'G'){
    return 'C';
  }else if (nucleotide == 'D'){
    return 'D';
  }else if (nucleotide == 'I'){
    return 'I';
  }else{
      printf("Nucleotide must be one of A, C, T, G, D, or I");
      exit(10);
      return ' ';
  }

}

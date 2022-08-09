#include <dirent.h> 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


int EndsWithGTC( char *string )
{
  string = strrchr(string, '.');

  if( string != NULL )
    return( strcmp(string, ".gtc") );

  return( -1 );
  
}


uint8_t read_byte(FILE* ptr);
uint32_t read_int(FILE* ptr);
uint8_t is_write_complete(uint8_t version, char* fileName, uint32_t seekValue, uint32_t offset, uint32_t count);
int get_num_intensity_only(char* fileName, uint32_t seekValue);
uint8_t get_slide_identifier(char* fileName, uint32_t seekValue);


int get_logr_ratios(char* fileName, uint32_t seekValue, uint32_t offset, uint8_t numBytes, uint8_t version, uint32_t count);
uint32_t __get_generic_array_numpy(char* fileName, uint32_t seekValue, uint32_t offset, uint8_t numBytes, uint32_t count);
uint8_t __get_generic(char* fileName, uint32_t seekValue);  
char* read_string(FILE* ptr);
uint8_t* __get_generic_array(uint32_t seekValue, uint32_t offset, uint32_t count, char* fileName);
  
uint8_t* get_genotypes(char* fileName, uint32_t seekValue);
  
int main(void) {
  int num_gtc_files = 1;//Need to allocate array and have no idea.
  int actual_use = 0;
  
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

  char buffer[3];
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
    printf("Number toc is :%i\n\n", number_toc_entries);



    //This shou;d be changed to a Hashtable later!
    //Now just not effiecent
    //Its a signed 16 bit number, thus 32 768?
    uint16_t id;
    uint32_t offset;

    //My onw bad implementation of a hashtabkle
    //We do not know how big Num_toc really is. so should make this more flexible.
    int keys[256];
    int values[256];
    

    for(int i = 0; i < number_toc_entries; ++ i){
     
      readAttempt = fread(&id, 2, 1, ptr);
      readAttempt = fread(&offset, 4, 1, ptr);

      keys[i] = id;
      values[i] = offset;      
      
    }
    //(uint8_t version, char* fileName, uint32_t seekValue, uint32_t offset, uint32_t count);

    uint32_t seekValue = 0;
    if(version == 5){
      seekValue = 1016;
      for(int ii = 0; ii < 256; ii ++){
	if(keys[ii] == 1016){
	  printf("Found it at! %i\n\n", values[ii]);
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
	printf("Found next value\n\n");
	seekValue = values[ii];
      }
    }
    uint8_t* actualArray = get_genotypes(gtcFiles[i], seekValue);
    
  }




  for(int i = 0; i < num_gtc_files; ++i){
    //Does not work
    //    free(gtcFiles[i]);
  }
    free(gtcFiles);
    printf("Got to end succesfully ...\n");
  return(0);
}

char* read_string(FILE* ptr){

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

  char* result;

  if(total_length == 0){
    result = "";

  }else{

   //    if(total_length > 100){
    //   free(result);
    //  result = (char*)malloc(total_length+1);
    //  }



    result=(char*)malloc(total_length+1);
    readAttempt = fread(result, total_length, 1, ptr);
    //////res = "";
    //////readAttempt = fread(res, total_length,1,ptr);
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
  printf("Second function hit \n\n");
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

  printf("I am line %i\n\n", ftell(ptr));
  printf("I made it to the reads part!\n\n");

  int readAttempt = fread(result, 1, num_entries, ptr);
  printf("Get to ends\n\n");
  fclose(ptr);

  
  return result;

}

uint8_t * get_base_calls_plus_strand(char** snps, int* ref_strands){
  
  return get_base_calls_generic(snps, ref_strands, 1, 0);
}

uint8_t* get_base_calls_generic(char** snps, int* ref_strands, uint8_t report_strand, uint8_t unknown_annotation){
  uint8_t genotypes = getGenotypes();

}

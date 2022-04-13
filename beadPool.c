//This is a helper function
//Should do all the reading and passes that to main File

//All my includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//This is a helper function
//Should do all the reading and passes that to main File

//All my includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Declaring the functions.
//On first iteration, each function will return sample data. Testing my c syntax
//On second iteration, each function will return proper data.
char* read_ushort(char a){
  char *b = (char*)malloc(4);
  *b = 'j';
  return b;
}

//char* read_ushort(char* filename);
long read_int(FILE* ptr); // I know, read_int returns a long...
char* read_float(char* filename);
int read_byte(FILE* ptr);
char* read_string(FILE* ptr);
char read_char(char* handle);
int decode_code_point(char **s);
int __parse_file(char filename[40]);


//atrributes. The sizes of these still needs to be calculated
//Would rather store them all on the heap for some flexibility
int main(){
  printf("Starting.\n");
  int t =  __parse_file("H3Africa_2017_20021485_A3.bpm");
  //  char* j = read_ushort('o');
  //  free(j);
  char names[25][10];
  char snps [25][10];
  char chroms [25][10];
  char map_infos [25][10];
  char addresses [25][10];
  char normalization_ids [25][10];
  char assay_types [25][10];
  char normalization_lookups [25][10];
  char ref_strands [25][10];
  char source_strands [25][10];
  int num_loci = 0;
  char manifest_name[30];
  char control_config[25];

  //COMPLEMENT_MAP = {"A": "T", "T": "A", "C": "G", "G": "C", "D": "D", "I": "I"};
  //This should be a map.
  char mapKeys[7] = {'A', 'T', 'C', 'G', 'D', 'I'};
  char mapVals[7] = {'T','A', 'G', 'C', 'D','I'};

  return 0;
}


//Actually declaring the functions
//I think these should be put into anther file and imported. Just figuring out the syntax

int __parse_file(char filename[40]){
  char buffer[3];

  char* control_config;
  FILE *ptr;
  int version_flag = 4096;
  char versionChar;
  int versionInt;
  int readIntVer;

  int num_loci; //Will need to make sure this datatype is beg enough
  
  ptr = fopen(filename,"rb");
  if (ptr == NULL) {
    printf("Cannot find filename.");
    exit(-9);
  }

  

  int err = fread(buffer,sizeof(buffer),1,ptr);
  if (err != 1) {
    printf("Unable to read Magic Number: <%d>\n",err);
    exit(-10);
  }
  
  if (strncmp(buffer,"BPM",3) != 0) {
    printf("Given file is not BPM <%d>\n",err);
    exit(-11);
  }

  //Possibly could have this in its own function
  int readAttempt = fread(&versionChar, sizeof(char), 1, ptr);
  if(readAttempt != 1){
    printf("Unable to read versionn: %d\n", readAttempt);
    exit(-14);
  }
  versionInt = (int)(versionChar);
  
  printf("Version Int is: %i\n\n", versionInt);
 
  if(versionInt != 1){
    printf("Wrong BPM version\n"); // not a BPM 1 file
    exit(-12);
  }


  readAttempt = fread(&readIntVer, sizeof(readIntVer), 1, ptr);
  if(readAttempt != 1){
    printf("Error reading in int: %d\n", readAttempt);
    exit(-12);
  }
  

  
  if(readIntVer & version_flag == version_flag){
    readIntVer = readIntVer ^ version_flag;
  }

  printf("Bit manipulated version: %i\n\n",readIntVer );
  
  if(readIntVer > 5 || readIntVer < 3){
    printf("Unsupported BPM version");
    exit(-13);
  }
  read_string(ptr);

  if(readIntVer > 1){
    printf("Reading in control_config\n\n");
    //    control_config = read_string(ptr);
  }

  //  num_loci = read_int(ptr);

  //  free(control_config);
  
  printf("Got to the end\n\n");
  return 1;
}

int read_byte(FILE* ptr){
  char readIn;
  int read;
  int readAttempt = fread(&readIn, sizeof(char), 1, ptr);
  if(readAttempt != 1){
    printf("Unable to read in byte: %d\n", readAttempt);
    exit(-14);
  }
  read = (int)(readIn);

  return read;
}

long read_int(FILE* ptr){
  long* num;
  int success = fread(num, 4, 1, ptr);
  if(success != 1){
    printf("int was read unsuccessfully:%i \n\n", success);
    exit(-14);
  }
  
  return *num;

  // return struct.unpack("<i", handle.read(4))[0]
}

char* read_string(FILE* ptr){//Getting size of Data is NB to properly declare size of storing varibles
  char readIn;
  int readAttempt; 
  int read;

  char* a = (char*)malloc(4);
  *a = 'B';
  char result[30];
  int total_length = 0;
  int  partial_length = read_byte(ptr);
  printf("Partial Len: %i\n\n", partial_length);
  int num_bytes = 0;
  while(partial_length & 0x80 > 0){
    total_length += (partial_length & 0x7F) << (7 * num_bytes);
    readAttempt = fread(&readIn, sizeof(char), 1, ptr);
    if(readAttempt != 1){
      printf("Error with reading String");
      exit(-15);
    }
    read = (int)(readIn);
    partial_length = read; //ord(struct.unpack("c", handle.read(1))[0])
    num_bytes += 1;

   }
  total_length += partial_length << (7 * num_bytes);
  printf("Got this far. Whats is totalLen: %i\n\n", total_length);
  total_length -=1;
  readAttempt = fread(&result, total_length, 1, ptr);

  if(readAttempt != 1){
    printf("Error reading in string");
    exit(-12);
  }
  printf("Made it past string. Read: %s\n\n", result);
  if(strlen(result) < total_length){
    printf("Unable to read the entire string");
    exit(-12);
  }

  //   result = handle.read(total_length);
  //   reselt = result.decode(utf8)
  //    if len(result) < total_length:
  //       raise Exception("Failed to read complete string")
  // else:
  //        return result

  return a;

}

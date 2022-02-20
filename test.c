#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

int main()
{
  printf("Test 35");
  return 0;

}


/*int att(){
  //I expect this part to chnge
  int numIndividuals = 5;
  int numRows = 10;


  char const* const fileOne = "a.txt";
  char const* const fileTwo = "b.txt";
  char const* const fileThree = "c.txt";
  char const* const fileFour = "d.txt";
  char const* const fileFive = "e.txt";
  //... 



  FILE * fileO = fopen(fileOne, "r");
  FILE * fileTw = fopen(fileTwo, "r");
  FILE * fileTh = fopen(fileThree, "r");
  FILE * fileFo = fopen(fileFour, "r");
  FILE * fileFi = fopen(fileFive, "r");

  char line[2];
  char linee[2];
  char *indi;
  uint8_t i = 0;
  int ii = 0;
  int stdRows;
  
  uint8_t* data = (uint8_t*)malloc(numRows*sizeof(i));
  
  uint8_t currentWord;
  if(numRows % 3 == 0){
    stdRows =  numRows/3;
  }else{
    stdRows = (numRows/3) + 1;
  }

  uint8_t position;
  while(i <= stdRows){
    uint8_t curWord = 0;
    while(ii <= numIndividuals){
      position = ii%3;
      int col  = ii/3;
      if(position == 0){
	//Insert into the array
	///	data[i][0] = curWord;
	curWord = 0;
      }else{
	int value;
	fscanf(fileO, "%d", &value);
	curWord = normilise(position, value, curWord);
      }

    i++;

    
    }
  }

  fclose(fileO);
  fclose(fileTw);
  fclose(fileTh);
  fclose(fileFo);
  fclose(fileFi);
  free(data);
  return 0;
}

*/
int mall(){
  int numElements = 5;
  uint8_t j = 42;
  uint8_t* x = (uint8_t*)malloc(numElements*sizeof(j));


  return 0;
}

int normalise(uint8_t postion, uint8_t value, uint8_t updated){
  value = postion*value;
  updated = updated&value;
  return updated;
}

int twoDArray(){
  uint8_t x [5][4];
  x[0][0] = 53;
  printf("the num%d\n", x[0][0]);
  return 0;
}

int bitMan(){
  printf("Workshop begins...\n\n");

  uint8_t x = 5;

  uint8_t y = 9;

  uint8_t c = x&y;

  printf("Value of c: %hu\n\n", c);
  return 0;
}

int readFile(){

  printf("Operation began\n\n");
  char name [35];
  
  FILE *fptr;

  fptr = fopen("./sample.txt", "r");

  fscanf(fptr, "%s",&name);
  printf("the name is : %s", name);
  

  printf("\n\n\n");

  fclose(fptr);


  printf("Operation Ends\n\n");

  return 0;

}

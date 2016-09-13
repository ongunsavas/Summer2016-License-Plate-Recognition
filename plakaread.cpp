 //g++ -o plakaorn plakaread.cpp -lopenalpr 
#include "alpr.h"
#include <stdio.h>
#include <dirent.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAXBUF 1024

#include "plakaread.h"

extern void readplate(char * pics, char * directory,char * country,char * region){
	alpr::Alpr openalpr(country);
	openalpr.setTopN(20);
	char * extended = (char*)malloc(sizeof(char) * MAXBUF);

	strcpy(extended,directory);
	strcat(extended,pics);

	openalpr.setDefaultRegion(region);

	alpr::AlprResults results = openalpr.recognize(extended);

 // Iterate through the results.  There may be multiple plates in an image,
 // and each plate return sthe top N candidates.
	if (openalpr.isLoaded() == false)
	{
		std::cerr << "Error loading OpenALPR" << std::endl;
		exit(1);
	}

	printf("%d matches found:\n",results.plates.size() );
	for (int i = 0; i < results.plates.size(); i++)
	{

		alpr::AlprPlateResult plate = results.plates[i];
		std::cout << "plate" << i << ": " << plate.topNPlates.size() << " results" << std::endl;

		for (int k = 0; k < plate.topNPlates.size(); k++)
		{
			alpr::AlprPlate candidate = plate.topNPlates[k];
			//if(candidate.matches_template)
			//	std::cout << "    - " << candidate.characters << "\t confidence: " << candidate.overall_confidence << std::endl;
			std::cout << "    - " << candidate.characters << "\t confidence: " << candidate.overall_confidence;
       		std::cout << "\t pattern_match: " << candidate.matches_template << std::endl;
    

		}
	}
	/* code */

}

int main(int argc, char const *argv[])
{
	char *specifics[argc];
	char * directory="./",
	*region="az", 
	*country= "eu";
	char specified = 0,debug = 0;
	unsigned short int i = 0, j = 0,k =0;


	for(i = 0;i < argc;i++)
	{
		specifics[i] = (char*)malloc(sizeof(char)*MAXBUF);
	}
	//prepare the directory/country/region etc according to args
	if(argc == 1){
		directory ="./";
		region = "az";
		country = "eu";


	}
	else if(argc > 1){

		for(i = 1; i < argc; i++)
		{
			if(!strcmp(argv[i], "-dir"))
			{
				directory = (char*)argv[i+1];

			}
			else if(!strcmp("-debug",argv[i])){
				debug = 1;
			}
			else if(!strcmp(argv[i],"-reg")){
				region = (char*)argv[i+1];
			}
			else if(!strcmp(argv[i],"-ctry"))
			{
				country = (char*)argv[i+1];	
			}
			else if(!strcmp("check",argv[i])){
				specified =1;

				for(j = i+1;j < argc; j++){
					//printf("%s\n",argv[j]);
					specifics[k++] =(char*) argv[j];
				}
				break;
			}
		}

	}

	if(debug)
	{
		printf("\n\n-=-=-=-=-==-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
		printf("dir: %s\nspecified: %d\nregion: %s\ncountry: %s\n",directory,specified,region,country);
		printf("-=-=-=-=-==-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n\n");

	}
	//if no files given, iteratively lookup in the directory given 
	if(!specified){
		int len;
		struct dirent *pDirent;
		DIR *pDir;

		pDir = opendir (directory);
		if (!pDir) {
			printf ("Cannot open directory '%s'\n", directory);
			return 1;
		}

		while ((pDirent = readdir(pDir))) {
			if(strstr(pDirent->d_name,".jpg")){
				printf ("[%s]\n", pDirent->d_name);
				readplate(pDirent->d_name,directory,country,region);
			}
		}
		closedir (pDir);
	}
	//if given, look up those files
	else {
		unsigned short int a = 0;
		for(a = 0; a < k;a++){
			if(debug) printf ("FILE: [%s]\n", specifics[a]);
			if(strstr(specifics[a],".jpg"))
			{
				//printf ("FILE: [%s]\n", specifics[a]);
				readplate(specifics[a],directory,country,region);
			}
			else printf("ERROR: %s is not a .jpg file!\n",specifics[a]);
		 }

	}


	return 0;
}

